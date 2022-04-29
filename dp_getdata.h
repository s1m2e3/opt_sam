#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>

class car
{
    public:

        int id;
        double time;
        float lat;
        float lon;
        float speed;
        float heading;
        int intersection;
        int phase;
        int platoon;
        int lane;
        double time_to_arrival;
        double time_between_vehicles;
        int inbound;

    car(int id_, float time_,float lat_,float lon_,float speed_, float heading_, int intersection_, int phase_,int lane_,int platoon_, double time_to_arrival_,double time_between_vehicles_ ,int inbound_)
    {
        id=id_;
        time=time_;
        lat = lat_;
        lon = lon_;
        speed = speed_;
        heading= heading_;
        intersection = intersection_;
        phase = phase_;
        platoon = platoon_;
        time_to_arrival=time_to_arrival_;
        lane = lane_;
        inbound = inbound_;
        time_between_vehicles=time_between_vehicles_;
    }
};

class intersection
{
    public:
            

        struct phase
        {
            int state;
            double p_minendtime;
            double p_maxendtime;
            double p_elapsedtime;
        };
        int id;
        double time;
        phase p1;
        phase p2;
        phase p3;
        phase p4;
        phase p5;
        phase p6;
        phase p7;
        phase p8;
        
    intersection(int id_,double time_)
    {
        id=id_;
        time=time_;
            
    };
};



class DP_Reader
{
    public:

    std::vector<car > veh_vector;
    std::vector<intersection> spat_vector;
    std::map<int,std::map<int,int>> pla_vector;
    std::vector<std::vector<std::string> > Read_csv(std::string filename);
    std::vector<car> Order_veh(std::vector<std::vector<std::string> > veh_string);
    std::vector<intersection> Order_int(std::vector<std::vector<std::string> > spat_string);
    std::map<int,std::map<int,int>> Get_platoons(std::vector<car> veh_vector);

    DP_Reader(std::string filename_vehicles ,std::string filename_spat);
    
};

//std::vector<std::vector<std::string> > DP_Reader::Read_csv(std::string filename);
//
//std::vector<car> DP_Reader::Order_veh(std::vector<std::vector<std::string> > veh_string);
//
//std::vector<std::vector<float>> DP_Reader::Order_int(std::vector<std::vector<std::string> > spat_string);
//
//std::map<int,std::map<int,int>> DP_Reader::Get_platoons(std::vector<car> veh_vector);
