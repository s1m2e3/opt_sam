#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include "dp_getdata.h"

Car::Car(int id_, float time_,float lat_,float lon_,float speed_, float heading_, int Intersection_, int phase_,int lane_,int platoon_, double time_to_arrival_,double time_between_vehicles_ ,int inbound_)
{
    id=id_;
    time=time_;
    lat = lat_;
    lon = lon_;
    speed = speed_;
    heading= heading_;
    Intersection = Intersection_;
    phase = phase_;
    platoon = platoon_;
    time_to_arrival=time_to_arrival_;
    lane = lane_;
    inbound = inbound_;
    time_between_vehicles=time_between_vehicles_;
}

Intersection::Intersection(int id_,double time_)
{
    id=id_;
    time=time_;
}


DP_Reader::DP_Reader(std::string filename_vehicles ,std::string filename_spat)
{
    std::vector<std::vector<std::string> > veh_string = Read_csv(filename_vehicles);
    std::vector<std::vector<std::string> > spat_string = Read_csv(filename_spat);
    veh_vector = Order_veh(veh_string);
    spat_vector = Order_int(spat_string);
    pla_vector = Get_platoons(veh_vector);

}

std::vector<std::vector<std::string> > DP_Reader::Read_csv(std::string filename_)
{
    std::vector<std::vector<std::string> > content;
    std::vector<std::string> row;
    std::string line,word;
    std::fstream file(filename_);
    if(file.is_open())
    {
        while(std::getline(file,line))
        {
            row.clear();
            std::stringstream str(line);
            while(getline(str, word, ','))
				row.push_back(word);
			content.push_back(row);
        }
    }
    return content;
}

std::vector<Car> DP_Reader::Order_veh(std::vector<std::vector<std::string> > veh_string_)
{
    std::vector<Car> veh_vector;
    for (int i=0; i!=veh_string_.size(); i++ )
    {
        int id=std::stoi(veh_string_[i][5]);
        double time=std::stod(veh_string_[i][4]);
        float lat=std::stof(veh_string_[i][7]);
        float lon=std::stof(veh_string_[i][8]);
        float speed=std::stof(veh_string_[i][10]);
        float heading=std::stof(veh_string_[i][11]);
        int Intersection = std::stoi(veh_string_[i][21]);
        int phase = std::stoi(veh_string_[i][19]);
        int platoon = std::stoi(veh_string_[i][23]);
        int lane = std::stoi(veh_string_[i][18]);
        int inbound = std::stoi(veh_string_[i][16]);
        double time_to_arrival = std::stod(veh_string_[i][22]);
        double time_between_veh=std::stod(veh_string_[i][24]);
        Car new_Car(id,time,lat,lon,speed,heading,Intersection,phase,lane,platoon,time_to_arrival,time_between_veh,inbound);
        veh_vector.push_back(new_Car);
    }
    return veh_vector;   
}

std::vector<Intersection> DP_Reader::Order_int(std::vector<std::vector<std::string> > spat_string_)
{
    std::vector<Intersection> spat_vector;
    for (int i=0; i!=spat_string_.size(); i++ )
    {
        
        int id=std::stoi(spat_string_[i][5]);
        double time=std::stod(spat_string_[i][3]);
        Intersection inter(id,time);
        inter.p1.state=std::stoi(spat_string_[i][9]);
        inter.p1.p_maxendtime=std::stod(spat_string_[i][11]);
        inter.p1.p_minendtime=std::stod(spat_string_[i][10]);
        inter.p1.p_elapsedtime=std::stod(spat_string_[i][12]);
        inter.p2.state=std::stoi(spat_string_[i][9+4]);
        inter.p2.p_maxendtime=std::stod(spat_string_[i][11+4]);
        inter.p2.p_minendtime=std::stod(spat_string_[i][10+4]);
        inter.p2.p_elapsedtime=std::stod(spat_string_[i][12+4]);
        inter.p3.state=std::stoi(spat_string_[i][9+8]);
        inter.p3.p_maxendtime=std::stod(spat_string_[i][11+8]);
        inter.p3.p_minendtime=std::stod(spat_string_[i][10+8]);
        inter.p3.p_elapsedtime=std::stod(spat_string_[i][12+8]);
        inter.p4.state=std::stoi(spat_string_[i][9+12]);
        inter.p4.p_maxendtime=std::stod(spat_string_[i][11+12]);
        inter.p4.p_minendtime=std::stod(spat_string_[i][10+12]);
        inter.p4.p_elapsedtime=std::stod(spat_string_[i][12+12]);
        inter.p5.state=std::stoi(spat_string_[i][9+16]);
        inter.p5.p_maxendtime=std::stod(spat_string_[i][11+16]);
        inter.p5.p_minendtime=std::stod(spat_string_[i][10+16]);
        inter.p5.p_elapsedtime=std::stod(spat_string_[i][12+16]);
        inter.p6.state=std::stoi(spat_string_[i][9+20]);
        inter.p6.p_maxendtime=std::stod(spat_string_[i][11+20]);
        inter.p6.p_minendtime=std::stod(spat_string_[i][10+20]);
        inter.p6.p_elapsedtime=std::stod(spat_string_[i][12+20]);
        inter.p7.state=std::stoi(spat_string_[i][9+24]);
        inter.p7.p_maxendtime=std::stod(spat_string_[i][11+24]);
        inter.p7.p_minendtime=std::stod(spat_string_[i][10+24]);
        inter.p7.p_elapsedtime=std::stod(spat_string_[i][12+24]);
        inter.p8.state=std::stoi(spat_string_[i][9+28]);
        inter.p8.p_maxendtime=std::stod(spat_string_[i][11+28]);
        inter.p8.p_minendtime=std::stod(spat_string_[i][10+28]);
        inter.p8.p_elapsedtime=std::stod(spat_string_[i][12+28]);

        spat_vector.push_back(inter);
    }
    return spat_vector;   
}

 std::map<int,std::map<int,int>> DP_Reader::Get_platoons(std::vector<Car> veh_vector_)
 {
    std::map<int,std::map<int,int>> pla_return;

    for (int i=0; i!=veh_vector.size(); i++)
    {
        int phase = veh_vector[i].phase;
        int Intersection = veh_vector[i].Intersection;
        int platoon = veh_vector[i].platoon;
        
        //Check if dictionary is empty for Intersection
        if(pla_return.empty())
        {
            pla_return[Intersection][phase]=platoon;
        }
        //Check if Intersection in dictionary
        else if (pla_return.find(Intersection)==pla_return.end())
        {
            pla_return[Intersection][phase]=platoon;
        }
        //Check if phase of Intersection in dictionary
        else if(pla_return[Intersection].find(phase)==pla_return[Intersection].end())
        {
            pla_return[Intersection][phase]=platoon;
        }
        //If phase at Intersection check if platoon is larger
        else if(pla_return[Intersection][phase]< platoon)
        {
            pla_return[Intersection][phase]=platoon;
        }
        
    }    
    return pla_return;

 }