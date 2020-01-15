#include "ofApp.h"
#include <cstdlib>

#define SHIPMASS 1.00f
#define STARTZ 1.2f
#define SHIPLENGTH 0.5f
#define SHIPWIDTH 0.4f
#define SHIPHEIGHT 0.15f
#define ASTMASS 100.5f
#define ASTWIDTH 0.5f
#define ASTHEIGHT 0.5f
#define ASTLENGTH 0.5f

//static const dVector3 yunit = { 0, 1, 0 }, zunit = { 0, 0, 1 };

//--------------------------------------------------------------
void ofApp::setup(){

    //Create World ODE
    dInitODE2(0);
    world = dWorldCreate();
    dWorldSetGravity(world,0.0,0.0,-0.5);
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    //ground = dCreatePlane (space,0,0,1,0);

    //Set up player
    player = new Player(world, space);

    int rand();

    //(rand() % 100)*3 + 1
    //(rand() % 36)*10

    //Set up asteroids
    //asteroids[0] = new Asteroid(world, space, 30, 0, (2*M_PI)/9);
    for(int i=0; i<5; i++){
        asteroids[i] = new Asteroid(world, space, ((rand() % 100)*1 + 1), ((rand() % 100)*1 + 1), (((rand() % 36)*10))*(M_PI/180));
    }

    // Set up the OpenFrameworks camera
    cam.setAutoDistance(false);

    //light.setSpotlight(45.f, 100.0f)
    light.setPosition(0,0,10);

}

