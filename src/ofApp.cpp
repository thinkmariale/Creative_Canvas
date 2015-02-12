#include "ofApp.h"

using namespace ofxCv;
using namespace cv;


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    count = 1;
    image.loadImage("img1.jpg");
    
    //mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    //mesh.enableColors();
    //mesh.enableIndices();
    
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(200);
    trackingColorMode = TRACK_COLOR_RGB;
    showDebug     = true;
  
    size_recorder  = ofVec2f(50,80);
    resetImg       = false;
    useSaturation  = false;
    isMoving       = false;
    intensityThreshold  = 80;

    //------
    makeTriangles = false;
    makeParticles = false;
    makeOutLine   = true;
    makeMesh      = false;
    makeRandom    = false;
    
    //----
    // get our colors
    numColors = 12;
    curColor  = 0;
    alpha     = 100;
    colorQuantizer.setNumColors(numColors);
    colorQuantizer.quantize(image.getPixelsRef());
    findContourColor();
    
    //setting gui
    setUpGUI();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // MOCING PARTICLES
    for(vector<CityParticles>::iterator m = city_particles.begin(); m != city_particles.end();++m){
        ofColor c = image.getColor(m->pos.x, m->pos.y);
        m->color = c;
        m->update();
    }

    if(isMoving)
    {
        // making mesh move
        int offset = 0;
        for(vector<ofMesh>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it)
        {
            int numVerts = it->getNumVertices();
            for (int i = 0; i<numVerts; ++i) {
                ofVec3f vert    = it->getVertex(i);
                float time      = ofGetElapsedTimef();
                float timeScale = 5.0;
                float displacementScale = 0.75;
                if(offset > offsets.size()) continue;
                ofVec3f timeOffsets = offsets[offset];
                
                vert.x += (ofSignedNoise(time*timeScale+timeOffsets.x)) * displacementScale;
                vert.y += (ofSignedNoise(time*timeScale+timeOffsets.y)) * displacementScale;
                vert.z += (ofSignedNoise(time*timeScale+timeOffsets.z)) * displacementScale;
                it->setVertex(i, vert);
                offset++;
            }
        }
    }
    if(resetImg)
    {
        resetImg = false;
        for(vector<ofMesh>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it)
            it->clear();
        myMeshes.clear();
        line.clear();
        triangle_mesh.clear();
        city_particles.clear();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
   
    ofBackground(ofColor::white);
    ofSetColor(255);
   
    ofEnableAlphaBlending();
    ofSetColor(255,255,255,alpha);
    image.draw(0, 0);
    ofDisableAlphaBlending();
    colorQuantizer.draw(ofPoint(0, image.getHeight() + 5));
    
    // draw the four rectangles
    ofNoFill();
    ofSetColor(ofColor::magenta);
    ofRect(mouseX - 25, mouseY - 25, size_recorder.x, size_recorder.y);
    
    // draw the four corresponding subsections
    ofPushMatrix();
        recorded.cropFrom(image, mouseX - 25, mouseY - 25, size_recorder.x, size_recorder.y);
        ofSetColor(ofColor::white);
    ofPopMatrix();
    
    recorded.draw(220, (image.height + 35));
   
    //mesh.draw();
    ofSetLineWidth(1);
    for(vector<ofxTriangleMesh>::iterator m = triangle_mesh.begin(); m != triangle_mesh.end();++m)
        m->draw();
    ofSetLineWidth(2);
    for(vector<ofMesh>::iterator m = myMeshes.begin(); m != myMeshes.end();++m)
        m->draw();
    
    ofFill();
    for(vector<CityParticles>::iterator m = city_particles.begin(); m != city_particles.end();++m){
        ofSetColor(m->color);
        ofCircle(m->pos.x, m->pos.y, m->radius);
    }

    line.draw();
    
    
    //--------------------------
    //contour
    if(showDebug)
    {
        ofSetLineWidth(1);
        ofSetColor(255);
        contourFinder.draw();
        ofNoFill();
        int n = contourFinder.size();
        for(int i = 0; i < n; i++) {
            // minimum area circle that encloses the contour
           /* ofSetColor(cyanPrint);
            float circleRadius;
            ofVec2f circleCenter = toOf(contourFinder.getMinEnclosingCircle(i, circleRadius));
            //if(circleRadius > 180) continue;
            ofCircle(circleCenter, circleRadius);
            */
            // convex hull of the contour
            ofSetColor(yellowPrint);
            ofPolyline convexHull = toOf(contourFinder.getConvexHull(i));
            convexHull.draw();
            
            ofSetColor(ofColor::white);
            ofPolyline mainLine = contourFinder.getPolyline(i);
            mainLine.draw();
            
           
            // some different styles of contour centers
            ofVec2f centroid = toOf(contourFinder.getCentroid(i));
            ofVec2f average = toOf(contourFinder.getAverage(i));
            ofVec2f center = toOf(contourFinder.getCenter(i));
            ofSetColor(cyanPrint);
            ofCircle(centroid, 1);
            ofSetColor(magentaPrint);
            ofCircle(average, 1);
            ofSetColor(yellowPrint);
            ofCircle(center, 1);
            
            // you can also get the area and perimeter using ofPolyline:
            // ofPolyline::getArea() and ofPolyline::getPerimeter()
            double area = contourFinder.getContourArea(i);
            double length = contourFinder.getArcLength(i);
            
            // balance is useful for detecting when a shape has an "arm" sticking out
            // if balance.length() is small, the shape is more symmetric: like I, O, X...
            // if balance.length() is large, the shape is less symmetric: like L, P, F...
            ofVec2f balance = toOf(contourFinder.getBalance(i));
            ofPushMatrix();
            ofTranslate(centroid.x, centroid.y);
            ofScale(5, 5);
            ofLine(0, 0, balance.x, balance.y);
            ofPopMatrix();
        }
    }
    
    
    drawHighlightString(ofToString((int) ofGetFrameRate()) + " fps", 80, (image.height + 35));
    drawHighlightString(ofToString((int) threshold) + " threshold", 80, (image.height + 55));
    //drawHighlightString(trackingColorMode == TRACK_COLOR_HSV ? "HSV tracking" : "RGB tracking", 100, (image.height + 85));
    
    ofPushMatrix();
    ofTranslate(10, (image.height + 35));
    ofFill();
    ofSetColor(0);
    ofRect(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofRect(0, 0, 64, 64);
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::createMesh(ofPolyline mainL, bool addLines)
{
    ofMesh m;
    m.setMode(OF_PRIMITIVE_LINE_STRIP);
    m.enableColors();
    m.enableIndices();
    
     for(int i = 0;i< mainL.size();i++)
    {
        ofPoint point = mainL[i];
        ofColor c = image.getColor(point.x, point.y);
        float intensity = c.getLightness();
        if(useSaturation)
            intensity = c.getSaturation();
        if(intensity >= intensityThreshold)
        {
            m.addVertex(point);
            m.addColor(c);
            offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000)));
        }
    }
    
    //adding some lines
    if(addLines)
    {
        float connectionDistance = 50;
        int numVerts = m.getNumVertices();
        for (int a=0; a<numVerts; ++a) {
            ofVec3f verta = m.getVertex(a);
            for (int b=a+1; b<numVerts; ++b) {
                ofVec3f vertb = m.getVertex(b);
                float distance = verta.distance(vertb);
                if (distance <= connectionDistance) {
                    m.addIndex(a);
                    m.addIndex(b);
                }
            }
        }
    }
    myMeshes.push_back(m);

}

