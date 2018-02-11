#include "features/Space.h"

#include "compute/SurfacesListComputation.h"
#include "compute/VertexComputation.h"

#include <cstdio>
#include <queue>
#include <climits>
#include <algorithm>
#include <cmath>
#include <compute/SurfaceComputation.h>
#include "cgal/SurfaceIntersection.h"

Space::Space(){

}

Space::Space(string pname)
{
    name = pname;
}

Space::~Space()
{
    //dtor
}


int Space::convertTrianglesToSurfaces(vector<Triangle*>& triangles){
    vector<Surface*> c_list;
    ull size = triangles.size();
    for (ull i = 0 ; i < size; i++){
        Surface* newcp = new Surface(*triangles[i]);
        c_list.push_back(newcp);
    }

    this->surfacesList.insert(this->surfacesList.end(), c_list.begin(), c_list.end());

    cout << "\ndone make Surfaces" << endl;
    return 0;
}


int Space::combineSurface(double degree){
    cout << "Combine Surfaces" << endl;

    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareLength);
    ull p_size = this->surfacesList.size();
    bool* checked = (bool*)malloc(sizeof(bool) * p_size);
    std::fill(checked, checked + p_size, false);

    vector<Surface*> new_poly_list;
    int combined_count = 0;
    for (ull i = 0 ; i < this->surfacesList.size() ; i++)
    {
        if (checked[i]) continue;
        checked[i] = true;

        ll count = -1;
        Surface* newcp = new Surface(this->surfacesList[i]);
        while(count != 0){
            newcp = attachSurfaces(newcp, i+1, checked, count, degree);
            if (newcp == NULL) break;
            printProcess(combined_count, this->surfacesList.size(), "combineSurface");
            combined_count += count;
        }
        if (newcp != NULL) new_poly_list.push_back(newcp);
    }

    freeSurfaces();
    this->surfacesList = new_poly_list;
    return 0;
}

Surface* Space::attachSurfaces(Surface* cp, ull start, bool* checked, ll& count, double degree)
{
    count = 0;
    if (cp->normal == CGAL::NULL_VECTOR) return NULL;
    for (ull id = start ; id < this->surfacesList.size() ; id++)
    {
        if (!checked[id])
        {
            Surface* sf = this->surfacesList[id];
            if (SurfacePairComputation::combine(cp, sf, degree) == 0)
            {
                printProcess(id, this->surfacesList.size(), "attachSurfaces");
                cp->triangles.insert(cp->triangles.end(), sf->triangles.begin(), sf->triangles.end());
                checked[id] = true;
                count++;
            }
        }
    }
    return cp;
}

int Space::updateNormal(){
    cout << "\n------------updateNormal------------\n" << endl;
    for (ull i = 0 ; i < (int)this->surfacesList.size() ; i++)
    {
        Surface* surface = this->surfacesList[i];
        if (!surface->updateNormal())
        {
            cout << surface->toJSONString() <<endl;
            cout << "Cannot make Normal" << endl;
            exit(-1);
        }

    }
    return 0;
}


int Space::simplifySegment(){
    cout << "\n------------simplifySegment------------\n" << endl;
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareLength);
    ull p_size = this->surfacesList.size();

    for (ull i = 0 ; i < p_size - 1; i++)
    {
        printProcess(i, p_size, "");
        for (ull j = i + 1; j < p_size ; j++)
        {
            int loop_count = 0;
            bool again = false;
            while (SurfacePairComputation::simplifyLineSegment(this->surfacesList[i], this->surfacesList[j], again) == 0)
            {
                again = true;
                loop_count++;
                if (loop_count > (int) this->surfacesList[j]->getVerticesSize()){
                    cout << "Infinite loop in Simplification" << endl;
                    exit(-1);
                }
                //loop
            }
        }
    }
    return 0;
}

int Space::handleDefect() {
    cout << "\n------------- handle Defect --------------\n" << endl;
    for (vector<Surface*>::size_type i = 0 ; i < this->surfacesList.size(); )
    {
        Surface* surface = this->surfacesList[i];
        surface->removeConsecutiveDuplication();
        SurfaceComputation::removeStraight(surface);
        surface->updateMBB();

        if (surface->isValid()){
            i++;
        }
        else{
            delete surface;
            this->surfacesList.erase(this->surfacesList.begin() + i);
            cout << "Erase unvalid surface" << endl;
        }
    }
    return 0;
}




int Space::match00(){
    cout << "\n------------- match00 --------------\n" << endl;

    updateMBB();
    double diff[3];
    for (int i = 0 ; i < 3 ; i++){
        diff[i] = -this->min_coords[i];
    }

    for (ull i = 0 ; i < this->surfacesList.size() ; i++)
    {
        this->surfacesList[i]->translate(diff);
//        if (this->surfacesList[i]->isValid())
//        {
//            this->surfacesList[i]->translate(diff);
//        }
//        else{
//            //this->polygon_list.erase(this->polygon_list.begin() + i);
//            cout << this->surfacesList[i]->toJSONString() <<endl;
//            cout << "un-valid surface in match00" << endl;
//            exit(-1);
//        }
    }

    for (ull i = 0 ; i < this->p_vertexList->size() ; i++){
        this->p_vertexList->at(i)->translate(diff);
    }

    return 0;
}

void Space::updateMBB(){
    vector<vector<double> > min_max;
    min_max = SLC::getMBB(this->surfacesList);
    for (int i = 0 ; i < 3 ; i++){
        this->min_coords[i] = min_max[0][i];
        this->max_coords[i] = min_max[1][i];
    }
}

