#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){

    //Create World ODE
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity (world,0,0,-0.5);
    ground = dCreatePlane (space,0,0,1,0);

    //Set up player
    player = new Player();

    // Set up the OpenFrameworks camera
    ofVec3f upVector;
    upVector.set(0, 0, 1);
    cam.setAutoDistance(false);
    cam.setPosition(10,10,10);
    cam.lookAt({0,0,0},upVector);
    cam.setUpAxis(upVector);
}

//--------------------------------------------------------------
void ofApp::update(){
    player->update();
    draw();
}
//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(50, 50, 50, 0);

    cam.begin();

    ofEnableDepthTest();

    ofSetColor(ofColor::grey);
    ofDrawPlane(100,100);

    ofSetColor(ofColor::black);
    ofTranslate(0,0,1);

    ofTranslate(0,0,0);
    //ofDrawBox((*player).x,(*player).y, 1, 2, 4, 1);
    player->draw();
    ofSetColor(255,255,255,255);

    ofDisableDepthTest();
    cam.end();

}
//--------------------------------------------------------------
void ofApp::exit() {

}
//--------------------------------------------------------------
static void nearCallback (void *, dGeomID o1, dGeomID o2) {

    myApp->collide(o1,o2);
}

//--------------------------------------------------------------
void ofApp::drawODEDemoCylinder(const dReal*pos_ode, const dQuaternion rot_ode, dReal len, dReal rad) {

    ofVec3f      position(pos_ode[0], pos_ode[1], pos_ode[2]);

    // ODEs quaternions are stored in a different order to openFrameworks:
    ofQuaternion rotation(rot_ode[1], rot_ode[2], rot_ode[3], rot_ode[0]);

    float rotationAmount;
    ofVec3f rotationAngle;
    rotation.getRotate(rotationAmount, rotationAngle);

    ofPushMatrix();
    ofTranslate(position);

    ofRotateDeg(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);

    // ODE Drawstuff's cylinder defaults to standing on its end (along Z axis).
    // OpenFramework's cylinder lies on its side (along Y axis). For drawing purposes,
    // we need another rotation of 90 degrees along X axis to make it look right.
    ofRotateDeg(90.0,1,0,0);

    ofDrawCylinder(rad,len);
    ofPopMatrix();
}

void ofApp::drawODEDemoBox(const dReal*pos_ode, const dQuaternion rot_ode, const dReal*sides_ode){

    dReal siz[3] = {sides_ode[0],sides_ode[1],sides_ode[2]};

    ofVec3f      position(pos_ode[0], pos_ode[1], pos_ode[2]);

    // ODEs quaternions are stored in a different order to openFrameworks:
    ofQuaternion rotation(rot_ode[1], rot_ode[2], rot_ode[3], rot_ode[0]);

    float rotationAmount;
    ofVec3f rotationAngle;
    rotation.getRotate(rotationAmount, rotationAngle);

    ofPushMatrix();
    ofTranslate(position);

    ofRotateDeg(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);
    ofDrawBox(0,0,0, siz[0],siz[1],siz[2]);
    ofPopMatrix();
}


void ofApp::collide(dGeomID o1, dGeomID o2)
{

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key) {

    case 'q': ofExit(); break;
    case OF_KEY_UP: player->accelerating = true; player->speed += 0.3; break;
    case OF_KEY_LEFT: player->steer += 0.5; break;
    case OF_KEY_RIGHT: player->steer -= 0.5; break;
    case OF_KEY_DOWN: break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch(key) {
    case OF_KEY_UP: player->accelerating = false; break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

Player::Player(){
    playerModel.loadModel("Space_ship.dae");
    playerModel.setScale(0.005,0.005,0.005);
    playerModel.setRotation(1, 90, 1, 0, 0);
}

void Player::draw(){
    playerModel.setPosition(x, y, 1);
    playerModel.drawFaces();
}

void Player::update(){
    if(accelerating){
        if(accel<600) accel = accel + 1;
    }
    else {
        if(accel >= 10) accel= accel - 1;
        else accel=0;
    }
    x = x + (accel/300);
}

void Player::rotate(int direction){
    playerModel.setRotation(playerModel.getNumRotations(), direction*5, 0, 1, 0);
}
