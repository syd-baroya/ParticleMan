#version  330 core
in vec4 vertPos;
in vec3 vertNor;
in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;
out vec3 pos;
out vec3 lightPos;

void main()
{
    pos=vertPos.xyz;
    gl_Position = P * V * M * vertPos;
    fragNor = (M * vec4(vertNor, 0.0)).xyz;
    fragPos = vec4(V * M * vec4(vertPos)).xyz;
    fragTex = vertTex;
    lightPos = vec3(V * vec4(100, 100, 100, 1));

}
