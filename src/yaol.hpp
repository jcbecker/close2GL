//Yet Another Object Loader


// TO - Do: clean VAOs and VBOs from memory

#ifndef YET_ANOTHER_OBJECT_LOADER
#define YET_ANOTHER_OBJECT_LOADER

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdio>
#include "globalUtils.hpp"
#include "close2GL.hpp"



struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    int colorIndex;
    glm::vec2 TexCoords;
};

class RenderableObject{
public:
    float minx, maxx, miny, maxy, minz, maxz;
    unsigned int nTriangles;
    unsigned int nMaterial;
    std::vector<glm::vec3> ambient;
    std::vector<glm::vec3> diffuse;
    std::vector<glm::vec3> specular;
    std::vector<float> shine;
    std::vector<Vertex> vertices;
    glm::mat4 modelMatrix;
    bool hasTexCoords;

    std::vector<glm::vec3> faceNormals;

    GLuint VAO, VBO;


    GLuint C2GLVAO, C2GLVBO;
    std::vector<Close2GLVertex> C2GLvertices;
    glm::mat4 modelClose2GL;


    std::vector<RasterizerVertex> C2GLRasVert;

    void minmaxTest(glm::vec3 vtt){
        if (minx > vtt.x) minx = vtt.x;
        if (miny > vtt.y) miny = vtt.y;
        if (minz > vtt.z) minz = vtt.z;

        if (maxx < vtt.x) maxx = vtt.x;
        if (maxy < vtt.y) maxy = vtt.y;
        if (maxz < vtt.z) maxz = vtt.z;

    }

    RenderableObject(const char* objFilePath){
        char ch;
        FILE* fp = fopen(objFilePath,"r");
        if (fp==NULL) {
            printf("ERROR: unable to open TriObj [%s]!\n",objFilePath);
            exit(1);
        }
        fscanf(fp, "%c", &ch);
        while(ch!= '\n') // skip the first line – object’s name
            fscanf(fp, "%c", &ch);
            
        
        fscanf(fp,"# triangles = %d\n", &nTriangles); 

        fscanf(fp,"Material count = %d\n", &nMaterial);
        glm::vec3 auxVec3;
        float afmf;
        // int ifmf;
        Vertex vfmf;
        for (unsigned int i=0; i<nMaterial; i++) {
            fscanf(fp, "ambient color %f %f %f\n", &(auxVec3.x), &(auxVec3.y), &(auxVec3.z));
            ambient.push_back(auxVec3);
            fscanf(fp, "diffuse color %f %f %f\n", &(auxVec3.x), &(auxVec3.y), &(auxVec3.z));
            diffuse.push_back(auxVec3);
            fscanf(fp, "specular color %f %f %f\n", &(auxVec3.x), &(auxVec3.y), &(auxVec3.z));
            specular.push_back(auxVec3);
            fscanf(fp, "material shine %f\n", &(afmf));
            shine.push_back(afmf);

            // std::cout << "test: " << ambient[i].x << ",  " << ambient[i].y << ", " << ambient[i].z << ":\n";
            // std::cout << "test: " << diffuse[i].x << ",  " << diffuse[i].y << ", " << diffuse[i].z << ":\n";
            // std::cout << "test: " << specular[i].x << ",  " << specular[i].y << ", " << specular[i].z << ":\n";
        }
        char wwtest[256];
        fscanf(fp, "Texture = %s\n", &(wwtest));
        if(wwtest[0] == 'Y'){
            this->hasTexCoords = true;
            printf("Loading [%s] WITH texture!\n",objFilePath);
        }else{
            this->hasTexCoords = false;
            printf("Loading [%s] WITHOUT texture!\n",objFilePath);
        }

        fscanf(fp, "%c", &ch);
        while(ch!= '\n')  // skip documentation line
            fscanf(fp, "%c", &ch);
        
        for (unsigned int i=0; i<nTriangles; i++){
            if(this->hasTexCoords){
                fscanf(fp, "v0 %f %f %f %f %f %f %d %f %f\n",
                &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
                &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
                &(vfmf.colorIndex),
                &(vfmf.TexCoords.x), &(vfmf.TexCoords.y));
                vertices.push_back(vfmf);
            }else{
                fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
                &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
                &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
                &(vfmf.colorIndex));
                vertices.push_back(vfmf);
            }

            // normals.push_back(bauxVec3);
            // colorIndexes.push_back(ifmf);
            if(this->hasTexCoords){
                fscanf(fp, "v1 %f %f %f %f %f %f %d %f %f\n",
                &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
                &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
                &(vfmf.colorIndex),
                &(vfmf.TexCoords.x), &(vfmf.TexCoords.y));
                vertices.push_back(vfmf);
            }else{
                fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
                &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
                &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
                &(vfmf.colorIndex));
                vertices.push_back(vfmf);
            }


