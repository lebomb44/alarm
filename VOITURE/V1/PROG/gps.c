#include "global.h"
#include "gps.h"

void gps_coord(void)
{
  GSMGPSPORT=GPS_enable;
  GSMGPSPORT=GSMGPS_disable;
  coordonnees_gps[0]='W';
  coordonnees_gps[1]='0';
  coordonnees_gps[2]='0';
  coordonnees_gps[3]='.';
  coordonnees_gps[4]='0';
  coordonnees_gps[5]='0';
  coordonnees_gps[6]='0';
  coordonnees_gps[7]='0';
  coordonnees_gps[8]='0';
  coordonnees_gps[9]='0';
  coordonnees_gps[10]='0';
  coordonnees_gps[11]='0';
  coordonnees_gps[12]='.';
  coordonnees_gps[13]='0';
  coordonnees_gps[14]='0';
  coordonnees_gps[15]='0';
  coordonnees_gps[16]='0';
  coordonnees_gps[17]='0';
  coordonnees_gps[18]='0';
  coordonnees_gps[19]='0';
}
