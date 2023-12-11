/******************************************************************************************************
Structs för Spelare (vänster och höger, alltså två instanser) och Boll (en instans) med koordinater, storlek och riktning av Carl-Anton Grandelius
******************************************************************************************************/

#ifndef TYPER_H
#define TYPER_H

typedef struct
{
    uint8_t x, y, bredd, hojd, poang;
} Spelare;

typedef struct
{
    float x, y;
    uint8_t bredd, hojd;
    float dx, dy;
} Boll;

#endif