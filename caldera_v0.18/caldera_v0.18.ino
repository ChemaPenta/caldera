/************************************************************************
  This is a project to control a Gavina machine
  with a Monochrome OLEDs based on SSD1306 drivers

  A 128x64 size display using I2C to communicate

  ChemaTronics v0.16

  Based on code written by Limor Fried/Ladyada  for Adafruit Industries.

************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define SSD1306_I2C_ADDRESS 0x3D

#define NAME Caldera
#define VERSION "Caldera v0.17"

#define PAUSA 3000 // tiempo en millis entre lecturas
#define click_time 3

#define SCRON 1000
#define SCROFF 2000

#define SERIESRESISTOR 8200
#define TCALDERA A2
#define TCAUDAL A3
#define TACS A6

#define QUEMADOR 7
#define BOMBA 5
#define VALVULA 3
#define TERMOSTATO 6

#define BOTUP 11
#define BOTSET 10
#define BOTDOWN 12

#define NUMSAMPLES 3
#define THERMISTORNOMINAL 10000
#define THERMISTORNOMINALACS 100000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

unsigned long time;
uint8_t estado = 0; //pantalla
uint8_t estadoQ = 0; //Quemador
uint8_t estadoB = 0; //Bomba
//uint8_t estadoV = 0; //Válvula
uint8_t estadoT = 0; //Tres vías
uint8_t estadoS = 0; //SalvaPantalla

static const unsigned PROGMEM char chematronics [] = {
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01100000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000011,B11110000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000011,B11110000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011000,B00000111,B11110000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B11111000,B00000111,B11111000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000001,B11111000,B00000111,B11111000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00111111,B00000001,B11111000,B00001111,B01111000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111000,B00111111,B00000011,B11111000,B00001111,B01111100,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111111,B11111000,B00111111,B10000011,B11111000,B00001111,B01111100,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111000,B01111111,B11111000,B00111111,B10000011,B11111000,B00011110,B00111100,B00000000,B00000000,
B00000000,B00000000,B00000001,B11111110,B00001111,B10000000,B11111000,B01111111,B11111000,B00111111,B10000111,B01111000,B00011110,B00111110,B00000000,B00000000,
B00000000,B00000000,B00000111,B11111111,B10001111,B10000000,B11111000,B01111100,B00000000,B00111111,B11000111,B01111000,B00011110,B00111110,B00000000,B00000000,
B00000000,B00000000,B00011111,B11111111,B10001111,B10000000,B11111000,B01111100,B00000000,B00111101,B11001111,B01111000,B00111110,B00011110,B00000000,B00000000,
B00000000,B00000000,B00011111,B10000011,B10001111,B10000000,B11111000,B01111100,B00000000,B00111101,B11101110,B01111000,B00111100,B01111111,B00000000,B00000000,
B00000000,B00000000,B00111110,B00000000,B10001111,B10000000,B11110000,B01111100,B00011000,B00111101,B11101110,B01111000,B00111111,B11111111,B00000000,B00000000,
B00000000,B00000000,B00111110,B00000000,B00001111,B10000000,B11110000,B01111111,B11111000,B00111100,B11111110,B01111000,B00111111,B11111111,B00000000,B00000000,
B00000000,B00000000,B01111100,B00000000,B00001111,B11111111,B11110000,B01111111,B11111000,B00111100,B11111100,B01111000,B01111111,B11111111,B10000000,B00000000,
B00000000,B00000000,B01111100,B00000000,B00001111,B11111111,B11110000,B01111111,B11111000,B00111100,B11111100,B01111000,B01111111,B00001111,B10000000,B00000000,
B00000000,B00000000,B01111100,B00000000,B00001111,B11111111,B11110000,B01111111,B00000000,B01111100,B01111100,B01111000,B01111000,B00000111,B10000000,B00000000,
B00000000,B00000000,B01111100,B00000000,B00001111,B11111111,B11110000,B01111000,B00000000,B01111100,B01111000,B01111000,B11111000,B00000111,B00000000,B00000000,
B00000000,B00000000,B01111100,B00000000,B00001111,B10000000,B11110000,B01111000,B00000000,B01111100,B00111000,B01111000,B11110000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00111110,B00000000,B00001111,B10000000,B11110000,B01111000,B00000000,B01111100,B00000000,B01111000,B11110000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00111110,B00000000,B00001111,B10000000,B11110000,B01111000,B00011000,B01111100,B00000000,B01111001,B10000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00011111,B00000001,B00001111,B10000000,B11110000,B01111111,B11111000,B01111100,B00000000,B01100000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00011111,B11111111,B00001111,B10000000,B11110000,B01111111,B11111000,B01111100,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000111,B11111111,B00001111,B10000001,B11110000,B01111111,B11111000,B01100000,B00000000,B00000000,B00000000,B00000000,B00000011,B11000000,
B00000000,B00000000,B00000001,B11111111,B00001111,B10000001,B11110000,B01111110,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00111111,B11000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111111,B11000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111111,B11000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00011111,B11000001,B11111000,B01000000,
B00000100,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B01111111,B11000011,B11100000,B00000000,
B00000111,B11100000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000001,B11111111,B11000011,B11000000,B00000000,
B00000111,B11111111,B11100000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000110,B00011111,B00000011,B11111111,B11000011,B11000000,B00000000,
B00000011,B11111111,B11111001,B11111000,B00000000,B00000000,B00000000,B00000001,B11110000,B00011110,B00011111,B00000011,B11100000,B00000011,B11000000,B00000000,
B00000000,B00111111,B11111001,B11111111,B11000000,B00000011,B11111110,B00000001,B11111000,B00011110,B00011111,B00000111,B11000000,B00000011,B11111111,B10000000,
B00000000,B00111110,B01111001,B11111111,B11110000,B00001111,B11111111,B00000001,B11111000,B00111110,B00011111,B00001111,B10000000,B00000011,B11111111,B11100000,
B00000000,B00111110,B00000001,B11111111,B11110000,B00011111,B11111111,B10000001,B11111100,B00111110,B00011111,B00001111,B10000000,B00000001,B11111111,B11100000,
B00000000,B00111110,B00000001,B11100000,B11111000,B00111111,B00001111,B11000001,B11111100,B00111110,B00011111,B00001111,B00000000,B00000001,B11111111,B11100000,
B00000000,B00111110,B00000001,B11100000,B11111000,B01111110,B00000111,B11000001,B11111110,B00111110,B00011111,B00011111,B00000000,B00000000,B00111111,B11100000,
B00000000,B00111110,B00000001,B11100000,B11111000,B01111100,B00000011,B11100001,B11111110,B00111110,B00011111,B00011111,B00000000,B00000000,B00000001,B11100000,
B00000000,B00111110,B00000001,B11100000,B11110000,B01111100,B00000011,B11100001,B11101111,B00111110,B00011111,B00011111,B00000000,B00000000,B00000001,B11100000,
B00000000,B00111110,B00000001,B11111111,B11110000,B01111000,B00000011,B11100001,B11101111,B00111110,B00011111,B00011111,B00000000,B00000000,B00000001,B11100000,
B00000000,B00111110,B00000001,B11111111,B11000000,B01111000,B00000011,B11100001,B11100111,B10111110,B00011111,B00011111,B00000000,B00000000,B00000011,B11100000,
B00000000,B00111110,B00000001,B11111111,B11000000,B01111000,B00000011,B11100001,B11100111,B10111110,B00011111,B00011111,B00000000,B00000011,B10011111,B11000000,
B00000000,B00111110,B00000001,B11100011,B11100000,B01111000,B00000011,B11100001,B11100011,B11111110,B00011111,B00001111,B10000000,B01000011,B11111111,B11000000,
B00000000,B00111110,B00000001,B11100001,B11110000,B01111100,B00000011,B11100001,B11100011,B11111110,B00011111,B00001111,B11000001,B11000011,B11111111,B00000000,
B00000000,B00111110,B00000001,B11100000,B11110000,B01111100,B00000011,B11100001,B11100001,B11111110,B00011111,B00001111,B11111111,B11000011,B11111110,B00000000,
B00000000,B00001100,B00000001,B11100000,B11111000,B01111100,B00000111,B11000011,B11100001,B11111110,B00011111,B00000111,B11111111,B11000000,B00000000,B00000000,
B00000000,B00000000,B00000001,B11100000,B11111000,B00111111,B00001111,B11000011,B11100000,B11111110,B00011111,B00000011,B11111111,B10000000,B00000000,B00000000,
B00000000,B00000000,B00000001,B11100000,B01111100,B00011111,B11111111,B10000011,B11100000,B11111110,B00011111,B00000000,B11111110,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B01111100,B00001111,B11111111,B00000011,B11100000,B01111110,B00011111,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000111,B11111100,B00000011,B11100000,B01111110,B00011110,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
  };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//Temperaturas fijadas

int Tcal; //caldera
int Tempacs; //acs


void leemeEEprom() {
  int eeAddress = 0;
  EEPROM.get( eeAddress, Tcal );

  if (Tcal < 50 or Tcal > 95) {
    Tcal = 80;
    EEPROM.put(eeAddress, Tcal);
  }
  eeAddress += sizeof(int);
  EEPROM.get( eeAddress, Tempacs );

  if (Tempacs < 40 or Tempacs > 95) {
    Tempacs = 60;
    EEPROM.put(eeAddress, Tempacs);
  }

}

void grabaEEprom() {
  int eeAddress = 0;
  EEPROM.update( eeAddress, Tcal );

  eeAddress += sizeof(int);
  EEPROM.update( eeAddress, Tempacs );

}


float leeTemp(int sensor) {

  

  /*********************************************
       0 => temperatura caldera
       1 => temperatura caudal
       2 => temperatura ACS
   *********************************************/

  uint8_t i;
  uint16_t samples[NUMSAMPLES];
  float average;
  float resist;
  float steinhart;
  float TN;

  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES; i++) {

    switch (sensor){
      case 0:
        samples[i] = analogRead(TCALDERA);
        TN=10000;
      break;
      case 1:
        samples[i] = analogRead(TCAUDAL);
        TN=10000;
      break;
      case 2:
        samples[i] = analogRead(TACS);
        TN=100000;
      break;

    }
    delay(5);
  }

  // average all the samples out
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;
  
  // convert the value to resistance
  resist = 1023 / average - 1;

  resist = SERIESRESISTOR / resist;
  
  steinhart = resist / TN; //THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C

  return steinhart;

}

  
void setup()   {
  Serial.begin(9600);

  pinMode(BOMBA, OUTPUT);
  pinMode(QUEMADOR, OUTPUT);
  pinMode(VALVULA, OUTPUT);

  pinMode(BOTUP, INPUT_PULLUP);
  pinMode(BOTDOWN, INPUT_PULLUP);
  pinMode(BOTSET, INPUT_PULLUP);
  pinMode(TERMOSTATO, INPUT_PULLUP);

  digitalWrite(QUEMADOR, HIGH);
  digitalWrite(BOMBA, LOW);

  time = millis();

  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, false);
  // init done

    // Clear the buffer.
  display.clearDisplay();
  
 display.drawBitmap(0,0,chematronics, SSD1306_LCDWIDTH,SSD1306_LCDHEIGHT, WHITE);


   display.display();
   delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  analogReference(EXTERNAL);

  leemeEEprom();

}

