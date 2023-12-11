/******************************************************************************************************
Header-fil till skarm.c, skriven av Suhaib Abdi Muhummed 
******************************************************************************************************/

#ifndef SKARM_H
#define SKARM_H

#include <stdint.h>
#include <stdbool.h>
#include <pic32mx.h>
#include "data.h"
#include "centrering.h"
#include "typer.h"

// (makro)Definitioner för kommando- och dataläge
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10) // 0001 0000 binart
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)    // 0001 0000 binart

// Definitioner för återställning
#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200) // 10 0000 0000 binart
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)   // 10 0000 0000 binart

// Definitioner för VDD och VBAT
#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)  // 0100 0000 binart
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20) // 0010 0000 binart
#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)  // 0100 0000 binart
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20) // 0010 0000 binart
#define ITOA_BUFSIZ (24) // buffertstorleken for att konvertera upp till 64-bitars heltal, 24.

// Funktioner
void delay(int cyc);
void skarmUppdatering();
uint8_t spiSkickaTaEmot(uint8_t data);
void initieraSkarm(void);
char *heltalTillStrang(int tal);
void visaStrang(char *s, int x, int y);
void ritaUtSpelare(Spelare spelare);
void ritaUtBoll(Boll boll);
void ritaRektangel(int startX, int slutX, int startY, int slutY);
void fargaPixel(uint8_t x, uint8_t y);
void fargaSkarmSvart(void);
int strangLangd(const char *s);
void skrivUtStrangCentrerad(char *s, CentreringsAxel axel, int x, int y);

#endif
