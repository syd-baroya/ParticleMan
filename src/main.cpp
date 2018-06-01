// Core libraries
#include <iostream>
#include <cmath>

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


using namespace std;
using namespace glm;

mat4 linint_between_two_orientations(vec3 ez_aka_lookto_1, vec3 ey_aka_up_1, vec3 ez_aka_lookto_2, vec3 ey_aka_up_2, float t)
{
    mat4 m1, m2;
    quat q1, q2;
    vec3 ex, ey, ez;
    ey = ey_aka_up_1;
    ez = ez_aka_lookto_1;
    ex = cross(ey, ez);
    m1[0][0] = ex.x;        m1[0][1] = ex.y;        m1[0][2] = ex.z;        m1[0][3] = 0;
    m1[1][0] = ey.x;        m1[1][1] = ey.y;        m1[1][2] = ey.z;        m1[1][3] = 0;
    m1[2][0] = ez.x;        m1[2][1] = ez.y;        m1[2][2] = ez.z;        m1[2][3] = 0;
    m1[3][0] = 0;            m1[3][1] = 0;            m1[3][2] = 0;            m1[3][3] = 1.0f;
    ey = ey_aka_up_2;
    ez = ez_aka_lookto_2;
    ex = cross(ey, ez);
    m2[0][0] = ex.x;        m2[0][1] = ex.y;        m2[0][2] = ex.z;        m2[0][3] = 0;
    m2[1][0] = ey.x;        m2[1][1] = ey.y;        m2[1][2] = ey.z;        m2[1][3] = 0;
    m2[2][0] = ez.x;        m2[2][1] = ez.y;        m2[2][2] = ez.z;        m2[2][3] = 0;
    m2[3][0] = 0;            m2[3][1] = 0;            m2[3][2] = 0;            m2[3][3] = 1.0f;
    q1 = quat(m1);
    q2 = quat(m2);
    quat qt = slerp(q1, q2, t); //<---
    qt = normalize(qt);
    mat4 mt = mat4(qt);
    //mt = transpose(mt);         //<---
    return mt;
}


double get_last_elapsed_time() {
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}

class Application : public EventCallbacks {
public:
	WindowManager *windowManager = nullptr;
    Camera *camera = nullptr;

    std::shared_ptr<Shape> shape;
	std::shared_ptr<Program> skeleton;
    
    // Contains vertex information for OpenGL
    GLuint VertexArrayID;
    
    // Data necessary to give our box to OpenGL
    GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox, VertexBufferIDimat;
    
    double gametime = 0;
    bool wireframeEnabled = false;
    bool mousePressed = false;
    bool mouseCaptured = false;
    glm::vec2 mouseMoveOrigin = glm::vec2(0);
    glm::vec3 mouseMoveInitialCameraRot;
    
    //animation matrices:
    mat4 animmat[200];
    int animmatsize=0;

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
        
//        shape = make_shared<Shape>();
//        shape->loadMesh(resourceDirectory + "/ballin.obj");
//        shape->resize();
//        shape->init();
//

        
        readtobone("../../resources/ballin.fbx",&all_animation,&root, &mesh_vertices, &mesh_vertices_count);
        

        
        root->set_animations(&all_animation,animmat,animmatsize);
        
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        
        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferID);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferID);
        
        
//        GLfloat mesh_floats[mesh_vertices_count];
//
//        for(int i=0; i<mesh_vertices_count/3; i++){
//            mesh_floats[i*3+0] = mesh_vertices[i][0];
//            mesh_floats[i*3+1] = mesh_vertices[i][1];
//            mesh_floats[i*3+2] = mesh_vertices[i][2];
//        }
//        for (int i = 0; i < mesh_vertices_count; i++)
//            mesh_floats[i] *=2;
//
//
//
//        glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_floats), mesh_floats, GL_STATIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        vector<vec3> pos;
        vector<unsigned int> imat;
        root->write_to_VBOs(vec3(0, 0, 0), pos, imat);
        size_stick = pos.size();
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pos.size(), pos.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        //indices of matrix:
        glGenBuffers(1, &VertexBufferIDimat);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimat);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imat.size(), imat.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);

        
        glBindVertexArray(0);
	}
	
	void init(const std::string& resourceDirectory) {
		GLSL::checkVersion();

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
        
        
        skeleton = std::make_shared<Program>();
        skeleton->setVerbose(true);
        skeleton->setShaderNames(resourceDirectory + "/skeleton.vert", resourceDirectory + "/skeleton.frag");
        if (!skeleton->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
       
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
		glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks.
		glm::mat4 V, M, P;
        P = getPerspectiveMatrix();
        V = camera->getViewMatrix();
        M = glm::mat4(1);

        /**************/
        /* DRAW SKELETON */
        /**************/
        
        skeleton->bind();
        //send the matrices to the shaders
        glBindVertexArray(VertexArrayID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferID);

        glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -8.0f));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
        M = TransZ * S;
        skeleton->setMVP(&M[0][0], &V[0][0], &P[0][0]);
        skeleton->setMatrixArray("Manim", 200, &animmat[0][0][0]);
    
        glDrawArrays(GL_LINES, 4, size_stick-4);
        glBindVertexArray(0);
        skeleton->unbind();

        
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
