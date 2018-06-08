#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;


uniform sampler2D tex;


void main()
{
    vec3 normal = normalize(fragNor);
    vec2 texcoord = 2 * vec2(fragTex.x, fragTex.y);
    vec3 texcolor = texture(tex, texcoord).rgb;
    
    
    vec3 lp = vec3(100,100,100);
    vec3 ld = normalize(lp - pos);
    float light = length(ld);
    
    //    if(texcolor.r < 0.4f)
    //    {
    //        texcolor.rgb = pos * vec3(0.5, 0.3, 1.0);
    //        color.rgb = texcolor * light * 0.5 + 0.5 * vec3(1,1,1);
    //    }
    //    else
    //    {
    //        texcolor.rgb = pos * vec3(0.5, 0.3, 1.0);
    //        color.rgb = texcolor * light * 0.4 + 0.6 * vec3(1,1,1);
    //    }
    
    
    //if(pos.x > 1)
    //{
    //    texcolor = pos;
    //}
    //else
    //{
    //    texcolor = 0.5 * pos + 0.5 * texcolor;
    //}
    
    texcolor = 0.5 * pos + 0.5 * texcolor;
    color.rgb = texcolor * light * 0.4 + 0.6 * vec3(1,1,1);
    
    color.a = 1;
    
}
