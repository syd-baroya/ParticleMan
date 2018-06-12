// Core libraries
#include <iostream>
#include <cmath>
#include <chrono>
#include <random>

// Third party libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <fbxsdk.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Local headers
#include "GLSL.h"
#include "Program.h"
#include "WindowManager.h"
#include "Shape.h"
#include "Camera.h"
#include "bone.h"

#define AMPLITUDE_FACTOR      0.005     // Amplitude of turbulence. 0.005 works well, but feel free to change
#define NUM_PARTICLES         600       // Number of particles. Any number works. Feel free to change.
#define GRAVITY               -2        // Gravity that the particles experience. -2 works well. Feel free to change.
#define PARTICLE_SCALE_FACTOR 0.015     // Sets particle size. Anywhere from 0.005 - 0.02 works. Feel free to change.
#define FADE_DISTANCE         2.0       // The distance at which particles begin to fade. Anywhere from 1.0 - 2.0 works. Feel free to change.

#define MODEL_SCALE_FACTOR    0.01      // Scales the FBX model. DON'T CHANGE. Must be set to 0.01.


using namespace std;
using namespace glm;

double get_last_elapsed_time() {
    static double lasttime = glfwGetTime();
    double actualtime = glfwGetTime();
    double difference = actualtime - lasttime;
    lasttime = actualtime;
    return difference;
}
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 generator (seed);

vec3 transTurbulence(float t, vec3 amp)
{
    if(t == 0) amp = vec3(1 - (pow(2 * (float) generator() / (generator.max()), 2)),
                    GRAVITY - (pow(2 * (float) generator() / (generator.max()), 2)),
                          1 - (pow(2 * (float) generator() / (generator.max()), 2)));
    
    
    return vec3(AMPLITUDE_FACTOR * amp.x * sin(3.14159 * t),
                AMPLITUDE_FACTOR * amp.y * sin(3.14159 * t),
                AMPLITUDE_FACTOR * amp.z * sin(3.14159 * t));
}

class Application : public EventCallbacks {
public:
    WindowManager *windowManager = nullptr;
    Camera *camera = nullptr;

    std::shared_ptr<Shape> particleShape;
    shared_ptr<Shape> skySphere;
    shared_ptr<Shape> floor;
    std::shared_ptr<Program> progSkeleton;
    std::shared_ptr<Program> progParticles;
    std::shared_ptr<Program> progSky;
    std::shared_ptr<Program> progFloor;
    
    
    GLuint particleTex;
    GLuint skyTex;
    
    // Contains vertex information for OpenGL
    GLuint VertexArrayIDFBXBones;
        
    // Contains vertex information for OpenGL
    GLuint VertexArrayIDMesh;
    // Data necessary to give our box to OpenGL
    GLuint VertexBufferIDMesh, VertexBufferIDWeightIndices, IndexBufferIDMeshIndices, VertexBufferIDWeights;
    int MeshVAOSize=0;
    
    //MESHSTUFF
    vector<vec3> meshpos;
    vector<unsigned int> meshindices;
    vector<ivec4> meshanimindices;
    vector<vec4> meshanimweights;
    
    double gametime = 0;
    bool wireframeEnabled = false;
    bool mousePressed = false;
    bool mouseCaptured = false;
    glm::vec2 mouseMoveOrigin = glm::vec2(0);
    glm::vec3 mouseMoveInitialCameraRot;
    
    //animation matrices:
    mat4 animmat[200];
    int animmatsize=0;
    
    glm::vec3 particle_initial_positions[NUM_PARTICLES];
    glm::vec3 particle_positions[NUM_PARTICLES];
    glm::vec3 particle_amplitudes[NUM_PARTICLES];
    float particle_times[NUM_PARTICLES];
    vector<vec3> bone_positions;

    Application() {
        camera = new Camera();
    }
    
