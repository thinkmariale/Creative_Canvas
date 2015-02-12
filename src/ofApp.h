#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxTriangleMesh.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxColorQuantizer.h"

#include "CityParticles.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);

    int count;
    ofImage image;
    ofImage recorded;
    
    ofVec2f size_recorder;
    
    float alpha;
    float intensityThreshold ;
    bool useSaturation;
    bool resetImg;
    bool isMoving;
    
    bool makeTriangles;
    bool makeParticles;
    bool makeOutLine;
    bool makeMesh;
    bool makeRandom;
    
    // MAIN IMAGE MESH
    vector<ofMesh> myMeshes;
    vector<ofVec2f> offsets;
    ofPolyline line;
  
    
    void addMesh();
    void createMesh(ofPolyline mainL, bool addLines);
    
    //triangles
    void createTriangle();
    vector<ofxTriangleMesh> triangle_mesh;
    
    //CityParticles
    vector<CityParticles> city_particles;
    void createCityParticles(ofPolyline mainL, float size);
    
    // Contour Stuff
    float threshold;
    bool showDebug;
    ofxCv::ContourFinder contourFinder;
    ofxCv::TrackingColorMode trackingColorMode;
    ofColor targetColor;
    void setContourColor();
    
    // Color Generator
    int numColors;
    int curColor;
    ofxColorQuantizer colorQuantizer;
    void findContourColor();
    
    //GUI stuff
    ofxUISuperCanvas *gui0;
    void setUpGUI();
    void guiEvent(ofxUIEventArgs &e);
    
};
