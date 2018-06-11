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
    
    if(texcolor.r < 0.1)// && texcolor.g > 0.9 && texcolor.b > 0.9)
        color.a = 0;
    else
        color.a = fade;
    
    //color.rgb = vec3(texcolor.r,texcolor.g,0);
    color.r = 4 * (1.0 - fade) ;
    color.g = fade;
    color.b = texcolor.g;
    
    
    //color = vec4(0,0,0,0.5);
}
