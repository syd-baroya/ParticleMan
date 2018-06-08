#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;
in vec3 mypos;

uniform vec3 camPos;
uniform sampler2D tex;


void main()
{
    vec3 normal = normalize(fragNor);
    //vec2 texcoord = 10 * vec2(fragTex.x, fragTex.y);
    vec2 texcoord =  vec2(fragTex.x - camPos.x/100, fragTex.y - camPos.z/100);
    
    vec3 texcolor = texture(tex, texcoord).rgb;
    
    
    vec3 lp = vec3(100,100,100);
    vec3 ld = normalize(lp - pos);
    float light = length(ld);
    
    //texcolor = 0.5 * pos + 0.5 * texcolor;
    //color.rgb = texcolor * light * 0.4 + 0.6 * vec3(1,1,1);
    //color.rgb = texcolor;
    texcolor = 0.5 * (mypos * vec3(0.1,0.0,0.1)) + 0.5 * texcolor;
    color.rgb = texcolor * light * 0.4;
    //color.rgb = mypos;
    
    if(mypos.y > 0.5)
    {
        color.a = 0.1;
    }
    else if(mypos.y == 0)
    {
        color.a = 0.5;
    }
    else
    {
        color.a = 1;
    }
    
}
