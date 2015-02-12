//
//  CityParticles.cpp
//  Creative_Image
//
//  Created by Mariale Montenegro on 2/8/15.
//
//

#include "CityParticles.h"


CityParticles::CityParticles(ofPoint pos_, float rad_, ofColor color_, float speed_, vector<ofPoint> waypoints_)
{
    pos    = pos_;
    radius = rad_;
    color  = color_;
    speed  = speed_;
    waypoints = waypoints_;
    curWaypoint = 0;
    for(int i = 0; i < waypoints.size();i++){
        if(waypoints[i] == pos){
            curWaypoint = i;
            break;
        }
    }
}

void CityParticles::update()
{
    // move towards current waypoint
    ofPoint newPos = waypoints[curWaypoint];
    ofPoint direction = newPos - pos;
    direction.normalize();
    direction *= speed;
    
    pos += direction;
    // If has arrived, go to next waypoint
    if((pos - newPos).length() <= 10)
        curWaypoint = (curWaypoint + 1) % waypoints.size();
    
}