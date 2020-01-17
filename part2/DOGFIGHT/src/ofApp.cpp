#include "ofApp.h"
#include <cstdlib>

#define BARRHEIGHT 3.0f
#define BARRLENGTH 200.0f
#define BARRWIDTH 1.0f
#define SHIPMASS 1.00f
#define STARTZ 1.2f
#define SHIPLENGTH 0.5f
#define SHIPWIDTH 0.4f
#define SHIPHEIGHT 0.15f
#define ASTMASS 100.5f
#define ASTWIDTH 3.0f
#define ASTHEIGHT 3.0f
#define ASTLENGTH 3.0f
#define BULLETRAD 0.08f
#define ASTSSIZE 50
#define BULLSIZE 15

//--------------------------------------------------------------
void ofApp::setup(){

    //Create World ODE
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);

    //Set up player
    player = new Player(world, space);

    //Barriers
    barriers[0] = new Barrier(world, space, 100.0f, 0.0f, 1);
    barriers[1] = new Barrier(world, space, 0.0f, 100.0f, 0);
    barriers[2] = new Barrier(world, space, -100.0f, 0.0f, 1);
    barriers[3] = new Barrier(world, space, 0.0f, -100.0f, 0);

    //Set up asteroids
    for(int i=0; i<ASTSSIZE; i++){
        asteroids[i] = new Asteroid(world, space, ofRandom(-95,95), ofRandom(-95,95), (ofRandom(0, 360))*(M_PI/180));
    }

    // Set up the OpenFrameworks camera
    cam.setAutoDistance(false);

    //light.setSpotlight(45.f, 100.0f)
    light.setPosition(0.0f,0.0f,50.0f);

}

