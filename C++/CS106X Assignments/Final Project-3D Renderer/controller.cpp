#include "controller.h"


controller::controller() {
    window.setSize(kControllerWindowWidth, kControllerWindowHeight);
    window.setLocation(0, 0);
    window.setResizable(false);
    window.setBackground(kBGColor);

    window.requestFocus();

    GContainer * gc = new GContainer(GContainer::LAYOUT_GRID); // the layout for the window
    gc->setSize(kControllerWindowWidth, kControllerWindowHeight);

    this->xrL = new GLabel("X Rotation");
    gc->addToGrid(xrL, 1, 1);

    this->xRSlider = new GSlider(kRotationSliderMin, kRotationSliderMax, 0);
    xRSlider->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(xRSlider, 1, 2);

    this->yrL = new GLabel("Y Rotation");
    gc->addToGrid(yrL, 2, 1);

    this->yRSlider = new GSlider(kRotationSliderMin, kRotationSliderMax, 0);
    yRSlider->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(yRSlider, 2, 2);

    this->zrL = new GLabel("Z Rotation");
    gc->addToGrid(zrL, 3, 1);

    this->zRSlider = new GSlider(kRotationSliderMin, kRotationSliderMax, 0);
    zRSlider->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(zRSlider, 3, 2);

    this->zL = new GLabel("Zoom");
    gc->addToGrid(zL, 4, 1);

    this->zoomSlider = new GSlider(kzoomSliderMin, kzoomSliderMax, kZoomSliderRatio);
    zoomSlider->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(zoomSlider, 4, 2);

    this->xtL = new GLabel("X Trans.");
    gc->addToGrid(xtL, 5, 1);

    this->xTSlider = new GSlider(kTransSliderMin, kTransSliderMax, 0);
    xTSlider->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(xTSlider, 5, 2);

    this->ztL = new GLabel("Z Trans.");
    gc->addToGrid(ztL, 6, 1);

    this->zTSlider = new GSlider(kTransSliderMin, kTransSliderMax, 0);
    zTSlider->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(zTSlider, 6, 2);

    this->l1L = new GLabel("Light an1.");
    gc->addToGrid(l1L, 7, 1);

    this->lightSlider1 = new GSlider(-PI*kRotationSliderRatio, PI*kRotationSliderRatio, 0);
    lightSlider1->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(lightSlider1, 7, 2);

    this->l2L = new GLabel("Light an2.");
    gc->addToGrid(l2L, 8, 1);

    this->lightSlider2 = new GSlider(-PI*kRotationSliderRatio/2, PI*kRotationSliderRatio/2, 0);
    lightSlider2->setSize(kSliderWidth, kSliderHeight);
    gc->addToGrid(lightSlider2, 8, 2);

    this->applyButton = new GButton("Apply");
    applyButton->setSize(kButtonWidth, kButtonHeight);
    gc->addToGrid(applyButton, 9, 2);
    applyButton->setActionListener(drawButtonClicked); // wire the buttons!

    this->animateButton = new GButton("Animate!");
    animateButton->setSize(kButtonWidth, kButtonHeight);
    gc->addToGrid(animateButton, 10, 2);
    animateButton->setActionListener(animateButtonClicked);

    this->stopAnimationButton = new GButton("Stop Animation");
    stopAnimationButton->setSize(kButtonWidth, kButtonHeight);
    gc->addToGrid(stopAnimationButton, 11, 2);
    stopAnimationButton->setActionListener(stopButtonClicked);

    this->resetButton = new GButton("Reset");
    resetButton->setSize(kButtonWidth, kButtonHeight);
    gc->addToGrid(resetButton, 12, 2);
    resetButton->setActionListener(resetButtonClicked);

    this->resetVsButton = new GButton("Reset Controllers");
    resetVsButton->setSize(kButtonWidth, kButtonHeight);
    gc->addToGrid(resetVsButton, 13, 2);
    resetVsButton->setActionListener(resetVsButtonClicked);

    this->exitButton = new GButton("Close");
    exitButton->setSize(kButtonWidth, kButtonHeight);
    gc->addToGrid(exitButton, 14, 2);
    exitButton->setActionListener(closeButtonClicked);

}
void controller::reset() {
    xRSlider->setValue(0);
    yRSlider->setValue(0);
    zRSlider->setValue(0);
    xTSlider->setValue(0);
    zTSlider->setValue(0);
    zoomSlider->setValue(kZoomSliderRatio);
}

void controller::close() {
    delete xRSlider;
    delete yRSlider;
    delete zRSlider;
    delete lightSlider1;
    delete lightSlider2;
    delete xTSlider;
    delete zTSlider;
    delete zoomSlider;
    delete applyButton;
    delete resetButton;
    delete animateButton;
    delete stopAnimationButton;
    delete exitButton;
    delete resetVsButton;
    delete xrL;
    delete yrL;
    delete zrL;
    delete xtL;
    delete ztL;
    delete zL;
    delete l1L;
    delete l2L;
    window.close();
}
