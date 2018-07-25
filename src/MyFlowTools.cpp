//
//  #include "MyFlowTools.h" MyFlowTools.cpp
//  jingzhe_client
//
//  Created by Pampa Nie on 26/03/2018.
//

#include "MyFlowTools.h"

MyFlowTools::MyFlowTools(){
    
}

MyFlowTools::~MyFlowTools(){
    
}
//--------------------------------------------------------------

void MyFlowTools::setup(int _w,int _h,float _ratio,string _id){
    drawWidth = _w;
    drawHeight = _h;
    flowWidth = _w / _ratio;
    flowHeight = _h / _ratio;
    ratio = _ratio;
    
    opticalFlow.setup(flowWidth, flowHeight);
    velocityMask.setup(drawWidth, drawHeight);
    
    // FLUID & PARTICLES
    fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight);
    //    velocityDots.setup(flowWidth / 4, flowHeight / 4);
    particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight);
    
    
    // VISUALIZATION
    //    displayScalar.setup(flowWidth, flowHeight);
    //    velocityField.setup(flowWidth / 4, flowHeight / 4);
    //    temperatureField.setup(flowWidth / 4, flowHeight / 4);
    //    pressureField.setup(flowWidth / 4, flowHeight / 4);
    //    velocityTemperatureField.setup(flowWidth / 4, flowHeight / 4);
    
    // MOUSE DRAW
    //    mouseForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
    
    myID = _id;
    setupGui();
    
}
//--------------------------------------------------------------

void MyFlowTools::update(ofFbo *_fboForFluidP,ofFbo *_obstacleFboP){
    
    opticalFlow.setSource(_fboForFluidP->getTexture());
    opticalFlow.update();
    velocityMask.setDensity(_fboForFluidP->getTexture());
    velocityMask.setVelocity(opticalFlow.getOpticalFlow());
    velocityMask.update();
    
    
    
    // fluid
    fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
    fluidSimulation.addDensity(velocityMask.getColorMask());
    fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
    //    fluidSimulation.addObstacle(obstacleFbo.getTexture());
    // try to add temporary obstacle
    
    fluidSimulation.addTempObstacle(_obstacleFboP->getTexture());
    
    
    fluidSimulation.update();
    
    if (particleFlow.isActive()) {
        particleFlow.setSpeed(fluidSimulation.getSpeed());
        particleFlow.setCellSize(fluidSimulation.getCellSize());
        particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
        particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
        //        particleFlow.addDensity(fluidSimulation.getDensity());
        particleFlow.setObstacle(fluidSimulation.getObstacle());
    }
    particleFlow.update();
    
    
}


//--------------------------------------------------------------
void MyFlowTools::draw(){
    
        switch(drawMode) {
            case DRAW_COMPOSITE: drawComposite(); break;
            case DRAW_PARTICLES: drawParticles(); break;
            case DRAW_FLUID_FIELDS: drawFluidFields(); break;
                //            case DRAW_FLUID_DENSITY: drawFluidDensity(); break;
                //            case DRAW_FLUID_VELOCITY: drawFluidVelocity(); break;
                //            case DRAW_FLUID_PRESSURE: drawFluidPressure(); break;
                //            case DRAW_FLUID_TEMPERATURE: drawFluidTemperature(); break;
                //            case DRAW_FLUID_DIVERGENCE: drawFluidDivergence(); break;
                //            case DRAW_FLUID_VORTICITY: drawFluidVorticity(); break;
                //            case DRAW_FLUID_BUOYANCY: drawFluidBuoyance(); break;
            case DRAW_FLUID_OBSTACLE: drawFluidObstacle(); break;
                //            case DRAW_FLOW_MASK: drawMask(); break;
                //            case DRAW_OPTICAL_FLOW: drawOpticalFlow(); break;
            case DRAW_SOURCE: drawSource(); break;
                //            case DRAW_MOUSE: drawMouseForces(); break;
                //            case DRAW_VELDOTS: drawVelocityDots(); break;
        }
    
//    drawFluidFields();
}
//--------------------------------------------------------------

void MyFlowTools::exit(){
    
}

//--------------------------------------------------------------

void MyFlowTools::setupGui() {
    
    gui.setup("particle settings");
    //    gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
    //    gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
    gui.add(ID.set(myID));
    gui.add(drawMode.set("draw mode", DRAW_NOTHING, DRAW_NOTHING, DRAW_MOUSE));
    
    drawMode.addListener(this,&MyFlowTools::drawModeSetName);
    gui.add(drawName.set("MODE", "draw name"));
    
    
    //    int guiColorSwitch = 0;
    //    ofColor guiHeaderColor[2];
    //    guiHeaderColor[0].set(160, 160, 80, 200);
    //    guiHeaderColor[1].set(80, 160, 160, 200);
    //    ofColor guiFillColor[2];
    //    guiFillColor[0].set(160, 160, 80, 200);
    //    guiFillColor[1].set(80, 160, 160, 200);
    //
    //    gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
    //    gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
    //    guiColorSwitch = 1 - guiColorSwitch;
    gui.add(particleFlow.parameters);
    
    //    gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
    //    gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
    //    guiColorSwitch = 1 - guiColorSwitch;
    gui.add(opticalFlow.parameters);
    
    //    gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
    //    gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
    //    guiColorSwitch = 1 - guiColorSwitch;
    gui.add(fluidSimulation.parameters);
    
    //    gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
    //    gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
    //    guiColorSwitch = 1 - guiColorSwitch;
    gui.add(velocityMask.parameters);
    
    gui.minimizeAll();
    
    
}
//--------------------------------------------------------------

