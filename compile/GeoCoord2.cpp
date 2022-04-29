/***************************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

***************************************************************************************

  GeoCoord.cpp
  Created by: K. Larry Head  
  University of Arizona   
  College of Engineering

  This code was developed under the supervision of Professor Larry Head
  in the Systems and Industrial Engineering Department.

***************************************************************************************/

#include "geoCoord2.h" 

using namespace std ;


void geoCoord::init(double longitude, double latitude, double altitude) {
	longitude_init = longitude ;
	latitude_init = latitude ;
	altitude_init = altitude ;
	R_sinLat = sin(latitude_init*pi/180.0) ;
	R_cosLat = cos(latitude_init*pi/180.0) ;
	R_sinLong = sin(longitude_init*pi/180.0) ;
	R_cosLong = cos(longitude_init*pi/180.0) ;

	lla2ecef(longitude_init, latitude_init, altitude_init) ;


}

double geoCoord::dms2d(double degree, double minutes, double seconds) {
	return ((seconds/60+minutes)/60 + (double) degree) ;
}

void geoCoord::local2ecef(double x, double y, double z) {
	
	//compute the new (x,y,z) in ecef coordinates

	ex = ex_init + (-R_sinLat*R_cosLong)*x + (-R_sinLong)*y + (-R_cosLat*R_cosLong)*z ;
	ey = ey_init + (-R_sinLat*R_sinLong)*x + (R_cosLong)*y + (-R_cosLat*R_sinLong)*z ;
	ez = ez_init + (R_cosLat)*x            +                  (-R_sinLat)*z ; 
	
}


void geoCoord::lla2ecef(double longitude, double latitude, double altitude) {

	//convert degrees to radians

	longitude_r = pi*longitude/180.0 ;
	latitude_r = pi*latitude/180.0 ;

	//compute constants
	N = a/sqrt(1.0l-pow(e*sin(latitude_r), 2)) ;
		
	//compute ECEF coordiates
	ex_init = (N+altitude)*cos(latitude_r)*cos(longitude_r) ;
	ey_init = (N+altitude)*cos(latitude_r)*sin(longitude_r) ;
	ez_init = (N*(1-pow(e, 2))+altitude)*sin(latitude_r) ;

}

void geoCoord::ecef2lla() {
	double hi = 0.0 ;
	double Ni = 0.0 ;
	double p = 0.0 ;
	double sin_lat = 0.0 ;

	double longitudei = 0.0 ;
	double latitudei_1 = 0.0 ;
	double latitudei = 0.0 ;

	double error = 1.0 ;


	//initialize 
	hi =  0.0 ;
	Ni = a ;
	p = sqrt(pow(ex,2)+pow(ey,2)) ;

	longitudei = atan2(ey, ex) ;

	while (error > 0.00000001)
	{
		sin_lat = z/(Ni*(1-pow(e,2)) + hi) ;
		latitudei_1 = atan((ez+(pow(e,2)*Ni*sin_lat))/p) ;
		Ni = a/sqrt(1.0l -(pow(e,2)*pow(sin(latitudei),2))) ;
		hi = p/cos(latitudei) - Ni ;

		error = abs(latitudei_1-latitudei) ;

		latitudei = latitudei_1 ;
	}
	longitude = longitudei*180.0/pi ;
	latitude = latitudei*180.0/pi ;
	altitude = hi ;

}