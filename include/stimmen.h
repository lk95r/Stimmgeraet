#ifndef _stimmen_h_INCLUDED
#define _stimmen_h_INCLUDED
#include <math.h>

#define t_E 41.2034
#define t_F 43.6535
#define t_Fis 46.2493
#define t_G 48.9994
#define t_Gis 51.9131
#define t_A 55.0
#define t_Ais 58.2705
#define t_H 61.7354
#define t_C 65.4064
#define t_Cis 69.2957
#define t_D 73.4162
#define t_Dis 77.7817



typedef struct
{
    int exp;
    float freq;
    char ton_name[2];
}Ton_t;

Ton_t Ton_gespielt(double freq_in);
float Abweichung(double freq_in,Ton_t T,float Toleranz);


#endif
