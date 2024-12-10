#pragma once
#include<vector>
#include<cmath>

#define MAX_LINE_LENGTH 128

typedef struct {
    float x, y, z;
} Vertex;
typedef struct {
    unsigned int v[3];
} Face;
typedef struct {
    float x, y, z;
}Normal;



typedef struct {

    std::vector<glm::vec3> vertices;

    std::vector<glm::vec3> nvectors;

    std::vector<glm::vec3> textures;

    
    
} Model;
void read_newline(char* str);
void read_obj_file(const char* filename, Model* model);



