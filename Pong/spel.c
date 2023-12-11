/******************************************************************************************************
Samtliga funktioner i denna fil ar skrivna av Carl-Anton Grandelius. Suhaib Abdi Muhummed ar ansvarig for korning av funktioner har som finns i skarm.c.
******************************************************************************************************/

#include "spel.h"
#include "centrering.h"
#include "typer.h"

#define MAX_POANG 3
static Spelare v_spelare = {20, 8, 4, 13, 0};  // typedef struct {uint8_t x, y, bredd, hojd, poang; } Spelare;
static Spelare h_spelare = {108, 8, 4, 13, 0}; // typedef struct {uint8_t x, y, bredd, hojd, poang; } Spelare;
static Boll boll = {63, 16, 3, 3, 1, -1};      // typedef struct {float x, y; uint8_t bredd, hojd; float dx, dy; } Boll;
static uint8_t iterationer = 0;
static uint8_t kollisionMedSpelare = 0;
static uint8_t speltyp = 0;

void hanteraSpelSlinga(uint8_t *lage)
{
    static uint8_t menyIterationer = 0;
    if (speltyp == 0)
    {
        v_spelare.poang = 0;
        h_spelare.poang = 0;
        iterationer = 0;
        visaStartmeny(&speltyp, &menyIterationer);
    }
    else
    {
        switch (*lage)
        {
        case 1:
            // Återställ spelare och boll till startpositioner
            v_spelare.y = 8;
            h_spelare.y = 8;
            boll.x = 63;
            boll.y = 16;

            // Bollens riktning hanteras särskilt för att skicka bollen åt det motsatta håll som den åkte senast (när det senast blev mål).
            // Detta spelar dock ingen roll för första körningen då det alltid blir -1, 1 på grund av de förinställda värdena
            if (boll.dy > 0)
            {
                boll.dy = -2;
            }
            else
            {
                boll.dy = 2;
            }
            if (boll.dx > 0)
            {
                boll.dx = -2;
            }
            else
            {
                boll.dx = 2;
            }
            // Flytta till nästa läge för att starta spelet
            *lage = 2;
            break;
        case 2:
            uppdateraSpelare(speltyp);
            uppdateraBoll(lage); // 'lage' är redan en pekare, så skicka den direkt
            skarmUppdatering();
            break;
        case 3:
            visaSpelavslut(lage); // Skicka pekaren till 'lage'=4
            break;
        }
    }
}
//pointers to store and track type of the game selected and number of iteration that the menu is displayed
void visaStartmeny(uint8_t *speltyp, uint8_t *menyIterationer)
{
    fargaSkarmSvart();
    skrivUtStrangCentrerad("1: 2 spelare", CentreraX, -1, 0);
    skrivUtStrangCentrerad("2: Du mot AI", CentreraX, -1, 8);
    skrivUtStrangCentrerad("3: AI mot AI", CentreraX, -1, 16);

    if (*menyIterationer < 30) // the menu wait 30 iteration and after that checks for button press to select game mode
    {
        (*menyIterationer)++;
    }
    else
    {
        if (arKnappIntryck(0))
        {
            *speltyp = 1;
            *menyIterationer = 0;
        }
        else if (arKnappIntryck(1))
        {
            *speltyp = 2;
            *menyIterationer = 0;
        }
        else if (arKnappIntryck(2))
        {
            *speltyp = 3;
            *menyIterationer = 0;
        }
    }
    skarmUppdatering();
}

void uppdateraSpelare(uint8_t speltyp)
{
    if (speltyp == 3)
    {
        uppdateraAI(&v_spelare, boll);
        uppdateraAI(&h_spelare, boll);
    }
    else
    {
        // flytta vanster spelare
        if (arKnappIntryck(2) && v_spelare.y > 0)
        {
            v_spelare.y -= 1;
        }
        if (arKnappIntryck(3) && v_spelare.y < 19)
        {
            v_spelare.y += 1;
        }
        // flytta hoger spelare (AI om speltyp ar 2)
        if (speltyp == 2)
        {
            uppdateraAI(&h_spelare, boll);
        }
        else
        {
            if (arKnappIntryck(0) && h_spelare.y > 0)
            {
                h_spelare.y -= 1;
            }
            if (arKnappIntryck(1) && h_spelare.y < 19)
            {
                h_spelare.y += 1;
            }
        }
    }/*
    
    // another way to write it
    if (speltyp == 3) { // AIvsAI
        uppdateraAI(&v_spelare, boll); // left player is also controled by AI
        uppdateraAI(&h_spelare, boll);

    }
    // Kontroller för höger spelare
    if (speltyp == 1) // PlayervsPlayer
    {
        if (arKnappIntryck(0) && h_spelare.y > 0) {
            h_spelare.y -= 1;
        }
        if (arKnappIntryck(1) && h_spelare.y < 19) {
            h_spelare.y += 1;
        }
       if (arKnappIntryck(2) && v_spelare.y > 0) // why are you moving the AI position
        {
            v_spelare.y -= 1;
        }
        if (arKnappIntryck(3) && v_spelare.y < 19) // is the third mode AIvsAI
        {
            v_spelare.y += 1;
        }
    } else if (speltyp == 2) //PlayervsAI
    {
        uppdateraAI(&h_spelare, boll);
    }*/
}

