#include "ofApp.h"

#define SHIPMASS 10.01
#define STARTZ 1.2
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
    dWorldSetGravity(world,0.0,0.0,-0.5);
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    ground = dCreatePlane (space,0,0,1,0);

    //Set up player
    player = new Player(world, space);

    //Set up asteroids
    asteroids[0] = new Asteroid(world, space);

    // Set up the OpenFrameworks camera
    cam.setAutoDistance(false);

    //light.setSpotlight(45.f, 100.0f)
    light.setPosition(0,0,10);

}

//--------------------------------------------------------------
void ofApp::update(){
    player->update(keys);
    dSpaceCollide (space,0,&nearCallback);
    dWorldStep (world, 0.05);
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofBackground(00, 00, 00, 00);

    ofVec3f upVector;
    cam.setPosition((player->camX - (sin(player->rad)*10.0)), (player->camY - (cos(player->rad)*10.0)), 4);
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
    int i,n;

    // only collide things with the ground
    int g1 = (o1 == asteroids[0]->box || o1 == ground);
    int g2 = (o2 == asteroids[0]->box || o2 == ground);
    if(!(g1 ^ g2)) return;

    const int N = 10;  // maximum number of contacts to be generated
    dContact contact[N];
    n = dCollide(o1,o2,N,&contact[0].geom,sizeof(dContact));
    if (n > 0) {  // if there is a collision
      for (i=0; i<n; i++) { // populate the array with contact points
        contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
          dContactSoftERP | dContactSoftCFM | dContactApprox1;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.slip1 = 0.5;
        contact[i].surface.slip2 = 0.5;
        contact[i].surface.soft_erp = 0.2;
        contact[i].surface.soft_cfm = 0.1;
        dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
        dJointAttach (c,
                      dGeomGetBody(contact[i].geom.g1),
                      dGeomGetBody(contact[i].geom.g2));
      }
    }
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

Player::Player(dWorldID world, dSpaceID space){
    modelBox.setScale(0.05*(SHIPLENGTH+0.11),0.05*SHIPWIDTH,0.05*SHIPHEIGHT);
    modelBox.setPosition(0,0,STARTZ);

    playerModel.loadModel("Space_ship.dae");
    playerModel.setScale(0.005,0.005,0.005);

    body = dBodyCreate(world);
    dBodySetPosition(body, 0, 0, STARTZ);
    dMassSetBox(&mass,1,SHIPLENGTH, SHIPWIDTH, SHIPHEIGHT);
    dMassAdjust(&mass, SHIPMASS);
    dBodySetMass(body, &mass);

    box = dCreateBox(0,SHIPLENGTH,SHIPWIDTH,SHIPHEIGHT);
    dGeomSetBody(box,body);

    //shipSpace = dSimpleSpaceCreate(space);
    dSpaceAdd (space, box);
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

    modelBox.draw();
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
    dBodyAddRelForce(body, 0, 0, 0.5);
}


Asteroid::Asteroid(dWorldID world, dSpaceID space){
    body = dBodyCreate(world);
    dMassSetBox(&mass, 1, ASTLENGTH*0.05, ASTWIDTH*0.05, SHIPHEIGHT*0.05);
    dMassAdjust(&mass, ASTMASS);
    dBodySetMass(body, &mass);
    box = dCreateBox(0, ASTLENGTH, ASTWIDTH, SHIPHEIGHT);
    dGeomSetBody(box, body);
    dBodySetPosition(body, 10.0, 0.0, STARTZ);

    asteroidBox.setScale(ASTLENGTH*0.05, ASTWIDTH*0.05, SHIPHEIGHT*0.05);
    asteroidBox.setPosition(10.0, 0.0, STARTZ);

    //astSpace = dSimpleSpaceCreate(space);
    dSpaceAdd(space, box);


}

void Asteroid::update(){
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);

    asteroidBox.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);

}

void Asteroid::draw(){
    asteroidBox.draw();

}