//--------------------------------------------------------------
void ofApp::createTriangle()
{
    if (line.size() > 2){
        
        ofPolyline lineRespaced = line;
        
        ofxTriangleMesh t_mesh;
        // add the last point (so when we resample, it's a closed polygon)
        lineRespaced.addVertex(lineRespaced[0]);
        // resample
        lineRespaced = lineRespaced.getResampledBySpacing(20);
        // I want to make sure the first point and the last point are not the same, since triangle is unhappy:
        lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
        // if we have a proper set of points, mesh them:
        if (lineRespaced.size() > 5){
            
            // angle constraint = 28
            // size constraint = -1 (don't constraint triangles by size);
            
            // t_mesh.triangulate(lineRespaced, 28, -1);
            
            
            // this is an alternative, constrain on size not angle:
            t_mesh.triangulate(lineRespaced, -1, 100);
            
            // see ofxTriangleMesh.h for info.
            
            
            
            for(int i=0;i< t_mesh.triangles.size();i++)
            {
                ofColor c = image.getColor(t_mesh.triangles[i].pts[0].x, t_mesh.triangles[i].pts[0].y);
                t_mesh.triangles[i].randomColor = c;
            }
            triangle_mesh.push_back(t_mesh);
        }
        
        line.clear();
    }
}
//--------------------------------------------------------------
void ofApp::createCityParticles(ofPolyline mainL, float size)
{

    int numPartiles = 5;
    int s = size/100;
    if(s >= 300) numPartiles = 50;
    if(s >= 100) numPartiles = 30;
    for(int i = 0;i< numPartiles;i++)
    {
        float rad = ofRandom(1, 4);
        float speed = ofRandom(0.05,0.8);
        
        ofPoint point = mainL[ofRandom(0,mainL.size()) ];
        ofColor c = image.getColor(point.x, point.y);
        float intensity = c.getLightness();

        CityParticles particle = CityParticles(point, rad, ofColor(ofRandom(255), ofRandom(255), ofRandom(255)), speed, mainL.getVertices());
        city_particles.push_back(particle);
    }

}
//--------------------------------------------------------------
void ofApp::findContourColor()
{
    setContourColor();
    
    int effect = -1;
    if(makeRandom)
        effect = ofRandom(3);
    
    for (int i = 0;i< contourFinder.size();i++)
    {
        ofPolyline mainLine = contourFinder.getPolyline(i);
        if(makeMesh || effect == 0)
        {
            createMesh(mainLine, true);
        }
        if(makeOutLine || effect == 1)
        {
            createMesh(mainLine, false);
        }
        if(makeParticles || effect == 2)
        {
            createCityParticles(mainLine, contourFinder.getMinAreaRect(i).size.area());
        }
        if(makeTriangles || effect == 3)
        {
            line = mainLine;
            createTriangle();
        }
    }
    
    curColor = (curColor + 1)%colorQuantizer.getColors().size();
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if(e.getName() == "draw triangles")
    {
        makeParticles = false;
        makeOutLine   = false;
        makeMesh      = false;
        makeRandom    = false;
    }
    if(e.getName() == "draw particles")
    {
        makeTriangles = false;
        makeOutLine   = false;
        makeMesh      = false;
        makeRandom    = false;
    }
    if(e.getName() == "draw outlines")
    {
        makeTriangles = false;
        makeParticles = false;
        makeMesh      = false;
        makeRandom    = false;
    }
    if(e.getName() == "draw mesh")
    {
        makeTriangles = false;
        makeParticles = false;
        makeOutLine   = false;
        makeRandom    = false;
    }
    if(e.getName() == "draw random")
    {
        makeTriangles = false;
        makeParticles = false;
        makeOutLine   = false;
        makeMesh      = false;
    }
}

