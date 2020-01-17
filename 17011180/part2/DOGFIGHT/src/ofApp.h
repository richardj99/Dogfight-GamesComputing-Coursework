#pragma once

#include "ofMain.h"
#include "ode/ode.h"
#include "ofxAssimpModelLoader.h"

#define ASTSSIZE 50
#define BULLSIZE 15

class Barrier{
public:
    Barrier(dWorldID world, dSpaceID space, float x, float y, int angle);
    void draw();

    ofBoxPrimitive barrierModel;
    dBodyID body;
    dMass mass;
    dGeomID barrierGeom;
};

class Bullet{
public:
    Bullet();
    void create(dWorldID world, dSpaceID space, float x, float y, dBodyID playerBody, dReal playerAngle);
    void draw();

    bool destroyed = false;
    ofSpherePrimitive bulletModel;
    dBodyID body;
    dMass mass;
    dGeomID bulletGeom;
    bool fired = false;
};

class Player{
public:
    Player(dWorldID world, dSpaceID space);
    void draw();
    void update(int keys[], dWorldID world, dSpaceID space);
    void shoot(dWorldID world, dSpaceID space);
    void mirror();

    dReal* getLocation();
    bool destroyed = false;
    bool mirrored = false;
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

    Bullet* bullets[BULLSIZE];
    int bulletNum = 0;

};

class Asteroid{
public:
    Asteroid(dWorldID world, dSpaceID space, float x, float y, float chosenAngle);
    void update();
    void draw();
    void mirror();

    bool mirrored = false;
    bool destroyed = false;
    ofxAssimpModelLoader astModel;
    ofBoxPrimitive asteroidBox;
    dBodyID body;
    dMass mass;
    dGeomID box;
    dReal speed=5;
    dReal angle;

};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

    int score = 0;
    ofEasyCam cam;
    ofLight light;
    dWorldID world;
    dSpaceID space;
    Player *player;
    Asteroid* asteroids[ASTSSIZE];
    Barrier* barriers[4];
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