    ~Application() {
        delete camera;
    }

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // Movement
        if (key == GLFW_KEY_W && action != GLFW_REPEAT) camera->vel.z = (action == GLFW_PRESS) * -0.2f;
        if (key == GLFW_KEY_S && action != GLFW_REPEAT) camera->vel.z = (action == GLFW_PRESS) * 0.2f;
        if (key == GLFW_KEY_A && action != GLFW_REPEAT) camera->vel.x = (action == GLFW_PRESS) * -0.2f;
        if (key == GLFW_KEY_D && action != GLFW_REPEAT) camera->vel.x = (action == GLFW_PRESS) * 0.2f;
        // Rotation
        if (key == GLFW_KEY_I && action != GLFW_REPEAT) camera->rotVel.x = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_K && action != GLFW_REPEAT) camera->rotVel.x = (action == GLFW_PRESS) * -0.02f;
        if (key == GLFW_KEY_J && action != GLFW_REPEAT) camera->rotVel.y = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_L && action != GLFW_REPEAT) camera->rotVel.y = (action == GLFW_PRESS) * -0.02f;
        if (key == GLFW_KEY_U && action != GLFW_REPEAT) camera->rotVel.z = (action == GLFW_PRESS) * 0.02f;
        if (key == GLFW_KEY_O && action != GLFW_REPEAT) camera->rotVel.z = (action == GLFW_PRESS) * -0.02f;
        // Polygon mode (wireframe vs solid)
        if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            wireframeEnabled = !wireframeEnabled;
            glPolygonMode(GL_FRONT_AND_BACK, wireframeEnabled ? GL_LINE : GL_FILL);
        }
        // Hide cursor (allows unlimited scrolling)
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            mouseCaptured = !mouseCaptured;
            glfwSetInputMode(window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            resetMouseMoveInitialValues(window);
        }
    }

    void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
        mousePressed = (action != GLFW_RELEASE);
        if (action == GLFW_PRESS) {
            resetMouseMoveInitialValues(window);
        }
    }
    
    void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos) {
        if (mousePressed || mouseCaptured) {
            float yAngle = (xpos - mouseMoveOrigin.x) / windowManager->getWidth() * 3.14159f;
            float xAngle = (ypos - mouseMoveOrigin.y) / windowManager->getHeight() * 3.14159f;
            camera->setRotation(mouseMoveInitialCameraRot + glm::vec3(-xAngle, -yAngle, 0));
        }
    }

    void resizeCallback(GLFWwindow *window, int in_width, int in_height) { }
    
    // Reset mouse move initial position and rotation
    void resetMouseMoveInitialValues(GLFWwindow *window) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouseMoveOrigin = glm::vec2(mouseX, mouseY);
        mouseMoveInitialCameraRot = camera->rot;
    }

    bone *root = NULL;
    int size_stick = 0;
    all_animations all_animation;
    FbxVector4* mesh_vertices;
    int mesh_vertices_count;
    
    
    void initGeom(const std::string& resourceDirectory) {
        for (int ii = 0; ii < 200; ii++)
            animmat[ii] = mat4(1);
        
        for (int ii = 0; ii < NUM_PARTICLES; ii++)
            particle_times[ii] = 0;
        
        for (int ii = 0; ii < NUM_PARTICLES; ii++)
            particle_amplitudes[ii] = vec3(1 - (pow(2 * (float) generator() / (generator.max()), 2)),
                                     GRAVITY - (pow(2 * (float) generator() / (generator.max()), 2)),
                                           1 - (pow(2 * (float) generator() / (generator.max()), 2)));
        
        // Initialize mesh.
        skySphere = make_shared<Shape>();
        skySphere->loadMesh(resourceDirectory + "/sphere.obj");
        skySphere->resize();
        skySphere->init();
        
        // Initialize mesh.
        floor = make_shared<Shape>();
        floor->loadMesh(resourceDirectory + "/multisquare.obj");
        floor->resize();
        floor->init();
        
        particleShape = make_shared<Shape>();
        particleShape->loadMesh(resourceDirectory + "/1d_square.obj");
        particleShape->resize();
        particleShape->init();

        int width, height, channels;
        char filepath[1000];
        
        //particle texture
        string str = resourceDirectory + "/orb.jpg";
        strcpy(filepath, str.c_str());
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &particleTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, particleTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //sky texture
        str = resourceDirectory + "/obsidian.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &skyTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        GLuint Tex1Location = glGetUniformLocation(progParticles->getPID(), "tex");
        GLuint Tex2Location = glGetUniformLocation(progSky->getPID(), "tex");
        // Then bind the uniform samplers to texture units:
        
        glUseProgram(progParticles->getPID());
        glUniform1i(Tex1Location, 0);
        glUseProgram(progSky->getPID());
        glUniform1i(Tex2Location, 1);
        
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
    
        
        readtobone(resourceDirectory + "/ballin2.fbx",&all_animation,&root, &meshpos, &meshindices,&meshanimindices,&meshanimweights);
        root->set_animations(&all_animation,animmat,animmatsize);
        
//        glGenVertexArrays(6, &VertexArrayIDFBXBones);
//        glBindVertexArray(VertexArrayIDFBXBones);
//
//        vector<vec3> pos;
//        vector<unsigned int> imat;
//        root->write_to_VBOs(vec3(0, 0, 0), pos, imat);
//        bone_positions = pos;
//        size_stick = pos.size();
//        //generate vertex buffer to hand off to OGL
//        glGenBuffers(1, &VertexBufferIDBonePositions);
//        //set the current state to focus on our vertex buffer
//        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDBonePositions);
//        //actually memcopy the data - only do this once
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pos.size(), pos.data(), GL_DYNAMIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//        //indices of matrix:
//        glGenBuffers(1, &VertexBufferIDimat);
//        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimat);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imat.size(), imat.data(), GL_DYNAMIC_DRAW);
//        glEnableVertexAttribArray(1);
//        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);
        
        //MESH STUFF
        
        //generate the VAO
        glGenVertexArrays(1, &VertexArrayIDMesh);
        glBindVertexArray(VertexArrayIDMesh);
        //generate vertex buffer to hand off to OGL
        
        glGenBuffers(1, &VertexBufferIDMesh);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDMesh);
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*meshpos.size(), meshpos.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glGenBuffers(1, &VertexBufferIDWeightIndices);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDWeightIndices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ivec4)*meshanimindices.size(), meshanimindices.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 4, GL_INT, GL_FALSE, (void*)0);
        
        glGenBuffers(1, &VertexBufferIDWeights);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDWeights);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*meshanimweights.size(), meshanimweights.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
