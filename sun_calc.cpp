#include <iostream>
#include <cmath>
#include "sun_calc.h"

#define PI 3.14159265

using namespace std;

// ecliptic longitude at a given moment of the sun
double ecliptic_longitude(int year, int month, int day, double hour, double minute, double second)
{


//julian day
if(month < 3) {
        year  = year - 1;
        month = month + 12;
}

double time = hour + minute / 60.0 + second / 3600.0;

double julian = 365.25*year + 30.6001*(month+1) - 15 + 1720996.5 + day + time / 24.0 ;
//m0 in degrees, m1 in degrees per day from a table, j2000 is julian 2000. 
double m0 = 357.5291, m1 = 0.98560028, j2000 = 2451545;

double mean_anomaly = m0+m1 * (julian - j2000);

	while(mean_anomaly > 180){
	mean_anomaly = mean_anomaly - 180;
	}

// mean anomaly correction
double c1 = 1.9148, c2 = 0.02, c3 = 0.0003;
double cor = c1 * sin(mean_anomaly*PI/180) + c2 * sin(2*mean_anomaly*PI/180) + c3 * sin(3*mean_anomaly*PI/180);

double true_anomaly = mean_anomaly + cor;

// ecliptical longitude
// perihelion of earth is 102.9372
double per = 102.9372;

// e is ecliptical longitude
double e = per + true_anomaly + 180;
	while (e > 360){ e = e - 360; }
return e;
}


// RA of SUN from ecliptical longitude
double sun_ra(double e)
{
/*
// FIRST WAY WITH APPROXIMATION
// coefficients a2 a4 a6 are from a table from the approximation
// maximum error 0.0003
// oblq = 23.45
double a2 = -2.468, a4 = 0.053, a6 = -0.0014;
double ra = e + a2 * sin(2*e*PI/180) + a4 * sin(4*e*PI/180) + a6 * sin(6*e*PI/180);
//return ra; 
*/

//SECOND WAY 
double oblq = 23.45;
double ra = atan2 ( sin(e*PI/180 ) * cos( oblq*PI/180 ) , cos(e*PI/180)  )*180/PI;
return ra;
}


// DEC of SUN from ecliptical longitude
double sun_de(double e)
{
// DEC of SUN from ecliptical longitude

/*
//FIRST WAY WITH APPROXIMATION
// coefficients d1 d3 d5 are from a table from the approximation
// maximum error 0.0003
double d1 = 22.8008, d3 = 0.5999, d5 = 0.0493;
double sin_eclip = sin(e*PI/180);
double dec =   d1 * sin_eclip +
		  d3 * sin_eclip * sin_eclip * sin_eclip + 
		  d5 * sin_eclip * sin_eclip * sin_eclip * sin_eclip * sin_eclip;
*/

//SECOND WAY
double oblq = 23.45;
double dec = asin( sin(e*PI/180)  * sin (oblq*PI/180) )*180/PI;
return dec;
}
