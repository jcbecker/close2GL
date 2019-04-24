#version 450 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;

void main(){
    gl_Position = aPos;
}