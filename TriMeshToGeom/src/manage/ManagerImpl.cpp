#include "manage/ManagerImpl.h"

#include "fileio/JSONMaker.h"
#include "fileio/CombinedIO.h"
#include <fstream>

using namespace std;

ManagerImpl::ManagerImpl()
{
    //ctor
}

ManagerImpl::~ManagerImpl()
{
    //dtor
}

int ManagerImpl::import(const char* file_path){
    this->objcl = di->import(file_path, this->check);
    if (this->objcl == NULL) return -1;
    else return 0;
}

int ManagerImpl::makeSurfaces(){
    return objcl->makeSurfaces(this->check);
}


int ManagerImpl::cleaning(){
    if (objcl->cleaning(check) == -1) return -1;

    //objcl->free();

    //TODO : Pinning and Make Graph
    //objcl->makeGraph(check);
    return 0;
}

int ManagerImpl::exportJSON(string f_path)
{
    ofstream fout;
    fout.open(f_path, ios::out|ios::trunc);

    if (!fout) return -1;
    if (JSONMaker::printJSON(fout, this->objcl->space_list))
    {
        return -1;
    }
    fout.close();
    return 0;
}

int ManagerImpl::exportCombined(string f_path)
{
    ofstream fout;
    fout.open(f_path, ios::out|ios::trunc|ios::binary);
    if (!fout) return -1;
    if (CombinedIO::exportBinary(fout, this->objcl->space_list))
    {
        return -1;
    }
    fout.close();
    return 0;
}

int ManagerImpl::importCombined(string f_path)
{
    ifstream fin;
    fin.open(f_path, ios::in|ios::binary);
    if (!fin) return -1;
    if (CombinedIO::importBinary(fin, this->objcl)) return 1;
    fin.close();
    return 0;
}