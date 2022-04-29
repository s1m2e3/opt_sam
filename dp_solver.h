#include <iostream>
//#include "ilcplex\cplex.h"
//#include "ilcplex\ilocplex.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/cplex/include/ilcplex/cplex.h"

#include "/opt/ibm/ILOG/CPLEX_Studio221/cplex/include/ilcplex/ilocplex.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/concert/include/ilconcert/ilomodel.h"
#include "/opt/ibm/ILOG/CPLEX_Studio221/concert/include/ilconcert/iloenv.h"
#include <cmath>
#include <limits>
//#include "dp_getdata.h"


class DP_Solver
{
    public:
        
        //Parameters 
        
        //Model objects
        IloEnv env;
        IloModel model(env);
        DP_Reader reader;
        
        
        //Functions 
        void WriteModel(DP_Reader reader_);
        void SolveModel();
        DP_Solver(DP_Reader reader);

};