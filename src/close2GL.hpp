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
        vp[3][0] = ((float)w/(float)2);// + 1.0f;// + 0.0f ?
        vp[3][1] = ((float)h/(float)2);// + 1.0f;

        return vp;
    }

    // Struct for the 2d pannel with texture (immutable)
    struct Close2GLRenderPanelVertex {
        glm::vec2 Position;
        glm::vec2 TextureCoord;
    };

    float areaOfTriangle(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2){
        float a = v0.x * v1.y - v1.x * v0.y;
             a += v1.x * v2.y - v2.x * v1.y;
             a += v2.x * v0.y - v0.x * v2.y;
        
        return a * 0.5f;
    }


    class Close2GlRender{
    public:
        GLuint C2GLRVAO, C2GLRVBO;
        std::vector<Close2GLRenderPanelVertex> verticesp;
        unsigned int textureUniform;
        Shader m_Shader;
        
        //{GL_POINT, GL_LINE, GL_FILL}
        GLuint rasterizePrimitive;

        GLuint faceCulling;
        GLuint orientationMode;
        bool backFaceCullingFlag;
        
        std::vector<Close2GLVertex> giseleVertices;
        std::vector<Close2GLVertex> cubeVertices;

        int scrW, scrH;

        glm::vec4 mClearColor;
        glm::vec4 mObjectColor;

        std::vector<glm::vec4> mColorBuffer;
        std::vector<float> mZBuffer;

        std::vector<RasterizerVertex> verticeStack;



        Close2GlRender(Shader p_Shader, int i_scrW, int i_scrH, glm::vec4 i_clearColor, glm::vec4 i_objectColor) : m_Shader(p_Shader){
            rasterizePrimitive = GL_FILL;

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
            

            GLuint faceCulling = GL_BACK;
            GLuint orientationMode = GL_CCW;
            bool backFaceCullingFlag = false;
            
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
                    this->mColorBuffer.push_back(this->mClearColor);
                    this->mZBuffer.push_back(AFTER_FAR_PLANE);
                }
            }

            verticeStack = std::vector<RasterizerVertex>();
            
            

            generateClose2GLVAOVBO();
        }

        void line(RasterizerVertex v0, RasterizerVertex v1){
            int x0 = (int) v0.Position.x;
            int y0 = (int) v0.Position.y;

            int x1 = (int) v1.Position.x;
            int y1 = (int) v1.Position.y;

            bool steep = false; 
            if (std::abs(x0-x1)<std::abs(y0-y1)) { 
                std::swap(x0, y0); 
                std::swap(x1, y1); 
                steep = true; 
            } 
            if (x0>x1) { 
                std::swap(x0, x1); 
                std::swap(y0, y1); 
            } 
            int dx = x1-x0; 
            int dy = y1-y0; 
            int derror2 = std::abs(dy)*2; 
            int error2 = 0; 
            int y = y0; 
            for (int x=x0; x<=x1; x++) { 
                if (steep) {
                    setPixelColor(y, x,  this->mObjectColor);
                } else {
                    setPixelColor(x, y,  this->mObjectColor);
                } 
                error2 += derror2; 
                if (error2 > dx) { 
                    y += (y1>y0?1:-1); 
                    error2 -= dx*2; 
                } 
            } 
        } 

        void vertice2RasterizerStack(RasterizerVertex v){
            if(rasterizePrimitive == GL_FILL){            
                verticeStack.push_back(v);
                if (verticeStack.size() == 3){
                    if(!CullFaceStack()){
                        rasterizeTriangle();
                    }
                    // To clear the stack
                    verticeStack = std::vector<RasterizerVertex>();
                }
            }else if(rasterizePrimitive == GL_POINT){
                setPixelColor(v.Position.x, v.Position.y, this->mObjectColor);
            }else{
                // setPixelColor(v.Position.x, v.Position.y,  this->mObjectColor);
                verticeStack.push_back(v);
                if (verticeStack.size() == 3){
                    if(!CullFaceStack()){
                        line(verticeStack[0], verticeStack[1]);
                        line(verticeStack[1], verticeStack[2]);
                        line(verticeStack[2], verticeStack[0]);
                    }
                    // To clear the stack
                    verticeStack = std::vector<RasterizerVertex>();
                }
            }

        }

        bool CullFaceStack(){
            if (!backFaceCullingFlag) return false;
            float a = areaOfTriangle(glm::vec2(verticeStack[0].Position), glm::vec2(verticeStack[1].Position), glm::vec2(verticeStack[2].Position));
            if (orientationMode == GL_CCW){
                return a < 0.0f;
            }
            else{
                return !(a < 0.0f);
            }
        }

        void rasterizeTriangle(){
            // Sorting vertices by height
            glm::vec4 v0 = verticeStack[0].Position;
            glm::vec4 v1 = verticeStack[1].Position;
            glm::vec4 v2 = verticeStack[2].Position;
            
            v0.x = (int) v0.x;
            v0.y = (int) v0.y;

            v1.x = (int) v1.x;
            v1.y = (int) v1.y;

            v2.x = (int) v2.x;
            v2.y = (int) v2.y;

            if (v0.y == v1.y && v0.y == v2.y) return;

            if (v0.y > v1.y) std::swap(v0, v1);
            if (v0.y > v2.y) std::swap(v0, v2);
            if (v1.y > v2.y) std::swap(v1, v2);
            
            int total_height = v2.y - v0.y;

            for (int i=0; i<total_height; i++) {
                bool second_half = i > v1.y-v0.y || v1.y==v0.y;
                int segment_height = second_half ? v2.y - v1.y : v1.y-v0.y;
                float alpha = (float)i/total_height;
                float beta  = (float)(i-(second_half ? v1.y-v0.y : 0))/segment_height;
                glm::vec4 A =               v0 + (v2 - v0)*alpha;
                glm::vec4 B = second_half ? v1 + (v2 - v1)*beta : v0 + (v1-v0)*beta;
                if (A.x > B.x) std::swap(A, B);
                for (int j=A.x; j<=B.x; j++) {
                    if(getPixelDeph(j, v0.y+i) > A.z){
                        setPixelDeph(j, v0.y+i, A.z);
                        setPixelColor(j, v0.y+i,  this->mObjectColor);
                    }
                }
            }
            
        }

        void setPrimitiveToRasterize(GLuint nrm){
            this->rasterizePrimitive = nrm;
        }

        void setPixelColor(int x, int y, glm::vec4 color){
            if(x < 0 || x >= this->scrW || y < 0 || y >= this->scrH){
                // std::cout << ".";
                return;
            }
            this->mColorBuffer[x + this->scrW * y] = color;
        }

        void updateClearColor(glm::vec4 iic){
            this->mClearColor = iic;
        }

        void testAndResizeBuffers(int i_scrW, int i_scrH){
            if (this->scrW == i_scrW && this->scrH == i_scrH){//No resize buffer is needed
                return;
            }

            std::cout << "Alert: Resizing buffers to " << i_scrW << ", " << i_scrH << ".\n";

            this->scrW = i_scrW;
            this->scrH = i_scrH;

            this->mColorBuffer = std::vector<glm::vec4>();
            this->mColorBuffer.reserve(this->scrW * this->scrH);

            this->mZBuffer = std::vector<float>();
            this->mZBuffer.reserve(this->scrW * this->scrH);

            for (int i = 0; i < this->scrH; i++) {
                for (int j = 0; j < this->scrW; j++) {
                    this->mColorBuffer.push_back(this->mClearColor);
                    this->mZBuffer.push_back(AFTER_FAR_PLANE);
                }
            }

            verticeStack = std::vector<RasterizerVertex>();

            glBindTexture(GL_TEXTURE_2D, this->textureUniform);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->scrW, this->scrH, 0, GL_RGBA,  GL_FLOAT, this->mColorBuffer.data());

        }

        void updateObjectColor(glm::vec4 i_oc){
            this->mObjectColor = i_oc;
        }

        void clearTextureColor(){
            for (int i = 0; i < this->scrH; i++) {
                for (int j = 0; j < this->scrW; j++) {
                    setPixelColor(j, i, mClearColor);
                }
            }
            verticeStack = std::vector<RasterizerVertex>();
        }

        void setPixelDeph(int x, int y, float dephV){
            if(x < 0 || x >= this->scrW || y < 0 || y >= this->scrH){
                // std::cout << ".";
                return;
            }
            this->mZBuffer[x + this->scrW * y] = dephV;
        }

        float getPixelDeph(int x, int y){
            if(x < 0 || x >= this->scrW || y < 0 || y >= this->scrH){
                std::cout << "Error: Wrong screen Cordinate.\n\n";
                exit(EXIT_FAILURE);
            }
            return this->mZBuffer[x + this->scrW * y];
        }

        void clearZBufferC2GL(){
            for (int i = 0; i < this->scrH; i++) {
                for (int j = 0; j < this->scrW; j++) {
                    setPixelDeph(j, i, AFTER_FAR_PLANE);
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
            glBindTexture(GL_TEXTURE_2D, this->textureUniform);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->scrW, this->scrH, 0, GL_RGBA,  GL_FLOAT, this->mColorBuffer.data());
            glGenerateMipmap(GL_TEXTURE_2D);


            this->m_Shader.use();
            this->m_Shader.setInt("texture1", 0);
        }

        void updateTextureInGPU(){
            glBindTexture(GL_TEXTURE_2D, this->textureUniform);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->scrW, this->scrH, GL_RGBA, GL_FLOAT, this->mColorBuffer.data());
        }

        void rasterize(std::vector<RasterizerVertex> vertices){
            unsigned int vs = vertices.size();
            float tx, ty;
            // int ix, iy;
            RasterizerVertex aav;
            for(int i = 0; i <  vs; i++){
                tx = vertices[i].Position.x;
                ty = vertices[i].Position.y;
                aav.Position = glm::vec4(tx, ty, vertices[i].Position.z, vertices[i].Position.w);
                vertice2RasterizerStack(aav);
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