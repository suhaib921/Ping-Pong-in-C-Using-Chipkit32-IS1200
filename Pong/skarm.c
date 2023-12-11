/******************************************************************************************************
skarm.c, de första fem funktionerna är till stor del kopierade (se kommentarer ovanför metoderna). De 8 efterföljande är skrivna av Suhaib Abdi Muhummed
******************************************************************************************************/

#include "skarm.h"

static uint8_t skarmbuffer[128][4]; // skärmen har en bredd på 128 pixlar och en höjd på 32 pixlar (arraystorleken skarmbuffer[128][4], men se pixelhantering i fargaPixel-funktionen

// Delay-funktion från is1200-example-projects h4xxel, https://github.com/is1200-example-projects/hello-serial/blob/master/main.c

void delay(int cyc)
{
  int i;
  for (i = cyc; i > 0; i--)
    ;
}

// Logiken kopierad och uppdaterad från tidigare labbfiler och is1200-example-projects https://github.com/is1200-example-projects. skarmUppdatering kan både kan skriva ut nagot eller rensa skarmen.

void skarmUppdatering() // for varje rad (4 st.) skickas varje kolumn (16 st.) till skarmen. Det sker i bit-form, alltså 16*8=128
{
  int rad, kolumn, k;
  int c;
  for (rad = 0; rad < 4; rad++)
  {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spiSkickaTaEmot(0x22); // satt vilken sida som ska skrivas till
    spiSkickaTaEmot(rad);

    spiSkickaTaEmot(0x0);  // satt startrad
    spiSkickaTaEmot(0x10); // satt slutrad

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (kolumn = 0; kolumn < 128; kolumn++)
    {
      // databyte som ska skickas till skarmen - en integerrepresentation av tecknet
      spiSkickaTaEmot(skarmbuffer[kolumn][rad]); // font[c * 8 + k], c multipliceras med 8 da varje tecken ar 8 bitar langt. Vi skickar varje bit i tecknet till skarmen.
    }
  }
}

// Logiken kopierad från tidigare labbfiler och is1200-example-projects https://github.com/is1200-example-projects. spiSkickaTaEmot gor att vi kan skicka och ta emot data samtidigt. Skickar och ta emot data via SPI-kommunikation mellan en huvudprocessor och en bildskarm.

uint8_t spiSkickaTaEmot(uint8_t data)
{
  while (!(SPI2STAT & 0x08)) // Vanta pa att sandbufferten blir tom, vilket bit 3 visar
    ;
  SPI2BUF = data;         // Skriv till bufferten
  while (!(SPI2STAT & 1)) // Vanta pa att mottagningsbufferten blir full, vilket bit 0 visar
    ;
  return SPI2BUF; // Returnera det som finns i mottagningsbufferten
}

// Inspirerad av (logiken väldigt lik) tidigare labbfiler och is1200-example-projects https://github.com/is1200-example-projects. Initiering av skärmen

void initieraSkarm(void)
{
  // Kommandon som 0xAE, 0x8D, 0xD9, 0xA1, 0xC8, 0xDA, 0x20, och 0xAF skickas med spiSkickaTaEmot for att konfigurera olika aspekter av displayen som aktiv spanning, reset, och visningslage, bland andra.
  DISPLAY_CHANGE_TO_COMMAND_MODE;
  delay(10);
  DISPLAY_ACTIVATE_VDD;
  delay(1000000);

  spiSkickaTaEmot(0xAE);  // kommando for att sla av displayen
  DISPLAY_ACTIVATE_RESET; // kommando for att sla pa reset
  delay(10);
  DISPLAY_DO_NOT_RESET; // ser till att reset inte ar pa
  delay(10);

  // Spanning till skarmen
  spiSkickaTaEmot(0x8D); // satta laddningspumpen
  spiSkickaTaEmot(0x14); // aktivera laddningspumpen
  spiSkickaTaEmot(0xD9); // satt precharge period, alltsa hur lange laddningspumpen ska ladda upp kondensatorerna?
  spiSkickaTaEmot(0xF1); // 11110001 laddningsfas 1 och 2? 15 klockcykler for fas 1, 1 klockcykel for fas 2

  DISPLAY_ACTIVATE_VBAT; // aktivera VBAT (battery voltage), spanningskalla for att halla koll pa tiden
  delay(10000000);

  spiSkickaTaEmot(0xA1); // set segment remap, alltsa hur segmenten ska vara organisekolumne
  spiSkickaTaEmot(0xC8); // set com scan remap, alltsa hur com pins ska vara organisekolumne

  spiSkickaTaEmot(0xDA); // set com pin config, alltsa hur manga com pins som ska anvandas
  spiSkickaTaEmot(0x20); // set seq com leftright remap, alltsa hur com pins ska vara organisekolumne

  spiSkickaTaEmot(0xAF); // kommando for att sla pa displayen
  fargaSkarmSvart();
  skarmUppdatering();
}

