//
// Created by dongmin on 18. 1. 12.
//

#include "HalfEdge.h"

bool HalfEdge::isSame(HalfEdge* he){
    if (this->vertices[0] == he->vertices[0]){
        if (this->vertices[1] == he->vertices[1])
            return true;
    }
    return false;
}

Surface *HalfEdge::getParent() const {
    return parent;
}

void HalfEdge::setParent(Surface *pSurface) {
    HalfEdge::parent = pSurface;
}

HalfEdge *HalfEdge::getOppositeEdge() const {
    return oppositeEdge;
}

void HalfEdge::setOppositeEdge(HalfEdge *oppositeEdge) {
    HalfEdge::oppositeEdge = oppositeEdge;
}

bool HalfEdge::isOpposite(HalfEdge * he) {
    if (this->vertices[0] == he->vertices[1]){
        if (this->vertices[1] == he->vertices[0])
            return true;
    }
    return false;
}