/*
struct sun_pos {
	double sunRa;
	double sunDec;
	double sunEclipticLongitude;	
};
sun_pos sun_calc(int year, int month, int day, int hour, int minute, int second);
*/
double ecliptic_longitude(int year, int month, int day, double hour, double minute, double second);
double sun_de(double e);
double sun_ra(double e);
