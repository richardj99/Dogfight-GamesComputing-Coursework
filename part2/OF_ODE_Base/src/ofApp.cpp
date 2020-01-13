#include "ofApp.h"

#define SHIPMASS 1
#define STARTZ 0.2
#define SHIPLENGTH 0.5
#define SHIPWIDTH 0.4
#define SHIPHEIGHT 0.15
#define ASTMASS 0.5
#define ASTWIDTH 0.3
#define ASTHEIGHT 0.10
#define ASTLENGTH 0.4

//static const dVector3 yunit = { 0, 1, 0 }, zunit = { 0, 0, 1 };

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
    //keys[OF_KEY_LEFT] = 0;

    //Set up asteroids
    asteroids[0] = new Asteroid(world);

    // Set up the OpenFrameworks camera
    cam.setAutoDistance(false);
    //cam.setPosition(0,-5,);

    //Set up Lamp
    //ofVec3f lightVector;
    //lightVector.set(0,0,1);

    //light.lookAt({0,0,0}, lightVector);
    //light.setSpotlight(45.f, 100.0f);
    light.setPosition(0,0,10);

}

//--------------------------------------------------------------
void ofApp::update(){
    dWorldStep (world, 0.05);
    player->update(keys);
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofBackground(00, 00, 00, 00);

    ofVec3f upVector;
    cam.setPosition((player->camX - (sin(player->rad)*10.0)), (player->camY - (cos(player->rad)*10.0)), 7);
    upVector.set(0.0,  0.0, 1.0);
    cam.lookAt({player->camX, player->camY, 0.5}, upVector);
    cam.begin();
    light.enable();

    ofEnableDepthTest();

    ofSetColor(ofColor::grey);
    ofDrawPlane(300,300);

    ofSetColor(ofColor::black);
    ofTranslate(0,0,1);

    ofTranslate(0,0,0);
    //ofDrawBox((*player).x,(*player).y, 1, 2, 4, 1);
    player->draw();
    asteroids[0]->draw();

    ofSetColor(255,255,255,255);

    ofDisableDepthTest();
    light.disable();
    cam.end();
    ofPopMatrix();

}
//--------------------------------------------------------------
void ofApp::exit() {

}
//--------------------------------------------------------------
static void nearCallback (void *, dGeomID o1, dGeomID o2) {

    myApp->collide(o1,o2);
}


void ofApp::collide(dGeomID o1, dGeomID o2)
{

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    keys[key] = 1;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
   keys[key] = 0;
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
    modelBox.setScale(0.05*(SHIPLENGTH+0.11),0.05*SHIPWIDTH,0.05*SHIPHEIGHT);
    modelBox.setPosition(0,0,STARTZ);

    playerModel.loadModel("Space_ship.dae");
    playerModel.setScale(0.005,0.005,0.005);

    body = dBodyCreate(world);
    dBodySetPosition(body, 0, 0, STARTZ+0.2);
    dMassSetBox(&mass,1,SHIPLENGTH*0.05, SHIPWIDTH*0.05, SHIPHEIGHT*0.05);
    dMassAdjust(&mass, SHIPMASS);
    dBodySetMass(body, &mass);

    box = dCreateBox(0,SHIPLENGTH*0.05,SHIPWIDTH*0.05,SHIPHEIGHT*0.05);
    dGeomSetBody(box,body);
}

void Player::draw(){
    ofPushMatrix();
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);


    ofQuaternion q(rot_ode[1], rot_ode[2], rot_ode[3], rot_ode[0]);
    float theta, x, y, z;
    q.getRotate(theta, x, y, z);
    modelBox.setGlobalOrientation(glm::quat(rot_ode[0],rot_ode[1],rot_ode[2],rot_ode[3]));
    playerModel.setRotation(1, theta, x, y, z);
    modelBox.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);
    playerModel.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);

    //modelBox.draw();
    playerModel.drawFaces();
    ofPopMatrix();
    //cout<<angle<<endl;
}

void Player::update(int keys[]){
    if(keys['q']) ofExit();
    if(keys[OF_KEY_UP]){ accelerating = true; if(speed<4.0) speed += 0.5;}
    else{if(speed >= 1.0f) speed -= 1.0f; else speed = 0;}
    if(keys[OF_KEY_LEFT]){angle += M_PI/45;}
    if(keys[OF_KEY_RIGHT]){ angle -= M_PI/45;}

    dMatrix3 R;
    if(angle != 0.0f){
        if(angle >= 2*M_PI) angle = angle - 2*M_PI;
        else if(angle<= 2*M_PI) angle = angle + 2*M_PI;
        dRFromAxisAndAngle(R, 0.0, 0.0, 1.0, angle);
        dBodySetRotation(body, R);
    }
    const dReal* rot_ode = dBodyGetQuaternion(body);
    dBodySetQuaternion(body, rot_ode);
    dBodyAddRelForce(body, speed, 0.0, 0.0);

    const dReal* pos_ode = dBodyGetPosition(body);
    camX = pos_ode[0];
    camY = pos_ode[1];
    camZ = pos_ode[2];
    rad = -1 * (modelBox.getRollRad() - 1.5);
}


Asteroid::Asteroid(dWorldID world){
    body = dBodyCreate(world);
    dBodySetPosition(body, 10.0, 0.0, STARTZ);
    dMassSetBox(&mass, 1, ASTLENGTH*0.05, ASTWIDTH*0.05, ASTHEIGHT*0.05);
    dMassAdjust(&mass, ASTMASS);
    dBodySetMass(body, &mass);
    box = dCreateBox(0, ASTLENGTH*0.05, ASTWIDTH*0.05, ASTHEIGHT*0.05);
    dGeomSetBody(box, body);

    asteroidBox.setScale(ASTLENGTH*0.05, ASTWIDTH*0.05, ASTHEIGHT*0.05);
    asteroidBox.setPosition(10, 0, STARTZ);

}

void Asteroid::update(){
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);

    asteroidBox.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);

}

void Asteroid::draw(){
    asteroidBox.draw();

}
