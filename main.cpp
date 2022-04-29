#include "dp_getdata.h"
#include "dp_solver.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>


int main()
{
    
    DP_Reader reader("edited_bsm_2.csv","edited_spat_manual.csv");
    DP_Solver solver(reader);
    return 0;
}