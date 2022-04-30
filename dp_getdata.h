#ifndef dp_getdata
#define dp_getdata

#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>

class Car
{
    public:

        int id;
        double time;
        float lat;
        float lon;
        float speed;
        float heading;
        int Intersection;
        int phase;
        int platoon;
        int lane;
        double time_to_arrival;
        double time_between_vehicles;
        int inbound;

    Car(int id_, float time_,float lat_,float lon_,float speed_, float heading_, int Intersection_, int phase_,int lane_,int platoon_, double time_to_arrival_,double time_between_vehicles_ ,int inbound_);
    
};

class Intersection
{
    struct phase{
    int state;
    double p_minendtime;
    double p_maxendtime;
    double p_elapsedtime;
    };
    
    
    public:
    
        int id;
        double time;
        
        phase p1,p2,p3,p4,p5,p6,p7,p8;
        
        
    Intersection(int id_,double time_);
    
};

class DP_Reader
{
    public:
        std::vector<Car > veh_vector;
        std::vector<Intersection> spat_vector;
        std::map<int,std::map<int,int>> pla_vector;
    
    std::vector<std::vector<std::string> > Read_csv(std::string filename_);
    std::vector<Car> Order_veh(std::vector<std::vector<std::string> > veh_string_);
    std::vector<Intersection> Order_int(std::vector<std::vector<std::string> > spat_string_);
    std::map<int,std::map<int,int>> Get_platoons(std::vector<Car> veh_vector_);

    DP_Reader(std::string filename_vehicles ,std::string filename_spat);
    
};

#endif
