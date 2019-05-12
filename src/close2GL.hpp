#ifndef CLOSE2GL_LIB
#define CLOSE2GL_LIB

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaderLoader.hpp"
#include "globalUtils.hpp"

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

    glm::mat4 getViewPortMatrix (unsigned int w, unsigned int h){
        // [w/2   0   w/2+x]
        // [ 0   h/2  h/2+y]
        // [ 0    0     1  ]
        glm::mat4 vp = glm::mat4(1.0f);
        vp[0][0] = (float)w/(float)2;
        vp[1][1] = (float)h/(float)2;
        vp[3][0] = ((float)w/(float)2) + 1.0f;
        vp[3][1] = ((float)h/(float)2) + 1.0f;

        return vp;
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
        
        std::vector<Close2GLVertex> giseleVertices;
        std::vector<Close2GLVertex> cubeVertices;

        int scrW, scrH;

        glm::vec4 mClearColor;
        glm::vec4 mObjectColor;

        std::vector<glm::vec4> mColorBuffer;
        std::vector<float> mZBuffer;




        Close2GlRender(Shader p_Shader, int i_scrW, int i_scrH, glm::vec4 i_clearColor, glm::vec4 i_objectColor) : m_Shader(p_Shader){
            Close2GLRenderPanelVertex aav;
            aav.Position = glm::vec2(-1.0f, -1.0f);
            // aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(0.0f, 0.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(1.0f, -1.0f);
            // aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(1.0f, 0.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(1.0f, 1.0f);
            // aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(1.0f, 1.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(1.0f, 1.0f);
            // aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(1.0f, 1.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(-1.0f, 1.0f);
            // aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(0.0f, 1.0f);
            verticesp.push_back(aav);

            aav.Position = glm::vec2(-1.0f, -1.0f);
            // aav.Position = aav.Position * 0.5f;
            aav.TextureCoord = glm::vec2(0.0f, 0.0f);
            verticesp.push_back(aav);

            this->scrW = i_scrW;
            this->scrH = i_scrH;

            this->mClearColor = i_clearColor;
            this->mObjectColor = i_objectColor;

            this->mColorBuffer = std::vector<glm::vec4>();
            this->mColorBuffer.reserve(this->scrW * this->scrH);

            this->mZBuffer = std::vector<float>();
            this->mZBuffer.reserve(this->scrW * this->scrH);

            for (int i = 0; i < this->scrH; i++) {
                for (int j = 0; j < this->scrW; j++) {
                    float rc = mClearColor.r;
                    float gc = mClearColor.g;
                    float bc = mClearColor.b;
                    mClearColor.r;
                    this->mColorBuffer.push_back(glm::vec4(rc, gc, bc, 1.0f));
                }
            }
            
            setPixelColor(this->scrW-1, this->scrH-1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            

            generateClose2GLVAOVBO();
        }

        void setPixelColor(int x, int y, glm::vec4 color){
            this->mColorBuffer[x + this->scrW * y] = color;
        }

        void clearTextureColor(){
            for (int i = 0; i < this->scrH; i++) {
                for (int j = 0; j < this->scrW; j++) {
                    setPixelColor(j, i, mClearColor);
                }
            }
        }

        void setTextureColor(glm::vec4 nColor){
            for (int i = 0; i < this->scrH; i++) {
                for (int j = 0; j < this->scrW; j++) {
                    setPixelColor(j, i, nColor);
                }
            }
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

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->scrW, this->scrH, 0, GL_RGBA,  GL_FLOAT, this->mColorBuffer.data());
            glGenerateMipmap(GL_TEXTURE_2D);


            this->m_Shader.use();
            this->m_Shader.setInt("texture1", 0);
        }

        void updateTextureInGPU(){
            glBindTexture(GL_TEXTURE_2D, this->textureUniform);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->scrW, this->scrH, GL_RGBA, GL_FLOAT, this->mColorBuffer.data());
        }

        void rasterize(std::vector<Close2GLVertex> vertices){
            unsigned int vs = vertices.size();
            float tx, ty;
            int ix, iy;
            for(int i = 0; i <  vs; i++){
                tx = vertices[i].Position.x;
                ty = vertices[i].Position.y;
                tx += 1.0f;
                ty += 1.0f;
                tx = tx * ((float) this->scrW  / (float) 2);
                ty = ty * ((float) this->scrH  / (float) 2);
                ix = tx;
                iy = ty;
                if(ix > 0 && ix < this->scrW && iy > 0 && iy < this->scrH){
                    setPixelColor(ix, iy, this->mObjectColor);
                }else{
                    std::cout << "Alert: Pixel out of range\n\n";
                }
            }

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