//        glGenBuffers(1, &VertexBufferIDMesh);
//        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDMesh);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*meshnormals.size(), meshnormals.data(), GL_DYNAMIC_DRAW);
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
//        glGenBuffers(1, &VertexBufferIDMesh);
//        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDMesh);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*meshtexcoords.size(), meshtexcoords.data(), GL_DYNAMIC_DRAW);
//        glEnableVertexAttribArray(2);
//        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        //indices of model:
        glGenBuffers(1, &IndexBufferIDMeshIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDMeshIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshindices.size() * sizeof(unsigned int), meshindices.data(), GL_STATIC_DRAW);
        MeshVAOSize = meshindices.size();
        glBindVertexArray(0);
        
        // End MESH STUFF
        
        
        for (int i = 0; i < NUM_PARTICLES; i++)
        {
            int joint = rand() % 70;
            particle_positions[i].x = animmat[joint][3][0] * MODEL_SCALE_FACTOR;
            particle_positions[i].y = animmat[joint][3][1] * MODEL_SCALE_FACTOR;
            particle_positions[i].z = animmat[joint][3][2] * MODEL_SCALE_FACTOR;
            
            particle_initial_positions[i].x = particle_positions[i].x;
            particle_initial_positions[i].y = particle_positions[i].y;
            particle_initial_positions[i].z = particle_positions[i].z;
            
            //cout << "POS: " << particle_positions[i].x << " " << particle_positions[i].y << " " << particle_positions[i].z << endl;
        }

        
        glBindVertexArray(0);
    }
    
    void init(const std::string& resourceDirectory) {
        GLSL::checkVersion();

        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        //transparency
        glEnable(GL_BLEND);
        //next function defines how to mix the background color with the transparent pixel in the foreground.
        //This is the standard:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Initialize the GLSL program.
        progSky = make_shared<Program>();
        progSky->setVerbose(true);
        progSky->setShaderNames(resourceDirectory + "/sky.vert", resourceDirectory + "/sky.frag");
        if (! progSky->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        progSky->init();
        progSky->addAttribute("vertPos");
        progSky->addAttribute("vertNor");
        progSky->addAttribute("vertTex");
        
        // Initialize the GLSL program.
        progFloor = make_shared<Program>();
        progFloor->setVerbose(true);
        progFloor->setShaderNames(resourceDirectory + "/floor.vert", resourceDirectory + "/floor.frag");
        if (! progFloor->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        progFloor->addUniform("camPos");
        progFloor->addAttribute("vertPos");
        progFloor->addAttribute("vertNor");
        progFloor->addAttribute("vertTex");
        
        progParticles = make_shared<Program>();
        progParticles->setVerbose(true);
        progParticles->setShaderNames(resourceDirectory + "/particle.vert", resourceDirectory + "/particle.frag");
        if(!progParticles->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        progParticles->addAttribute("vertPos");
        progParticles->addAttribute("vertNor");
        progParticles->addAttribute("vertTex");
        
        progSkeleton = std::make_shared<Program>();
        progSkeleton->setVerbose(true);
        progSkeleton->setShaderNames(resourceDirectory + "/skeleton.vert", resourceDirectory + "/skeleton.frag");
        if (!progSkeleton->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        progSkeleton->addUniform("Manim");
        progSkeleton->addUniform("campos");
        progSkeleton->addAttribute("vertPos"); //meshpos[]
        progSkeleton->addAttribute("vertImat"); //meshanimindices[]
        progSkeleton->addAttribute("vertWeights"); //meshanimweights[]
        
        
        camera->pos = vec3(-1.0f, -1.0f, -8.0f);
    }
    
    glm::mat4 getPerspectiveMatrix() {
        float fov = 3.14159f / 4.0f;
        float aspect = windowManager->getAspect();
        return glm::perspective(fov, aspect, 0.01f, 10000.0f);
    }

    void render() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        double frametime = get_last_elapsed_time();
        static double totaltime_ms=0;
        totaltime_ms += frametime*1000.0;
        static double totaltime_untilframe_ms = 0;
        totaltime_untilframe_ms += frametime*1000.0;
        
        for (int ii = 0; ii < 200; ii++)
            animmat[ii] = mat4(1);
        
        //animation frame system
        int anim_step_width_ms = 8490 / 204;
        static int frame = 0;
        if (totaltime_untilframe_ms >= anim_step_width_ms)
        {
            totaltime_untilframe_ms = 0;
            frame++;
        }
        root->play_animation(frame,"Take 001");    //name of current animation

        // Clear framebuffer.
        glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create the matrix stacks.
        glm::mat4 V, M, P;
        P = getPerspectiveMatrix();
        V = camera->getViewMatrix();
        M = glm::mat4(1);

        /**************/
        /* DRAW SKELETON */
        /**************/
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        progSkeleton->bind();
        //send the matrices to the shaders
        glBindVertexArray(VertexArrayIDFBXBones);

        glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(MODEL_SCALE_FACTOR, MODEL_SCALE_FACTOR, MODEL_SCALE_FACTOR));
        M = TransZ * S;
        progSkeleton->setMVP(&M[0][0], &V[0][0], &P[0][0]);
        progSkeleton->setMatrixArray("Manim", 200, &animmat[0][0][0]);
        //glBindVertexArray(0);
        glBindVertexArray(VertexArrayIDMesh);
        glDrawElements(GL_TRIANGLES, MeshVAOSize, GL_UNSIGNED_INT, (void*)0);
        progSkeleton->unbind();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        static float angle = 0.0;
        angle += 0.0005;
        progSky->bind();
        mat4 Rx = glm::rotate(glm::mat4(1), 3.14159f/2.0f,glm::vec3(1, 0, 0));
        mat4 Ry = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));
        mat4 makeItBig = glm::scale(glm::mat4(1), glm::vec3(50, 50, 50));
        mat4 followTheCam = glm::translate(glm::mat4(1), vec3(-camera->pos.x, 0, -camera->pos.z));
        M = followTheCam * Ry * Rx * makeItBig;
        glFrontFace(GL_CW);
        progSky->setMVP(&M[0][0], &V[0][0], &P[0][0]);
        skySphere->draw(progSky, true);
        
        progSky->unbind();
        
        progFloor->bind();
        M = glm::translate(mat4(1), vec3(0, 0.0, 0)) * followTheCam * Ry*  Rx * makeItBig;
        progFloor->setMVP(&M[0][0], &V[0][0], &P[0][0]);
        progFloor->setVector3("camPos", camera->pos.x, camera->pos.y, camera->pos.z);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particleTex);
        floor->draw(progFloor, false);
        progFloor->unbind();
        
        glFrontFace(GL_CCW);
        
        progParticles->bind();
        glUniformMatrix4fv(progParticles->getUniform("P"), 1, GL_FALSE, &P[0][0]);

        mat4 faceTheCam = glm::rotate(glm::mat4(1), camera->rot.y, glm::vec3(0, 1, 0));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particleTex);
        for(int i = 0; i < NUM_PARTICLES; i++)
        {
            float fade = distance(particle_initial_positions[i], particle_positions[i]);
            if(fade < FADE_DISTANCE)
            {
                particle_positions[i] += transTurbulence(particle_times[i], particle_amplitudes[i]);
            }
            else
            {
                particle_times[i] = 0;
                int joint = 1 + rand() % 72;
                while( joint == 5)
                    joint = 1 + rand() % 72;
                particle_positions[i].x = animmat[joint][3][0] * MODEL_SCALE_FACTOR;
                particle_positions[i].y = animmat[joint][3][1] * MODEL_SCALE_FACTOR;
                particle_positions[i].z = animmat[joint][3][2] * MODEL_SCALE_FACTOR;
                
                particle_initial_positions[i].x = particle_positions[i].x;
                particle_initial_positions[i].y = particle_positions[i].y;
                particle_initial_positions[i].z = particle_positions[i].z;
                
                if(particle_initial_positions[i].y <= 0)
                    cout << "JOINT SUCKS: " << joint << endl;
            }
            
            M = glm::translate(TransZ, particle_positions[i]) * faceTheCam;
            M = scale(M, vec3(PARTICLE_SCALE_FACTOR * (1 - particle_times[i]),
                              PARTICLE_SCALE_FACTOR * (1 - particle_times[i]),
                              PARTICLE_SCALE_FACTOR * (1 - particle_times[i])));
            progParticles->setMVP(&M[0][0], &V[0][0], &P[0][0]);
            progParticles->setFloat("fade", (FADE_DISTANCE - fade) / FADE_DISTANCE);
            particleShape->draw(progParticles, false);
        }
        progParticles->unbind();
        
        for (int ii = 0; ii < NUM_PARTICLES; ii++)
        {
            float addme = 0.01 * pow((float) generator() / generator.max(), 2);
            //cout << "addme: " << addme << endl;
            if(particle_times[ii] + addme < 1)
                particle_times[ii] += addme;
            else
                particle_times[ii] = 0;
        }

        
    }
};

int main(int argc, char **argv) {
    std::string resourceDir = "../../resources";
    if (argc >= 2) {
        resourceDir = argv[1];
    }

    Application *application = new Application();

    // Initialize window.
    WindowManager * windowManager = new WindowManager();
    windowManager->init(800, 600);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // Initialize scene.
    application->init(resourceDir);
    application->initGeom(resourceDir);
    
    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle())) {
        // Update camera position.
        application->camera->update();
        // Render scene.
        application->render();

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}