//--------------------------------------------------------------
void ofApp::setContourColor()
{
    targetColor = colorQuantizer.getColors()[curColor];
    contourFinder.setTargetColor(targetColor, trackingColorMode);
    
    threshold = ofMap(image.width *0.5, 0, ofGetWidth(), 0, 255);
    contourFinder.setThreshold(threshold);
    contourFinder.findContours(image);
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == 'm')
        isMoving  = !isMoving;
    if( key == 'p'){
        for(vector<ofMesh>::iterator it = myMeshes.begin(); it != myMeshes.end(); ++it)
            it->clear();
        myMeshes.clear();

    }
    if( key == 't'){
        line.clear();
        triangle_mesh.clear();
    }

    if(key == ' ' )
    {
        findContourColor();
    }
    if(key == 'c')
    {
        curColor = (curColor + 1)%colorQuantizer.getColors().size();
        setContourColor();
        
    }
    if(key == 'd')
    {
        showDebug = !showDebug;
    }
    if(key == 'i')
    {
        count++;
        if(count > 3)count = 1;
        string name = "img" +  ofToString(count) + ".jpg";
        image.loadImage(name);
        resetImg = true;
        curColor  = 0;
        colorQuantizer.getColors().clear();
        colorQuantizer.quantize(image.getPixelsRef());
        findContourColor();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
   
    //threshold = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
    //contourFinder.setThreshold(threshold);
    //contourFinder.findContours(image);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
 //if(drawTriangles)
   // line.addVertex(ofPoint(x, y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    if(image.width >= x && image.height >= y)
    {
       // targetColor = image.getColor(x, y);
        //contourFinder.setTargetColor(targetColor, trackingColorMode);
    
        // triangle lines
       // line.clear();
   
        //if(drawTriangles) {
         //   makeTriangles = true;
            //line.addVertex(ofPoint(x, y));
        //}
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    //if(drawTriangles){
    //    createTriangle();
     //   makeTriangles = false;
    //}
}

//--------------------------------------------------------------
void ofApp::setUpGUI()
{
    gui0 = new ofxUISuperCanvas("SLIDER WIDGETS");
    gui0->setPosition(image.width + 20, 0);
    gui0->addSpacer();
    
    gui0->addLabel("IMAGE APHA");
    gui0->addSlider("alpha channel", 0, 255, &alpha);
    gui0->addSpacer();
    
    gui0->addLabel("RESET");
    gui0->addToggle("reset", &resetImg, 44, 44);
    gui0->addSpacer();
    
    gui0->addLabel("DRAWING FEATURES");
    gui0->addToggle("draw triangles", &makeTriangles, 22, 22);
    gui0->addToggle("draw particles", &makeParticles, 22, 22);
    gui0->addToggle("draw outlines", &makeOutLine, 22, 22);
    gui0->addToggle("draw mesh", &makeMesh, 22, 22);
    gui0->addToggle("draw random", &makeRandom, 22, 22);
    gui0->addSpacer();

    gui0->addLabel("THRESHOLD TYPE");
    gui0->addToggle("Use Saturation", &useSaturation, 22, 22);
    gui0->addSpacer();
    
    gui0->addLabel("THRESHOLD");
    gui0->addSlider("IMAGE", 0.0, 400, &intensityThreshold);
    gui0->addSpacer();
    
    gui0->addLabel("DIMENSION BARS");
    gui0->addSlider("WIDTH", 0.0, 400, &size_recorder.x);
    gui0->addSlider("HEIGHT", 0.0, 400, &size_recorder.y);
    
    gui0->autoSizeToFitWidgets();
    ofAddListener(gui0->newGUIEvent,this,&ofApp::guiEvent);
}
