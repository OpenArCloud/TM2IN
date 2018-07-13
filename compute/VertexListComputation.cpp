//
// Created by dongmin on 18. 1. 18.
//

#include <cgal/Features_to_CGAL_object.h>
#include "VertexComputation.h"
#include "VertexListComputation.h"
#include "features/HalfEdge.h"

void VertexListComputation::deleteVertexList(vector<Vertex*>& vertices){
    for (ull i = 0 ; i < vertices.size() ; i++){
        delete vertices[i];
    }
    vertices.clear();
}

Vertex *VertexListComputation::getCenter(vector<Vertex *> vertices) {
    double x = 0,y = 0,z = 0;
    for (ull i = 0 ; i < vertices.size() ; i++){
        x += vertices[i]->x();
        y += vertices[i]->y();
        z += vertices[i]->z();
    }
    Vertex* newVertex = new Vertex(x/ vertices.size(), y / vertices.size(), z / vertices.size());
    return newVertex;
}


bool VertexListComputation::checkDuplicate(vector<Vertex *> vertices) {
    vector<Vertex*> sorted_v_list(vertices);

    sort(sorted_v_list.begin(), sorted_v_list.end(), VertexComputation::greater);
    for (ull i = 0 ; i < sorted_v_list.size() - 1; i++){
        if (sorted_v_list[i] == sorted_v_list[i+1]){
            return true;
        }
    }
    return false;
}
