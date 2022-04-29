#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include "dp_getdata.h"



DP_Reader::DP_Reader(std::string filename_vehicles ,std::string filename_spat)
    {
        std::vector<std::vector<std::string> > veh_string = Read_csv(filename_vehicles);
        std::vector<std::vector<std::string> > spat_string = Read_csv(filename_spat);
        veh_vector = Order_veh(veh_string);
        spat_vector = Order_int(spat_string);
        pla_vector = Get_platoons(veh_vector);

    }

std::vector<std::vector<std::string> > DP_Reader::Read_csv(std::string filename)
{
    std::vector<std::vector<std::string> > content;
    std::vector<std::string> row;
    std::string line,word;
    std::fstream file(filename);
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

std::vector<car> DP_Reader::Order_veh(std::vector<std::vector<std::string> > veh_string)
{
    std::vector<car> veh_vector;
    for (int i=0; i!=veh_string.size(); i++ )
    {
        int id=std::stoi(veh_string[i][5]);
        double time=std::stod(veh_string[i][4]);
        float lat=std::stof(veh_string[i][7]);
        float lon=std::stof(veh_string[i][8]);
        float speed=std::stof(veh_string[i][10]);
        float heading=std::stof(veh_string[i][11]);
        int intersection = std::stoi(veh_string[i][21]);
        int phase = std::stoi(veh_string[i][19]);
        int platoon = std::stoi(veh_string[i][23]);
        int lane = std::stoi(veh_string[i][18]);
        int inbound = std::stoi(veh_string[i][16]);
        double time_to_arrival = std::stod(veh_string[i][22]);
        double time_between_veh=std::stod(veh_string[i][24]);
        car new_car(id,time,lat,lon,speed,heading,intersection,phase,lane,platoon,time_to_arrival,time_between_veh,inbound);
        veh_vector.push_back(new_car);
    }
    return veh_vector;   
}

std::vector<intersection> DP_Reader::Order_int(std::vector<std::vector<std::string> > spat_string)
{
    std::vector<intersection> spat_vector;
    for (int i=0; i!=spat_string.size(); i++ )
    {
        
        int id=std::stoi(spat_string[i][5]);
        double time=std::stod(spat_string[i][3]);
        intersection inter(id,time);
        inter.p1.state=std::stoi(spat_string[i][9]);
        inter.p1.p_maxendtime=std::stod(spat_string[i][11]);
        inter.p1.p_minendtime=std::stod(spat_string[i][10]);
        inter.p1.p_elapsedtime=std::stod(spat_string[i][12]);
        inter.p2.state=std::stoi(spat_string[i][9+4]);
        inter.p2.p_maxendtime=std::stod(spat_string[i][11+4]);
        inter.p2.p_minendtime=std::stod(spat_string[i][10+4]);
        inter.p2.p_elapsedtime=std::stod(spat_string[i][12+4]);
        inter.p3.state=std::stoi(spat_string[i][9+8]);
        inter.p3.p_maxendtime=std::stod(spat_string[i][11+8]);
        inter.p3.p_minendtime=std::stod(spat_string[i][10+8]);
        inter.p3.p_elapsedtime=std::stod(spat_string[i][12+8]);
        inter.p4.state=std::stoi(spat_string[i][9+12]);
        inter.p4.p_maxendtime=std::stod(spat_string[i][11+12]);
        inter.p4.p_minendtime=std::stod(spat_string[i][10+12]);
        inter.p4.p_elapsedtime=std::stod(spat_string[i][12+12]);
        inter.p5.state=std::stoi(spat_string[i][9+16]);
        inter.p5.p_maxendtime=std::stod(spat_string[i][11+16]);
        inter.p5.p_minendtime=std::stod(spat_string[i][10+16]);
        inter.p5.p_elapsedtime=std::stod(spat_string[i][12+16]);
        inter.p6.state=std::stoi(spat_string[i][9+20]);
        inter.p6.p_maxendtime=std::stod(spat_string[i][11+20]);
        inter.p6.p_minendtime=std::stod(spat_string[i][10+20]);
        inter.p6.p_elapsedtime=std::stod(spat_string[i][12+20]);
        inter.p7.state=std::stoi(spat_string[i][9+24]);
        inter.p7.p_maxendtime=std::stod(spat_string[i][11+24]);
        inter.p7.p_minendtime=std::stod(spat_string[i][10+24]);
        inter.p7.p_elapsedtime=std::stod(spat_string[i][12+24]);
        inter.p8.state=std::stoi(spat_string[i][9+28]);
        inter.p8.p_maxendtime=std::stod(spat_string[i][11+28]);
        inter.p8.p_minendtime=std::stod(spat_string[i][10+28]);
        inter.p8.p_elapsedtime=std::stod(spat_string[i][12+28]);

        spat_vector.push_back(inter);
    }
    return spat_vector;   
}

 std::map<int,std::map<int,int>> DP_Reader::Get_platoons(std::vector<car> veh_vector)
 {
    std::map<int,std::map<int,int>> pla_return;

    for (int i=0; i!=veh_vector.size(); i++)
    {
        int phase = veh_vector[i].phase;
        int intersection = veh_vector[i].intersection;
        int platoon = veh_vector[i].platoon;
        
        //Check if dictionary is empty for intersection
        if(pla_return.empty())
        {
            pla_return[intersection][phase]=platoon;
        }
        //Check if intersection in dictionary
        else if (pla_return.find(intersection)==pla_return.end())
        {
            pla_return[intersection][phase]=platoon;
        }
        //Check if phase of intersection in dictionary
        else if(pla_return[intersection].find(phase)==pla_return[intersection].end())
        {
            pla_return[intersection][phase]=platoon;
        }
        //If phase at intersection check if platoon is larger
        else if(pla_return[intersection][phase]< platoon)
        {
            pla_return[intersection][phase]=platoon;
        }
        
    }    
    return pla_return;

 }