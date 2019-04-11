//Yet Another Object Loader

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

class RendableObject{
public:
    float minx, maxx, miny, maxy, minz, maxz;
    unsigned int nTriangles;
    unsigned int nMaterial;
    std::vector<glm::vec3> ambient;
    std::vector<glm::vec3> diffuse;
    std::vector<glm::vec3> specular;
    std::vector<float> shine;
    std::vector<Vertex> vertices;
    // std::vector<glm::vec3> normals;
    // std::vector<int> colorIndexes;
    std::vector<glm::vec3> faceNormals;

    GLuint VAO, VBO;
    void minmaxTest(glm::vec3 vtt){
        if (minx > vtt.x) minx = vtt.x;
        if (miny > vtt.y) miny = vtt.y;
        if (minz > vtt.z) minz = vtt.z;

        if (maxx < vtt.x) maxx = vtt.x;
        if (maxy < vtt.y) maxy = vtt.y;
        if (maxz < vtt.z) maxz = vtt.z;

    }

    RendableObject(const char* objFilePath){
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
        int ifmf;
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

            std::cout << "teste: " << ambient[i].x << ",  " << ambient[i].y << ", " << ambient[i].z << ":\n";
            std::cout << "teste: " << diffuse[i].x << ",  " << diffuse[i].y << ", " << diffuse[i].z << ":\n";
            std::cout << "teste: " << specular[i].x << ",  " << specular[i].y << ", " << specular[i].z << ":\n";
        }
        fscanf(fp, "%c", &ch);
        while(ch!= '\n')  // skip documentation line
            fscanf(fp, "%c", &ch);
        

        int firstrount = 1;
        std::cout << "Triangulos para ler: " << nTriangles <<"\n";
        for (unsigned int i=0; i<nTriangles; i++){
            fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
            &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
            &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
            &(vfmf.colorIndex));
            vertices.push_back(vfmf);
            if(firstrount){
                minx = vfmf.Position.x;
                maxx = vfmf.Position.x;
                miny = vfmf.Position.y;
                maxy = vfmf.Position.y;
                minz = vfmf.Position.z;
                maxz = vfmf.Position.z;
                firstrount = 0;
            }
            minmaxTest(vfmf.Position);

            // normals.push_back(bauxVec3);
            // colorIndexes.push_back(ifmf);
            
            fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
            &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
            &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
            &(vfmf.colorIndex));
            vertices.push_back(vfmf);
            minmaxTest(vfmf.Position);
            
            fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
            &(vfmf.Position.x), &(vfmf.Position.y), &(vfmf.Position.z),
            &(vfmf.Normal.x), &(vfmf.Normal.y), &(vfmf.Normal.z),
            &(vfmf.colorIndex));
            vertices.push_back(vfmf);
            minmaxTest(vfmf.Position);
            
            fscanf(fp, "face normal %f %f %f\n",
            &(auxVec3.x), &(auxVec3.y), &(auxVec3.z));
            faceNormals.push_back(auxVec3);

            // Tris[i].Color[0] = (unsigned char)(int)(255*(diffuse[color_index[0]].x));
            // Tris[i].Color[1] = (unsigned char)(int)(255*(diffuse[color_index[0]].y));
            // Tris[i].Color[2] = (unsigned char)(int)(255*(diffuse[color_index[0]].z));
        }

        std::cout << "minimos: (" << minx << ", " << miny << ", " << minz << ")\n";
        std::cout << "maximos: (" << maxx << ", " << maxy << ", " << maxz << ")\n";

        fclose(fp);

        glGenVertexArrays(1, &VAO); 
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(),  &this->vertices[0], GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        std::cout << "maior: " << getMaxOffsetsize() << "\n";
        
        std::cout << "centro: (" << getCenterBBoxOCS().x << ", " << getCenterBBoxOCS().y << ", " << getCenterBBoxOCS().z << ")\n";

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

    
    
};


#endif