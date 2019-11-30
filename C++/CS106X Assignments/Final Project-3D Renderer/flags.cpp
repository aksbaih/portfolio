#include "flags.h"
using namespace  std;
 void drawButtonClicked() {
    animateFlag = false;
    drawOriginalFlag = false;
    applyCurrentFlag = true;
}
 void animateButtonClicked() {
    animateFlag = true;
    drawOriginalFlag = false;
    applyCurrentFlag = false;
}
 void stopButtonClicked() {
    animateFlag = false;
    drawOriginalFlag = false;
    applyCurrentFlag = false;
}
 void resetButtonClicked() {
    animateFlag = false;
    drawOriginalFlag = true;
    applyCurrentFlag = false;
}

 bool getAnimateFlag() {
     return animateFlag;
 }
 bool getApplyFlag() {
     return applyCurrentFlag;
 }
 bool getResetFlag() {
     return drawOriginalFlag;
 }

 void setResetFlag(bool v) {
     drawOriginalFlag = v;
 }
 void setApplyFlag(bool v) {
     applyCurrentFlag = v;
 }

 void closeButtonClicked() {
     exitFlag = true;
 }

 bool getCloseFlag() {
     return exitFlag;
 }

 void resetVsButtonClicked() {
     resetControllers = true;
 }


 bool getControllersReset() {
     if(resetControllers) {
         resetControllers = false;
         return true;
     }
     return false;
 }
