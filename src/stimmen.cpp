#include "stimmen.h"


/**
*\brief Zuordnung von Nennfrequenz und Name
*
*\param double freq_in Frequenz, die zugeordnet werden soll
*/
Ton_t Ton_gespielt(double freq_in)
{
    Ton_t T;
    int Oktave;
    T.ton_name[0] = ' ';
    T.ton_name[1] = ' ';
    // Oktave Feststellen
    if (freq_in > (2489.02 + 2638.02) / 2)
        Oktave = 6; // Oktave 7 wäre bei >4kHz - kann nicht gesampled werden
    else if (freq_in > (1318.51 + 1244.51) / 2)
        Oktave = 5;
    else if (freq_in > (659.255 + 622.254) / 2)
        Oktave = 4;
    else if (freq_in > (329.628 + 311.127) / 2)
        Oktave = 3;
    else if (freq_in > (164.814 + 155.563) / 2)
        Oktave = 2;
    else if (freq_in > (82.4069 + 77.7817) / 2)
        Oktave = 1;
    else
        Oktave = 0;

    // gemessene Frequenz auf Grundlegende Oktave bringen
    T.exp = Oktave;
    freq_in = freq_in / pow(2, T.exp);

    //Zuordnung von Nennfrequenz und Name

    if (freq_in < (t_E + t_F) / 2.0)
    {
        T.freq = t_E;
        T.ton_name[0] = 'E';
    }
    else if (freq_in < (t_F + t_Fis) / 2.0)
    {
        T.freq = t_F;
        T.ton_name[0] = 'F';
    }
    else if (freq_in < (t_Fis + t_G) / 2.0)
    {
        T.freq = t_Fis;
        T.ton_name[0] = 'F';
        T.ton_name[1] = '#';
    }
    else if (freq_in < (t_G + t_Gis) / 2.0)
    {
        T.freq = t_G;
        T.ton_name[0] = 'G';
    }
    else if (freq_in < (t_Gis + t_A) / 2.0)
    {
        T.freq = t_Gis;
        T.ton_name[0] = 'F';
        T.ton_name[1] = '#';
    }
    else if (freq_in < (t_A + t_Ais) / 2.0)
    {
        T.freq = t_A;
        T.ton_name[0] = 'A';
    }
    else if (freq_in < (t_Ais + t_H) / 2.0)
    {
        T.freq = t_Ais;
        T.ton_name[0] = 'A';
        T.ton_name[1] = '#';
    }
    else if (freq_in < (t_H + t_C) / 2.0)
    {
        T.freq = t_H;
        T.ton_name[0] = 'H';
    }
    else if (freq_in < (t_C + t_Cis) / 2.0)
    {
        T.freq = t_C;
        T.ton_name[0] = 'C';
    }
    else if (freq_in < (t_Cis + t_D) / 2.0)
    {
        T.freq = t_Cis;
        T.ton_name[0] = 'C';
        T.ton_name[1] = '#';
    }
    else if (freq_in < (t_D + t_Dis) / 2.0)
    {
        T.freq = t_D;
        T.ton_name[0] = 'D';
    }
    else if (freq_in < (t_Dis + t_E * 2) / 2.0)
    {
        T.freq = t_Dis;
        T.ton_name[0] = 'D';
        T.ton_name[1] = '#';
    }

    return T;
}


/**
 * \brief Stellt Abweichung zum Soll fest
 * \param double freq_in - gemessene Frequenz
 * \param Ton_t T zugeordneter Ton
 * \param float Toleranz Toleranzfaktor, Angewendet auf Sollfrequenz
 * 
 */
float Abweichung(double freq_in, Ton_t T, float Toleranz)
{
    float delta_f;

    // Minus: zu niedrig, hoeher stimmen--------//Positiv: zu hoch, tiefer stimmern
    delta_f = freq_in - (T.freq * pow(2, T.exp));
    if (abs((delta_f)) < Toleranz * (T.freq * pow(2, T.exp)))
    {
        delta_f = 0;
    }
    return delta_f;
}
