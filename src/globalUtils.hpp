#ifndef C2GLRG
#define C2GLRG

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Close2GLVertex {
    glm::vec2 Position;
    glm::vec3 Normal;
    int colorIndex;
};

struct RasterizerVertex {
    glm::vec2 Position;
};



#endif