// Inspirerad av (logiken väldigt lik) tidigare labbfiler och is1200-example-projects https://github.com/is1200-example-projects. Funktionen heltalTillStrang tar en integer som argument och returnerar en pekare till en strang.
char *heltalTillStrang(int tal)
{
  // Deklarera variabler.
  // 'position' används för iteration, 'tecken' för att lagra talets tecken.
  int position, tecken;

  // 'konverteringsBuffert' används för att lagra det konverterade talet som sträng.
  static char konverteringsBuffert[ITOA_BUFSIZ];

  // 'storstNegativt' hanterar det största negativa 32-bitars talet.
  static const char storstNegativt[] = "-2147483648";

  // Sätt det sista elementet i bufferten till nulltecken för att markera strängens slut.
  konverteringsBuffert[ITOA_BUFSIZ - 1] = '\0';

  // Spara talets tecken.
  tecken = tal;

  // Hantera det största negativa talet som ett specialfall.
  if (tal < 0 && tal - 1 > 0)
  {
    // Kopiera 'storstNegativt' till bufferten.
    for (position = 0; position < sizeof(storstNegativt); position++)
      konverteringsBuffert[position + 1] = storstNegativt[position];
    position = 0;
  }
  else
  {
    // Gör talet positivt om det är negativt.
    if (tal < 0)
      tal = -tal;

    // Positionen för den första ASCII-siffran.
    position = ITOA_BUFSIZ - 2;

    // Loop för att konvertera varje siffra i det positiva talet till en sträng.
    do
    {
      konverteringsBuffert[position] = (tal % 10) + '0';
      tal = tal / 10;
      position--;
    } while (tal > 0);

    // Lägg till minustecknet om talet var negativt.
    if (tecken < 0)
    {
      konverteringsBuffert[position] = '-';
      position--;
    }
  }
  // Returnera en pekare till den första siffran i den skapade strängen.
  return &konverteringsBuffert[position + 1];
}

// En modifierad version av utskriftsmetod skriven av Axel Isaksson/F Lundevall, se COPYING.txt-fil.
void skrivUtStrang(char *s, int x, int y)
{
  int teckenIndex, kolumnIndex, radIndex, aktuelltTecken;
  int antalTrycktaTecken;
  char sBuffert[16];

  if (x < 0 || x > (127 - 8) || y < 0 || y > (31 - 8))
    return;

  if (!s)
    return;

  for (teckenIndex = 0; teckenIndex < 16; teckenIndex++)
  {
    if (*s)
    {
      sBuffert[teckenIndex] = *s;
      s++;
    }
    else
    {
      sBuffert[teckenIndex] = ' ';
    }
  }

  for (antalTrycktaTecken = 0; antalTrycktaTecken < 16; antalTrycktaTecken++)
  {
    aktuelltTecken = sBuffert[antalTrycktaTecken];

    for (kolumnIndex = 0; kolumnIndex < 8; kolumnIndex++)
    {
      int teckenSlice = font[aktuelltTecken * 8 + kolumnIndex];

      for (radIndex = 0; radIndex < 8; radIndex++)
      {
        if ((teckenSlice >> radIndex) & 0x1)
          fargaPixel(x + 8 * antalTrycktaTecken + kolumnIndex, y + radIndex);
      }
    }
  }
}

