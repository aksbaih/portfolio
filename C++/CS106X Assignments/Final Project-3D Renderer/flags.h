#ifndef FLAGS_H
#define FLAGS_H
#include <iostream>

static bool animateFlag = false;   // this flag is raised to signal animation
                            // animation stops when it's lowered

static bool drawOriginalFlag = true;   // this flag is raised to draw the original scene
                                // After drawing, the flag is lowered

static bool applyCurrentFlag = false;  // this flag is raised to apply the current transformation once.
                                // it's lowered after application

static bool exitFlag = false;  // this flag is raised to signal closing the app
static bool resetControllers = false;  // this flag is raised to reset the controllers

// helper functions as click listeners for the controller buttons, they change the flags to signal actions to Main
void drawButtonClicked();
void animateButtonClicked();
void stopButtonClicked();
void resetButtonClicked();
void closeButtonClicked();
void resetVsButtonClicked();

// helper functions that get the values of the flags for the Main so it does the actions signaled
bool getAnimateFlag();
bool getApplyFlag();
bool getResetFlag();
bool getCloseFlag();
bool getControllersReset();

// helper functions that reset the values of the flags after the necessary action is taken
void setResetFlag(bool v = false);
void setApplyFlag(bool v = false);

#endif // FLAGS_H
