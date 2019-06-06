#ifndef C2GLRG
#define C2GLRG

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define AFTER_FAR_PLANE 9999999.0f

struct Close2GLVertex {
    glm::vec2 Position;
    glm::vec3 Normal;
    int colorIndex;
};

struct RasterizerVertex {
    glm::vec4 Position;
    glm::vec3 Normal;
    glm::vec3 OriginalPos;
    glm::vec2 TexCoords;
};

struct InRasterizerVertex {
    glm::vec4 Position;
    glm::vec3 Normal;
    glm::vec4 Color;
    glm::vec3 OriginalPos;
    glm::vec2 TexCoords;
};

struct TextureStruct{
    unsigned char *data;
    int width, height, channels;
    unsigned int ID;
    GLenum addres;
};



#endif