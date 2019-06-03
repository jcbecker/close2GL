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

        // uniform vec3 lightPos;
        glm::vec3 lightPosition; // Light position in CCS

        // uniform vec3 lcolor;
        glm::vec4 lightColor;
        
        // uniform bool useLight;
        bool useLight = false;
        
        // uniform bool isGouraud;
        bool isGouraud;

        // uniform bool gouraudSpecular;
        bool gouraudSpecular = true;

        // uniform mat4 model;
        glm::mat4 model;
        
        // uniform mat4 view;
        glm::mat4 view;
        
        // uniform mat4 projection;
        glm::mat4 projection;


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

        // Receive a vertex, shade his color and return the same vertex

        // To-do calculate matrix multiplication out of virtual shader
        InRasterizerVertex vertexShading(InRasterizerVertex v){
            //Calculate Gouraud Shading
            if(this->useLight && this->isGouraud){
                v.OriginalPos = glm::vec3(this->view  * this->model * glm::vec4(v.OriginalPos, 1.0f));

                // ambient
                float ambientStrength = 0.3;
                glm::vec3 ambient = ambientStrength * this->lightColor;

                // diffuse 
                v.Normal = glm::normalize(v.Normal);
                glm::vec3 lightDir = glm::normalize(this->lightPosition - v.OriginalPos);
                float diff = glm::max(glm::dot(v.Normal, lightDir), 0.0f);
                glm::vec3 diffuse = diff * this->lightColor;

                // specular
                float specularStrength = 0.8;
                if(!(this->gouraudSpecular)){
                    specularStrength = 0.0;
                }
                glm::vec3 viewDir = glm::normalize( - v.OriginalPos);
                glm::vec3 reflectDir = glm::reflect(-lightDir, v.Normal);
                float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), 32);
                glm::vec3 specular = specularStrength * spec * this->lightColor;

                glm::vec3 LightingColor = ambient + diffuse + specular;

                v.Color = glm::vec4(LightingColor, 1.0f) * v.Color;
            }
            
            return v;
        }

        void rasterizeTriangle(){
            // Sorting vertices by height
            InRasterizerVertex v0, v1, v2;

            v0.Position = verticeStack[0].Position;
            v1.Position = verticeStack[1].Position;
            v2.Position = verticeStack[2].Position;

            v0.Normal = verticeStack[0].Normal;
            v1.Normal = verticeStack[1].Normal;
            v2.Normal = verticeStack[2].Normal;

            v0.OriginalPos = verticeStack[0].OriginalPos;
            v1.OriginalPos = verticeStack[1].OriginalPos;
            v2.OriginalPos = verticeStack[2].OriginalPos;

            v0.Color = this->mObjectColor;
            v1.Color = this->mObjectColor;
            v2.Color = this->mObjectColor;
            
            v0.Position.x = (int) v0.Position.x;
            v0.Position.y = (int) v0.Position.y;

            v1.Position.x = (int) v1.Position.x;
            v1.Position.y = (int) v1.Position.y;

            v2.Position.x = (int) v2.Position.x;
            v2.Position.y = (int) v2.Position.y;

            if (v0.Position.y == v1.Position.y && v0.Position.y == v2.Position.y) return;
            
            // Vertex shader program
            v0 = vertexShading(v0);
            v1 = vertexShading(v1);
            v2 = vertexShading(v2);
            

            if (v0.Position.y > v1.Position.y) std::swap(v0, v1);
            if (v0.Position.y > v2.Position.y) std::swap(v0, v2);
            if (v1.Position.y > v2.Position.y) std::swap(v1, v2);
            
            int total_height = v2.Position.y - v0.Position.y;

            for (int i=0; i<total_height; i++) {
                bool second_half = i > v1.Position.y-v0.Position.y || v1.Position.y==v0.Position.y;
                int segment_height = second_half ? v2.Position.y - v1.Position.y : v1.Position.y-v0.Position.y;

                // alpha is the proportion of i in total height
                float alpha = (float)i/total_height;

                // beta is the proportion of i in the actual segment height
                float beta  = (float)(i-(second_half ? v1.Position.y-v0.Position.y : 0))/segment_height;

                // A a vector from v0 to v2 scaled by alpha
                glm::vec4 A =               v0.Position + (v2.Position - v0.Position)*alpha;

                // B a vector from v1 to v2 scaled by beta, or a vecor to v0 to v1 scaled by beta 
                glm::vec4 B = second_half ? v1.Position + (v2.Position - v1.Position)*beta : v0.Position + (v1.Position-v0.Position)*beta;
                
                if (A.x > B.x) std::swap(A, B);
                for (int j=A.x; j<=B.x; j++) {
                    float aprop = (float) (B.x - j) / (float) (B.x - A.x);
                    float zFragment = glm::mix(B.z, A.z, aprop);
                    int pix, piy;
                    pix = j;
                    piy = v0.Position.y+i;

                    // To-do: remove from for some of the lines below
                    float Px, Py, Yv0, Yv1, Yv2, Xv0, Xv1, Xv2, Wv0, Wv1, Wv2;
                    Px = (float) pix;
                    Py = (float) piy;

                    Xv0 = (float) v0.Position.x;
                    Yv0 = (float) v0.Position.y;

                    Xv1 = (float) v1.Position.x;
                    Yv1 = (float) v1.Position.y;

                    Xv2 = (float) v2.Position.x;
                    Yv2 = (float) v2.Position.y;

                    float dem;
                    dem = (Yv1 - Yv2) * (Xv0 - Xv2) + (Xv2 - Xv1) * (Yv0 - Yv2);

                    Wv0 = (Yv1 - Yv2) * (Px - Xv2) + (Xv2 - Xv1) * (Py - Yv2);
                    Wv0 = Wv0 / dem;

                    Wv1 = (Yv2 - Yv0) * (Px - Xv2) + (Xv0 - Xv2) * (Py - Yv2);
                    Wv1 = Wv1/dem;

                    Wv2 = 1 - Wv0 - Wv1;

                    glm::vec4 pColor;
                    pColor = v0.Color * Wv0 + v1.Color * Wv1 + v2.Color * Wv2;

                    if(getPixelDeph(pix, piy) > zFragment){
                        setPixelDeph(pix, piy, zFragment);
                        setPixelColor(pix, piy,  pColor);
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
            RasterizerVertex aav;
            for(int i = 0; i <  vs; i++){
                aav = vertices[i];
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