void loop() {
  int buttonState = 0;
  int caudal, cal, Ttacs; // temperaturas acutales de caudal, caldera y agua caliente sanitaria

  cal = (int)leeTemp(0);

  caudal = (int)leeTemp(1);

  Ttacs = (int)leeTemp(2);

  /********************************************
       botonera, control
    *******************************************
       posibles estados de la pantalla
       
       0= reposo (muestra temperaturas)
       1= configurar temperatura de la caldera
       2= configurar temperatura ACS
       3= mostrar información del software
   ********************************************/

  buttonState = digitalRead(BOTSET);
  if (buttonState == LOW) {
    delay(click_time);
    buttonState = digitalRead(BOTSET);
    if (buttonState == LOW) {
      // se ha pulsado set por más de click_time
      if (estado!=0) grabaEEprom();       
      
      if (++estado > 4) estado = 0;

      if (estado==0) estadoS=0;
      
      
    }
  }

  switch (estado) {
    case 0:
      //estamos en reposo 
      //salva pantalla
       switch (estadoS) {
         case 0: //puesta info de temperaturas en pantalla
          
          if (millis() - time > 1200) {
            time = millis();
            if (++estadoS > 1) estadoS = 0;
            display.clearDisplay();
  
            display.drawBitmap(0,0,chematronics, SSD1306_LCDWIDTH,SSD1306_LCDHEIGHT, WHITE);

            display.display();

          } else {
            pantalla(cal, caudal, Ttacs);
          }
        break;

        case 1: //logo en pantalla
          if (millis() - time > 800) {
            time = millis();
            if (++estadoS > 2) estadoS = 0;
            display.clearDisplay();

          }          
        break;
        case 2: //Pantalla negra
          if (millis() - time > 2000) {
            time = millis();
            if (++estadoS > 2) estadoS = 0;
            //display.clearDisplay();
            pantalla(cal, caudal, Ttacs);
          }          
        break;
        
        }
          
      break;
    case 1:
      //estamos en set temp caldera

      pantallaSET("Caldera", Tcal);
      buttonState = digitalRead(BOTUP);
      if (buttonState == LOW) {
        delay(click_time);
        buttonState = digitalRead(BOTUP);
        if (buttonState == LOW) {
          // se ha pulsado UP por más de click_time millis
          if(++Tcal>90) Tcal=90;
          pantallaSET("Caldera", Tcal);

        }
      }

      buttonState = digitalRead(BOTDOWN);
      if (buttonState == LOW) {
        delay(click_time);
        buttonState = digitalRead(BOTDOWN);
        if (buttonState == LOW) {
          // se ha pulsado DOWN por más de x millis
          if(--Tcal<50) Tcal=50;
          pantallaSET("Caldera", Tcal);

        }
      }


      break;
    case 2:
      //estamos en set temp ACS

      pantallaSET("ACS", Tempacs);
      buttonState = digitalRead(BOTUP);
      if (buttonState == LOW) {
        delay(click_time);
        buttonState = digitalRead(BOTUP);
        if (buttonState == LOW) {
          // se ha pulsado UP por más de x millis
          if(++Tempacs>80) Tempacs=80;
          pantallaSET("ACS", Tempacs);

        }
      }

      buttonState = digitalRead(BOTDOWN);
      if (buttonState == LOW) {
        delay(click_time);
        buttonState = digitalRead(BOTDOWN);
        if (buttonState == LOW) {
          // se ha pulsado DOWN por más de x millis
          if(--Tempacs<40) Tempacs=40;
          pantallaSET("ACS", Tempacs);

        }
      }
      break;
      case 3:
        //estamos en info. Mostrar en pantalla version e info

          display.clearDisplay();
  
          display.drawBitmap(0,0,chematronics, SSD1306_LCDWIDTH,SSD1306_LCDHEIGHT, WHITE);


          display.display();
          delay(1500);

          // Clear the buffer.
          display.clearDisplay();
  
        pantallaINFO(1);

      break;
  }


    
    //**************Control temperatura caldera (quemador)

    if (cal < (Tcal - 5)) { //77

      digitalWrite(QUEMADOR, LOW);

      estadoQ=1;

      pantallaTIP(0, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

      Serial.println("Quemador on");

    }
    if (cal > (Tcal + 2)) { //82

      digitalWrite(QUEMADOR, HIGH);

      estadoQ=0;

      pantallaTIP(0, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

      Serial.println("Quemador off");

    }

    // **************fin control quemador *****************


    // ************* Control temperatura ACS *************************

   
    // temperatura ACS control

    if (caudal < (Ttacs-10)) {  //hay caudal de ACS

      if (Ttacs < (Tempacs - 2)) { //58 calentar ACS

        //digitalWrite(VALVULA, HIGH); //apago válvula de 3 vías (queda en ACS)
        estadoT=0;
        //encender bomba de recirculación
        estadoB=1;
   
      }

      if (Ttacs > Tempacs + 3) { //63 apago bomba

        estadoB=0;


      } // fin de temperatura ACS control

    } else { //no hay flujo ACS
      
    

      //******************* gestión calefacción ************************

      int value;
      value = digitalRead(TERMOSTATO);  //lectura digital de pin

      if (value == HIGH) { //termostato en apagado

        estadoB=0; //apago bomba circulación
        
        //digitalWrite(VALVULA, HIGH); //apago válvula de 3 vías (ACS)
        estadoT=0;

        pantallaTIP(2, 0); //(tip,valor)  tip 0=quemador 1=bomba 2=válvula 3 vías

      } else {
        
        //Serial.println("Encendido");
        //digitalWrite(VALVULA, LOW); //enciendo válvula de 3 vías (Calefacción)
        estadoT=1;

        estadoB=1;
        
        pantallaTIP(2, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

      }

      //******************* fin gestión  calefacción *******************
    }

    //compongo el estado de la Bomba de recirculación (low=off, high=on))
    if (estadoB == 0) {
        
        digitalWrite(BOMBA, LOW);

        pantallaTIP(1, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

        Serial.println("Bomba off");
        
    } else {
      
        digitalWrite(BOMBA, HIGH);

        pantallaTIP(1, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
        
        Serial.println("Bomba on");
 
    }

    //compongo el estado de la válvula de 3 vías (low=calefacción=on, high=ACS=off)
    if (estadoT == 0) {

      digitalWrite(VALVULA, HIGH);
            
      pantallaTIP(2, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

      Serial.println("Válvula en ACS");
      
    } else {
      digitalWrite(VALVULA, LOW);
            
      pantallaTIP(2, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

      Serial.println("Válvula en calefacción");
      
    }
        Serial.print("Temp Caldera: ");
        Serial.print(cal);
        Serial.print (" * setTemp caldera: ");
        Serial.print (Tcal);
        Serial.print (" * Temp caudal: ");
        Serial.print (caudal);
        Serial.print (" * TempACS: ");
        Serial.print (Ttacs);
        Serial.print (" * setTempACS: ");
        Serial.println (Tempacs);
}



void pantalla(int cal, int caudal, int Ttacs) {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 9);
  display.print("Caldera: ");

  display.setCursor(26, 29);
  display.print("Caudal: ");

  display.setCursor(43, 49);
  display.print("ACS: ");

  if (cal<-99){
    //-xxx
    display.setCursor(118, 3);
  }else if (cal<-9){
    //-xx
    display.setCursor(106, 3);
  }else if (cal<0){
    //-x
    display.setCursor(94, 3);
  }else if (cal>9){
    //xx   
    display.setCursor(94, 3);
  }else{
    //x
    display.setCursor(82, 3);
  }
  //display.setCursor(120-26, 3);
  display.print("o");

    if (caudal<-99){
    //-xxx
    display.setCursor(118, 23);
  }else if (caudal<-9){
    //-xx
    display.setCursor(106, 23);
  }else if (caudal<0){
    //-x
    display.setCursor(94, 23);
  }else if (caudal>9){
    //xx   
    display.setCursor(94, 23);
  }else{
    //x
    display.setCursor(82, 23);
  }
  //display.setCursor(120-2, 23);
  display.print("o");
    if (Ttacs<-99){
    //-xxx
    display.setCursor(118, 43);
  }else if (Ttacs<-9){
    //-xx
    display.setCursor(106, 43);
  }else if (Ttacs<0){
    //-x
    display.setCursor(94, 43);
  }else if (Ttacs>9){
    //xx   
    display.setCursor(94, 43);
  }else{
    //x
    display.setCursor(82, 43);
  }
  //display.setCursor(120-38, 43);
  display.print("o");

  display.setTextSize(2);
  display.setCursor(70, 5);
  display.print(cal);
  display.setCursor(70, 25);
  display.print(caudal);
  display.setCursor(70, 45);
  display.print(Ttacs);
  pantallaTIP(0, estadoQ);
  pantallaTIP(1, estadoB);
  pantallaTIP(2, estadoT);
  
  display.display();
}

void pantallaSET(char set[15], int temp) {

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  display.print(set);
  display.setTextSize(5);
  display.setCursor(35, 25);
  display.print(temp);
  display.setCursor(95, 20);
  display.setTextSize(2);
  display.print("o");
  display.display();
}

void pantallaINFO(int i){

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Chematronics");
  display.setCursor(15, 20);
  display.print(VERSION);
  display.display();
  

}
void pantallaTIP(int tip, int valor) {

  
  display.setTextSize(1);

  if (valor == 1) {
    display.setTextColor(WHITE);
  } else {
    display.setTextColor(BLACK);
  }

  switch (tip) {
    case 0: //quemador

      display.setCursor(0, 55);
      display.print("Q");

      break;

    case 1: //Bomba
      display.setCursor(0, 45);
      display.print("B");
      break;

    case 2: //Termostato
      display.setCursor(0, 35);
      display.print("T");
      break;
  }
  display.display();

}
