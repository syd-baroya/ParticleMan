#version 330 core
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;
in vec3 fragPos;
in vec3 lightPos;

uniform sampler2D tex;
uniform float fade;
uniform int switch_particles;
uniform float t;
uniform vec2 offset1;
uniform vec2 offset2;

void main()
{
    if(switch_particles==0){
        vec3 normal = normalize(fragNor);
        vec3 texcolor = texture(tex, fragTex).rgb;
    
        if(texcolor.r < 0.1)// && texcolor.g > 0.9 && texcolor.b > 0.9)
            color.a = 0;
        else
            color.a = fade;
    
        //color.rgb = vec3(texcolor.r,texcolor.g,0);
        color.r = 4 * (1.0 - fade);
        if(fade < 0.5) color.g = fade;
        else color.g = 0.5 - fade;
        color.b = texcolor.g;
    
    
        //color = vec4(0,0,0,0.5);
    }
    else if(switch_particles==1){
        vec3 lightDir = normalize(lightPos - fragPos);
        vec3 viewDir = normalize(-fragPos);
        vec3 normal = normalize(fragNor);
        
        // Ambient calculation
        vec3 ambientColor = vec3(0);
        
        // Diffuse calculation
        //    float diffuseFactor = max(dot(lightDir, normal), 0); // Clamp to prevent color from reaching back side
        float diffuseFactor = (dot(lightDir, normal) + 1) / 2; // Normalize so the color bleeds onto the back side
        vec3 diffuseColor = vec3(0.6, 0.6, 0.4) * diffuseFactor;
        
        // Specular calculation
        float specularStrength = 0.5f;
        vec3 reflectDir = reflect(-lightDir, normal);
        float shininess = 3.0f;
        float specularFactor = pow(max(dot(viewDir, reflectDir), 0), shininess);
        vec3 specularColor = vec3(1) * specularFactor * specularStrength;
        
//        vec4 texcolor = texture(tex, fragTex);
        vec2 temp_vtex1;
        temp_vtex1.x = fragTex.x/5;
        temp_vtex1.y = fragTex.y/4;
        
        vec2 temp_vtex2;
        temp_vtex2.x = (fragTex.x + 1) / 5;
        temp_vtex2.y = (fragTex.y + 1 )/ 4;
        
        vec4 tcol1 = texture(tex, temp_vtex1+offset1);
        vec4 tcol2 = texture(tex, temp_vtex2+offset2);
        vec4 texcolor = tcol2*(t) + tcol1*(1.0-t);
        
        color = vec4(diffuseColor + specularColor, 1) + texcolor;
        if(texcolor.r < 0.1)
            color.a = 0;
        else
            color.a = fade;
    }
}
