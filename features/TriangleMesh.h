#ifndef TRIANGLEMESH_H_INCLUDED
#define TRIANGLEMESH_H_INCLUDED

#include <vector>
#include <string>
#include "predefine.h"
#include "features/Triangle.h"
#include "features/TriangleMeshGraph.h"
using namespace std;

class TriangleMesh{
public:
    string name;
    vector<Triangle*> triangles;
    vector<Vertex*> vertices;

    TriangleMesh(){}

    void init();

    bool checkClosed();

    int partitionByComponent(vector<TriangleMesh *> &new_mesh_list);

    void clear();

    void updateVertexByTriangleList();
    bool isFurniture();
private:
    TriangleMeshGraph* graph;

    void makeGraph();
};

#endif // TRIANGLEMESH_H_INCLUDED
