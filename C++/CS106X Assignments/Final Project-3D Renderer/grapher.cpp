#include "grapher.h"
#include <iostream>

grapher::grapher() {
    window.setSize(kWindowWidth, kWindowHeight);
    window.setLocation(0, 0);
    window.setResizable(false);
    window.setBackground("white");
    window.setAutoRepaint(false); // to make it faster, only paint after finishing
    window.setLineStyle(GPolygon::LINE_NONE);
    window.requestFocus();

}

grapher::~grapher() {
}

void grapher::close() {
    window.close();
}

void grapher::drawTriangle(triangle * tri, float cosineLight, vec<3>& translation) {

    // Generate the corresponding gray scale color
    cosineLight = (cosineLight + 1) * kBrightnessRange;  // correct the range of the shade to be from 0 to 1
    int color = cosineLight;    // set the red component
    color = (color << 8) + cosineLight; // shift Red to Green and add the Green component
    color = (color << 8) + cosineLight; // shift Red to Red and Green to Green and add the Blue component
    window.setFillColor(color);

    // Draw the triangle as a polygon of its translated verticies
    window.fillPolygon({tri->e(0, 0)+translation[0], tri->e(0, 2)+translation[2],
                        tri->e(1, 0)+translation[0], tri->e(1, 2)+translation[2],
                        tri->e(2, 0)+translation[0], tri->e(2, 2)+translation[2]});

}

void grapher::visualize3Dmatrix(Vector<triangle *>& tris, vec<3>& LightVec, vec<3> translation) {
    PriorityQueue<triangle*> drawingOrder; // stores the indicies of the points to be drawn

    for(triangle* tri : tris) {
        if(tri->getNormal()[1] > 0) {
            drawingOrder.add(tri, tri->e(0, 1)); // draw with priority being the y component
        }

    }

    window.clear();
    while(!drawingOrder.isEmpty()) {
        triangle * indexNow = drawingOrder.dequeue(); // from back to front in the y-axis
        drawTriangle(indexNow, indexNow->findBrightness(LightVec), translation);
    }

    window.repaint();
}