void MyFlowTools::drawGui() {
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    gui.draw();
    ofPopStyle();
}
//--------------------------------------------------------------

void MyFlowTools::drawModeSetName(int &_value) {
    switch(_value) {
        case DRAW_NOTHING:            drawName.set("Draw Nothing");        break;
        case DRAW_COMPOSITE:        drawName.set("Composite");             break;
        case DRAW_PARTICLES:        drawName.set("Particles");             break;
        case DRAW_FLUID_FIELDS:        drawName.set("Fluid Fields");         break;
            //        case DRAW_FLUID_DENSITY:    drawName.set("Fluid Density  "); break;
            //        case DRAW_FLUID_VELOCITY:    drawName.set("Fluid Velocity (3)"); break;
            //        case DRAW_FLUID_PRESSURE:    drawName.set("Fluid Pressure (4)"); break;
            //        case DRAW_FLUID_TEMPERATURE:drawName.set("Fld Temperature(5)"); break;
            //        case DRAW_FLUID_DIVERGENCE: drawName.set("Fld Divergence "); break;
            //        case DRAW_FLUID_VORTICITY:    drawName.set("Fluid Vorticity"); break;
            //        case DRAW_FLUID_BUOYANCY:    drawName.set("Fluid Buoyancy "); break;
        case DRAW_FLUID_OBSTACLE:    drawName.set("Fluid Obstacle");     break;
            //        case DRAW_OPTICAL_FLOW:        drawName.set("Optical Flow   (6)"); break;
            //        case DRAW_FLOW_MASK:        drawName.set("Flow Mask      (7)"); break;
        case DRAW_SOURCE:            drawName.set("Source");             break;
            //        case DRAW_MOUSE:            drawName.set("Left Mouse     (8)"); break;
            //        case DRAW_VELDOTS:            drawName.set("VelDots        (0)"); break;
    }
}


//--------------------------------------------------------------

void MyFlowTools::setParticleColor(ofColor _color){
    particleFlow.setParticleColor(_color);
}
//-------------------------------------------------------------
void MyFlowTools::setParticleSpeedX(int _speed){
    particleFlow.setGravity({
        particleFlow.getGravity()[0] + _speed,
        particleFlow.getGravity()[1]});
}
//--------------------------------------------------------------
ofColor MyFlowTools::getParticleColor(){
    return particleFlow.getParticleColor();
}

//--------------------------------------------------------------

void MyFlowTools::setFlowToDrawRatio(float _ratio){
    ratio = _ratio;
    flowWidth = drawWidth / ratio;
    flowHeight = drawHeight / ratio;
}
//--------------------------------------------------------------

float MyFlowTools::getFlowToDrawRatio(){
    return ratio;
}
//--------------------------------------------------------------

void MyFlowTools::setDrawWidth(int _w){
    drawWidth = _w;
    flowWidth = drawWidth / ratio;
}
//--------------------------------------------------------------

int MyFlowTools::getDrawWidth(){
    return drawWidth;
}
//--------------------------------------------------------------

void MyFlowTools::setDrawHeight(int _h){
    drawHeight = _h;
    flowHeight = drawHeight / ratio;
}
//--------------------------------------------------------------

int MyFlowTools::getDrawHeight(){
    return drawHeight;
}
//--------------------------------------------------------------

int MyFlowTools::getFlowWidth(){
    return flowWidth;
}
//--------------------------------------------------------------

int MyFlowTools::getFlowHeight(){
    return flowHeight;
}
//--------------------------------------------------------------

void MyFlowTools::drawComposite(int _x, int _y, int _width, int _height){
    ofPushStyle();
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    fluidSimulation.draw(_x, _y, _width, _height);
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    if (particleFlow.isActive())
        particleFlow.draw(_x, _y, _width, _height);
    ofPopStyle();
}
//--------------------------------------------------------------

void MyFlowTools::drawParticles(int _x, int _y, int _width, int _height){
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    if (particleFlow.isActive())
        particleFlow.draw(_x, _y, _width, _height);
    ofPopStyle();
}
//--------------------------------------------------------------

void MyFlowTools::drawFluidFields(int _x, int _y, int _width, int _height){
    ofPushStyle();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    pressureField.setPressure(fluidSimulation.getPressure());
    pressureField.draw(_x, _y, _width, _height);
    velocityTemperatureField.setVelocity(fluidSimulation.getVelocity());
    velocityTemperatureField.setTemperature(fluidSimulation.getTemperature());
    velocityTemperatureField.draw(_x, _y, _width, _height);
    temperatureField.setTemperature(fluidSimulation.getTemperature());
    
    ofPopStyle();
}

//--------------------------------------------------------------

void MyFlowTools::drawFluidObstacle(int _x, int _y, int _width, int _height) {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    fluidSimulation.getObstacle().draw(_x, _y, _width, _height);
    ofPopStyle();
}

void MyFlowTools::drawSource(int _x, int _y, int _width, int _height){
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    //    cameraFbo.draw(_x, _y, _width, _height);  ====== TODO
    ofPopStyle();
}