// ritaUtSpelare, av Suhaib Abdi Muhummed

void ritaUtSpelare(Spelare spelare)
{
  ritaRektangel(spelare.x, spelare.x + spelare.bredd, spelare.y, spelare.y + spelare.hojd);
}

// ritaUtBoll, av Suhaib Abdi Muhummed

void ritaUtBoll(Boll boll)
{
  ritaRektangel(boll.x, boll.x + boll.bredd, boll.y, boll.y + boll.hojd);
}

// ritaRektangel, av Suhaib Abdi Muhummed

void ritaRektangel(int startX, int slutX, int startY, int slutY)
{
  if (slutY < startY || slutX < startX)
  {
    return;
  }

  int rad, kolumn;
  int hojd = slutY - startY;
  int bredd = slutX - startX;

  for (rad = startY; (rad < 32) && (rad < (startY + hojd)); rad++)
  {
    for (kolumn = startX; (kolumn < 128) && (kolumn < (startX + bredd)); kolumn++)
    {
      fargaPixel(kolumn, rad);
    }
  }
}

// fargaPixel, av Suhaib Abdi Muhummed

/*
yforskjutning                         y=0-32                   aktuell_y_byte
_________________________________________________________________
0 byte representerar                [8] vertikala pixlar        0
----------------------------------------
1 byte representerar                [8] vertikala pixlar        1
----------------------------------------
2 byte representerar                [8] vertikala pixlar        2
----------------------------------------
3 byte representerar                [8] vertikala pixlar        3
__________________________________________________________________

exampelvis om jag vill aktivera pixel 14 på y axel och x axeln 36.
yforskjutning= 14%8= 6
aktuell_y_byte= (14 - 6) / 8= 1
skarmbuffer[36][1] |= 0x1 << 6;
0010 0000

*/

void fargaPixel(uint8_t x, uint8_t y)
{
  if (x < 0 || x > 127 || y < 0 || y > 31)
  {
    return;
  }
  else
  {
    int yforskjutning = y % 8;
    int aktuell_y_byte = (y - yforskjutning) / 8;
    skarmbuffer[x][aktuell_y_byte] |= 0x1 << yforskjutning;
  }
}

// fargaSkarmSvart, av Suhaib Abdi Muhummed
// Kommentar: Iställer för att uppdatera skärmbufferten skulle vi egentligen kunna skicka spiSkickaTaEmot(0x0); Detta skulle kunna lösas med en "bool clear" som parameter i skarmUppdatering-funktionen högre upp.

void fargaSkarmSvart(void)
{
  int rad, kolumn;
  for (rad = 0; rad < 4; rad++)
  {
    for (kolumn = 0; kolumn < 128; kolumn++)
    {
      skarmbuffer[kolumn][rad] = 0x00;
    }
  }
}

// strangLangd, av Suhaib Abdi Muhummed
// Hämta antal bokstaver i en strang

int strangLangd(const char *s)
{
  int langd = 0;
  while (s[langd] != '\0')
  {
    langd++;
  }
  return langd;
}

// skrivUtStrangCentrerad, av Suhaib Abdi Muhummed
// Skriv ut en strang centrerad utifrån antal bokstaver och mitten av skarmens bredd/hojd (64/16)

void skrivUtStrangCentrerad(char *s, CentreringsAxel axel, int x, int y)
{
  int langd = strangLangd(s);

  if ((axel == CentreraX || axel == CentreraBada) && x == -1)
  {
    x = 64 - (langd * 8 / 2);
  }
  if ((axel == CentreraY || axel == CentreraBada) && y == -1)
  {
    y = 16 - 4;
  }

  skrivUtStrang(s, x, y);
}
