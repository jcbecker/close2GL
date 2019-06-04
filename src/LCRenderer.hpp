// Middle layer to choose the renderer.

// Since we are implementing a second renderer, we will
// create this middle layer to choose the renderer

#ifndef L2CR
#define L2CR

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Renderer {
    CLOSE2GL,
    OPENGL,
    CLOSE2GLRASTERIZATION
};

class RendererLayer{
public:
    Renderer choice;
    float znear;
    float zfar;

    RendererLayer(){
        this->znear = 1.0f;
        this->zfar = 30.0f;
        this->choice = OPENGL;
    }

    // glm::mat4 getProjectionMatrix(){

    // }
};


#endif