//--------------------------------------------------------------
void ofApp::update(){
    //Update Player
    player->update(keys, world, space);
    //Update Asteroids and Bullets (in array)
    for(int i=0; i<ASTSSIZE; i++){
        asteroids[i]->update();
    }
    //Step through physics/collisions
    dWorldStep (world, 0.05f);
    dSpaceCollide (space, nullptr ,&nearCallback);
    //Checks if any asteroids/bullets were 'destroyed' during collision resolutions and reinstantiates them.
    for(int i=0; i<ASTSSIZE; i++){
        if(asteroids[i]->destroyed) asteroids[i] = new Asteroid(world, space, ofRandom(-95,95), ofRandom(-95,95), (ofRandom(0, 360))*(M_PI/180));
        if(i<BULLSIZE){
            if(player->bullets[i]->destroyed) player->bullets[i] = new Bullet();
        }
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();

    ofBackground(00, 00, 00, 00);

    ofVec3f upVector;
    //Sets  camera position to behind the player (regardless of position and angle of rotation
    cam.setPosition((player->camX - (sin(player->rad)*10.0)), (player->camY - (cos(player->rad)*10.0)), 7.0f);
    upVector.set(0.0,  0.0, 1.0);
    cam.lookAt({player->camX+(sin(player->rad)*4.0), player->camY+(cos(player->rad)*4.0), 0.5}, upVector);
    cam.begin();
    light.enable();

    ofEnableDepthTest();

    ofSetColor(ofColor::black);
    ofDrawPlane(200,200);

    ofSetColor(ofColor::black);
    ofTranslate(0,0,1);

    ofTranslate(0,0,0);
    //ofDrawBox((*player).x,(*player).y, 1, 2, 4, 1);
    player->draw();
    for(int i=0; i<ASTSSIZE; i++){
        asteroids[i]->draw();
        if(i<4) barriers[i]->draw();
    }

    ofSetColor(255,255,255,255);

    ofDisableDepthTest();
    light.disable();
    cam.end();
    stringstream ss;
    ss<<"Dogfight"<<endl<<"Current Score: " << score;
    ofDrawBitmapStringHighlight(ss.str().c_str(), 300, 20, ofColor::white, ofColor::black);
    ofPopMatrix();
}
//--------------------------------------------------------------
static void nearCallback (void *, dGeomID o1, dGeomID o2) {
    myApp->collide(o1,o2);
}


void ofApp::collide(dGeomID o1, dGeomID o2){
    if(o1 == player->box || o2==player->box){ // if player is in the collision
        for(int i=0; i<ASTSSIZE; i++){
            if(o1==asteroids[i]->box || o2==asteroids[i]->box){ //if player meets asteroid
                cout<<"Game Over, Final Score: "<< score<<endl;
                ofExit();
            }
            else if(i<4){
                if(o1==barriers[i]->barrierGeom || o2==barriers[i]->barrierGeom){ //if player meets barrier
                    player->mirrored = true; // calls mirror function to teleport the user to the other end of the arena
                    return;
                }
            }
        }
    }
    for(int i = 0; i<ASTSSIZE; i++){
        if(o1==asteroids[i]->box || o2==asteroids[i]->box){
            for(int j=0; j<4; j++){
                if(o1==barriers[j]->barrierGeom || o2==barriers[j]->barrierGeom){ //if asteroid meets barrier
                    asteroids[i]->mirrored = true; // calls mirror function to teleport the user to the other end of the arena
                    return;
                } else if(j<BULLSIZE){
                    if(o1 == player->bullets[j]->bulletGeom || o2 == player->bullets[j]->bulletGeom){ //if asteroid meets bullet
                        asteroids[i]->destroyed = true;
                        player->bullets[j]->destroyed = true;
                        score += 10;
                        return;
                    }
                }
            }
        }
    }
    for(int i=0; i<BULLSIZE; i++){
        if(o1 == player->bullets[i]->bulletGeom || o2 == player->bullets[i]->bulletGeom){
            for(int j=0; j<4; j++){
                if(o1 == barriers[j]->barrierGeom || o2 == barriers[j]->barrierGeom){
                    player->bullets[i]->destroyed = true;
                    return;
                }
            }
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

Barrier::Barrier(dWorldID world, dSpaceID space, float x, float y, int angle){
    body = dBodyCreate(world);
    if(angle != 0){
        dMatrix3 R;
        dRFromAxisAndAngle(R, 0.0f, 0.0f, 1.0f, M_PI/2);
        dBodySetRotation(body, R);
        const dReal* rot_ode = dBodyGetQuaternion(body);
        barrierModel.setGlobalOrientation(glm::quat(rot_ode[0],rot_ode[1],rot_ode[2],rot_ode[3]));
    }
    barrierModel.set(BARRLENGTH, BARRWIDTH, BARRHEIGHT);
    barrierModel.setPosition(x, y, BARRHEIGHT/2.0f);

    dBodySetPosition(body, x, y, BARRHEIGHT/2.0f);
    dMassSetBox(&mass, 1, BARRLENGTH, BARRWIDTH, BARRHEIGHT);
    dMassAdjust(&mass, 0.5f);
    dBodySetMass(body, &mass);

    barrierGeom = dCreateBox(space, BARRLENGTH, BARRWIDTH, BARRHEIGHT);
    dGeomSetBody(barrierGeom, body);
}

void Barrier::draw(){
    const dReal* pos_ode = dBodyGetPosition(body);
    barrierModel.setPosition(pos_ode[0], pos_ode[1], BARRHEIGHT/2.0f);
    ofSetColor(ofColor::red);
    barrierModel.draw();
}

Player::Player(dWorldID world, dSpaceID space){
    //Create Bounding Box for Spaceship to map Spaceship Rotation/Position
    modelBox.set((SHIPLENGTH+0.11),SHIPWIDTH,SHIPHEIGHT);
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

    box = dCreateBox(space,SHIPLENGTH*0.05,SHIPWIDTH*0.05,SHIPHEIGHT*0.05);
    dGeomSetBody(box,body);

    for(int i=0; i<BULLSIZE; i++){
        bullets[i] = new Bullet();
    }
}

void Player::draw(){
    ofPushMatrix();
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);
    dBodySetPosition(body, pos_ode[0], pos_ode[1], BARRHEIGHT/2.0f);

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

    for(int i=0; i<BULLSIZE; i++){
        if(bullets[i]->fired){
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

    if(mirrored){
        mirror();
    }
}

void Player::shoot(dWorldID world, dSpaceID space){
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);
    for(int i=0; i<BULLSIZE; i++){
        if(!bullets[i]->fired){
            bulletNum++;
            bullets[i]->create(world, space, pos_ode[0], pos_ode[1], body, angle);
            break;
        }
    }
}

void Player::mirror(){
    const dReal* pos_ode = dBodyGetPosition(body);

    dReal x = -1*pos_ode[0];
    if(x>0) x -= 10.0f;
    else x+=10.0f;
    dReal y = -1*pos_ode[1];
    if(y>0) y-=10.0f;
    else y+=10.0f;

    dBodySetPosition(body, x, y, STARTZ);
    mirrored = false;
}


Asteroid::Asteroid(dWorldID world, dSpaceID space, float x, float y, float chosenAngle){
    angle = chosenAngle;

    //ODE Body
    body = dBodyCreate(world);
    dMassSetBox(&mass, 1, ASTLENGTH, ASTWIDTH, ASTHEIGHT);
    dMassAdjust(&mass, ASTMASS);
    dBodySetMass(body, &mass);
    box = dCreateBox(space, ASTLENGTH, ASTWIDTH, ASTHEIGHT);
    dGeomSetBody(box, body);
    dBodySetPosition(body, x, y, STARTZ);

    //OF Bounding Box
    asteroidBox.set(ASTLENGTH, ASTWIDTH, ASTHEIGHT);
    asteroidBox.setPosition(10.0, 0.0, STARTZ);
    //DAE Model
    astModel.loadModel("Asteroid.dae");
    astModel.setScale(0.005, 0.005, 0.005);

}

void Asteroid::update(){
    dMatrix3 R;
    dRFromAxisAndAngle(R, 0.0, 0.0, 1.0, angle);
    dBodySetRotation(body, R);
    dBodyAddRelForce(body, 20.0f, 0.0f, 0.0f);
    dBodyAddTorque(body, 0.5f, 0.5f, 0.5f);

    if(mirrored){
        mirror();
    }
}

void Asteroid::draw(){
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);

    dBodySetPosition(body, pos_ode[0], pos_ode[1], STARTZ);

    asteroidBox.setGlobalOrientation(glm::quat(rot_ode[0], rot_ode[1], rot_ode[2], rot_ode[3]));
    ofQuaternion q(rot_ode[1], rot_ode[2], rot_ode[3], rot_ode[0]);
    float theta, x, y, z;
    q.getRotate(theta, x, y, z);

    asteroidBox.setPosition(pos_ode[0], pos_ode[1], STARTZ);
    astModel.setRotation(1, theta, x, y, z);
    astModel.setPosition(pos_ode[0], pos_ode[1], pos_ode[2]);

    ofSetColor(ofColor::grey);
    astModel.drawFaces();
}

void Asteroid::mirror(){
    const dReal* pos_ode = dBodyGetPosition(body);

    dReal x = -1*pos_ode[0];
    if(x>0) x -= 10.0f;
    else x+=10.0f;
    dReal y = -1*pos_ode[1];
    if(y>0) y-=10.0f;
    else y+=10.0f;

    dBodySetPosition(body, x, y, STARTZ);
    mirrored = false;
}

Bullet::Bullet(){;}

void Bullet::draw(){
    const dReal* pos_ode = dBodyGetPosition(body);
    const dReal* rot_ode = dBodyGetQuaternion(body);

    bulletModel.setGlobalOrientation(glm::quat(rot_ode[0], rot_ode[1], rot_ode[2], rot_ode[3]));
    bulletModel.setPosition(pos_ode[0], pos_ode[1], STARTZ);

    dBodySetPosition(body, pos_ode[0], pos_ode[1], STARTZ);
    ofSetColor(ofColor::blueSteel);
    bulletModel.draw();
}

void Bullet::create(dWorldID world, dSpaceID space, float x, float y, dBodyID playerBody, dReal playerAngle){
    fired = true;

    dMatrix3 R;
    dRFromAxisAndAngle(R, 0.0f, 0.0f, 1.0f, playerAngle);
    const dReal* playerRotation = dBodyGetQuaternion(playerBody);
    //OF Bounding Box
    bulletModel.setRadius(BULLETRAD);
    bulletModel.setPosition(x, y, STARTZ);

    body = dBodyCreate(world);
    dBodySetQuaternion(body, playerRotation);
    dBodySetRotation(body, R);
    dBodySetPosition(body, x, y, STARTZ);
    dMassSetSphere(&mass, 1.0f, BULLETRAD);

    dMassAdjust(&mass, 0.03f);
    dBodySetMass(body, &mass);

    bulletGeom = dCreateSphere(space, BULLETRAD);
    dGeomSetBody(bulletGeom, body);
    dBodyAddRelForce(body, 10.0f, 0.0f, 0.0f);
}
