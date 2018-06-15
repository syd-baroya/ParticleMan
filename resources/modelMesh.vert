#version 410 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in ivec4 vertImat;
layout(location = 2) in vec4 vertWeights;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 Manim[200];
uniform vec3 campos;
void main()
{
    float w0 = vertWeights.x;
    float w1 = vertWeights.y;
    float w2 = vertWeights.z;
    float w3 = vertWeights.w;
    
    uint wi0 = uint(vertImat.x);
    uint wi1 = uint(vertImat.y);
    uint wi2 = uint(vertImat.z);
    uint wi3 = uint(vertImat.w);
    mat4 Mw0 = (Manim[wi0]);
    mat4 Mw1 = (Manim[wi1]);
    mat4 Mw2 = (Manim[wi2]);
    mat4 Mw3 = (Manim[wi3]);
    
    vec4 pos;

    float sum = w0 + w1 + w2 + w3;
    w0 = w0 / sum;
    w1 = w1 / sum;
    w2 = w2 / sum;
    w3 = w3 / sum;

//    vec3 vm0 = vec3(Mw0[3][0], Mw0[3][1], Mw0[3][2]);
//    vec3 vm1 = vec3(Mw1[3][0], Mw1[3][1], Mw1[3][2]);
//    vec3 vm2 = vec3(Mw2[3][0], Mw2[3][1], Mw2[3][2]);
//    vec3 vm3 = vec3(Mw3[3][0], Mw3[3][1], Mw3[3][2]);
//
//    Mw0[3][0] = 0; Mw0[3][1] = 0; Mw0[3][2] = 0;
//    Mw1[3][0] = 0; Mw1[3][1] = 0; Mw1[3][2] = 0;
//    Mw2[3][0] = 0; Mw2[3][1] = 0; Mw2[3][2] = 0;
//    Mw3[3][0] = 0; Mw3[3][1] = 0; Mw3[3][2] = 0;
    
//    Mw0[3][0] -= vertPos.x; Mw0[3][1] -= vertPos.y; Mw0[3][2] -= vertPos.z;
//    Mw1[3][0] -= vertPos.x; Mw1[3][1] -= vertPos.y; Mw1[3][2] -= vertPos.z;
//    Mw2[3][0] -= vertPos.x; Mw2[3][1] -= vertPos.y; Mw2[3][2] -= vertPos.z;
//    Mw3[3][0] -= vertPos.x; Mw3[3][1] -= vertPos.y; Mw3[3][2] -= vertPos.z;

//    Mw0 = inverse(Mw0);
//    Mw1 = inverse(Mw1);
//    Mw2 = inverse(Mw2);
//    Mw3 = inverse(Mw3);
//    Mw0 = transpose(Mw0);
//    Mw1 = transpose(Mw1);
//    Mw2 = transpose(Mw2);
//    Mw3 = transpose(Mw3);

//    vec4 dp0 = (Mw0 * vec4(vertPos, 1.0)) - vec4(vertPos, 1.0);
//    vec4 dp1 = (Mw1 * vec4(vertPos, 1.0)) - vec4(vertPos, 1.0);
//    vec4 dp2 = (Mw2 * vec4(vertPos, 1.0)) - vec4(vertPos, 1.0);
//    vec4 dp3 = (Mw3 * vec4(vertPos, 1.0)) - vec4(vertPos, 1.0);
//    pos = vec4(vertPos, 1.0) + dp0 * w0 + dp1 * w1 + dp2 * w2 + dp3 * w3;
//    pos.xyz = vertPos.xyz + ( (w0 * vm0) +
//                             (w1 * vm1) +
//                             (w2 * vm2) +
//                             (w3 * vm3) );
//
//   pos = vec4(pos.xyz, 1.0) * (w0 * Mw0 + w1 * Mw1 + w2 * Mw2 + w3 * Mw3);
//

    
    pos.x = vertPos.x + ( (w0 * Mw0[3][0]) +
                          (w1 * Mw1[3][0]) +
                          (w2 * Mw2[3][0]) +
                          (w3 * Mw3[3][0]) );

    pos.y = vertPos.y + ( (w0 * Mw0[3][1]) +
                          (w1 * Mw1[3][1]) +
                          (w2 * Mw2[3][1]) +
                          (w3 * Mw3[3][1]) );

    pos.z = vertPos.z + ( (w0 * Mw0[3][2]) +
                          (w1 * Mw1[3][2]) +
                          (w2 * Mw2[3][2]) +
                          (w3 * Mw3[3][2]) );

    gl_Position = P * V * M * vec4(pos.xyz, 1);
    
}

