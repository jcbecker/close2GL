// Middle layer to choose the renderer.

// Since we are implementing a second renderer, we will
// create this middle layer to choose the renderer

#ifndef L2CR
#define L2CR

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum RendererChoices {
    CLOSE2GL,
    OPENGL
};

class RendererLayer{
public:
    RendererChoices choice;
    float znear;
    float zfar;

    RendererLayer(){
        this->znear = 1.0f;
        this->zfar = 3000.0f;
    }

    // glm::mat4 getProjectionMatrix(){

    // }
};


#endif