/**********************************************************************
  This is a project to control a Gavina machine


    ChemaTronics v1.05 
        * MQTT working
        * Adding caldera functions
        - Buttons
        - inputs and outputs

    Changed platform from Arduino Nano to ESP32 (from v0.xx)
   
  Based on code written by Limor Fried/Ladyada
    and Tony DiCola for Adafruit Industries.
    
  MIT license, all text above must be included in any redistribution
 ***********************************************************************/
#include "config.h"
#include "MQTT.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include "logo.h"

#include "funciones.h"

void setup() {
  Serial.begin(115200);
  delay(500);


 Serial.println(VERSION);
 
  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);
  
        if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
        Serial.println("SSD1306 allocation failed");
        //for(;;);
       
      }
      Serial.print("Pantalla inicializada");
        // Clear the buffer.
      display.clearDisplay();
      
     display.drawBitmap(0, 0, chematronics, 128, 64, 1);
    
    
       display.display();

  //Activar pins
  pinMode(BOMBA, OUTPUT);
  pinMode(QUEMADOR, OUTPUT);
  pinMode(VALVULA, OUTPUT);

  //pinMode(BOTUP, INPUT_PULLUP);
  //pinMode(BOTDOWN, INPUT_PULLUP);
  //pinMode(BOTSET, INPUT_PULLUP);
  //pinMode(TERMOSTATO, INPUT_PULLUP);

  digitalWrite(QUEMADOR, HIGH);
  digitalWrite(BOMBA, LOW);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);

  int i = 0;

  WiFi.begin(SECRET_SSID, SECRET_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    i++;
    delay(500);
    Serial.print(".");
    if (i>TOCON) ESP.restart();
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&set);

  prevmillis1 = millis();
  prevmillis2 = millis();
  
       delay(2000);

  leemeEEprom();
}