void uppdateraBoll(uint8_t *lage)
{
    fargaSkarmSvart();
    uint8_t poang = flyttaBoll(); //Calls the function flyttaBoll() to update the ball's position. This function presumably also checks for collisions and returns a value indicating whether a point has been scored.
    if (poang == 0)
    {
        *lage = 2;
    }
    else
    {
        *lage = 3;
    }
}

//lage pointer represents the current game mode
void visaSpelavslut(uint8_t *lage)
{
    fargaSkarmSvart();
    if (v_spelare.poang == MAX_POANG || h_spelare.poang == MAX_POANG)
    {
        if (iterationer < 150)
        {
            if (v_spelare.poang > h_spelare.poang)
            {
                skrivUtStrangCentrerad("V vinner!", CentreraBada, -1, -1);
                skrivUtStrang(heltalTillStrang(v_spelare.poang), 0, 22);
                skrivUtStrang(heltalTillStrang(h_spelare.poang), 119, 22);
            }
            else
            {
                skrivUtStrangCentrerad("H vinner!", CentreraBada, -1, -1);
                skrivUtStrang(heltalTillStrang(v_spelare.poang), 0, 22);
                skrivUtStrang(heltalTillStrang(h_spelare.poang), 119, 22);
            }
            tandLampor(255);
            iterationer++;
        }
        else
        {
            // tandLampor(0);
            iterationer = 0;
            speltyp = 0;
            main();
        }
    }
    else
    {
        if (iterationer < 60)
        {

            skrivUtStrangCentrerad("Ny runda...", CentreraBada, -1, -1);
            skrivUtStrang(heltalTillStrang(v_spelare.poang), 0, 22);
            skrivUtStrang(heltalTillStrang(h_spelare.poang), 119, 22);
            iterationer++;
        }
        else
        {
            iterationer = 0;
            *lage = 1;
        }
    }
    skarmUppdatering();
}

uint8_t flyttaBoll()
{
    boll.x = boll.x + boll.dx;
    boll.y = boll.y + boll.dy;
    if (boll.y > 29 || boll.y < 3) // check if the ball has hit the top or bottom wall
    {
        boll.dy = boll.dy * -1; //reverse the y-direction of the ball with -1 making the ball bounce off the wall and speed the same
    }
    uint8_t tidigareKollision = kollisionMedSpelare;
    if (kollision() && tidigareKollision != kollisionMedSpelare)
    {
        boll.dx = boll.dx * -1.2;
    }

    ritaUtBoll(boll);
    ritaUtSpelare(v_spelare);
    ritaUtSpelare(h_spelare);

    if (boll.x > 124)
    {
        v_spelare.poang = v_spelare.poang + 1;
        return 1;
    }
    else if (boll.x < 3)
    {
        h_spelare.poang = h_spelare.poang + 1;
        return 1;
    }
    else
    {
        return 0;
    }
}

void spelEjStartat()
{
    fargaSkarmSvart();
    skrivUtStrangCentrerad("Pong!", CentreraX, -1, 8);
    skrivUtStrangCentrerad("Klicka & starta", CentreraX, -1, 16);
    skarmUppdatering();
}

uint8_t kollision()
{
    // Kollision med vänstra paddeln
    if (boll.x <= (v_spelare.x + v_spelare.bredd) // checks if the right edge of the ball is to the right of the left edge of the paddle
        &&
        (boll.x + boll.bredd) >= v_spelare.x // checks if the left edge of the ball is to the right of the left edge of the paddle
        &&
        boll.y <= (v_spelare.y + v_spelare.hojd) // checks if the bottom edge of the ball is below the top edge of the paddle
        &&
        (boll.y + boll.hojd) >= v_spelare.y)// checks if the bottom edge of the ball is above the top bottom of the paddle
    {

        kollisionMedSpelare = 1;
        return 1;
    }

    // Kollision med högra paddeln
    if (boll.x <= (h_spelare.x + h_spelare.bredd) &&
        (boll.x + boll.bredd) >= h_spelare.x &&
        boll.y <= (h_spelare.y + h_spelare.hojd) &&
        (boll.y + boll.hojd) >= h_spelare.y)
    {

        kollisionMedSpelare = 2;
        return 1;
    }

    return 0;
}

void uppdateraAI(Spelare *ai_spelare, Boll boll)
{
    uint8_t mittPunktPaddel = ai_spelare->y + (ai_spelare->hojd / 2);
    uint8_t mittPunktBoll = boll.y + (boll.hojd / 2);

    // Flytta AI-paddeln mot bollens position med viss begränsning
    if (mittPunktBoll < mittPunktPaddel && ai_spelare->y > 0)
    {
        ai_spelare->y -= 1; // Flytta uppåt
    }
    else if (mittPunktBoll > mittPunktPaddel && ai_spelare->y < (32 - ai_spelare->hojd))
    {
        ai_spelare->y += 1; // Flytta nedåt
    }
}
