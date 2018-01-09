#include "manage/MergingRoomMaker.h"
#include <logic/SurfacesListCalculation.h>

using namespace std;


int MergingRoomMaker::pre_process(double degree){
    return 0;
}


int MergingRoomMaker::process_generation(Space* space, int& maxGeneration, int& currentGeneration, double& degree){
    ll p_size = space->surfacesList.size();
    while (true && maxGeneration-- > 0){
        cout << "generation " << currentGeneration << ": " << space->surfacesList.size()<< endl;
        if (space->combineSurface(degree) == -1)
        {
            cout << "combine error" << endl;
            return -1;
        }

//        if (space->snapSurface(0.01) == -1){ cout << "snap Surface" << endl; return -1;}
//        if (space->handleDefect(0.1) == -1){ cout << "cannot handle defect" << endl; return -1; }

        if (p_size == (int)space->surfacesList.size()) {
            cout << "generation " << currentGeneration  << " done.. "<< endl;
            break;
        }
        else p_size = (int)space->surfacesList.size();

        currentGeneration++;
        this->generation_writer->write();
        if (degree < 15) degree += 0.05;
    }
    return 0;
}

int MergingRoomMaker::constructSpace(CombineParameter* cp){
    int maxGENperOneCycle = cp->maxGEN;
    double startDegree = cp->startDegree;
    bool simplify_mode = cp->simplifyLine;
    bool snap_mode = cp->snapSurface;

    double diff = 0.0001;

    if (this->space_list.size() == 0) {
        cout << "there is no space" << endl;
        return -1;
    }
    int temp_maxGENperOneCycle = maxGENperOneCycle;
    for (ull it = 0 ; it < this->space_list.size(); it++)
    {
        Space* space = this->space_list[it];
        this->generation_writer->start(space);

        for (unsigned int s_i = 0 ; s_i < this->space_list[it]->surfacesList.size() ;s_i++){
            if (space->surfacesList[s_i]->checkDuplicate(this->check)){
                cout << "it has duplicate Vertex" << endl;
                return -1;
            }
        }

        double degree = startDegree;
        double angle = 0.1;
        int gen = 0;
        this->generation_writer->write();

        while (true){
            ll p_size = space->surfacesList.size();
            if (process_generation(space, maxGENperOneCycle, gen, degree)) return -1;
            if (p_size == (int)space->surfacesList.size()) {
                cout << "generation " << gen  << " done.. "<< endl;
                break;
            }
            maxGENperOneCycle = temp_maxGENperOneCycle;

            if (angle < 1.0) angle += 0.1;
            cout << "simplify and handleDefect" << endl;

            if (simplify_mode)
                if (space->simplifySegment() == -1){ cout << "simplify error" << endl; return -1;}
            if (space->handleDefect(angle) == -1){ cout << "cannot handle defect" << endl; return -1; }
            //space->updateNormal();
        }

        sort(space->surfacesList.begin(), space->surfacesList.end(), Surface::compareArea);
        SLC::tagID(space->surfacesList);

        if (snap_mode){
            if (space->snapSurface(diff) == -1){ cout << "snap Surface" << endl; return -1;}
            if (process_generation(space, maxGENperOneCycle, gen, degree)) return -1;
            if (space->handleDefect(angle) == -1){ cout << "cannot handle defect" << endl; return -1; }
        }
    }
    return 0;
}


int MergingRoomMaker::makeSimpleSpaces(SpaceMaker* sm){
    for (ull s_i = 0 ; s_i < this->space_list.size();s_i++){
        Space* space = this->space_list[s_i];
        space->updateNormal();
        //Space* new_space = new Space(space->name, space->checker);
        sm->checker = space->checker;
        space->surfacesList = sm->makeSimpleSurfaces(space->surfacesList);
        //this->simple_space_list.push_back(new_space);
    }

    return 0;
}



int MergingRoomMaker::rotateSurfaces(){
    for (ull it = 0 ; it < this->space_list.size(); it++)
    {
        Space* space = this->space_list[it];
        space->rotateSpaceByFloorTo00();
        if (space->match00() == -1){
            cout << "match00 error" << endl;
            return -1;
        }
    }
    return 0;
}