void loop() {
  int buttonState = 0;

  int i=0;

  if (leeTemp()) {
    alarma=true;

    Serial.println();
    Serial.println("******************************************");
    Serial.println("  AAAAAALLLLLAAAAAAAAARRRRRRRMMMMMMMMM  ");
    Serial.println("   Probable rotura de sensor");
    Serial.println("******************************************");
    Serial.println();
    
  }else{
    Serial.println("lecturas correctas de temperaturas");
  }

  MQTT_connect();
  
         if (millis() - prevmillis2 > MQTTtime) { //Mandamos datos a MQTT cada MQTTtime millis
            prevmillis2 = millis();
              for (i=0;i<3;i++){

                switch (i) {
                      case 0: //temperatura caldera
                        Serial.print("\nSending caldera temp: ");
                        Serial.print(temps[i]);
                        Serial.print("...");
                        if (! calTC.publish(temps[i])) {
                          Serial.println(F("Failed"));
                        } else {
                          Serial.println(F("OK!"));
                        }
                      break;

                      case 1: //temperatura entrada agua
                        Serial.print("\nSending caudal: ");
                        Serial.print(temps[i]);
                        Serial.print("...");
                        if (! calTI.publish(temps[i])) {
                          Serial.println(F("Failed"));
                        } else {
                          Serial.println(F("OK!"));
                        }
                      break;

                      case 2: //temperatura ACS
                        Serial.print("\nSending ACS: ");
                        Serial.print(temps[i]);
                        Serial.print("...");
                        if (! calTA.publish(temps[i])) {
                          Serial.println("Failed");
                        } else {
                          Serial.println("OK!");
                        }
                      break;                                            
                      
                 }//fin del switch
              }

          } 


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &set) {
      Serial.print(F("Got: "));
      Serial.println((char *)set.lastread);
    }
  }

  String dato=(char *)set.lastread;

  dato.trim();

  /********************************************
       botonera, control
    *******************************************
       posibles estados de la pantalla
       
       0= reposo (muestra temperaturas)
       1= configurar temperatura de la caldera
       2= configurar temperatura ACS
       3= mostrar información del software
   ********************************************/
   
 

    if (checkBUT(BOTSET)) {
      // se ha pulsado set 
      prevmillis2=millis();
      if (estado!=0) grabaEEprom();       
      
      if (++estado > 4) estado = 0;

      if (estado==0) estadoS=false;  
    
    }
       
     switch (estado) {
        case 0:
          //estamos en reposo 
          //salva pantalla
          /*
          Serial.print("estado/estadoS: ");
          Serial.print(estado);
          Serial.print("/");
          Serial.println(estadoS);
          */
          
          if (checkBUT(BOTUP)) {
            estadoS=true;
            prevmillis1 = millis();
           
          }
          /*
          Serial.print("millis vs prevmillis1 ");
          Serial.print(millis());
          Serial.print(" vs ");
          Serial.println(prevmillis1);
          */
          if (estadoS){
            if (millis() - prevmillis1 > SCRON) {
              estadoS = false;
              display.clearDisplay();
              display.display();
            } else{
              pantalla();
            }
          } else {
            display.clearDisplay();
            display.display();
          }
 
              
          break;
        case 1:
          //estamos en set temp caldera

          if (millis() - prevmillis2 > SCRONSET) {
            estado=0;

          } else {
    
              pantallaSET("Caldera", Tcal);
    
              if (checkBUT(BOTUP)) {
                // se ha pulsado UP por más de click_time millis
                if(++Tcal>90) Tcal=90;
                pantallaSET("Caldera", Tcal);
      
              }
              
              if (checkBUT(BOTDOWN)) {
                // se ha pulsado DOWN por más de x millis
                if(--Tcal<50) Tcal=50;
                pantallaSET("Caldera", Tcal);
      
              }

          } //endif millis()

          break;
        case 2:
          //estamos en set temp ACS

          if (millis() - prevmillis2 > SCRONSET) {
            estado=0;

          } else {          
    
              pantallaSET("ACS", Tempacs);
    
              if (checkBUT(BOTUP)) {
                // se ha pulsado UP por más de x millis
                if(++Tempacs>80) Tempacs=80;
                pantallaSET("ACS", Tempacs);
      
              }
        
              if (checkBUT(BOTDOWN)) {
                // se ha pulsado DOWN por más de x millis
                if(--Tempacs<40) Tempacs=40;
                pantallaSET("ACS", Tempacs);
      
              }
          }
          
          break;
          case 3:
            //estamos en info. Mostrar en pantalla version e info

              if (millis() - prevmillis2 > SCRONSET) {
                estado=0;
    
              } else {            

                  display.clearDisplay();
          
                  display.drawBitmap(0,0,chematronics, SSD1306_LCDWIDTH,SSD1306_LCDHEIGHT, WHITE);
        
        
                  display.display();
                  delay(1500);
        
                  // Clear the buffer.
                  display.clearDisplay();
          
                  pantallaINFO(1);

              }
    
          break;
          
      }//fin del switch case



    //if alarma
    if (!alarma){
          
          //**************Control temperatura caldera (quemador)
      
          if (temps[0] < (Tcal - 5)) { //77
      
            digitalWrite(QUEMADOR, LOW);
      
            estadoQ=true;
      
            
              pantallaTIP(0, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
          
            //Serial.println("Quemador on");
      
          }
          if (temps[0] > (Tcal + 2)) { //82
      
            digitalWrite(QUEMADOR, HIGH);
      
            estadoQ=false;
      
          
              pantallaTIP(0, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
           
            //Serial.println("Quemador off");
      
          }
      
          // **************fin control quemador *****************
      
      
          // ************* Control temperatura ACS *************************
      
         
          // temperatura ACS control
      
          if (temps[1] < (temps[2]-10)) {  //hay caudal de ACS (caudal)<(Ttacs)
      
            if (temps[2] < (Tempacs - 2)) { //58 calentar ACS
      
              //digitalWrite(VALVULA, HIGH); //apago válvula de 3 vías (queda en ACS)
              estadoT=0;
              //encender bomba de recirculación
              estadoB=1;
         
            }
      
            if (temps[2] > Tempacs + 3) { //63 apago bomba
      
              estadoB=0;
      
      
            } // fin de temperatura ACS control
      
          } else { //no hay flujo ACS
            
          
      
            //******************* gestión calefacción ************************
      
           // int value;
           // value = digitalRead(TERMOSTATO);  //lectura digital de pin
      
            if ((dato.equalsIgnoreCase("on"))) { //termostato en encendido
      
              //Serial.println("Encendido");
              //digitalWrite(VALVULA, LOW); //enciendo válvula de 3 vías (Calefacción)
              estadoT=1;
      
              estadoB=1;
              
              //pantallaTIP(2, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
      
              
      
            } else {
      
              if (temps[2] > Tempacs + 3) { //63 apago bomba
      
                estadoB=0;
      
               } // fin de temperatura ACS control
      
              //estadoB=0; //apago bomba circulación
              //digitalWrite(VALVULA, HIGH); //apago válvula de 3 vías (ACS)
              estadoT=0;
      
              //pantallaTIP(2, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
              
      
            }
      
            //******************* fin gestión  calefacción *******************
          }
      
          //compongo el estado de la Bomba de recirculación (low=off, high=on))
          if (estadoB == 0) {
              
              digitalWrite(BOMBA, LOW);
      
              pantallaTIP(1, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
      
              //Serial.println("Bomba off");
              
          } else {
            
              digitalWrite(BOMBA, HIGH);
      
              pantallaTIP(1, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
              
              //Serial.println("Bomba on");
       
          }
      
          //compongo el estado de la válvula de 3 vías (low=calefacción=on, high=ACS=off)
          if (estadoT == 0) {
      
            digitalWrite(VALVULA, HIGH);
                  
            pantallaTIP(2, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
      
            //Serial.println("Válvula en ACS");
            
          } else {
            digitalWrite(VALVULA, LOW);
                  
            pantallaTIP(2, 1); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
      
            //Serial.println("Válvula en calefacción");
            
          }
    } else {
      //estamos en alarma así que se apaga todo
      alarm();
    }




  
}




int leeTemp(void) {

  /*********************************************
       0 => temperatura caldera
       1 => temperatura caudal
       2 => temperatura ACS
   *********************************************/

  uint8_t i,j;
  int alarma=false;
  uint16_t samples[NUMSAMPLES];
  float average;
  float resist;
  float steinhart;
  float TN;


  for (j=0;j<3;j++){
    // take N samples in a row, with a slight delay
    for (i = 0; i < NUMSAMPLES; i++) {
  
      switch (j){
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
    } //end for i
  
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

  //Serial.print ("Steinharts = "+(char)steinhart);
  if(steinhart > 250) steinhart = 250;

  temps[j]=(int)steinhart;
  
  if (temps[j]>250) temps[j]=250;
  if (temps[j]<-10) temps[j]=-10;
  
  if (temps[j]>100 || temps[j]<0) alarma=true;

  Serial.println(temps[j]);

  }//end for j


  return alarma;

}
