#include <iostream>
#include "/opt/ibm/ILOG/CPLEX_Studio221/cplex/include/ilcplex/cplex.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/cplex/include/ilcplex/ilocplex.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/concert/include/ilconcert/ilomodel.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/concert/include/ilconcert/iloenv.h"
#include "dp_solver.h"
#include <cmath>
#include <limits>

DP_Solver::DP_Solver(DP_Reader reader_)
{
    reader=reader_;
    Writemymodel(reader);
    Solvemymodel();
    
    float UB=100000000000000;
    float LB=0; 
    //while(UB-LB>10)
    //{
    //    Solvemymodel();
//
    //} 
}

void DP_Solver::Writemymodel(DP_Reader reader)
{
    bool opt_by_vehicles =False;
    bool phase8 = True;
    //Sets
    
    //Number of intersections
    int int_num = reader.spat_vector.size();

    
    if (phase8){
        int pha_num=8;
    }
    //Number of phases
    else {
        int pha_num=12;
    }
    
    //Number of cycles
    int k=1

    //Big enough number
    int m=1000000000;

    // Vector of lanes per phase per intersection  
    std::map<int,std::map<int,std::vector<int> > > lanes_per_phase_per_int;
    
    // Number of platoons per lane per phase per intersection
    std::map<int,std::map<int,std::map<int,int> > > platoon_num;
    //Parameters of vehicle,extracted to reduce iterations

    std::map<int,std::map<int,std::map<int,std::map<int,double> > > > head_arr_time_map;
    std::map<int,std::map<int,std::map<int,std::map<int,double> > > > tail_arr_time_map;
    std::map<int,std::map<int,std::map<int,std::map<int,double> > > > time_bet_veh_map;
    std::map<int,std::map<int,std::map<int,std::map<int,int> >  > > num_veh_per_platoon;
    

    for (int i=0; i!=reader.veh_vector.size();i++)
    {
        int belonging_intersection = reader.veh_vector[i].intersection;
        int belonging_phase = reader.veh_vector[i].phase;
        int belonging_lane = reader.veh_vector[i].lane;
        int belonging_platoon = reader.veh_vector[i].lane;
        int inbound = reader.veh_vector[i].inbound;
        double arr_time = reader.veh_vector[i].time_to_arrival;
        double time_bet_vehicle = reader.veh_vector[i].time_between_vehicles;
        num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1
        if (inbound==1)
        {
            //Check if intersection exists in map:
            if (lanes_per_phase_per_int.find(belonging_intersection)==lanes_per_phase_per_int.end())
            {
                lanes_per_phase_per_int[belonging_intersection][belonging_phase].push_back(belonging_lane);
                platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]=belonging_platoon;
                head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                time_bet_veh_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=time_bet_vehicle;
                tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1
        

            }
            //Check if phase exists in intersection
            else if (lanes_per_phase_per_int[belonging_intersection].find(belonging_phase)==lanes_per_phase_per_int[belonging_intersection].end())
            {
                lanes_per_phase_per_int[belonging_intersection][belonging_phase].push_back(belonging_lane);
                platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]=belonging_platoon;
                head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                time_bet_veh_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=time_bet_vehicle;
                num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1
        
            }
            //Check if lane exists in phase in intersection
            else   
            {

                int counter = 0;
                for(int j=0;j!=lanes_per_phase_per_int[belonging_intersection][belonging_phase].size();j++)
                {
                    if (belonging_lane == lanes_per_phase_per_int[belonging_intersection][belonging_phase][j])
                    {
                        counter++;
                    }
                }
                if (counter==0)
                {
                    lanes_per_phase_per_int[belonging_intersection][belonging_phase].push_back(belonging_lane);
                }

                if (platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase].find(belonging_lane)==platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase].end())
                {
                    platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]=belonging_platoon;
                    head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                    tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                    time_bet_veh_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=time_bet_vehicle;
                    num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1
        

                }
                else if (platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]< belonging_platoon)
                {
                    platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]=belonging_platoon;
                
                    if (head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane].find(belonging_platoon)==head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane].end())

                    {
                        head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                        tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                        num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1
                        time_bet_veh_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=time_bet_vehicle;
        
                    }
                    else if (head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon] > arr_time)
                    {
                        head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                        
                    }
                    else if (tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon] < arr_time)
                    {
                        tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                        
                    }
                    else
                    {
                        num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]++;
                    }
                }
            }
        }

    }

    // Number of lanes per phase per intersection 
    std::map<int,std::map<int,int>> lanes_num;

    std::map<int,std::map<int,std::vector<int>>>::iterator it;
    for (it = lanes_per_phase_per_int.begin();it != lanes_per_phase_per_int.end(); it++)
    {
        int iter_intersection=it.first;
        std::map<int,std::vector<int>>::iterator it2
        for (it2= lanes_per_phase_per_int[iter_intersection].begin();it2!=lanes_per_phase_per_int[iter_intersection].end(); it2++)
        {
            int iter_phase=it2.first;
            lanes_num[iter_intersection][iter_phase]=lanes_per_phase_per_int[iter_intersection][iter_phase].size();
        }
    }


    //Parameters
    
    //Elapsed, Min and Max Green time per phase per intersection
    IloCplex::IloArray<IloCplex::NumArray> gmin_t(myenv,int_num);
    IloCplex::IloArray<IloCplex::NumArray> gmax_t(myenv,int_num);
    IloCplex::IloArray<IloCplex::NumArray> e_t(myenv,int_num);
    
    for (int i=0; i!=int_num;i++)
    {
        intersection int_ = reader.spat_vector[i];
        phase int_p1= int_.p1;
        phase int_p2= int_.p2;
        phase int_p3= int_.p3;
        phase int_p4= int_.p4;
        phase int_p5= int_.p5;
        phase int_p6= int_.p6;
        phase int_p7= int_.p7;
        phase int_p8= int_.p8;
        
        gmin_t[i]=IloCplex::NumArray(myenv,pha_num);
        gmax_t[i]=IloCplex::NumArray(myenv,pha_num);
        e_t[i]=IloCplex::NumArray(myenv,pha_num);
        for (int j=0; j!=pha_num;j++)
        {
            if (j+1==1)
            {
                gmin_t[i][j]=int_p1.p_minendtime;
                gmax_t[i][j]=int_p1.p_maxendtime;
                e_t[i][j]=int_p1.p_elapsedtime;
            }
            else if (j+1==2)
            {
                gmin_t[i][j]=int_p2.p_minendtime;
                gmax_t[i][j]=int_p2.p_maxendtime;
                e_t[i][j]=int_p2.p_elapsedtime;
            }
            else if (j+1==3)
            {
                gmin_t[i][j]=int_p3.p_minendtime;
                gmax_t[i][j]=int_p3.p_maxendtime;
                e_t[i][j]=int_p3.p_elapsedtime;
            }
            else if (j+1==4)
            {
                gmin_t[i][j]=int_p4.p_minendtime;
                gmax_t[i][j]=int_p4.p_maxendtime;
                e_t[i][j]=int_p4.p_elapsedtime;
            }
            else if (j+1==5)
            {
                gmin_t[i][j]=int_p5.p_minendtime;
                gmax_t[i][j]=int_p5.p_maxendtime;
                e_t[i][j]=int_p5.p_elapsedtime;
            }
            else if (j+1==6)
            {
                gmin_t[i][j]=int_p6.p_minendtime;
                gmax_t[i][j]=int_p6.p_maxendtime;
                e_t[i][j]=int_p6.p_elapsedtime;
            }
            else if (j+1==7)
            {
                gmin_t[i][j]=int_p7.p_minendtime;
                gmax_t[i][j]=int_p7.p_maxendtime;
                e_t[i][j]=int_p7.p_elapsedtime;
            }
            else if (j+1==8)
            {
                gmin_t[i][j]=int_p8.p_minendtime;
                gmax_t[i][j]=int_p8.p_maxendtime;
                e_t[i][j]=int_p8.p_elapsedtime;
            }
        }
    }
    
    //Red Clearance Time and Yellow Time:
    float y_t= 3;
    float r_t=1.5;
    //Maximum slack time:
    float S_t=100;
    //Shockwave time:
    float Sh_t=2;
    
    //Pending Update for cycle 2
    //Head and Tail Arrival Time to Intersection per platoon per lane per phase per intersection:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::Array<IloCplex::IloNumArray>>>> ha_t(myenv,int_num);
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::Array<IloCplex::IloNumArray>>>> ta_t(myenv,int_num);
    //Time distance between vehicles per platoon per lane per phase per intersection
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::Array<IloCplex::IloNumArray>>>> tpv_t(myenv,int_num);
    //Number of vehicles per platoon per Intersection per Phase:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::Array<IloCplex::IloNumArray>>>> v_n(myenv,int_num);
    for (int c=0; c!=k;c++)
    {   

        ha_t[c]=IloCplex::IloArray;
        ta_t[c]=IloCplex::IloArray;
        v_n[c]=IloCplex::IloArray;
        tpv_t[c]=IloCplex::IloArray;
        for (int i=0; i!=int_num;i++)
    {
        ha_t[c][i]=IloCplex::IloArray;
        ta_t[c][i]=IloCplex::IloArray;
        tpv_t[c][i]=IloCplex::IloArray;
        v_n[c][i]=IloCplex::IloArray;
        
        for (int j=0; j!=pha_num;j++)
        {
            ha_t[c][i][j]=IloCplex::IloArray;
            ta_t[c][i][j]=IloCplex::IloArray;
            tpv_t[c][i][j]=IloCplex::IloArray;
            v_n[c][i][j]=IloCplex::IloArray;
            for (int l=0;l!=lanes_num[i][j];l++)
            {
                ha_t[c][i][j][l]=IloCplex::IloNumArray;
                ta_t[c][i][j][l]=IloCplex::IloNumArray;
                tpv_t[c][i][j][l]=IloCplex::IloNumArray;
                v_n[c][i][j][l]=IloCplex::IloNumArray;
                for (int m=0; m!=platoon_num[i][j][l];m++)
                {
                    ha_t[c][i][j][l][m]=head_arr_time_map[i][j][l][m];
                    ta_t[c][i][j][l][m]=tail_arr_time_map[i][j][l][m];
                    tpv_t[c][i][j][l][m]=time_bet_veh_map[i][j][l][m];
                    v_n[c][i][j][l][m]=num_veh_per_platoon[i][j][l][m];

                }    
            }
        }
    }
    }
    



    //Time from one Intersection to the next intersection: 
    IloCplex::IloIntArray ni_t(myenv,3);
    ni_t[0]=20;
    ni_t[1]=20
    ni_t[2]=20
    
    
    //Variables instantiation and naming
    std::stringstream name;

    //Green Times per intersection per phase per cycle
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumVarArray>> g_t(myenv,k);
    //Initial Time of green per intersection per phase per cycle
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumVarArray>> i_t(myenv,k);
     //Slack Time per intersection per phase per cycle
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumVarArray>> s_t(myenv,k);
    //Phase Duration per intersection per phase per cycle
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumVarArray>> v_t(myenv,k);
    for (int m=0;m!=k;m++)
    {

        g_t[m]=IloCplex::IloArray(myenv,int_num);
        i_t[m]=IloCplex::IloArray(myenv,int_num);
        s_t[m]=IloCplex::IloArray(myenv,int_num);
        v_t[m]=IloCplex::IloArray(myenv,int_num);
            for (int i=0;i!=int_num;i++)
        {
            g_t[m][i]= IloCplex::IloNumVarArray(myenv,pha_num);
            i_t[m][i]= IloCplex::IloNumVarArray(myenv,pha_num);
            s_t[m][i]= IloCplex::IloNumVarArray(myenv,pha_num);
            v_t[m][i]= IloCplex::IloNumVarArray(myenv,pha_num);
            for (int j=0; j!=pha_num;j++)
            {
                name << "green_time_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                g_t[m][i][j]=IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name); 
                name << "initial_time_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                i_t[m][i][j]=IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name);  
                name << "slack_time_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                s_t[m][i][j]=IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name); 
                name << "phase_duration_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                v_t[m][i][j]=IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name);
            }
        }
    }
    
    //Queue Delay at intersection per platoon per lane per phase per intersection at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> d_s(myenv,k);
    //Signal Delay at intersection per phase per cycle per platoon at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>>  d_q(myenv,k);
    //Queue Delay at next intersection per phase per cycle per platoon at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>>  dn_s(myenv,k);
    //Signal Delay at next intersection per phase per cycle per platoon at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>>  dn_q(myenv,k);
    //Number of Passing Vehicles at next intersection per phase per cycle per platoon at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> pv_n(myenv,k);
    //Number of Passing Vehicles at next intersection per phase per cycle per platoon which will pass to the next intersection at cycle k: 
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> npv_n(myenv,k);
    //State(0,1) of ith vehicle in platoon subject to signal delay at intersection per phase per cycle per platoon at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> pvs_b(myenv,k);
    //State(0,1) of ith vehicle in platoon  subject to signal delay at next intersection per phase per cycle per platoon which will pass to the next intersection at cycle k: 
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> npvs_b(myenv,k);
    //State(0,1) of ith vehicle in platoon  subject to queue delay at next intersection per phase per cycle per platoon at cycle k:
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> pvq_b(myenv,k);
    //State(0,1) of ith vehicle in platoon  subject to queue delay at next intersection per phase per cycle per platoon which will pass to the next intersection at cycle k: 
    IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloArray<IloCplex::IloNumArray<IloCplex::IloNumVarArray>>>> npvq_b(myenv,k);
    
    for(int n=0;n!=k;n++)
    {

        d_s[n]= IloCplex::IloArray(myenv,int_num);
        d_q[n]= IloCplex::IloArray(myenv,int_num);
        dn_s[n]= IloCplex::IloArray(myenv,int_num);
        dn_q[n]= IloCplex::IloArray(myenv,int_num);
        pv_n[n]= IloCplex::IloArray(myenv,int_num);
        npv_n[n]= IloCplex::IloArray(myenv,int_num);
        pvs_b[n]= IloCplex::IloArray(myenv,int_num);
        npvs_b[n]= IloCplex::IloArray(myenv,int_num);
        pvq_b[n]= IloCplex::IloArray(myenv,int_num);
        npvq_b[n]= IloCplex::IloArray(myenv,int_num);

        for (int i=0;i!=int_num;i++)
        {
            d_s[n][i]= IloCplex::IloArray(myenv,pha_num);
            d_q[n][i]= IloCplex::IloArray(myenv,pha_num);
            dn_s[n][i]= IloCplex::IloArray(myenv,pha_num);
            dn_q[n][i]= IloCplex::IloArray(myenv,pha_num);
            pv_n[n][i]= IloCplex::IloArray(myenv,pha_num);
            npv_n[n][i]= IloCplex::IloArray(myenv,pha_num);
            pvs_b[n][i]= IloCplex::IloArray(myenv,pha_num);
            npvs_b[n][i]= IloCplex::IloArray(myenv,pha_num);
            pvq_b[n][i]= IloCplex::IloArray(myenv,pha_num);
            npvq_b[n][i]= IloCplex::IloArray(myenv,pha_num);
            



            for (int j=0; j!=pha_num;j++)
            {
                
                d_s[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                d_q[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                dn_s[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                dn_q[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                pv_n[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                npv_n[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                pvs_b[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                npvs_b[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                pvq_b[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
                npvq_b[n][i][j]= IloCplex::IloArray(myenv,lanes_num[i][j]);
            
                for (int l=0; l!=lanes_num[i][j]; l++)
                {
                    d_s[n][i][j][l]= IloCplex::IloNumVarArray(myenv,platoon_num[i][j][l]);
                    d_q[n][i][j][l]= IloCplex::IloNumVarArray(myenv,platoon_num[i][j][l]);
                    dn_s[n][i][j][l]= IloCplex::IloNumVarArray(myenv,platoon_num[i][j][l]);
                    dn_q[n][i][j][l]= IloCplex::IloNumVarArray(myenv,platoon_num[i][j][l]);
                    pv_n[n][i][j][l]= IloCplex::IloNumVarArray(myenv,platoon_num[i][j][l]);
                    npv_n[n][i][j][l]= IloCplex::IloNumVarArray(myenv,platoon_num[i][j][l]);
                    pvs_b[n][i][j][l]= IloCplex::IloArray(myenv,platoon_num[i][j][l];
                    npvs_n[n][i][j][l]= IloCplex::IloArray(myenv,platoon_num[i][j][l]);
                    pvq_b[n][i][j][l]= IloCplex::IloArray(myenv,platoon_num[i][j][l]);
                    npvq_b[n][i][j][l]= IloCplex::IloArray(myenv,platoon_num[i][j][l]);
            
    
                    for (int m=0; m!=platoon_num[i][j][l];m++)
                    {
                    
                        name << "signal_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m) ;
                        d_s[n][i][j][l][m]= IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name);
                        name << "queue_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m) ;
                        d_q[n][i][j][l][m]= IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name);
                        name << "next_signal_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m);
                        dn_s[n][i][j][l][m]= IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name);
                        name << "next_queue_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m) ;
                        dn_q[n][i][j][l][m]= IloCplex::IloNumVar(myenv,0,IloCplex::IloInfinity,IloFloat,name);
                        name << "passing_vehicles_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m) ;
                        pv_n[n][i][j][l][m]= IloCplex::IloNumVar(myenv,0,v_n[n][i][j][l][m],IloFloat,name);
                        name << "next_passing_vehicles_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m);
                        npv_n[n][i][j][l][m]= IloCplex::IloNumVar(myenv,0,v_n[n][i][j][l][m],IloFloat,name);
                        
                        pvs_b[n][i][j][l][m]= IloCplex::IloVarArray(myenv,v_n[n][i][j][l][m];
                        npvs_n[n][i][j][l][m]= IloCplex::IloVarArray(myenv,v_n[n][i][j][l][m]);
                        pvq_b[n][i][j][l][m]= IloCplex::IloVarArray(myenv,v_n[n][i][j][l][m]);
                        npvq_b[n][i][j][l][m]= IloCplex::IloVarArray(myenv,v_n[n][i][j][l][m]);
                                                
                        for (int o=0; o!=v_n[n][i][j][l][m];o++)
                        {
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_signal_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m) ;
                            pvs_b[n][i][j][l][m][o]= IloCplex::IloNumVar(myenv,0,1,IloFloat,name);
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_signal_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m);
                            npvs_b[n][i][j][l][m][o]= IloCplex::IloNumVar(myenv,0,1,IloFloat,name);
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_queue_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m) ;
                            pvq_b[n][i][j][l][m][o]= IloCplex::IloNumVar(myenv,0,1,IloFloat,name);
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_queue_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(m);
                            npvq_b[n][i][j][l][m][o]= IloCplex::IloNumVar(myenv,0,1,IloFloat,name);
                        }
                    }                  
                }       
            } 
        }

    }
        
    

    //Constraints 1: Set of Precedence Times and Time relationship

    //Time relationship between phases:
    for (int i=0; i!=int_num; i++)
    {
        for (int j=0; j!=k; j++)
        {
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"2_1";
            IloRange r1(myenv,i_t[j][i][2],i_t[j][i][1]+s_t[j][i][1]+v_t[j][i][1],i_t[j][i][2],name);
            mymodel.add(r1);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"6_5";
            IloRange r2(myenv,i_t[j][i][6],i_t[j][i][5]+s_t[j][i][5]+v_t[j][i][5],i_t[j][i][6],name);
            mymodel.add(r2);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"3_2";
            IloRange r3(myenv,i_t[j][i][3],i_t[j][i][2]+s_t[j][i][2]+v_t[j][i][2],i_t[j][i][3],name);
            mymodel.add(r3);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"3_6";
            IloRange r4(myenv,i_t[j][i][3],i_t[j][i][6]+s_t[j][i][6]+v_t[j][i][6],i_t[j][i][3],name);
            mymodel.add(r4);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"7_2";
            IloRange r5(myenv,i_t[j][i][7],i_t[j][i][2]+s_t[j][i][2]+v_t[j][i][2],i_t[j][i][7],name);
            mymodel.add(r5);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"7_6";
            IloRange r6(myenv,i_t[j][i][7],i_t[j][i][6]+s_t[j][i][6]+v_t[j][i][6],i_t[j][i][7],name);
            mymodel.add(r6);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"4_3";
            IloRange r7(myenv,i_t[j][i][4],i_t[j][i][3]+s_t[j][i][3]+v_t[j][i][3],i_t[j][i][4],name);
            mymodel.add(r7);
            name="Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"8_7";
            IloRange r8(myenv,i_t[j][i][8],i_t[j][i][7]+s_t[j][i][7]+v_t[j][i][7],i_t[j][i][8],name);
            mymodel.add(r8);

        }
    }

   //Relationship between cycle times//Double check:
    if (k>1)
    {
            for (int i=0; i!=int_num; i++)
        {
            {
                IloRange r1(myenv,i_t[1][i][1],i_t[0][i][4]+v_t[0][i][4],i_t[1][i][1]);
                IloRange r2(myenv,i_t[1][i][1],i_t[0][i][8]+v_t[0][i][8],i_t[1][i][1]);
                IloRange r3(myenv,i_t[1][i][5],i_t[0][i][4]+v_t[0][i][4],i_t[1][i][5]);
                IloRange r4(myenv,i_t[1][i][5],i_t[0][i][8]+v_t[0][i][8],i_t[1][i][5]);
                mymodel.add(r1);
                mymodel.add(r2);
                mymodel.add(r3);
                mymodel.add(r4);
                
            }
        }
    }
    

    //Composition of variable time:

    for (int i=0; i!=int_num; i++)
    {
        
        for (int j=0; j!=k; j++)
        {
            for (int p=0; p!=phase_num;p++)
            {
                name="Composition_Variable_time_cycle"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(p);
                IloRange r1(myenv,v_t[j][i][p],g_t[j][i][p]+y_t+r_t,v_t[j][i][p],name);
                
            }
        }
    }


    //Maximum allowable slack time:
    
    for (int i=0;i!=int_num;i++)
    {
        IloCplex::IloExp exp(env);
        for (int j=0;j!=k;j++)
        {
            for (int l=0;l!=phase_num;l++)
            {
                exp+=s_t[j][i][l];
            }
        }
        name="Maximum_slack_time_intersection"<<std::to_string(i);
        IloRange ri(myenv,0,exp,S_t,name);
    }

    //Ranges for future green times and Initialize time for current phases:
    //Check with Larry differences between initial time for ring 1 and ring 2
    for (int l=0;l!=k;l++)
    {
        for (int i=0;i!=int_num;i++)
        {
            int p1_state=reader.spat_vector[i].p1.state;
            int p2_state=reader.spat_vector[i].p2.state;
            int p3_state=reader.spat_vector[i].p3.state;
            int p4_state=reader.spat_vector[i].p4.state;
            int p5_state=reader.spat_vector[i].p5.state;
            int p6_state=reader.spat_vector[i].p6.state;
            int p7_state=reader.spat_vector[i].p7.state;
            int p8_state=reader.spat_vector[i].p8.state;

            double it=reader.spat_vector[i].time;
            
            for (int j=0;j!=pha_num;j++)
            {
                name="Ranges_green_time_cycle"<<std::to_string(l)<<"intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)
                IloRange r1(myenv,gmin_t[i][j],g_t[l][i][j],gmax_t[i][j],name);
                name="Minimum_green_time_cycle"<<std::to_string(l)<<"intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)
                IloRange r2(myenv,gmin_t[i][j]-e_t[i][j],g_t[l][i][j],IloCplex::IloInfinity,name);
                name="Initial_time_cycle"<<std::to_string(l)<<"intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)
                IloRange r3(myenv,it,i_t[0][i][p],it,name);
                if(p+1==1 && p1_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                    
                }
                else if(p+1==2 && p2_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==3 && p3_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==4 && p4_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==5 && p5_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==6 && p6_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==7 && p7_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==8 && p8_state==1)
                {
                    mymodel.add(r2);
                    mymodel.add(r3);
                }
                else if(p+1==1 && (p1_state==2||p1_state==3))
                {
                    mymodel.add(r1);
                    
                }
                else if(p+1==2 && (p2_state==2||p2_state==3))
                {
                    mymodel.add(r1);
                }
                else if(p+1==3 && (p3_state==2||p3_state==3))
                {
                    mymodel.add(r1);
                }
                else if(p+1==4 && (p4_state==2||p4_state==3))
                {
                    mymodel.add(r1);
                }
                else if(p+1==5 && (p5_state==2||p5_state==3))
                {
                    mymodel.add(r1);
                }
                else if(p+1==6 && (p6_state==2||p6_state==3))
                {
                    mymodel.add(r1);
                }
                else if(p+1==7 && (p7_state==2||p7_state==3))
                {
                    mymodel.add(r1);
                }
                else if(p+1==8 && (p8_state==2||p8_state==3))
                {
                    mymodel.add(r1);
                }
            }
        }
    }
    //Constraints 2: Number of passing vehicles at current and downstream intersection and update with next cycle
    for (int c=0; c!=k; c++)
    {
        for (int i=0; i!=int_num; i++)
        {
            for (int j=0; j!=pha_num ; j++)
            {
                for(int l=0; l!=lanes_num[i][j]; l++)
                {
                    for(int p=0;p!=platoon_num[i][j][l]; p++)
                    {
                        if (c>0)
                        {
                            name="update_of_passing_vehicles_intersection"<< std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            IloRange r2(myenv,0,pv_n[c][i][j][l][p],v_n[c][i][j][l][p]-pv_n[c-1][i][j][l][p],name);
                            mymodel.add(r2);
                        }
                        
                        name="passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                        IloRange r1(myenv,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p],i_t[c][i][j]+v_t[c][i][j],IloInfinity,name);
                        mymodel.add(r1);
                        //W-E downstream pass
                        if(i==0 || i==1)
                        {
                            if (j==6)
                            {
                                name="downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r3(myenv,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i+1][j]+v_t[c][i+1][j],IloInfinity,name);
                                mymodel.add(r3);
                            }
                            else if(j==7)
                            {
                                name="downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r4(myenv,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i+1][6]+v_t[c][i+1][6],IloInfinity,name);
                                mymodel.add(r4);

                            }
                        }
                        //E-W downstream pass
                        else if(i==1 || 2)
                        {
                            if (j==2)
                            {
                                name="downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r5(myenv,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i-1][j]+v_t[c][i-1][j],IloInfinity,name);
                                mymodel.add(r5);
                            }
                            else if(j==3)
                            {

                                name="downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r6(myenv,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i+1][2]+v_t[c][i+1][2],IloInfinity,name);
                                mymodel.add(r6);
                            }
                        }
                    }
                }
            }
        }
    }     

    //Constraints 3 : Delay Calculation and relationship with time

    //Signal Delay calculation at current intersection
    //Number of Vehicles Subject to Signal Delay at current intersection and at forward intersection
    IloExpr expr1(myenv);
    IloExpr expr2(myenv);
    IloExpr expr3(myenv);
    IloExpr expr4(myenv);
    IloExpr expr5(myenv);
    IloExpr expr6(myenv);
    IloExpr expr7(myenv);
    for (int c=0; c!=k; c++)
    {
        for (int i=0; i!=int_num; i++)
        {
            for (int j=0; j!=pha_num ; j++)
            {
                for(int l=0; l!=lanes_num[i][j]; l++)
                {
                    for(int p=0;p!=platoon_num[i][j][l]; p++)
                    {
                        for (int o=0; o!=v_n[c][i][j][k][l][p]; o++)
                        {
                            name="computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            IloRange r1(myenv,i_t[c][i][j]-ha_t[c][i][j][l][p]-o*tpv_t[c][i][j][l][p],M*pvs_b[c][i][j][l][p][o],IloInfinity,name);
                            expr1+=(tpv_t[c][i][j][l][p]*o)*pvs_b[c][i][j][l][p][o]);
                            mymodel.add(r1);
                            
                            if (i==0 ||i==1 )
                            {
                                if(j==6)
                                {
                                    name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r2(myenv,i_t[c][i+1][j]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    name="given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r3(myenv,i_t[c][i+1][j]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    expr2+=(tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                    
                                    mymodel.add(r2);
                                    mymodel.add(r3);
                                }
                                if (j==7)
                                {
                                    name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r4(myenv,i_t[c][i+1][6]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    name="given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r5(myenv,i_t[c][i+1][6]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    expr3+=(tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                    
                                    mymodel.add(r4);
                                    mymodel.add(r5);
                                }
                            }
                            else if(i==1||i==2)
                            {
                                if(j==2)
                                {
                                    name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r6(myenv,i_t[c][i-1][j]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    name="given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r7(myenv,i_t[c][i-1][j]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    expr4+=(tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                    
                                    
                                    mymodel.add(r6);
                                    mymodel.add(r7);
                                }
                                if (j==3)
                                {
                                    name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r8(myenv,i_t[c][i-1][2]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    name="given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    IloRange r9(myenv,i_t[c][i-1][2]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    expr5+=(tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                    
                                    mymodel.add(r8);
                                    mymodel.add(r9);

                                }
                            }
                        }
                        name="signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                        IloRange r10(myenv,expr1, d_s[c][i][j][l][p],IloInfinity,name);
                        mymodel.add(r10);
                        if (i==1||i==0 && j==6)
                        {
                           name="future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            IloRange r11(myenv,expr2,dn_s[c][i][j][l][p],IloInfinity,name);
                            mymodel.add(r11); 
                        }
                        
                        else if (i==1||i==0 && j==7)
                        {
                            name="future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            IloRange r12(myenv,expr3,dn_s[c][i][j][l][p],IloInfinity,name);
                            mymodel.add(r12);
                        }
                        else if(i==1||i==2 && j==2)
                        {
                            name="future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            IloRange r13(myenv,expr4,dn_s[c][i][j][l][p],IloInfinity,name);
                            mymodel.add(r13);
                        }
                        else if(i==1||i==2 && j==3)
                        {
                            name="future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            IloRange r14(myenv,expr5,dn_s[c][i][j][l][p],IloInfinity,name);
                            mymodel.add(r14);
                        }
                        expr1.clear();
                        expr2.clear();
                        expr3.clear();
                        expr4.clear();
                        expr5.clear();         
                    }
                }
            }
        }
    } 

    
    //Queue Delay calculation at current intersection
    //Number of Vehicles Subject to Queue Delay at Current intersection
    IloExpr expr20;
    IloExpr expr21;
    IloExpr expr22;
    IloExpr expr23;
    IloExpr expr24;
    IloExpr expr25;
    IloExpr expr26;
    IloExpr expr27;
    IloExpr expr28;
    IloExpr expr29;
    for (int c=0; c!=k; c++)
    {
        for (int i=0; i!=int_num; i++)
        {
            for (int j=0; j!=pha_num ; j++)
            {
                for(int l=0; l!=lanes_num[i][j]; l++)
                {
                    for(int p=0;p!=platoon_num[i][j][l]; p++)
                    {
                        for (int o=0; o!=v_n[c][i][j][k][l][p]; o++)
                        {
                            if (p==0)
                            {
                                expr1+=(pvs_b[c][i][j][l][p][o]);
                            }
                            
                            else if (j==6 && p==platoon_num[i][j][l]&& i!=2)
                            {
                                expr3+=(pvs_b[c][i+1][j][l][p][o]);
                            }
                            else if (j==7 && p==platoon_num[i][j][l]&& i!=2)
                            {
                                expr6+=(pvs_b[c][i+1][j][l][p][o]);
                            }

                            else if(j==2 && p==platoon_num[i][j][l]&& i!=0)
                            {
                                expr5+=(pvs_b[c][i-1][j][l][p][o]);
                                
                            }
                            else if(j==2 && && i!=2)
                            {
                                expr6+=(npvs_b[c][i+1][j][l][p][o]);
                            }
                            else if(j==3 && p==platoon_num[i][j][l]&& i!=2)
                            {
                                expr7+=(pvs_b[c][i-1][j][2][p][o]);
                            }

                           else
                            {
                                expr2+=(pvs_b[c][i][j][l][p-1][o]);
                                expr4+=(npvs_b[c][i][j][l][p-1][o]);
                                
                            }
                            
                        }
                        
                        for (int o=0; o!=v_n[c][i][j][k][l][p]; o++)
                        {
                            if (p==0)
                            {
                                name="computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_signal_queue_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r1 (myenv,0, (i_t[c][i][j]+(S_t*expr1)-ta_t[c][i][j][l][p])-o*(tpv_t[c][i][j][l][p]-S_t),m*pvq_b[c][i][j][l][p][o],name);
                                mymodel.add(r1);
                                expr20+=o*(tpv_t[c][i][j][l][p]-S_t)*pvq_b[c][i][j][l][p][o];

                            }
                            else 
                            {
                                name="computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_signal_queue_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r2 (myenv,0, (i_t[c][i][j]+S_t*(expr2))-ha_t[c][i][j][l][p])-o*(tpv_t[c][i][j][l][p]-S_t),m*pvq_b[c][i][j][l][p][o],name);
                                mymodel.add(r2);
                                expr21+=o*(tpv_t[c][i][j][l][p]-S_t)*pvq_b[c][i][j][l][p][o];
                            }
                            
                            if (i==1||i==0)
                            {
                                if (j==6)
                                {
                                    if (p==0)
                                    {
                                        name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r3 (myenv,0, (i_t[c][i+1][j]+S_t*(expr3))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r3);
                                        expr22+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }
                                    else
                                    {
                                        name="given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r4 (myenv,0, (i_t[c][i+1][j]+S_t*(expr4))-(ha_t[c][i][j][l][p]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r4);
                                        expr23+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    } 
                                }
                                else if (j==7)
                                {
                                    if (p==0)
                                    {
                                        name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r7 (myenv,0, (i_t[c][i+1][6]+S_t*(expr6))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r7);
                                        expr24+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }
                                    else 
                                    {
                                        name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r8 (myenv,0, (i_t[c][i+1][6]+S_t*(expr4))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r8);
                                        expr25+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }

                                }
                            }
                            else (i==1||i==2)
                            {
                                if (j==2)
                                {
                                    if(p==0)
                                    {
                                        name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r5 (myenv,0, (i_t[c][i-1][j]+S_t*(expr5))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r5);
                                        expr26=o*(tvp_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }
                                    else
                                    {
                                        name="given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r6 (myenv,0, (i_t[c][i-1][j]+S_t*(expr4))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r6);
                                        expr27=o*(tvp_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }
                                }
                                else if (j==3)
                                {

                                    if (p==0)
                                    {
                                        name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r9 (myenv,0, (i_t[c][i-1][2]+S_t*(expr7))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r9);
                                        expr28=o*(tvp_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }
                                    else
                                    {
                                        name="given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                        IloRange r10 (myenv,0, (i_t[c][i-1][2]+S_t*(expr4))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name);
                                        mymodel.add(r10);
                                        expr29=o*(tvp_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    }
                                }
                            }


                        }
                        name="queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                        if (p==0)
                        {
                            IloRange r12(myenv,0,d_q[c][i][j][l][p],expr20,name);
                            mymodel.add(12);
                        }
                        else
                        {
                            IloRange r13(myenv,0,d_q[c][i][j][l][p],expr21,name);
                            mymodel.add(13);
                        }


                        if (i==1||i==0 && j==6)
                        {
                            if (p==0)
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r14(myenv,expr22,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r14);
                            else
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r15(myenv,expr23,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r15);
                            } 
                        }
                        
                        else if (i==1||i==0 && j==7)
                        {
                            if (p==0)
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r16(myenv,expr24,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r16);
                            else
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r17(myenv,expr25,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r17);
                            }
                        }
                        else if(i==1||i==2 && j==2)
                        {
                            if (p==0)
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r18(myenv,expr26,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r18);
                            else
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r19(myenv,expr27,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r19);
                            }
                        }
                        else if(i==1||i==2 && j==3)
                        {
                            if (p==0)
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r20(myenv,expr28,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r20);
                            else
                            {
                                name="future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                IloRange r21(myenv,expr29,dn_q[c][i][j][l][p],IloInfinity,name);
                                mymodel.add(r21);
                            }
                        }
                        expr1.clear();
                        expr2.clear();
                        expr3.clear();
                        expr4.clear();
                        expr5.clear();
                        expr6.clear();
                        expr7.clear();
                        
                        expr20.clear();
                        expr21.clear();
                        expr22.clear();
                        expr23.clear();
                        expr24.clear();
                        expr25.clear();
                        expr26.clear();
                        expr27.clear();
                        expr28.clear();
                        expr29.clear();


                        
                    }
                }
            }
        }
                    }
                }
            }
        }
    }

    //Add objective function
    for (int c=0; c!=k; c++)
    {
        for (int i=0; i!=int_num; i++)
        {
            for (int j=0; j!=pha_num ; j++)
            {
                for(int l=0; l!=lanes_num[i][j]; l++)
                {
                    for(int p=0;p!=platoon_num[i][j][l]; p++)
                    {
                        for (int o=0; o!=v_n[c][i][j][k][l][p]; o++)
                        {
                        expr1+=(d_s[c][i][j][k][l][p]+dn_s[c][i][j][k][l][p]+d_q[c][i][j][k][l][p]+dn_q[c][i][j][k][l][p]); 
                        }
                        
                    }
                }
            }
        }
    }
    IloObjective obj(myenv,expr1,IloObjective::Minimize);
    mymodel.add(obj);
    
}    


void DP_Solver::Solvemymodel()
{
    IloCplex cplex(mymodel);
    cplex.exportModel("model.lp");
    bool solved == false;
    try {
      // Try to solve with CPLEX (and hope it does not raise an exception!)
      solved = cplex.solve();
    } catch(const IloException& e) {
      std::cerr << "\n\nCPLEX Raised an exception:\n";
      std::cerr << e << "\n";
      env.end();
      throw;
    }
    if (solved)
    {
        std::cout<<"Objective_Value_is"<<cplex.getObjValue();
        for (int c=0; c!=k; c++)
        {
            for (int i=0; i!=int_num; i++)
            {
                for (int j=0; j!=pha_num ; j++)
                {
                    std::cout<<"Green_time_for_intersection"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<cplex.getValue(g_t[c][i][j])<<"\n";
                    std::cout<<"Initial_time_for_intersection"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<cplex.getValue(i_t[c][i][j])<<"\n";
                    for(int l=0; l!=lanes_num[i][j]; l++)
                    {
                        for(int p=0;p!=platoon_num[i][j][l]; p++)
                        {
                            std::cout<<"Signal_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<cplex.getValue(d_s[c][i][j][l][p]<<"\n");
                            std::cout<<"Next_Signal_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<cplex.getValue(dn_s[c][i][j][l][p]<<"\n");
                            std::cout<<"Queue_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<cplex.getValue(d_q[c][i][j][l][p]<<"\n");
                            std::cout<<"Next_Queue_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<cplex.getValue(dn_q[c][i][j][l][p]<<"\n");
                        }
                    }
                }
            }
        }
    }    
}    

//void DP_Solver::CutConstraints()
//{
    
//}
