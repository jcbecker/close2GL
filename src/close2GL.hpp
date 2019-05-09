#ifndef CLOSE2GL_LIB
#define CLOSE2GL_LIB

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaderLoader.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

#include <vector>

namespace C2GL{
    glm::mat4 translate(glm::mat4 model, glm::vec3 dTranslate){
        glm::mat4 mTranslate = glm::mat4(1.0f);
        mTranslate[3][0] = dTranslate.x;
        mTranslate[3][1] = dTranslate.y;
        mTranslate[3][2] = dTranslate.z;

        return model * mTranslate;
    }

    glm::mat4 scale(glm::mat4 model, glm::vec3 dScale){
        glm::mat4 mScale = glm::mat4(1.0f);
        mScale[0][0] = dScale.x;
        mScale[1][1] = dScale.y;
        mScale[2][2] = dScale.z;

        return model * mScale;
    }

    glm::mat4 frustum(float l, float r, float b, float t, float n, float f){
        glm::mat4 proj = glm::mat4(1.0f);
        proj[0][0] = 2.0f * n  / (r - l);
        proj[1][1] = 2.0f * n  / (t - b);
        proj[2][2] = ((f + n) / (f - n)) * -1.0f;
        proj[3][2] = ((2.0f * f * n) / (f - n)) * -1.0f;
        proj[2][3] = -1.0f;
        proj[3][3] = 0.0f;

        proj[2][0] = (r + l) / (r - l);
        proj[2][1] = (t + b) / (t - b);
        return proj;
    }

    // Struct for the 2d pannel with texture (immutable)
    struct Close2GLRenderPanelVertex {
        glm::vec2 Position;
        glm::vec2 TextureCoord;
    };

    class Close2GlRender{
    public:
        GLuint C2GLRVAO, C2GLRVBO;
        std::vector<Close2GLRenderPanelVertex> verticesp;
        unsigned int textureUniform;
        Shader m_Shader;

        Close2GlRender(Shader p_Shader) : m_Shader(p_Shader){
            Close2GLRenderPanelVertex aav;
            aav.Position = glm::vec2(-1.0f, -1.0f);
            aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(0.0f, 0.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(1.0f, -1.0f);
            aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(1.0f, 0.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(1.0f, 1.0f);
            aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(1.0f, 1.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(1.0f, 1.0f);
            aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(1.0f, 1.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(-1.0f, 1.0f);
            aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(0.0f, 1.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(-1.0f, -1.0f);
            aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(0.0f, 0.0f);
            verticesp.push_back(aav);

            generateClose2GLVAOVBO();
        }

        void generateClose2GLVAOVBO(){
            glGenVertexArrays(1, &this->C2GLRVAO); 
            glGenBuffers(1, &this->C2GLRVBO);


            glBindVertexArray(this->C2GLRVAO);

            glBindBuffer(GL_ARRAY_BUFFER, this->C2GLRVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Close2GLRenderPanelVertex) * this->verticesp.size(),  &this->verticesp[0], GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Close2GLRenderPanelVertex), (void*)0);
            glEnableVertexAttribArray(0);

            // Normal attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Close2GLRenderPanelVertex), (void*)offsetof(Close2GLRenderPanelVertex, TextureCoord));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);


            glGenTextures(1, &this->textureUniform);
            glBindTexture(GL_TEXTURE_2D, this->textureUniform); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            unsigned int width, height;
            width = height = 512;
            float checkImage[height][width][4];
            float ri, gi, bi;

            for (int i = 0; i < height; i++) {//checkImageHeight
                for (int j = 0; j < width; j++) {//checkImageWidth
                    ri = (i*j)/(float)(512*512);
                    gi = i/(float) 512;
                    bi = j/(float) 512;
                    checkImage[i][j][0] = (float) gi;
                    checkImage[i][j][1] = (float) gi;
                    checkImage[i][j][2] = (float) gi;
                    checkImage[i][j][3] = (float) 1.0f;
                }
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,  GL_FLOAT, checkImage);
            glGenerateMipmap(GL_TEXTURE_2D);
            

            this->m_Shader.use();
            this->m_Shader.setInt("texture1", 0);
        }

        // void setShader(Shader * p_shader){
        //     this->m_Shader = Shader(p_shader);
        // }

        void draw(){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->textureUniform);
            
            this->m_Shader.use();
            // std::cout << "shader value: " << this->m_Shader.ID << ":::\n\nn";
            glBindVertexArray(this->C2GLRVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

    };
}

#endif