void Space::freeSurfaces(){
    for (ull i = 0 ; i < this->surfacesList.size() ; i++)
    {
        delete(this->surfacesList[i]);
    }
    this->surfacesList.clear();
}



void Space::rotateSpaceByFloorTo00(){
    cout << " ---------- rotate -------------" << endl;
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareArea);
    int floor_index = SLC::findFirstSurfaceIndexSimilarWithAxis(this->surfacesList, 2);
    Surface* floor = this->surfacesList[floor_index];
    floor->updateMBB();
    //Plane_3 plane(this->surfacesList[floor_index]->v_list[0]->getCGALPoint(), floor->av_normal);
    //floor->makePlanar(plane);

    Vector_3 vector_z(0,0,1);
    double angle = -CGALCalculation::getAngle(floor->normal, vector_z);
    if (angle == 0.0){
        cout << "angle is 0.0" << endl;
        return;
    }

    Vector_3 unit_vector = CGAL::cross_product(vector_z, floor->normal);
    unit_vector = unit_vector / sqrt(unit_vector.squared_length());
    cout << "rotate " << angle << ", " << unit_vector.squared_length()<< endl;
    assert(unit_vector.squared_length() < 1.000001 && unit_vector.squared_length() > 0.99999);

    double cos_value = cos(angle * PI /180.0);
    double sin_value = sin(angle * PI /180.0);

    double ux = unit_vector.x();
    double uy = unit_vector.y();
    double uz = unit_vector.z();

    Transformation rotateZ(cos_value + ux*ux *(1-cos_value), ux*uy*(1-cos_value) - (uz * sin_value), ux*uz*(1-cos_value) + uy*sin_value,
                            uy*ux*(1-cos_value) + uz * sin_value,cos_value + uy*uy*(1-cos_value), uy*uz*(1-cos_value)- (ux*sin_value),
                            uz*ux*(1-cos_value)-uy*sin_value , uz*uy*(1-cos_value) + ux * sin_value, cos_value + uz*uz*(1-cos_value),
                            1);

    for (ull i = 0 ; i < this->p_vertexList->size() ; i++){
        Point_3 p = VertexComputation::getCGALPoint(this->p_vertexList->at(i));
        p = p.transform(rotateZ);
        this->p_vertexList->at(i)->setCoords(p.x(), p.y(), p.z());
    }
}



int Space::snapSurface(double p_diff){
    cout << "snap Surface " << endl;
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareLength);
    for (ull i = 0 ; i < this->surfacesList.size() ; i++){
        Surface* sfi = this->surfacesList[i];
        for (ull j = i + 1 ; j < this->surfacesList.size() ; j++){
            Surface* sfj = this->surfacesList[j];
            if (sfj->getVerticesSize() < 3) continue;
            //Same Normal and isNeighbor
            if (Checker::CanbeMerged(sfi->normal, sfj->normal, 10.0)){
                // sfi->snapping(sfj, p_diff);
            }
            if (sfj->getVerticesSize() < 3 || sfi->getVerticesSize() < 3) cout << "snapping make wrong surface---" << endl;

        }

    }
    return 0;
}

int Space::checkDuplicateVertexInSurfaces() {
    for (unsigned int s_i = 0 ; s_i < this->surfacesList.size() ;s_i++){
        if (surfacesList[s_i]->checkDuplicate()){
            cout << "it has duplicate Vertex" << endl;
            return -1;
        }
    }
    return 0;
}

int Space::makeSurfacesPlanar() {
    for (ull i = 0 ; i < this->surfacesList.size() ; i++){
        SurfaceComputation::flatten(this->surfacesList[i]);
    }
    return 0;
}

void Space::sortSurfacesByArea() {
    sort(this->surfacesList.begin(), this->surfacesList.end(), Surface::compareArea);
}

void Space::tagID() {
    SurfacesListComputation::tagID(this->surfacesList);
}

void Space::putVerticesAndUpdateIndex(vector<Vertex *> &vertices) {
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); sfID++){
        vector<Vertex*> vt = this->surfacesList[sfID]->getVerticesList();
        for (unsigned int i = 0 ; i < vt.size() ; i++){
            vt[i]->index = vertices.size();
            vertices.push_back(vt[i]);
        }
    }
}

void Space::resolveIntersectionINTRASurface() {
    int newSurfaceCount = 0;
    for (int sfID = 0 ; sfID < this->surfacesList.size(); ) {
        vector<Surface*> newSurfaces = SurfaceIntersection::resolveSelfIntersection(this->surfacesList[sfID]);
        if (newSurfaces.size() != 0){
            cout << "Surface : " << sfID << endl;
            cout << "---------------------------------" << endl;
            //this->surfacesList.insert(this->surfacesList.end(), newSurfaces.begin(), newSurfaces.end());
            //newSurfaceCount += newSurfaces.size();
            sfID++;

        } else{
            this->surfacesList.erase(this->surfacesList.begin() + sfID);
        }
    }
    cout << "Intersect Surfaces : " << this->surfacesList.size() << endl;
    cout << "new Surface : " << newSurfaceCount << endl;
}

void Space::resolveIntersectionINTERSurface() {

}

void Space::clearTrianglesListInSurfaces() {
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); sfID++) {
        this->surfacesList[sfID]->clearTriangleList();
    }
}

void Space::triangulateSurfaces() {
    this->hasTriangulation = true;
    for (unsigned int sfID = 0 ; sfID < this->surfacesList.size(); sfID++) {
        Surface* pSurface = this->surfacesList[sfID];
        SurfaceComputation::triangulate(pSurface);
    }
}

