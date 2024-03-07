#include <iostream>
#include <cstring>
#include <cmath>
#include "IOManager.h"

using namespace std;

int main(void) {

    const char* path = "D:/DeskTop/temp/aaa/d.txt";
    string writePath = "D:/DeskTop/temp/aaa/d1.txt";

    auto r = new OBJReader();
    auto mesh = new PolyMesh();
    r->read(path, mesh);

    auto w = new OBJWriter();
    w->write(writePath, mesh);

    cout << "done" << endl;

    return 0;
}
