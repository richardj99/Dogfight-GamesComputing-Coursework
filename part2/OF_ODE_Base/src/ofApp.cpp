#include "ofApp.h"

#define shipMass 1
#define startZ 0.2
#define SHIPLENGTH 0.5
#define SHIPWIDTH 0.4
#define SHIPHEIGHT 0.15

static const dVector3 yunit = { 0, 1, 0 }, zunit = { 0, 0, 1 };

//--------------------------------------------------------------
void ofApp::setup(){

    //Create World ODE
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    //dWorldSetGravity (world,0,0,-0.5);
    ground = dCreatePlane (space,0,0,1,0);

    //Set up player
    player = new Player(world);

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
    dWorldStep (world, 0.05);
    player->update();
    draw();
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofPopMatrix();
    ofBackground(50, 50, 50, 0);

    cam.begin();

    ofEnableDepthTest();

    ofSetColor(ofColor::grey);
    ofDrawPlane(200,200);

    ofSetColor(ofColor::black);
    ofTranslate(0,0,1);

    ofTranslate(0,0,0);
    //ofDrawBox((*player).x,(*player).y, 1, 2, 4, 1);
    player->draw();
    ofSetColor(255,255,255,255);

    ofDisableDepthTest();
    cam.end();
    ofPushMatrix();

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
    case OF_KEY_UP: player->accelerating = true; if(player->speed<10) player->speed += 0.5; break;
    case OF_KEY_LEFT: player->angle -= 0.1; break;
    case OF_KEY_RIGHT: player->angle += 0.1; break;
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

Player::Player(dWorldID world){
    //modelBox.setScale(SHIPLENGTH, SHIPWIDTH, SHIPHEIGHT);
    modelBox.setScale(0.05*(SHIPLENGTH+0.11),0.05*SHIPWIDTH,0.05*SHIPHEIGHT);
    modelBox.setPosition(0,0,startZ);

    playerModel.loadModel("Space_ship.dae");
    playerModel.setScale(0.005,0.005,0.005);

    body[0] = dBodyCreate(world);
    dBodySetPosition(body[0], 0, 0, startZ);
    dMassSetBox(&mass,1,SHIPLENGTH, SHIPWIDTH, SHIPHEIGHT);
    dMassAdjust(&mass, shipMass);
    dBodySetMass(body[0], &mass);
    box[0] = dCreateBox(0,SHIPLENGTH,SHIPWIDTH,SHIPHEIGHT);
    dGeomSetBody(box[0],body[0]);
    dBodySetLinearVel(body[0], 0.1, 0, 0);

    //Thruster
    body[1] = dBodyCreate(world);
    dBodySetPosition(body[1], 0.5*SHIPLENGTH, 0, startZ);
    box[1] = dCreateBox(0,0.1,0.1,0.1);
    dGeomSetBody(box[1],body[1]);

    //Create FixedJoint
    joint = dJointCreateFixed(world,0);
    dJointAttach(joint, body[0], body[1]);
    const dReal *a = dBodyGetPosition(body[1]);
}

void Player::draw(){
    const dReal* pos_ode = dBodyGetPosition(body[0]);
    const dReal* rot_ode = dBodyGetQuaternion(body[0]);

    ofQuaternion q(rot_ode[1], rot_ode[2], rot_ode[3], rot_ode[0]);
    float theta, x, y, z;
    q.getRotate(theta, x, y, z);
    playerModel.setRotation(1, 90, 1, 0, 0);
    playerModel.setRotation(1, theta, x, y, z);
    modelBox.setGlobalOrientation(glm::quat(rot_ode[0],rot_ode[1],rot_ode[2],rot_ode[3]));
    playerModel.drawFaces();
    modelBox.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);
    playerModel.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);
    //modelBox.draw();
}

void Player::update(){
    dMatrix3 R;
    if(angle != 0.0f){
        if(angle >= 6.4) angle = angle - 6.4;
        else if(angle<= -6.4) angle = angle + 6.4;
        dRFromEulerAngles(R, 0, 0, angle);
        dBodySetRotation(body[0], R);
    }
    if(!accelerating){
        if(*dBodyGetLinearVel(body[0]) >= 0.5f) speed -= 0.5f;
        else speed = 0;
    }
    const dReal* rot_ode = dBodyGetQuaternion(body[0]);
    dBodySetQuaternion(body[0], rot_ode);

    //dBodySetLinearVel(body[0], speed, 0, 0);
    cout<<speed<<endl;
    dBodyAddForce(body[0], speed*rot_ode[1], speed*rot_ode[2], 0.0);
    //dJointSetFixedParam (joint,dParamVel2,-speed);
    //dJointSetFixedParam (joint,dParamFMax2,0.3);
}
