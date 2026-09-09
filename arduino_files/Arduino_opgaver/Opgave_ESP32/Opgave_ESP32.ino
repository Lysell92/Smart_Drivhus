// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX

#include <SPI.h>
#include <RH_RF95.h>
#include <DIYables_LCD_I2C.h>

#define RFM95_CS 5
#define RFM95_RST 14
#define RFM95_INT 2
#define RF95_FREQ 433.0

// Change to 434.0 or other frequency, must match RX's freq!
//#define RF95_FREQ 915.0
DIYables_LCD_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

void setup() 
{
  //rf95.init();
  lcd.init(); // Initialize the LCD I2C display
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello World!");
  delay(1000);
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);



  while (!rf95.init()) {
  Serial.println("LoRa radio init failed");
  while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  // Set spreading factor to the max.
  rf95.setSpreadingFactor(12);
  rf95.setFrequency(RF95_FREQ); //433
  rf95.setTxPower(23, false);
  //rf95.setSignalBandwidth(400000);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Packet received");
      lcd.setCursor(0, 1);
      lcd.print("Len: ");
      lcd.print(len);
      lcd.setCursor(0, 2);
      lcd.print("RSSI: ");
      lcd.print(rf95.lastRssi());
      lcd.setCursor(0, 3);
      lcd.print("SNR: ");
      lcd.print(rf95.lastSNR());
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("I am alone");
    }
  }
}




/* 
Hvilken frekvens sender LoRa radioen på?
Vores LoRa sender frekvenser på 433 MHz.

Hvilke andre frekvenser kan LoRa radioer sende på?
Rfm 95-97 sender typisk fra 868 eller 915 MHz.

Hvorfor findes de med forskellige frekvenser?
Umiddelbart for at undgå interferens, hvis for mange enheder sender på samme frekvens. Det kan resulterer i problemer med signalet. 

Hvad er fordelene og ulemperne ved hhv. en lavere eller højere frekvens?
Lavere frekvens sender mere dataer og er derfor mere effektive. Men de er også mere ustabile, og dataen kan derfor nemmere gå tabt.
Længere frekvenser kan ikke sende ligeså meget data, men er derimod langt mere stabile og kan sende over langt større afstande.

Her er en liste over nogle af de vigtigste begreber indenfor radiokommunikation og LoRa. 
Fordel begreberne mellem jer og forklar på skift overfor hinanden hvad de betyder.
RSSI:
Received Signal Strength Indicator måler radiosignalets styrke fra en enhed. Den bliver vist som et negativt nummer og jo tættere på nul værdien er, jo større er styrken.

SNR:
Signal-to-Noise Ratio måler det ønskede signal i forhold til støjen forårsaget af signalet. 

Link Budget:
Man kan måle hvor meget af signalet der kan gå tabt og om infrastrukturen tillader at signalet kan nå det ønskede mål. 

CSS (Chirp Spread Spectrum)
I stedet for en fast frekvens øger og sænker den frekvensen over tid, hvilket gør den mere resistent overfor støj, formindsker multi-path fading(finder mål til den ønskede enhed
igennem flere vej, hvilket reducere signalets styrke og doppler shifts indvirkning(hvordan frekvensen ændres afhængigt af afstanden.

Spreading Factor (SF)
Kontrollere hastigheden af signalet. Et lavere sf sender data hurtigere over korte afstand. En højere sf sender dataen langsommere, men med større robusthed. 

Bandwidth(BW)
Definere breden af frekvenskanalen. 

Coding Rate (CR):
Definere proportionen af brugbare data bits ubrugelige bits. Det bliver defineret som 4/5, 4/6, 4/7, 4/8 hvor fire payload bits bliver sammenfattet med bits som er fejlkorrigerende. 

Sync Word:
En unik bit tilføjet til starte af de pakker som bliver sendt, som adskiller de forskellige netværk. Man skal have det samme sync word, hvis man skal kommunikere på de samme netværk.

Preamble: 
Er en bit i pakken, som synkroniserer modtager og afsenderen i tid og frekvens.

Time on Air (ToA): 
Den totale tid det tager at sende en pakke. Den afhænger af SF, BW, CR og payload længden. En højere SF kan fordoble TOA pr. skridt og rækkevidden, men bruger mere batteri
og sænker netværkets kapacitet. 

Duty Cycle: 
Hvor lang tid enheden må sende signal kontra, hvor lang tid den skal være stille. Det forhindrer at netværket bliver overfyldt og reducere støj. I europa er det lovpligtigt.
Hvis der er udstedt en 1% duty cycle, betyder det at radio signalet kun må være aktiv 1 sekund pr 100. sekund.

TX Power:
Justere radiosignalets styrke, typiks fra et spektrum mellem -9 dBm til +22 dbm. Højere tx power forstærker radiosignalet, men bruger også yderligere batteri. 
Der er på samme måde som dutycycle også lovpligtigt begrænsninger på signalstyrkets tilladte rækkevidde.




Modificer koden, så modtager-enheden skriver RSSI og SNR-værdier til displayet.
Find en anden gruppe og fordel rollerne som transmitter og receiver mellem jer på skift, så 
begge grupper kan sende og modtage data.
Gå en runde på skolen og test transceiverens rækkevidde. På hvor lang afstand kan I få de to 
radioer kommunikere med hinanden? 
*/
