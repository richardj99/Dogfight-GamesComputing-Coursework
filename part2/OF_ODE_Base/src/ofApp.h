#pragma once

#include "ofMain.h"
#include "ode/ode.h"

class Player{
public:
    Player();

    bool accelerating = false;
    int accel = 0;
    int health = 100;
    int x = 0;
    int y = 0;


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

    dWorldID world;
    dSpaceID space;
    Player *player;

    /* The actual implementation of the broadphase collision callback;
     * see below for how this works with the ODE library.
     */
    void collide (dGeomID o1, dGeomID o2);

    void drawODEDemoBox(const dReal*pos_ode, const dQuaternion rot_ode, const dReal*sides_ode);
    void drawODEDemoCylinder(const dReal*pos_ode, const dQuaternion rot_ode, dReal len, dReal rad);    
};
/* ODE requires a global function to use as the collision callback; this
 * function, combined with the ofApp pointer, allows us to put the collision
 * code within myApp.
 */
static void nearCallback (void *, dGeomID o1, dGeomID o2);
extern ofApp *myApp;
