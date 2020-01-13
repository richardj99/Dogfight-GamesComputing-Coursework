#pragma once

#include "ofMain.h"
#include "ode/ode.h"
#include "ofxAssimpModelLoader.h"

class Player{
public:
    Player(dWorldID world);
    void draw();
    void update(int keys[]);

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
    dJointID joint;
    dReal speed=0;
    dReal steer=0;
    dReal angle=0;
    float camX, camY, camZ, rad;

};

class Asteroid{
public:
    Asteroid(dWorldID world);
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
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofEasyCam cam;
    ofLight light;
    dWorldID world;
    dSpaceID space;
    Player *player;
    Asteroid* asteroids[1];
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