            if(this->hasTexCoords){
                fscanf(fp, "v2 %f %f %f %f %f %f %d %f %f\n",
                &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
                &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
                &(vfmf.colorIndex),
                &(vfmf.TexCoords.x), &(vfmf.TexCoords.y));
                vertices.push_back(vfmf);
            }else{
                fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
                &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
                &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
                &(vfmf.colorIndex));
                vertices.push_back(vfmf);
            }
            
            fscanf(fp, "face normal %f %f %f\n",
            &(auxVec3.x), &(auxVec3.y), &(auxVec3.z));
            faceNormals.push_back(auxVec3);

            // Tris[i].Color[0] = (unsigned char)(int)(255*(diffuse[color_index[0]].x));
            // Tris[i].Color[1] = (unsigned char)(int)(255*(diffuse[color_index[0]].y));
            // Tris[i].Color[2] = (unsigned char)(int)(255*(diffuse[color_index[0]].z));
        }

        // std::cout << "min position values in axes (z, y, z): (" << minx << ", " << miny << ", " << minz << ")\n";
        // std::cout << "max position values in axes (z, y, z): (" << maxx << ", " << maxy << ", " << maxz << ")\n";

        fclose(fp);

        // Workaround to get light in cow more natural
        if (objFilePath == "../assets/models/cow_up.in"){
            for (unsigned int i = 0; i < vertices.size(); i++){
                vertices[i].Normal.z *= -1.0f;
            }
        }


        setBoundingBox();
        generateVAOVBO();
        generateModelOriginFixedSize();

        generateClose2GLVAOVBO();
    }

    void setBoundingBox(){
        if (vertices.size() < 0){
            return;
        }
        minx = vertices[0].Position.x;
        maxx = vertices[0].Position.x;
        miny = vertices[0].Position.y;
        maxy = vertices[0].Position.y;
        minz = vertices[0].Position.z;
        maxz = vertices[0].Position.z;
        unsigned int vss = this->vertices.size();
        for (unsigned int i = 0; i < vss; i++){
            minmaxTest(vertices[i].Position);
        }

    }
    
    void generateVAOVBO(){
        glGenVertexArrays(1, &VAO); 
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(),  &this->vertices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        if(this->hasTexCoords){
            // Texture Coordenates
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            glEnableVertexAttribArray(2);
        }

        glBindVertexArray(0);
    }

    void generateClose2GLVAOVBO(){
        glGenVertexArrays(1, &C2GLVAO); 
        glGenBuffers(1, &C2GLVBO);
    }

    void updateClose2GLVertices(glm::mat4 mvp){
        glm::vec4 avm;
        Close2GLVertex aav;
        C2GLvertices = std::vector<Close2GLVertex>();
        unsigned int vss = this->vertices.size();
        C2GLvertices.reserve(vss); // Use reserve, maybe can save some fps in close2gl
        for(unsigned int yai = 0; yai < vss; yai++){
            avm = glm::vec4(vertices[yai].Position, 1.0f);
            avm = mvp * avm;
            if (avm.w < 0.0f || avm.w == 0.0f || avm.w == -0.0f){

            }else{
                avm = avm/avm.w;
                aav.Position = glm::vec2(avm);
                aav.Normal = vertices[yai].Normal;
                aav.colorIndex = vertices[yai].colorIndex;
                C2GLvertices.push_back(aav);
            }
        }
    }

    void updateClose2GLRasterizationVertices(glm::mat4 mvp, glm::mat4 model, glm::mat4 view){
        // vec3 cNormal = mat3(transpose(inverse(view * model))) * aNormal;
        glm::mat3 viewModelInvTrans = glm::mat3(glm::transpose(glm::inverse(view * model)));
        RasterizerVertex v0, v1, v2;
        RasterizerVertex aav;
        C2GLRasVert = std::vector<RasterizerVertex>();
        unsigned int vss = this->vertices.size();
        C2GLRasVert.reserve(vss); // Use reserve, maybe can save some fps in close2gl
        
        if(vss%3){
            std::cout << "Erro, vertex to rasterize dont represent a mesh of triangles!\n\n";
            exit(EXIT_FAILURE);
        }
        
        for(unsigned int yai = 0; yai < vss; yai+=3){
            v0.Position = glm::vec4(vertices[yai].Position, 1.0f);
            v1.Position = glm::vec4(vertices[yai + 1].Position, 1.0f);
            v2.Position = glm::vec4(vertices[yai + 2].Position, 1.0f);

            v0.OriginalPos = vertices[yai].Position;
            v1.OriginalPos = vertices[yai + 1].Position;
            v2.OriginalPos = vertices[yai + 2].Position;
            
            v0.Normal = vertices[yai].Normal;
            v1.Normal = vertices[yai + 1].Normal;
            v2.Normal = vertices[yai + 2].Normal;

            v0.TexCoords = vertices[yai].TexCoords;
            v1.TexCoords = vertices[yai + 1].TexCoords;
            v2.TexCoords = vertices[yai + 2].TexCoords;

            v0.Position = mvp * v0.Position;
            v1.Position = mvp * v1.Position;
            v2.Position = mvp * v2.Position;

            // vec3 cNormal = mat3(transpose(inverse(view * model))) * aNormal;
            v0.Normal = viewModelInvTrans * v0.Normal;
            v1.Normal = viewModelInvTrans * v1.Normal;
            v2.Normal = viewModelInvTrans * v2.Normal;

            if (wClippingTest(v0.Position.w) || wClippingTest(v1.Position.w) || wClippingTest(v2.Position.w)){
                // continue;
            }else{
                v0.Position = v0.Position/v0.Position.w;
                v1.Position = v1.Position/v1.Position.w;
                v2.Position = v2.Position/v2.Position.w;

                if (clippingSpaceTest(v0.Position) && clippingSpaceTest(v1.Position) && clippingSpaceTest(v2.Position)){
                    aav = v0;
                    C2GLRasVert.push_back(aav);
                    
                    aav = v1;
                    C2GLRasVert.push_back(aav);
                    
                    aav = v2;
                    C2GLRasVert.push_back(aav);
                }
            }
        }

        if(C2GLRasVert.size()%3){
            std::cout << "Erro, vertex to rasterize dont represent a mesh of triangles!\n\n";
            exit(EXIT_FAILURE);
        }

    }

    bool wClippingTest(float iw){
        return (iw < 0.0f || iw == 0.0f || iw == -0.0f);
    }

    bool clippingSpaceTest(glm::vec4 vtt){
        return (vtt.z > -1.0 && vtt.z < 1.0 &&
        vtt.x > -1.0 && vtt.x < 1.0 &&
        vtt.y > -1.0 && vtt.y < 1.0);
    }

    void updateClose2GLVertices(glm::mat4 mvp, glm::mat4 vport, glm::mat4 model, glm::mat4 view){
        updateClose2GLRasterizationVertices(mvp, model, view);
        unsigned int vss = this->C2GLRasVert.size();
        for(unsigned int yai = 0; yai < vss; yai++){
            this->C2GLRasVert[yai].Position = vport * this->C2GLRasVert[yai].Position;
        }

    }


    void updateClose2GLBuffers(){
        glBindVertexArray(C2GLVAO);

        glBindBuffer(GL_ARRAY_BUFFER, C2GLVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Close2GLVertex) * this->C2GLvertices.size(),  &this->C2GLvertices[0], GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Close2GLVertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Close2GLVertex), (void*)offsetof(Close2GLVertex, Normal));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    float getMaxOffsetsize(){
        float offx = maxx - minx;
        float offy = maxy - miny;
        float offz = maxz - minz;
        if (offx >= offy && offx >= offz){
            return offx;
        }else if(offy >= offz){
            return offy;
        }
        return offz;
    }

    //this method return the center of a bounding box of the loaded object, relative to Object Coordinate System
    glm::vec3 getCenterBBoxOCS(){
        float cbbx = (maxx + minx)/2;
        float cbby = (maxy + miny)/2;
        float cbbz = (maxz + minz)/2;
        return glm::vec3(cbbx, cbby, cbbz);
    }

    void generateModelOriginFixedSize(){
        float scalasetNorm;
        scalasetNorm = 1.5f/this->getMaxOffsetsize();
        this->modelMatrix = glm::mat4(1.0f);
        this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scalasetNorm , scalasetNorm , scalasetNorm));
        this->modelMatrix = glm::translate(this->modelMatrix, this->getCenterBBoxOCS() * -1.0f);

        this->modelClose2GL = glm::mat4(1.0f);
        this->modelClose2GL = C2GL::scale(this->modelClose2GL, glm::vec3(scalasetNorm , scalasetNorm , scalasetNorm));
        this->modelClose2GL = C2GL::translate(this->modelClose2GL, this->getCenterBBoxOCS() * -1.0f);
        // this->modelClose2GL = C2GL::translate();
    }

    void drawTriangles(){
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        glBindVertexArray(0);
    }

    void drawTrianglesClose2GL(){
        glBindVertexArray(this->C2GLVAO);
        glDrawArrays(GL_TRIANGLES, 0, this->C2GLvertices.size());
        glBindVertexArray(0);
    }
};


#endif