//--------------------------------------------------------------
void ofApp::update(){
    dWorldStep (world, 0.05f);
    player->update(keys, world, space);
    for(int i=0; i<5; i++){
        asteroids[i]->update();
    }
    dSpaceCollide (space, nullptr ,&nearCallback);
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofBackground(00, 00, 00, 00);

    ofVec3f upVector;
    cam.setPosition((player->camX - (sin(player->rad)*10.0)), (player->camY - (cos(player->rad)*10.0)), 7.0f);
    upVector.set(0.0,  0.0, 1.0);
    cam.lookAt({player->camX, player->camY, 0.5}, upVector);
    cam.begin();
    light.enable();

    ofEnableDepthTest();

    ofSetColor(ofColor::black);
    ofDrawPlane(100,100);

    ofSetColor(ofColor::black);
    ofTranslate(0,0,1);

    ofTranslate(0,0,0);
    //ofDrawBox((*player).x,(*player).y, 1, 2, 4, 1);
    player->draw();
    for(int i=0; i<5; i++){
        asteroids[i]->draw();
    }

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
    int g1 = (o1 == asteroids[0]->box || o1 == asteroids[1]->box || o1 == asteroids[2]->box || o1 == asteroids[3]->box || o1 == asteroids[4]->box);
    int g2 = (o2 == asteroids[0]->box || o2 == asteroids[1]->box || o2 == asteroids[2]->box || o2 == asteroids[3]->box || o2 == asteroids[4]->box);
    if(!(g1 ^ g2)) return;
    cout<<"Colliding!"<<endl;

    const int N = 10;  // maximum number of contacts to be generated
    dContact contact[N];
    n = dCollide(o1,o2,N,&contact[0].geom,sizeof(dContact));
    if (n > 0) {  // if there is a collision
      for (i=0; i<n; i++) { // populate the array with contact points
        contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
          dContactSoftERP | dContactSoftCFM | dContactApprox1;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.slip1 = 0.1;
        contact[i].surface.slip2 = 0.1;
        contact[i].surface.soft_erp = 20.5;  //0.5
        contact[i].surface.soft_cfm = 0.2;  //0.2
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
void ofApp::windowResized(int w, int h){

}

Player::Player(dWorldID world, dSpaceID space){
    //Create Bounding Box for Spaceship to map Spaceship Rotation/Position
    modelBox.setScale(0.05*(SHIPLENGTH+0.11),0.05*SHIPWIDTH,0.05*SHIPHEIGHT);
    modelBox.setPosition(0,0,STARTZ);

    //Load Spaceship Model and scale down to sensible size
    playerModel.loadModel("Space_ship.dae");
    playerModel.setScale(0.005,0.005,0.005);

    //Create Physics Bod
    body = dBodyCreate(world);
    dBodySetPosition(body, 0.0f, 0.0f, STARTZ);
    dMassSetBox(&mass,1,SHIPLENGTH, SHIPWIDTH, SHIPHEIGHT);
    dMassAdjust(&mass, SHIPMASS);
    dBodySetMass(body, &mass);

    box = dCreateBox(space,SHIPLENGTH,SHIPWIDTH,SHIPHEIGHT);
    dGeomSetBody(box,body);

    //shipSpace = dSimpleSpaceCreate(space);
    //dSpaceAdd (space, box);
}

void Player::draw(){
    ofPushMatrix();
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);

    //dBodySetTorque(body, 0.0f, 0.0f, 0.0f);
    dBodySetPosition(body, pos_ode[0], pos_ode[1], STARTZ);

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

    for(int i=0; i<3; i++){
        //cout<<bullets[i];
        if(bulletNum>0){
            cout<<i<< " is ready"<<endl;
            bullets[i]->draw();
        }
    }
}

void Player::update(int keys[], dWorldID world, dSpaceID space){
    if(keys['q']) ofExit();
    if(keys[OF_KEY_UP]){ accelerating = true; if(speed<4.0f) speed += 0.5f;}
    else{if(speed >= 1.0f) speed -= 1.0f; else speed = 0;}
    if(keys[OF_KEY_LEFT]){angle += M_PI/90;}
    if(keys[OF_KEY_RIGHT]){ angle -= M_PI/90;}
    if(keys[OF_KEY_RIGHT_CONTROL]){
        keys[OF_KEY_RIGHT_CONTROL] = 0;
        cout<<"Let's Create this bullet"<<endl;
        shoot(world, space);
    }

    dMatrix3 R;
    if(angle != 0.0f){
        if(angle >= 2.0f*M_PI) angle = angle - 2.0f*M_PI;
        else if(angle<= 2.0f*M_PI) angle = angle + 2.0f*M_PI;
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

    //for(int i=0; i<3; i++){
    //    bullets[i]->update()
    //}
}

void Player::shoot(dWorldID world, dSpaceID space){
    cout<<"Should we fire this bullet"<<endl;
    const dReal* pos_ode = dBodyGetPosition(body);
    for(int i=0; i<3; i++){
        if(bullets[i] == NULL){
            cout<<"I think we should"<<endl;
            bulletNum++;
            bullets[i] = new Bullet(world, space, pos_ode[0], pos_ode[1]);
            break;
        }
    }
}


Asteroid::Asteroid(dWorldID world, dSpaceID space, float x, float y, float angle){
    body = dBodyCreate(world);
    dMassSetBox(&mass, 1, ASTLENGTH*0.05, ASTWIDTH*0.05, ASTHEIGHT*0.05);
    dMassAdjust(&mass, ASTMASS);
    dBodySetMass(body, &mass);
    box = dCreateBox(space, ASTLENGTH, ASTWIDTH, ASTHEIGHT);
    dGeomSetBody(box, body);
    cout<<x<<" "<<y<<endl;
    dBodySetPosition(body, x, y, STARTZ);

    asteroidBox.setScale(ASTLENGTH*0.05, ASTWIDTH*0.05, ASTHEIGHT*0.05);
    asteroidBox.setPosition(10.0, 0.0, STARTZ);

    dMatrix3 R;
    cout<<angle<<endl;
    dRFromAxisAndAngle(R, 0.0, 0.0, 1.0, angle);
    dBodySetRotation(body, R);
    dBodyAddRelForce(body, 5.0f, 0.0f, 0.0f);

}

void Asteroid::update(){
}

void Asteroid::draw(){
    dBodySetTorque(body, 0.0f, 0.0f, 0.0f);
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);
    dBodySetPosition(body, pos_ode[0], pos_ode[1], STARTZ);

    asteroidBox.setGlobalOrientation(glm::quat(rot_ode[0], rot_ode[1], rot_ode[2], rot_ode[3]));
    asteroidBox.setPosition(pos_ode[0], pos_ode[1], STARTZ);
    ofSetColor(ofColor::grey);
    asteroidBox.draw();

}

Bullet::Bullet(dWorldID world, dSpaceID space, float x, float y){
    cout<<"Bullet Time"<<endl;

    //OF Bounding Box
    modelCyl.set(0.01f, 0.5f);
    modelCyl.setPosition(x+0.5, y+0.5, STARTZ);

    body = dBodyCreate(world);
    dBodySetPosition(body, 0.0f, 0.0f, STARTZ);
    dMassSetCylinder(&mass, 1.0f, 1, 0.1f, 0.5f);
    dMassAdjust(&mass, 0.01f);
    dBodySetMass(body, &mass);

    cyl = dCreateCylinder(space, 0.1f, 0.5f);
    dGeomSetBody(cyl, body);


}

void Bullet::draw(){
    cout<<"drawing"<<endl;
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);

    dBodySetPosition(body, pos_ode[0], pos_ode[1], STARTZ);
    ofSetColor(ofColor::white);
    modelCyl.draw();

}
