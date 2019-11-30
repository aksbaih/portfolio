#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <gwindow.h>
#include <gobjects.h>
#include "vec.h"
#include "matrix.h"
#include "pqueue.h"
#include "set.h"
#include "vector.h"
#include "transformation.h"
#include "triangle.h"
#include "gslider.h"
#include "gbutton.h"
#include "glabel.h"
#include "gcontainer.h"
#include "flags.h"


class controller
{
public:
    controller();

    GSlider *xRSlider, *yRSlider, *zRSlider;    // rotations sliders
    GSlider *lightSlider1, *lightSlider2;    // light source angles sliders
    GSlider *xTSlider, *zTSlider, *zoomSlider;  // translations and zoom sliders
    GButton *applyButton, *resetButton, *animateButton, *stopAnimationButton, *exitButton, *resetVsButton; // command buttons
    GLabel *xrL, *yrL, *zrL, *xtL, *ztL, *zL, *l1L, *l2L;   // labels for the sliders

    const float kRotationSliderRatio = (300/PI);
    const float kZoomSliderRatio = 1000.0;
    void reset();   // Resets the controllers values to defaults
    void close();   // Closes the window after deleteing all dynamic allocations

private:
    const float kControllerWindowWidth = 300;
    const float kControllerWindowHeight = 500;
    const double kSliderHeight = 30;
    const double kSliderWidth = 0.7*kControllerWindowWidth;
    const int kSliderXOffset = 599;
    const int kRotationSliderMin = -100;
    const int kRotationSliderMax = 100;
    const int kzoomSliderMin = 800;
    const int kzoomSliderMax = 1200;
    const int kTransSliderMin = -10;
    const int kTransSliderMax = 10;
    const int kButtonWidth = kSliderWidth;
    const int kButtonHeight = kSliderHeight;
    const int kBGColor = 14606046;
    const float kVSpacing = 26;

    GWindow window; // the window that shows the controllers
};


#endif // CONTROLLER_H
