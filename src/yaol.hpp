//Yet Another Object Loader


// TO - Do: clean VAOs and VBOs from memory

#ifndef YET_ANOTHER_OBJECT_LOADER
#define YET_ANOTHER_OBJECT_LOADER

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdio>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    int colorIndex;
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

    std::vector<glm::vec3> faceNormals;

    GLuint VAO, VBO;


    GLuint C2GLVAO, C2GLVBO;
    std::vector<Vertex> C2GLvertices;


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
        fscanf(fp, "%c", &ch);
        while(ch!= '\n')  // skip documentation line
            fscanf(fp, "%c", &ch);
        
        for (unsigned int i=0; i<nTriangles; i++){
            fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
            &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
            &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
            &(vfmf.colorIndex));
            vertices.push_back(vfmf);

            // normals.push_back(bauxVec3);
            // colorIndexes.push_back(ifmf);
            
            fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
            &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
            &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
            &(vfmf.colorIndex));
            vertices.push_back(vfmf);
            
            fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
            &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
            &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
            &(vfmf.colorIndex));
            vertices.push_back(vfmf);
            
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
        for (unsigned int i = 0; i < vertices.size(); i++){
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

        glBindVertexArray(0);
        // std::cout << "Bigger size Offset: " << getMaxOffsetsize() << "\n";
        
        // std::cout << "Center of Mesh: (" << getCenterBBoxOCS().x << ", " << getCenterBBoxOCS().y << ", " << getCenterBBoxOCS().z << ")\n";

    }

    void generateClose2GLVAOVBO(){
        glGenVertexArrays(1, &C2GLVAO); 
        glGenBuffers(1, &C2GLVBO);
        C2GLvertices.reserve(this->vertices.size());
    }

    void updateClose2GLBuffers(glm::mat4 mvp){
        glm::vec4 avm;
        for(unsigned int yai = 0; yai < vertices.size(); yai++){
            avm = glm::vec4(vertices[yai].Position, 1.0f);
            avm = mvp * avm;
            C2GLvertices[yai].Position = glm::vec3(avm);
            C2GLvertices[yai].Normal = vertices[yai].Normal;
            C2GLvertices[yai].colorIndex = vertices[yai].colorIndex;
        }
        // std::cout << "Vert: (" << vertices[0].Position.x << ", " << vertices[0].Position.y << ", " << vertices[0].Position.z << ")\n";
        C2GLvertices[0].Position = glm::vec3(-0.5f, -0.5f, 0.0f);
        C2GLvertices[1].Position = glm::vec3( 0.5f, -0.5f, 0.0f);
        C2GLvertices[2].Position = glm::vec3( 0.0f,  0.5f, 0.0f);

        glBindVertexArray(C2GLVAO);

        glBindBuffer(GL_ARRAY_BUFFER, C2GLVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->C2GLvertices.size(),  &this->C2GLvertices[0], GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
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
    }

    void drawTriangles(){
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        glBindVertexArray(0);
    }

    void drawTrianglesClose2GL(){
        std::cout << "Tset: (" << C2GLvertices[0].Position.x << ", " << C2GLvertices[0].Position.y << ", " << C2GLvertices[0].Position.z << ")\n";
        glBindVertexArray(this->C2GLVAO);
        glDrawArrays(GL_TRIANGLES, 0, this->C2GLvertices.size());
        glBindVertexArray(0);
    }
};


#endif