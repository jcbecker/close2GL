#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightingColor;

uniform vec3 lightPos;
uniform vec3 lcolor;
uniform vec3 viewPos;
uniform bool useLight;
uniform bool isGouraud;
uniform bool gouraudSpecular;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;

void main(){
    gl_Position = mvp * vec4(aPos, 1.0);

    if(useLight && isGouraud){
        vec3 Position = vec3(model * vec4(aPos, 1.0));
        vec3 cNormal = mat3(transpose(inverse(model))) * aNormal;
        
        // ambient
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lcolor;
        
        // diffuse 
        vec3 norm = normalize(cNormal);
        vec3 lightDir = normalize(lightPos - Position);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lcolor;
        
        // specular
        float specularStrength = 0.8;
        if (!gouraudSpecular){
            specularStrength = 0.0;
        }
        vec3 viewDir = normalize(viewPos - Position);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lcolor;      

        LightingColor = ambient + diffuse + specular;
    }else{
        LightingColor = vec3(1, 1, 1);
    }

    Normal = mat3(transpose(inverse(model))) * aNormal; 
    FragPos = vec3(model * vec4(aPos, 1.0));
}