//
//  CityParticles.h
//  Creative_Image
//
//  Created by Mariale Montenegro on 2/8/15.
//
//

#ifndef __Creative_Image__CityParticles__
#define __Creative_Image__CityParticles__

#include <stdio.h>
#include "ofMain.h"

class CityParticles {
public:
    ofPoint pos;
    float radius;
    ofColor color;
    float speed;
  
    vector<ofPoint> waypoints;
    int curWaypoint;
    
    //constructor
    CityParticles(ofPoint pos_, float rad_, ofColor color_, float speed_, vector<ofPoint> waypoints_);
    void update();
    
};

#endif /* defined(__Creative_Image__CityParticles__) */
