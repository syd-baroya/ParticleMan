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
    mat4 Mw0 = Manim[wi0];
    mat4 Mw1 = Manim[wi1];
    mat4 Mw2 = Manim[wi2];
    mat4 Mw3 = Manim[wi3];
    
    vec4 pos = vec4(1.0);
    float best = w0;
    float worst = w0;
    mat4 bestmat = Mw0;
    
//    if(w0 > w1 && w0 > w2 && w0 > w3){  w0 = 1; }
//    if(w1 > w0 && w1 > w2 && w1 > w3){  w1 = 1; }
//    if(w2 > w0 && w2 > w1 && w2 > w3){  w2 = 1; }
//    if(w3 > w0 && w3 > w1 && w3 > w2){  w3 = 1; }
//
//    if(w0 < w1 && w0 < w2 && w0 < w3){  w0 = 0; }
//    if(w1 < w0 && w1 < w2 && w1 < w3){  w1 = 0; }
//    if(w2 < w0 && w2 < w1 && w2 < w3){  w2 = 0; }
//    if(w3 < w0 && w3 < w1 && w3 < w2){  w3 = 0; }

    if(w1 > best)
    {
        best = w1;
        bestmat = Mw1;
    }
    if(w2 > best)
    {
        best = w2;
        bestmat = Mw2;
    }
    if(w3 > best)
    {
        best = w3;
        bestmat = Mw3;
    }
    
    if(w1 < worst)
    {
        worst = w1;
    }
    if(w2 < worst)
    {
        worst = w2;
    }
    if(w3 < worst)
    {
        worst = w3;
    }
    
//    w0 = ((w0 - worst) / (best - worst));
//    w1 = ((w1 - worst) / (best - worst));
//    w2 = ((w2 - worst) / (best - worst));
//    w3 = ((w3 - worst) / (best - worst));
//

    float sum = w0 + w1 + w2 + w3;
    w0 = w0 / sum;
    w1 = w1 / sum;
    w2 = w2 / sum;
    w3 = w3 / sum;

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

    gl_Position = P * V * M * pos;
    
}





//#version 410 core
//layout(location = 0) in vec3 vertPos;
//layout(location = 1) in int vertimat;
//
//uniform mat4 P;
//uniform mat4 V;
//uniform mat4 M;
//uniform mat4[200] Manim;
//
//void main() {
//    mat4 Ma = Manim[vertimat];
//    vec4 pos = Ma*vec4(vertPos,1.0);
//
//    //the animation matrix already holds the end position for the segment
//    pos.x = Ma[3][0];
//    pos.y = Ma[3][1];
//    pos.z = Ma[3][2];
//    gl_Position = P * V * M * pos;
//}
