#include "modelfile.h"

using namespace std;

modelfile::modelfile() {

}

void modelfile::loadObject(string fileName, matrix<3> &verts, matrix<3> &norms, matrix<4>& faces) {
    ifstream inpFile (fileName.c_str());
    if(inpFile.fail()) error("File does not exist; unable to open the file.");
    while(!inpFile.fail()) {
        string line;
        getline(inpFile, line);
        if(line.size() < 2) continue;

        istringstream ss;
        ss.str(line);
        string com;
        ss >> com;

        if(com == "v") {    // vertix
            vec<3> newV;
            float e;
            ss >> e; newV.setEntry(0, e);
            ss >> e; newV.setEntry(1, e);
            ss >> e; newV.setEntry(2, e);
            verts.addCol(newV);
            continue;
        } else if(com == "vn") {    // normal
            vec<3> newV;
            float e;
            ss >> e; newV.setEntry(0, e);
            ss >> e; newV.setEntry(1, e);
            ss >> e; newV.setEntry(2, e);
            norms.addCol(newV);
            continue;
        } else if (com == "f") {    // face
            vec<4> newV;
            string e;
            for(int i = 0; i < 3; i ++) {
                ss >> e;
                newV.setEntry(i, stringToInteger(e.substr(0, e.find("/"))) - 1); // numbers before "/"
            }
            newV.setEntry(3, stringToInteger(e.substr(e.find("/", e.find("/")+1)+1)) - 1); // numbers after "/.../"
            faces.addCol(newV);
            continue;
        }
    }
    inpFile.close();
}

