#include <iostream>
#include "error.h"
#include "grapher.h"
#include "console.h"
#include "matrix.h"
#include "vec.h"
#include "modelfile.h"
#include <fstream>
#include <string>
#include "transformation.h"
#include "gtimer.h"
#include "flags.h"
#include "triangle.h"
#include "controller.h"

using namespace std;

/* function: updateScene
 * This function applies all transformations parametarized by the user input on the sliders
 * of the controller window. It uses a commulative transformation matrix to composite all the
 * transformations before multiplying them by the points matricies saving lots of computation
 */
static void updateScene(Vector<triangle *>& tris, matrix<3>& verts, matrix<3>& norms, transformation& t, controller& c, vec<3>& translation, grapher& w, vec<3>& lightVec) {
    matrix<3> commTransform = t.xRotationMatrix3D(c.xRSlider->getValue()/c.kRotationSliderRatio);
    commTransform = t.yRotationMatrix3D(c.yRSlider->getValue()/c.kRotationSliderRatio) * commTransform;
    commTransform = t.zRotationMatrix3D(c.zRSlider->getValue()/c.kRotationSliderRatio) * commTransform;
    commTransform = t.zoomMatrix3D(c.zoomSlider->getValue()/c.kZoomSliderRatio) * commTransform;
    translation.setEntry(0, translation[0] + c.xTSlider->getValue());
    translation.setEntry(2, translation[2] - c.zTSlider->getValue()); // remember, the z axis is flipped
    verts = commTransform * verts;
    norms = commTransform * norms;
    w.visualize3Dmatrix(tris, lightVec, translation);
}

/* funcrtion: exitApp
 * This function is called when the user hits the "Close" button and it frees all the memory that the
 * program has dynamically allocated and closes all the windows opened preparing for termination
 */
static void exitApp(controller& c, grapher& g, Vector<triangle *>& tris) {
    c.close();
    g.close();
    for(triangle * t: tris) delete t;
    cout << "Give me an A+ :p!" << endl;
}

/* function: printHelp
 * This function prints the instructions on how to use the app for the user.
 */
void printHelp() {
    cout << "Welcome! This app will help you view '.obj' format files. After opening a file, a control window will show up." << endl;
    cout << "The controllers help you set the parameters they indicate (Trans. stands for translation, an#. stands for parameter angle # used for polar positioning)." << endl;
    cout << "'Reset' resets the drawing to the original." << endl;
    cout << "'Reset Controllers' resets the controllers to the default values." << endl;
    cout << "'Apply' Applies the transformation described by sliders to the current scene." << endl;
    cout << "'Animate' Continuously applies the transformation described by the sliders to the scene creating an animation, you can change the sliders while the animation is going. I.e. the sliders represent the change from a frame to the next." << endl;
    cout << "'Stop Animation' Pauses the animation." << endl;
    cout << "'Close' Closes all windows and ends the program." << endl;
    cout << "Try opening sample file 'cube.obj' with '50' zoom factor." << endl << endl;

}

int main() {
    printHelp(); // show instructions

    matrix<3> verts, norms; // store all the verticies (points) and normals
    matrix<4> faces;        // used to temporarily store the indicies of 3 verticies and a normal for a face in each column
    Vector<triangle *> tris;// stores pointers to all triangles in the model

    while(true) { // try oprning the file or print error and try again.
        cout << "File: ";
        string filename; cin >> filename;
        try {
            modelfile Model;
            Model.loadObject(filename, verts, norms, faces);    // stores configurations in 'faces'
            cout << "Opened successfully!" << endl;
            break;
        } catch (exception e) {
            cout << "This file doesn't exist! Try again." << endl;
        }
    }

    float initialScale = 1; // This will be used later to give a starting zoom
    cout << "Please choose an initial zooming scale for the model (no 0, normal 1): "; cin >> initialScale;
    if(initialScale == 0) initialScale = 1;


    loadTriagnles(tris, verts, norms, faces);   // stores configurations in 'tris'
    faces.empty();  // not needed anymore now we have "tris"

    grapher Window; // the window where the model is shown
    transformation t;   // generates all transformation matricies
    controller c;   // thw window where the controllers are shown

    verts = (t.zoomMatrix3D(initialScale) * t.xRotationMatrix3D(PI)) * verts; // corrective rotation and starting zoom
    norms = (t.zoomMatrix3D(initialScale) * t.xRotationMatrix3D(PI)) * norms;

    matrix<3> OriginalVerts = verts;    // store so can be recovered when user hits 'reset'
    matrix<3> OriginalNorms = norms;

    vec<3> translation(Window.kWindowWidth/2, 0, Window.kWindowHeight/2); // corrective translation

    while(true) {
        if(getCloseFlag()) { // exit the app
            exitApp(c, Window, tris);
            return 0;
        }

        bool drawOriginalFlag = getResetFlag();
        bool applyCurrentFlag = getApplyFlag();
        bool animateFlag = getAnimateFlag();

        setResetFlag(); // Resets to false
        setApplyFlag();

        vec<3> lightVec(0, 1, 0); // reset the light vec to be the y direction

        if(getControllersReset()) c.reset(); // resets controllers if the flag is raised

        if(!drawOriginalFlag) {  // updates the light vec by rotating it around the z and x
            matrix<3> lightVecs; // axes according to the angles the user specifies using a matrix
            lightVecs.addCol(lightVec);
            lightVecs = t.zRotationMatrix3D(c.lightSlider1->getValue()/c.kRotationSliderRatio) * lightVecs;
            lightVecs = t.xRotationMatrix3D(c.lightSlider2->getValue()/c.kRotationSliderRatio) * lightVecs;
            lightVec = lightVecs.getCol(0);
            lightVec.normalize();
        }

        if(drawOriginalFlag) {  // reset the model when the flag is raised
            translation.setEntry(0, Window.kWindowWidth/2);
            translation.setEntry(2, Window.kWindowHeight/2);
            verts = OriginalVerts;
            norms = OriginalNorms;
            Window.visualize3Dmatrix(tris, lightVec, translation);
        }
        if(applyCurrentFlag || animateFlag) {   // advance to the next frame by applying all transformations
            updateScene(tris, verts, norms, t, c, translation, Window, lightVec);
        }
        // Note: applyCurrentFlag is lowered every iteration but animateFlag is not, that's the only difference between both features
    }
}
