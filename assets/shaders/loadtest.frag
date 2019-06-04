#version 450 core

in vec3 Normal;
in vec3 FragPos;
in vec3 LightingColor;
in vec2 TexCoord;
// in vec3 LightPosViewSpace;

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 lcolor;
uniform vec3 lightPos;
// uniform vec3 viewPos;
uniform bool useLight;
uniform bool isGouraud;
uniform bool hasTexCoords;
uniform bool useTexturesFlag;
uniform sampler2D mandrilTexture;

void main(){
    vec3 rawColor = uColor;
    if(hasTexCoords && useTexturesFlag){
        rawColor = vec3(texture(mandrilTexture, TexCoord));
    }
    if(useLight && !isGouraud){
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lcolor;

        
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lcolor;

        float specularStrength = 0.5;
        vec3 viewDir = normalize(- FragPos);
        vec3 reflectDir = reflect(-lightDir, norm); 

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lcolor; 


        vec3 result = (ambient + diffuse + specular) * rawColor;
        FragColor = vec4(result, 1.0);
    }
    if (!useLight){
        FragColor = vec4(rawColor, 1.0);
    }
    if(useLight && isGouraud){
        FragColor = vec4(LightingColor * rawColor, 1.0);
    }

}