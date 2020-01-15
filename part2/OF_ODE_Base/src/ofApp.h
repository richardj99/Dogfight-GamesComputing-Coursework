#pragma once

#include "ofMain.h"
#include "ode/ode.h"
#include "ofxAssimpModelLoader.h"

class Bullet{
public:
    Bullet(dWorldID world, dSpaceID space, float x, float y);
    void draw();

    ofCylinderPrimitive modelCyl;
    dBodyID body;
    dMass mass;
    dGeomID cyl;
    bool fired = false;


};

class Player{
public:
    Player(dWorldID world, dSpaceID space);
    void draw();
    void update(int keys[], dWorldID world, dSpaceID space);
    void shoot(dWorldID world, dSpaceID space);

    dReal* getLocation();

    ofxAssimpModelLoader playerModel;
    ofBoxPrimitive modelBox;
    bool accelerating = false;
    dReal rotating = 0;
    int accel = 0;
    int health = 100;
    dBodyID body;
    dMass mass;
    dGeomID box;
    dReal speed=0;
    dReal steer=0;
    dReal angle=0;
    float camX, camY, camZ, rad;

    Bullet* bullets[3];
    int bulletNum = 0;

};

class Asteroid{
public:
    Asteroid(dWorldID world, dSpaceID space, float x, float y, float angle);
    void update();
    void draw();

    ofBoxPrimitive asteroidBox;
    dBodyID body;
    dMass mass;
    dGeomID box;
    dReal speed=5;

};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void windowResized(int w, int h);

    ofEasyCam cam;
    ofLight light;
    dWorldID world;
    dSpaceID space;
    Player *player;
    Asteroid* asteroids[5];
    dJointGroupID contactgroup;
    dGeomID ground;

    int keys[65536];

    /* The actual implementation of the broadphase collision callback;
     * see below for how this works with the ODE library.
     */
    void collide (dGeomID o1, dGeomID o2);   
};
/* ODE requires a global function to use as the collision callback; this
 * function, combined with the ofApp pointer, allows us to put the collision
 * code within myApp.
 */
static void nearCallback (void *, dGeomID o1, dGeomID o2);
extern ofApp *myApp;
