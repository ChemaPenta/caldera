
//Caldera things
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>

#define NAME Caldera
#define VERSION "Caldera MQTT v1.05"

#define PAUSA 3000 // tiempo en millis entre lecturas
#define click_time 3

#define TOCON 15 //intentos para conectar wifi, si no reset

#define SCRON 3000
#define SCRONSET 10000
#define SCRLOGO 800

#define SERIESRESISTOR 8200
#define TCALDERA 36
#define TCAUDAL 25
#define TACS 26

#define QUEMADOR 4 //7
#define BOMBA 5 //5
#define VALVULA 16 //3
//#define TERMOSTATO 6

#define BOTUP 2 // 11
#define BOTSET 14 //10
#define BOTDOWN 15 //12
#define TOUCHTH 80

#define NUMSAMPLES 3
#define THERMISTORNOMINAL 10000
#define THERMISTORNOMINALACS 100000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950

unsigned long prevmillis1;
unsigned long prevmillis2;
unsigned long prevmillis3;

uint8_t estado = 0; //pantalla
uint8_t estadoQ = 0; //Quemador
uint8_t estadoB = 0; //Bomba
//uint8_t estadoV = 0; //Válvula
uint8_t estadoT = 0; //Tres vías
uint8_t estadoS = false; //SalvaPantalla
uint8_t alarma = false; //protección
int temps[3];

//Temperaturas fijadas

int Tcal; //caldera
int Tempacs; //acs

//Oled things

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
