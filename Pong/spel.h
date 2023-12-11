/******************************************************************************************************
Header-fil till spel.c, skriven av Carl-Anton Grandelius
******************************************************************************************************/

#ifndef SPEL_H
#define SPEL_H

#include "main.h"
#include "skarm.h"
#include "data.h"
#include <pic32mx.h>
#include <stdint.h>

void hanteraSpelSlinga(uint8_t *lage);
void visaStartmeny(uint8_t *speltyp, uint8_t *menyIterationer);
void uppdateraSpelare(uint8_t speltyp);
void uppdateraBoll(uint8_t *lage);
void visaSpelavslut(uint8_t *lage);
uint8_t flyttaBoll();
void spelEjStartat();
uint8_t kollision();
void uppdateraAI(Spelare *ai_spelare, Boll boll);

#endif