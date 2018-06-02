#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;


uniform sampler2D tex;
uniform float fade;

void main()
{
    vec3 normal = normalize(fragNor);
    vec3 texcolor = texture(tex, fragTex).rgb;
    
    color.rgb = texcolor;
    
    color.a = fade;
    
    //color = vec4(0,0,0,0.5);
}
