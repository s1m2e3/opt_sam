#include "geoCoord2.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

std::vector<std::vector<std::string> > Read_csv(std::string filename);
std::vector<std::vector<double>> process_string(std::vector<std::vector<std::string> > string_vector);

int main()
{
    
    std::vector<std::vector<std::string> > content = Read_csv("processed_lanes.csv");
    std::cout<<"read csv";
    std::vector<std::vector<double>> final_coordinates = process_string(content);
    std::cout<<"processed string";
    std::ofstream Myfile("coordinates.txt");
    for (int row=0;row!=final_coordinates.size();row++)
    {
        
        Myfile <<std::to_string(final_coordinates[row][0])+","+std::to_string(final_coordinates[row][1])+","+std::to_string(final_coordinates[row][2])+","+std::to_string(final_coordinates[row][3])+","+std::to_string(final_coordinates[row][4])+","+std::to_string(final_coordinates[row][5])+"\n";
    }
    Myfile.close();
    return 0;
}

std::vector<std::vector<std::string> > Read_csv(std::string filename)
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

std::vector<std::vector<double>> process_string(std::vector<std::vector<std::string> > string_vector)
{
    std::vector<std::vector<double>> coordinates;
    for (int i=0; i!=string_vector.size();i++)
    {
        
        geoCoord point;
        geoCoord point2;
        //std::cout<<"start x row_"<<string_vector[i][0]<<"__"<<std::atof(string_vector[i][5].c_str())<<"\n";
        //std::cout<<typeid(string_vector[i][5].c_str()).name();
        //std::cout<<"start y row_"<<string_vector[i][0]<<"__"<<std::atof(string_vector[i][6].c_str())<<"\n";
        try
        {
        
        double x1=std::atof(string_vector[i][4].c_str());
        double y1=std::atof(string_vector[i][5].c_str());   
        double x2=std::atof(string_vector[i][7].c_str());
        double y2=std::atof(string_vector[i][8].c_str());


        double ref_long = point.dms2d(-112,-8,-10.17924);
        double ref_lat = point.dms2d(33,50,20.94756);
        point.init(ref_long,ref_lat,double(507));
        point.local2ecef(x1,y1,0);
        point.ecef2lla();
        
        std::vector<double> row;
        double push=point.longitude;
        row.push_back(push);
        push =point.latitude;
        row.push_back(push);
        push = point.altitude;
        row.push_back(push);
        
        ref_long = point2.dms2d(-112,-8,-10.17924);
        ref_lat = point2.dms2d(33,50,20.94756);
        point2.init(ref_long,ref_lat,double(507));
        point2.local2ecef(x2,y2,0);
        point2.ecef2lla();
        push=point2.longitude;
        row.push_back(push);
        push=point2.latitude;    
        row.push_back(push);
        push = point2.altitude;
        row.push_back(push);
        coordinates.push_back(row); 

        }
        catch (const std::exception& e)
        {
            std::cout <<e.what();
        }

        




    }   
    return coordinates;   
    
}