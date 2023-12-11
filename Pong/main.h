/******************************************************************************************************
Header-fil till Pong-spelets main.c, av Carl-Anton Grandelius.
******************************************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include "spel.h"
#include "skarm.h"
#include <stdint.h>
#include <pic32mx.h>
#include <stdbool.h>

int main(void);
void hardvaruInitiering();
void tandLampor(uint8_t aktiverade);
uint8_t arKnappIntryck(uint8_t knapp);
uint16_t las_analog(uint8_t pin);
void userIsr(void);
void enable_interrupt();
void hanteraSpelSlinga(uint8_t *lage);
void spelEjStartat();

#endif
