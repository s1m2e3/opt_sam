#include <iostream>
#include "/opt/ibm/ILOG/CPLEX_Studio221/cplex/include/ilcplex/cplex.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/cplex/include/ilcplex/ilocplex.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/concert/include/ilconcert/ilomodel.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/concert/include/ilconcert/iloenv.h"
#include "dp_solver.h"
#include "dp_getdata.h"
#include <cmath>
#include <limits>

DP_Solver::DP_Solver(DP_Reader reader_)
{
    model = IloModel(env);
    WriteModel(reader_);
    SolveModel();
    
    float UB=100000000000000;
    float LB=0; 
    //while(UB-LB>10)
    //{
    //    Solvemymodel();
//
    //} 
}

void DP_Solver::WriteModel(DP_Reader reader)
{
    bool opt_by_vehicles =false;
    bool phase8 = true;
    //Sets
    
    //Number of intersections
    int int_num = reader.spat_vector.size();
    
    //Number of phases
    int pha_num=8;
    
    //Number of cycles
    int k=1;

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
    std::map<int,std::map<int,std::map<int,int> >  >  platoon_num_lane_per_phase_per_int;
    
    for (int i=0; i!=reader.veh_vector.size();i++)
    {
        int belonging_intersection = reader.veh_vector[i].Intersection;
        int belonging_phase = reader.veh_vector[i].phase;
        int belonging_lane = reader.veh_vector[i].lane;
        int belonging_platoon = reader.veh_vector[i].lane;
        int inbound = reader.veh_vector[i].inbound;
        double arr_time = reader.veh_vector[i].time_to_arrival;
        double time_bet_vehicle = reader.veh_vector[i].time_between_vehicles;
        num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1;
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
                num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1;
        

            }
            //Check if phase exists in intersection
            else if (lanes_per_phase_per_int[belonging_intersection].find(belonging_phase)==lanes_per_phase_per_int[belonging_intersection].end())
            {
                lanes_per_phase_per_int[belonging_intersection][belonging_phase].push_back(belonging_lane);
                platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]=belonging_platoon;
                head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                time_bet_veh_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=time_bet_vehicle;
                num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1;
        
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
                    num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1;
        

                }
                else if (platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]< belonging_platoon)
                {
                    platoon_num_lane_per_phase_per_int[belonging_intersection][belonging_phase][belonging_lane]=belonging_platoon;
                
                    if (head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane].find(belonging_platoon)==head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane].end())

                    {
                        head_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                        tail_arr_time_map[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=arr_time;
                        num_veh_per_platoon[belonging_intersection][belonging_phase][belonging_lane][belonging_platoon]=1;
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
        int iter_intersection=it->first;
        std::map<int,std::vector<int>>::iterator it2;
        for (it2= lanes_per_phase_per_int[iter_intersection].begin();it2!=lanes_per_phase_per_int[iter_intersection].end(); it2++)
        {
            int iter_phase=it2->first;
            lanes_num[iter_intersection][iter_phase]=lanes_per_phase_per_int[iter_intersection][iter_phase].size();
        }
    }


    //Parameters
    
    //Elapsed, Min and Max Green time per phase per intersection
    IloArray<IloNumArray> gmin_t(env,int_num);
    IloArray<IloNumArray> gmax_t(env,int_num);
    IloArray<IloNumArray> e_t(env,int_num);
    IloArray<IloNumArray> int_st(env,int_num);
    IloNumArray int_i_t(env,int_num);
    int_i_t[0]=reader.spat_vector[0].time;
    int_i_t[1]=reader.spat_vector[0].time;
    int_i_t[2]=reader.spat_vector[0].time;

    for (int i=0; i!=int_num;i++)
    {
        Intersection int_ = reader.spat_vector[i];
        //Intersection::phase int_p1= int_.p1;
        //Intersection::phase int_p2= int_.p2;
        //Intersection::phase int_p3= int_.p3;
        //Intersection::phase int_p4= int_.p4;
        //Intersection::phase int_p5= int_.p5;
        //Intersection::phase int_p6= int_.p6;
        //Intersection::phase int_p7= int_.p7;
        //Intersection::phase int_p8= int_.p8;
        int_st[i]=IloNumArray(env,pha_num);
        gmin_t[i]=IloNumArray(env,pha_num);
        gmax_t[i]=IloNumArray(env,pha_num);
        e_t[i]=IloNumArray(env,pha_num);
        for (int j=0; j!=pha_num;j++)
        {
            if (j+1==1)
            {
                gmin_t[i][j]=int_.p1.p_minendtime;
                gmax_t[i][j]=int_.p1.p_maxendtime;
                e_t[i][j]=int_.p1.p_elapsedtime;
                int_st[i][j]=int_.p1.state;
            }
            else if (j+1==2)
            {
                gmin_t[i][j]=int_.p2.p_minendtime;
                gmax_t[i][j]=int_.p2.p_maxendtime;
                e_t[i][j]=int_.p2.p_elapsedtime;
                int_st[i][j]=int_.p2.state;
            }
            else if (j+1==3)
            {
                gmin_t[i][j]=int_.p3.p_minendtime;
                gmax_t[i][j]=int_.p3.p_maxendtime;
                e_t[i][j]=int_.p3.p_elapsedtime;
                int_st[i][j]=int_.p3.state;
            }
            else if (j+1==4)
            {
                gmin_t[i][j]=int_.p4.p_minendtime;
                gmax_t[i][j]=int_.p4.p_maxendtime;
                e_t[i][j]=int_.p4.p_elapsedtime;
                int_st[i][j]=int_.p4.state;
            }
            else if (j+1==5)
            {
                gmin_t[i][j]=int_.p5.p_minendtime;
                gmax_t[i][j]=int_.p5.p_maxendtime;
                e_t[i][j]=int_.p5.p_elapsedtime;
                int_st[i][j]=int_.p5.state;
            }
            else if (j+1==6)
            {
                gmin_t[i][j]=int_.p6.p_minendtime;
                gmax_t[i][j]=int_.p6.p_maxendtime;
                e_t[i][j]=int_.p6.p_elapsedtime;
                int_st[i][j]=int_.p6.state;
            }
            else if (j+1==7)
            {
                gmin_t[i][j]=int_.p7.p_minendtime;
                gmax_t[i][j]=int_.p7.p_maxendtime;
                e_t[i][j]=int_.p7.p_elapsedtime;
                int_st[i][j]=int_.p7.state;
            }
            else if (j+1==8)
            {
                gmin_t[i][j]=int_.p8.p_minendtime;
                gmax_t[i][j]=int_.p8.p_maxendtime;
                e_t[i][j]=int_.p8.p_elapsedtime;
                int_st[i][j]=int_.p7.state;
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
    IloArray<IloArray<IloArray<IloArray<IloNumArray>>>> ha_t(env,k);
    IloArray<IloArray<IloArray<IloArray<IloNumArray>>>> ta_t(env,k);
    //Time distance between vehicles per platoon per lane per phase per intersection
    IloArray<IloArray<IloArray<IloArray<IloNumArray>>>> tpv_t(env,k);
    //Number of vehicles per platoon per Intersection per Phase:
    IloArray<IloArray<IloArray<IloArray<IloNumArray>>>> v_n(env,k);
    for (int c=0; c!=k;c++)
    {   

        ha_t[c]=IloArray<IloArray<IloArray<IloNumArray>>>(env,int_num);
        ta_t[c]=IloArray<IloArray<IloArray<IloNumArray>>>(env,int_num);
        v_n[c]=IloArray<IloArray<IloArray<IloNumArray>>>(env,int_num);
        tpv_t[c]=IloArray<IloArray<IloArray<IloNumArray>>>(env,int_num);
        for (int i=0; i!=int_num;i++)
    {
        ha_t[c][i]=IloArray<IloArray<IloNumArray>>(env,pha_num);
        ta_t[c][i]=IloArray<IloArray<IloNumArray>>(env,pha_num);
        tpv_t[c][i]=IloArray<IloArray<IloNumArray>>(env,pha_num);
        v_n[c][i]=IloArray<IloArray<IloNumArray>>(env,pha_num);
        
        for (int j=0; j!=pha_num;j++)
        {
            ha_t[c][i][j]=IloArray<IloNumArray>(env,lanes_num[i][j]);
            ta_t[c][i][j]=IloArray<IloNumArray>(env,lanes_num[i][j]);
            tpv_t[c][i][j]=IloArray<IloNumArray>(env,lanes_num[i][j]);
            v_n[c][i][j]=IloArray<IloNumArray>(env,lanes_num[i][j]);
            for (int l=0;l!=lanes_num[i][j];l++)
            {
                ha_t[c][i][j][l]=IloNumArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                ta_t[c][i][j][l]=IloNumArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                tpv_t[c][i][j][l]=IloNumArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                v_n[c][i][j][l]=IloNumArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                for (int m=0; m!=platoon_num_lane_per_phase_per_int[i][j][l];m++)
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
    IloIntArray ni_t(env,3);
    ni_t[0]=20;
    ni_t[1]=20;
    ni_t[2]=20;
        
    //Variables instantiation and naming
    std::stringstream name;

    //Green Times per intersection per phase per cycle
    IloArray<IloArray<IloNumVarArray>> g_t(env,k);
    //Initial Time of green per intersection per phase per cycle
    IloArray<IloArray<IloNumVarArray>> i_t(env,k);
     //Slack Time per intersection per phase per cycle
    IloArray<IloArray<IloNumVarArray>> s_t(env,k);
    //Phase Duration per intersection per phase per cycle
    IloArray<IloArray<IloNumVarArray>> v_t(env,k);


    for (int m=0;m!=k;m++)
    {

        g_t[m]=IloArray<IloNumVarArray>(env,int_num);
        i_t[m]=IloArray<IloNumVarArray>(env,int_num);
        s_t[m]=IloArray<IloNumVarArray>(env,int_num);
        v_t[m]=IloArray<IloNumVarArray>(env,int_num);
            for (int i=0;i!=int_num;i++)
        {
            g_t[m][i]= IloNumVarArray(env,pha_num);
            i_t[m][i]= IloNumVarArray(env,pha_num);
            s_t[m][i]= IloNumVarArray(env,pha_num);
            v_t[m][i]= IloNumVarArray(env,pha_num);
            for (int j=0; j!=pha_num;j++)
            {
                name << "green_time_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                g_t[m][i][j]=IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                name.str(""); 
                name << "initial_time_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                i_t[m][i][j]=IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str()); 
                name.str(""); 
                name << "slack_time_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                s_t[m][i][j]=IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                name.str(""); 
                name << "phase_duration_cycle_"<<std::to_string(m)<<"_intersection_" << std::to_string(i) << "phase_" << std::to_string(j);
                v_t[m][i][j]=IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                name.str("");
            }
        }
    }
    
    //Queue Delay at intersection per platoon per lane per phase per intersection at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>> d_s(env,k);
    //Signal Delay at intersection per phase per cycle per platoon at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>  d_q(env,k);
    //Queue Delay at next intersection per phase per cycle per platoon at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>  dn_s(env,k);
    //Signal Delay at next intersection per phase per cycle per platoon at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>  dn_q(env,k);
    //Number of Passing Vehicles at next intersection per phase per cycle per platoon at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>> pv_n(env,k);
    //Number of Passing Vehicles at next intersection per phase per cycle per platoon which will pass to the next intersection at cycle k: 
    IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>> npv_n(env,k);
    //State(0,1) of ith vehicle in platoon subject to signal delay at intersection per phase per cycle per platoon at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>> pvs_b(env,k);
    //State(0,1) of ith vehicle in platoon  subject to signal delay at next intersection per phase per cycle per platoon which will pass to the next intersection at cycle k: 
    IloArray<IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>> npvs_b(env,k);
    //State(0,1) of ith vehicle in platoon  subject to queue delay at next intersection per phase per cycle per platoon at cycle k:
    IloArray<IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>> pvq_b(env,k);
    //State(0,1) of ith vehicle in platoon  subject to queue delay at next intersection per phase per cycle per platoon which will pass to the next intersection at cycle k: 
    IloArray<IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>> npvq_b(env,k);
    
    for(int n=0;n!=k;n++)
    {

        d_s[n]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,int_num);
        d_q[n]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,int_num);
        dn_s[n]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,int_num);
        dn_q[n]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,int_num);
        pv_n[n]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,int_num);
        npv_n[n]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,int_num);
        pvs_b[n]= IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>(env,int_num);
        npvs_b[n]= IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>(env,int_num);
        pvq_b[n]= IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>(env,int_num);
        npvq_b[n]= IloArray<IloArray<IloArray<IloArray<IloNumVarArray>>>>(env,int_num);

        for (int i=0;i!=int_num;i++)
        {
            d_s[n][i]= IloArray<IloArray<IloNumVarArray>>(env,pha_num);
            d_q[n][i]= IloArray<IloArray<IloNumVarArray>>(env,pha_num);
            dn_s[n][i]= IloArray<IloArray<IloNumVarArray>>(env,pha_num);
            dn_q[n][i]= IloArray<IloArray<IloNumVarArray>>(env,pha_num);
            pv_n[n][i]= IloArray<IloArray<IloNumVarArray>>(env,pha_num);
            npv_n[n][i]= IloArray<IloArray<IloNumVarArray>>(env,pha_num);
            pvs_b[n][i]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,pha_num);
            npvs_b[n][i]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,pha_num);
            pvq_b[n][i]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,pha_num);
            npvq_b[n][i]= IloArray<IloArray<IloArray<IloNumVarArray>>>(env,pha_num);
            



            for (int j=0; j!=pha_num;j++)
            {
                
                d_s[n][i][j]= IloArray<IloNumVarArray>(env,lanes_num[i][j]);
                d_q[n][i][j]= IloArray<IloNumVarArray>(env,lanes_num[i][j]);
                dn_s[n][i][j]= IloArray<IloNumVarArray>(env,lanes_num[i][j]);
                dn_q[n][i][j]= IloArray<IloNumVarArray>(env,lanes_num[i][j]);
                pv_n[n][i][j]= IloArray<IloNumVarArray>(env,lanes_num[i][j]);
                npv_n[n][i][j]= IloArray<IloNumVarArray>(env,lanes_num[i][j]);
                pvs_b[n][i][j]= IloArray<IloArray<IloNumVarArray>>(env,lanes_num[i][j]);
                npvs_b[n][i][j]= IloArray<IloArray<IloNumVarArray>>(env,lanes_num[i][j]);
                pvq_b[n][i][j]= IloArray<IloArray<IloNumVarArray>>(env,lanes_num[i][j]);
                npvq_b[n][i][j]= IloArray<IloArray<IloNumVarArray>>(env,lanes_num[i][j]);
            
                for (int l=0; l!=lanes_num[i][j]; l++)
                {
                    d_s[n][i][j][l]= IloNumVarArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    d_q[n][i][j][l]= IloNumVarArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    dn_s[n][i][j][l]= IloNumVarArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    dn_q[n][i][j][l]= IloNumVarArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    pv_n[n][i][j][l]= IloNumVarArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    npv_n[n][i][j][l]= IloNumVarArray(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    pvs_b[n][i][j][l]= IloArray<IloNumVarArray>(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    npvs_b[n][i][j][l]= IloArray<IloNumVarArray>(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    pvq_b[n][i][j][l]= IloArray<IloNumVarArray>(env,platoon_num_lane_per_phase_per_int[i][j][l]);
                    npvq_b[n][i][j][l]= IloArray<IloNumVarArray>(env,platoon_num_lane_per_phase_per_int[i][j][l]);
            
    
                    for (int m=0; m!=platoon_num_lane_per_phase_per_int[i][j][l];m++)
                    {
                    
                        name << "signal_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) <<"platoon_" << std::to_string(m) ;
                        d_s[n][i][j][l][m]= IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                        name.str("");
                        name << "queue_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m) ;
                        d_q[n][i][j][l][m]= IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                        name.str("");
                        name << "next_signal_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m);
                        dn_s[n][i][j][l][m]= IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                        name.str("");
                        name << "next_queue_delay_time__cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) <<"platoon_" << std::to_string(m) ;
                        dn_q[n][i][j][l][m]= IloNumVar(env,0,IloInfinity,ILOFLOAT,name.str().c_str());
                        name.str("");
                        name << "passing_vehicles_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m) ;
                        pv_n[n][i][j][l][m]= IloNumVar(env,0,v_n[n][i][j][l][m],ILOFLOAT,name.str().c_str());
                        name.str("");
                        name << "next_passing_vehicles_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m);
                        npv_n[n][i][j][l][m]= IloNumVar(env,0,v_n[n][i][j][l][m],ILOFLOAT,name.str().c_str());
                        name.str("");
                        
                        pvs_b[n][i][j][l][m]= IloNumVarArray(env,v_n[n][i][j][l][m]);
                        npvs_b[n][i][j][l][m]= IloNumVarArray(env,v_n[n][i][j][l][m]);
                        pvq_b[n][i][j][l][m]= IloNumVarArray(env,v_n[n][i][j][l][m]);
                        npvq_b[n][i][j][l][m]= IloNumVarArray(env,v_n[n][i][j][l][m]);
                                                
                        for (int o=0; o!=num_veh_per_platoon[i][j][l][m];o++)
                        {
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_signal_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m) ;
                            pvs_b[n][i][j][l][m][o]= IloNumVar(env,0,1,ILOFLOAT,name.str().c_str());
                            name.str("");
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_signal_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m);
                            npvs_b[n][i][j][l][m][o]= IloNumVar(env,0,1,ILOFLOAT,name.str().c_str());
                            name.str("");
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_queue_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m) ;
                            pvq_b[n][i][j][l][m][o]= IloNumVar(env,0,1,ILOFLOAT,name.str().c_str());
                            name.str("");
                            name << "vehicle_"<<std::to_string(o)<<"subject_to_queue_delay_cycle_"<<std::to_string(n)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l)<< "platoon_" << std::to_string(m);
                            npvq_b[n][i][j][l][m][o]= IloNumVar(env,0,1,ILOFLOAT,name.str().c_str());
                            name.str("");
                        }
                    }                  
                }       
            } 
        }

    }
        
    

    //Constraints 1: Set of Precedence Times and Time relationship
    IloExpr exp;
    IloRangeArray cons;
    //Time relationship between phases:
    for (int i=0; i!=int_num; i++)
    {
        for (int j=0; j!=k; j++)
        {
            exp=i_t[j][i][1]+s_t[j][i][1]+v_t[j][i][1];
            model.add(exp==i_t[j][i][2]);
            exp.clear();
            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"2_1";
            //IloRange r1(env,i_t[j][i][2],exp,i_t[j][i][2]);
            //cons.add(env,i_t[j][i][2],exp,i_t[j][i][2]);
            //name.str("");
            exp=i_t[j][i][5]+s_t[j][i][5]+v_t[j][i][5];
            model.add(exp==i_t[j][i][6]);
            exp.clear();
            
            //mymodel.add(r1);
            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"6_5";
            //IloRange r2(env,i_t[j][i][6],i_t[j][i][5]+s_t[j][i][5]+v_t[j][i][5],i_t[j][i][6],name.str().c_str());
            //name.str("");
            //mymodel.add(r2);
            exp=i_t[j][i][2]+s_t[j][i][2]+v_t[j][i][2];
            model.add(exp==i_t[j][i][3]);
            model.add(exp==i_t[j][i][7]);
            exp.clear();


            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"3_2";
            //IloRange r3(env,i_t[j][i][3],i_t[j][i][2]+s_t[j][i][2]+v_t[j][i][2],i_t[j][i][3],name.str().c_str());
            //name.str("");
            //mymodel.add(r3);
            exp=i_t[j][i][6]+s_t[j][i][6]+v_t[j][i][6];
            model.add(exp==i_t[j][i][3]);
            model.add(exp==i_t[j][i][7]);
            exp.clear();

            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"3_6";
            //IloRange r4(env,i_t[j][i][3],i_t[j][i][6]+s_t[j][i][6]+v_t[j][i][6],i_t[j][i][3],name.str().c_str());
            //name.str("");
            //mymodel.add(r4);

            

            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"7_2";
            //IloRange r5(env,i_t[j][i][7],i_t[j][i][2]+s_t[j][i][2]+v_t[j][i][2],i_t[j][i][7],name.str().c_str());
            //name.str("");
            //mymodel.add(r5);
            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"7_6";
            //IloRange r6(env,i_t[j][i][7],i_t[j][i][6]+s_t[j][i][6]+v_t[j][i][6],i_t[j][i][7],name.str().c_str());
            //name.str("");
            //mymodel.add(r6);
            exp=i_t[j][i][3]+s_t[j][i][3]+v_t[j][i][3];
            model.add(exp==i_t[j][i][4]);
            exp.clear();

            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"4_3";
            //IloRange r7(env,i_t[j][i][4],i_t[j][i][3]+s_t[j][i][3]+v_t[j][i][3],i_t[j][i][4],name.str().c_str());
            //name.str("");
            //mymodel.add(r7);
            exp=i_t[j][i][7]+s_t[j][i][7]+v_t[j][i][7];
            model.add(exp==i_t[j][i][8]);
            exp.clear();
            
            //name<<"Time_relationship_between_phases_cycle_"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"8_7";
            //IloRange r8(env,i_t[j][i][8],i_t[j][i][7]+s_t[j][i][7]+v_t[j][i][7],i_t[j][i][8],name.str().c_str());
            //name.str("");
            //mymodel.add(r8);

        }
    }

   //Relationship between cycle times//Double check:
    if (k>1)
    {
            for (int i=0; i!=int_num; i++)
        {
            {
                
                exp=i_t[0][i][4]+v_t[0][i][4];
                model.add(exp==i_t[1][i][1]);
                exp.clear();


                //name<<"relationship_between_cycle_times_phase_1_4";
                //IloRange r1(env,i_t[1][i][1],i_t[0][i][4]+v_t[0][i][4],i_t[1][i][1]);
                //mymodel.add(r1);
                //name.str("");
                exp=i_t[0][i][8]+v_t[0][i][8];
                model.add(exp==i_t[1][i][1]);
                exp.clear();


                //name<<"relationship_between_cycle_times_phase_1_8";
                //IloRange r2(env,i_t[1][i][1],i_t[0][i][8]+v_t[0][i][8],i_t[1][i][1]);
                //mymodel.add(r2);
                //name.str("");
                exp=i_t[0][i][4]+v_t[0][i][4];
                model.add(exp==i_t[1][i][5]);
                exp.clear();

                //name<<"relationship_between_cycle_times_phase_5_4";
                //IloRange r3(env,i_t[1][i][5],i_t[0][i][4]+v_t[0][i][4],i_t[1][i][5]);
                //mymodel.add(r3);
                //name.str("");

                exp=i_t[0][i][8]+v_t[0][i][8];
                model.add(exp==i_t[1][i][5]);
                exp.clear();

                //name<<"relationship_between_cycle_times_phase_5_8";
                //IloRange r4(env,i_t[1][i][5],i_t[0][i][8]+v_t[0][i][8],i_t[1][i][5]);
                //mymodel.add(r4);
                //name.str("");
            }
        }
    }
    

    //Composition of variable time:

    for (int i=0; i!=int_num; i++)
    {
        
        for (int j=0; j!=k; j++)
        {
            for (int p=0; p!=pha_num;p++)
            {
                exp=g_t[j][i][p]+y_t+r_t;
                model.add(exp==v_t[j][i][p]);
                exp.clear();
                
                //name<<"Composition_Variable_time_cycle"<<std::to_string(j)<<"intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(p);
                //IloRange r1(env,v_t[j][i][p],g_t[j][i][p]+y_t+r_t,v_t[j][i][p],name.str().c_str());
               
            }
        }
    }


    //Maximum allowable slack time:
    
    for (int i=0;i!=int_num;i++)
    {
        
        for (int j=0;j!=k;j++)
        {
            for (int l=0;l!=pha_num;l++)
            {
                exp+=s_t[j][i][l];
            }
        }
        model.add(exp<=S_t);
        model.add(0<=exp);
        exp.clear();
        
        //name<<"Maximum_slack_time_intersection"<<std::to_string(i);
        //IloRange ri(env,0,exp,S_T,name.str().c_str());
    }

    //Ranges for future green times and Initialize time for current phases:
    //Check with Larry differences between initial time for ring 1 and ring 2
    for (int l=0;l!=k;l++)
    {
        for (int i=0;i!=int_num;i++)
        {
            
            for (int j=0;j!=pha_num;j++)
            {
                
                if (int_st[i][j]==1)
                {
                    exp=g_t[l][i][j];
                    model.add(exp>=gmin_t[i][j]-e_t[i][j]);
                    model.add(i_t[0][i][j]==int_i_t[i]);
                    exp.clear();
                }
                else if( int_st[i][j]==2 || int_st[i][j]==3)
                {
                    exp=g_t[l][i][j];
                    model.add(exp<=gmax_t[i][j]);
                    model.add(gmin_t[i][j]<=exp);
                    exp.clear();
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
                    for(int p=0;p!=platoon_num_lane_per_phase_per_int[i][j][l]; p++)
                    {
                        if (c>0)
                        {
                            exp=pv_n[c][i][j][l][p]+pv_n[c][i][j][l][p];
                            model.add(exp<=v_n[c][i][j][l][p]);
                            exp.clear();
                            //name<<"update_of_passing_vehicles_intersection"<< std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r2(env,0,pv_n[c][i][j][l][p],v_n[c][i][j][l][p]-pv_n[c-1][i][j][l][p],name.str().c_str());
                            //mymodel.add(r2);
                        }
                        
                        exp=ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p];
                        model.add(exp<=i_t[c][i][j]+v_t[c][i][j]);
                        exp.clear();


                        //name<<"passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                        //IloRange r1(env,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p],i_t[c][i][j]+v_t[c][i][j],IloInfinity,name);
                        //mymodel.add(r1);
                        //W-E downstream pass
                        if(i==0 || i==1)
                        {
                            if (j==6)
                            {
                                exp=ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p];
                                model.add(exp<=i_t[c][i+1][j]+v_t[c][i+1][j]);
                                exp.clear();
                                //name<<"downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                //IloRange r3(env,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i+1][j]+v_t[c][i+1][j],IloInfinity,name);
                                //mymodel.add(r3);
                            }
                            else if(j==7)
                            {
                                exp=ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p];
                                model.add(exp<=i_t[c][i+1][6]+v_t[c][i+1][6]);
                                exp.clear();

                                //name<<"downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                //IloRange r4(env,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i+1][6]+v_t[c][i+1][6],IloInfinity,name);
                                //mymodel.add(r4);

                            }
                        }
                        //E-W downstream pass
                        else if(i==1 || 2)
                        {
                            if (j==2)
                            {
                                exp=ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p];
                                model.add(exp<=i_t[c][i-1][j]+v_t[c][i-1][j]);
                                exp.clear();

                                //name<<"downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                //IloRange r5(env,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i-1][j]+v_t[c][i-1][j],IloInfinity,name);
                                //mymodel.add(r5);
                            }
                            else if(j==3)
                            {
                                exp=ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p];
                                model.add(exp<=i_t[c][i-1][2]+v_t[c][i-1][2]);
                                exp.clear();
                                //name<<"downstream_passing_vehicles_computation_cycle"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                //IloRange r6(env,ta_t[c][i][j][l][p]+tpv_t[c][i][j][l][p]*pv_n[c][i][j][l][p]+d_q[c][i][j][l][p]+ni_t[i]+dn_q[c][i][j][l][p]+,i_t[c][i+1][2]+v_t[c][i+1][2],IloInfinity,name);
                                //mymodel.add(r6);
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
    IloExpr expr1(env);
    IloExpr expr2(env);
    IloExpr expr3(env);
    IloExpr expr4(env);
    IloExpr expr5(env);

    for (int c=0; c!=k; c++)
    {
        for (int i=0; i!=int_num; i++)
        {
            for (int j=0; j!=pha_num ; j++)
            {
                for(int l=0; l!=lanes_num[i][j]; l++)
                {
                    for(int p=0;p!=platoon_num_lane_per_phase_per_int[i][j][l]; p++)
                    {
                        for (int o=0; o!=num_veh_per_platoon[i][j][l][p]; o++)
                        {
                            exp=i_t[c][i][j]-ha_t[c][i][j][l][p]-o*tpv_t[c][i][j][l][p]; 
                            model.add(exp<=m*pvs_b[c][i][j][l][p][o]);
                            exp.clear();
                            //name<<"computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r1(env,i_t[c][i][j]-ha_t[c][i][j][l][p]-o*tpv_t[c][i][j][l][p],M*pvs_b[c][i][j][l][p][o],IloInfinity,name);
                            //mymodel.add(r1);
                            expr1+=((tpv_t[c][i][j][l][p]*o)*pvs_b[c][i][j][l][p][o]);
                            
                            
                            if (i==0 ||i==1 )
                            {
                                if(j==6)
                                {
                                    exp=i_t[c][i+1][j]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p]; 
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    exp=i_t[c][i+1][j]-(ha_t[c][i][j][l][p]+ni_t[i])-o*tpv_t[c][i][j][l][p];
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r2(env,i_t[c][i+1][j]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //name<<"given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r3(env,i_t[c][i+1][j]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //mymodel.add(r2);
                                    //mymodel.add(r3);
                                    expr2+=((tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                    
                                    
                                }
                                if (j==7)
                                {
                                    exp=i_t[c][i+1][6]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p]; 
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    exp=i_t[c][i+1][6]-(ha_t[c][i][j][l][p]+ni_t[i])-o*tpv_t[c][i][j][l][p];
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r4(env,i_t[c][i+1][6]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //name<<"given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r5(env,i_t[c][i+1][6]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //mymodel.add(r4);
                                    //mymodel.add(r5);
                                    expr3+=((tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                }
                            }
                            else if(i==1||i==2)
                            {
                                if(j==2)
                                {
                                 
                                    exp=i_t[c][i-1][j]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p]; 
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    exp=i_t[c][i-1][j]-(ha_t[c][i][j][l][p]+ni_t[i])-o*tpv_t[c][i][j][l][p];
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                 
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r6(env,i_t[c][i-1][j]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //name<<"given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r7(env,i_t[c][i-1][j]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //mymodel.add(r6);
                                    //mymodel.add(r7);
                                    expr4+=((tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);
                                    
                                }
                                if (j==3)
                                {
                                    
                                    exp=i_t[c][i-1][2]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p]; 
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    exp=i_t[c][i-1][2]-(ha_t[c][i][j][l][p]+ni_t[i])-o*tpv_t[c][i][j][l][p];
                                    model.add(exp<=m*npvs_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r8(env,i_t[c][i-1][2]-(i_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //name<<"given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_signal_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r9(env,i_t[c][i-1][2]-(ha_t[c][i][j]+ni_t[i])-o*tpv_t[c][i][j][l][p],M*npvs_b[c][i][j][l][p][o],IloInfinity,name);
                                    //mymodel.add(r8);
                                    //mymodel.add(r9);
                                    expr5+=((tpv_t[c][i][j][l][p]*o)*npvs_b[c][i][j][l][p][o]);

                                }
                            }
                        }
                        //name<<"signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                        //IloRange r10(env,expr1, d_s[c][i][j][l][p],IloInfinity,name);
                        model.add(expr1<=d_s[c][i][j][l][p]);
                        if (i==1||i==0 && j==6)
                        {
                            //name<<"future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r11(env,expr2,dn_s[c][i][j][l][p],IloInfinity,name);
                            model.add(expr2<=dn_s[c][i][j][l][p]); 
                        }
                        
                        else if (i==1||i==0 && j==7)
                        {
                            //name<<"future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r12(env,expr3,dn_s[c][i][j][l][p],IloInfinity,name);
                            model.add(expr3<=dn_s[c][i][j][l][p]);
                        }
                        else if(i==1||i==2 && j==2)
                        {
                            //name<<"future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r13(env,expr4,dn_s[c][i][j][l][p],IloInfinity,name);
                            model.add(expr4<=dn_s[c][i][j][l][p]);
                        }
                        else if(i==1||i==2 && j==3)
                        {
                            //name<<"future_signal_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r14(env,expr5,dn_s[c][i][j][l][p],IloInfinity,name);
                            model.add(expr5<=dn_s[c][i][j][l][p]);
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
                    for(int p=0;p!=platoon_num_lane_per_phase_per_int[i][j][l]; p++)
                    {
                        for (int o=0; o!=num_veh_per_platoon[i][j][l][p]; o++)
                        {
                            if (p==0)
                            {
                                for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                {
                                    expr1+=(pvs_b[c][i][j][l][p][iter]);
                                }
                                exp= (i_t[c][i][j]+(S_t*expr1)-ta_t[c][i][j][l][p])-o*(tpv_t[c][i][j][l][p]-S_t);
                                model.add(exp<=m*pvq_b[c][i][j][l][p][o]);
                                expr1.clear();
                                exp.clear();
                                expr20+=o*(tpv_t[c][i][j][l][p]-S_t)*pvq_b[c][i][j][l][p][o];
                                //name<<"computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_signal_queue_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                //IloRange r1 (env,0, (i_t[c][i][j]+(S_t*expr1)-ta_t[c][i][j][l][p])-o*(tpv_t[c][i][j][l][p]-S_t),m*pvq_b[c][i][j][l][p][o],name.str().c_str());
                                //mymodel.add(r1);
                                if ((i==1 || i==0) && j==6)
                                {

                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(pvs_b[c][i+1][j][l][platoon_num_lane_per_phase_per_int[i][j][l]][iter]);

                                    }
                                    exp=(i_t[c][i+1][j]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr22+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r3 (env,0, (i_t[c][i+1][j]+S_t*(expr3))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r3);
                                    
                                }
                                else if ((i==1 || i==0) && j==7)
                                {
                                    
                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(pvs_b[c][i+1][j][l][platoon_num_lane_per_phase_per_int[i][j][l]][iter]);
                                    }
                                    exp=(i_t[c][i+1][6]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr24+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r7 (env,0, (i_t[c][i+1][6]+S_t*(expr6))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r7);
                                    
                                }
                                else if((i==1||i==2)&& j==2)
                                {
                                    
                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(pvs_b[c][i-1][j][l][platoon_num_lane_per_phase_per_int[i][j][l]][iter]);

                                    }
                                    exp=(i_t[c][i-1][j]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr26=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r5 (env,0, (i_t[c][i-1][j]+S_t*(expr5))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r5);
                                    

                                }
                                else if((i==1||i==2)&& j==3)
                                {
                                    
                                    for(int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(pvs_b[c][i-1][j][2][platoon_num_lane_per_phase_per_int[i][j][l]][iter]);
                                    }
                                    exp=(i_t[c][i-1][2]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr28=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r9 (env,0, (i_t[c][i-1][2]+S_t*(expr7))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r9);
                                    

                                }  

                            }
                            else 
                            {
                                for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                {
                                    expr1+=(pvs_b[c][i][j][l][p-1][iter]);
                                }
                                exp=(i_t[c][i][j]+S_t*(expr1))-(ha_t[c][i][j][l][p]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                model.add(exp<=m*pvq_b[c][i][j][l][p][o]);
                                exp.clear();
                                expr1.clear();
                                expr21+=o*(tpv_t[c][i][j][l][p]-S_t)*pvq_b[c][i][j][l][p][o];
                                //name<<"computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_signal_queue_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                //IloRange r2 (env,0, (i_t[c][i][j]+S_t*(expr2))-ha_t[c][i][j][l][p])-o*(tpv_t[c][i][j][l][p]-S_t),m*pvq_b[c][i][j][l][p][o],name.str().c_str());
                                //mymodel.add(r2);
                                

                                if ((i==1 || i==0) && j==6)
                                {

                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(npvs_b[c][i][j][l][p-1][iter]);
                                    }
                                    exp=(i_t[c][i+1][j]+S_t*(expr1))-(ha_t[c][i][j][l][p]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr23+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r4 (env,0, (i_t[c][i+1][j]+S_t*(expr4))-(ha_t[c][i][j][l][p]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r4);

                                }
                                else if ((i==1 || i==0) && j==7)
                                {
                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(npvs_b[c][i][j][l][p-1][iter]);
                                    }
                                    exp= (i_t[c][i+1][6]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr25+=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r8 (env,0, (i_t[c][i+1][6]+S_t*(expr4))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r8);
                                    
                                }
                                else if((i==1||i==2)&& j==2)
                                {
                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(npvs_b[c][i][j][l][p-1][iter]);
                                    }
                                    
                                    exp=(i_t[c][i-1][j]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear(); 
                                    expr27=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];
                                    //name<<"given_no_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r6 (env,0, (i_t[c][i-1][j]+S_t*(expr4))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r6);
                                   

                                } 
                                else if((i==1||i==2)&& j==3)
                                {

                                    for (int iter=0; iter!=num_veh_per_platoon[i][j][l][p]; iter++)
                                    {
                                        expr1+=(npvs_b[c][i][j][l][p-1][iter]);
                                    }
                                    exp=(i_t[c][i-1][2]+S_t*(expr1))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t);
                                    model.add(exp<=m*npvq_b[c][i][j][l][p][o]);
                                    exp.clear();
                                    expr1.clear();
                                    expr29=o*(tpv_t[c][i][j][l][p]-S_t)*npvq_b[c][i][j][l][p][o];

                                    //name<<"given_signal_delay_computation_vehicle_in_order_"<<std::to_string(o)<<"subject_to_future_queue_delay_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                                    //IloRange r10 (env,0, (i_t[c][i-1][2]+S_t*(expr4))-(i_t[c][i][j]+ni_t[i])-o*(tpv_t[c][i][j][l][p]-S_t),m*npvq_b[c][i][j][l][p][o],name.str().c_str());
                                    //mymodel.add(r10);
                                    

                                } 
                               
                            }
                        }

                            
                    if (p==0)
                    {
                        model.add(d_q[c][i][j][l][p]<=expr20);
                        //IloRange r12(env,0,d_q[c][i][j][l][p],expr20,name);
                        //mymodel.add(12);
                         if ((i==1 || i==0) && j==6)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr22);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r14(env,expr22,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r14);
                         }
                         else if ((i==1 || i==0) && j==7)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr24);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r16(env,expr24,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r16);
                         }
                         else if ((i==1 || i==2) && j==2)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr26);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r18(env,expr26,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r18);
                         }
                         else if ((i==1 || i==2) && j==3)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr28);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r20(env,expr28,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r20);
                         }
                    }
                    else
                    {
                        model.add(dn_q[c][i][j][l][p]<=expr21);
                        //IloRange r13(env,0,d_q[c][i][j][l][p],expr21,name);
                        //mymodel.add(13);
                        if ((i==1 || i==0) && j==6)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr23);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r15(env,expr23,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r15);
                         }
                         else if ((i==1 || i==0) && j==7)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr25);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r17(env,expr25,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r17);
                         }
                         else if ((i==1 || i==2) && j==2)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr27);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r19(env,expr27,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r19);
                         }
                         else if ((i==1 || i==2) && j==3)
                         {
                            model.add(dn_q[c][i][j][l][p]<=expr29);
                            //name<<"future_queue_delay_computation_at_cycle_"<<std::to_string(c)<<"intersection_" << std::to_string(i) << "phase_" << std::to_string(j) << "lane_" << std::to_string(l) "platoon_" << std::to_string(p);
                            //IloRange r21(env,expr29,dn_q[c][i][j][l][p],IloInfinity,name);
                            //mymodel.add(r21);
                         }
                    }

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
    
    //Add objective function
    for (int c=0; c!=k; c++)
    {
        for (int i=0; i!=int_num; i++)
        {
            for (int j=0; j!=pha_num ; j++)
            {
                for(int l=0; l!=lanes_num[i][j]; l++)
                {
                    for(int p=0;p!=platoon_num_lane_per_phase_per_int[i][j][l]; p++)
                    {
                        
                        exp+=(d_s[c][i][j][l][p]+dn_s[c][i][j][l][p]+d_q[c][i][j][l][p]+dn_q[c][i][j][l][p]); 
                        
                    }
                }
            }
        }
    }
    IloObjective obj(env,exp,IloObjective::Minimize);
    model.add(obj);

    IloCplex cplex(model);
    cplex.exportModel("model.lp");
    bool solved = false;
    
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
                    double gt_solution = cplex.getValue(g_t[c][i][j]);
                    double it_solution = cplex.getValue(i_t[c][i][j]);
                    std::cout<<"Green_time_for_intersection"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<std::to_string(gt_solution)<<"\n";
                    std::cout<<"Initial_time_for_intersection"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<std::to_string(it_solution)<<"\n";
                    for(int l=0; l!=lanes_num[i][j]; l++)
                    {
                        for(int p=0;p!=platoon_num_lane_per_phase_per_int[i][j][l]; p++)
                        {
                            double ds_solution=cplex.getValue(d_s[c][i][j][l][p]);
                            double dns_solution=cplex.getValue(dn_s[c][i][j][l][p]);
                            double dq_solution=cplex.getValue(d_q[c][i][j][l][p]);
                            double dnq_solution=cplex.getValue(dn_q[c][i][j][l][p]);

                            std::cout<<"Signal_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<std::to_string(ds_solution)<<"\n";
                            std::cout<<"Next_Signal_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<std::to_string(dns_solution)<<"\n";
                            std::cout<<"Queue_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<std::to_string(dq_solution)<<"\n";
                            std::cout<<"Next_Queue_delay_intersection_"<<std::to_string(i)<<"phase_"<<std::to_string(j)<<"lane"<<std::to_string(l)<<"platoon_"<<std::to_string(p)<<std::to_string(dnq_solution)<<"\n";
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
