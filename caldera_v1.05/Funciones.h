
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

int checkBUT(int button){
  int buttonState;
  int estado=false;
  
      buttonState = touchRead(button);
      /*
      Serial.print("el botón ");
      Serial.print(button);
      Serial.print(" está en estado: ");
      Serial.println(buttonState);
      delay(1000);
      */
      if (buttonState <= TOUCHTH) {
        delay(click_time);
        buttonState = touchRead(button);
        /*
        Serial.print("el botón ");
        Serial.print(button);
        Serial.print(" está en estado: ");
        Serial.println(buttonState);
        delay(1000);
        */
        if (buttonState <= TOUCHTH) {

          estado = true;
           
          
        }
      } 
  return estado;  
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
  if (alarma){
    display.setCursor(0, 0);
    display.print("ALARMA-sensor roto?");
  }
  display.setCursor(15, 20);
  display.print(VERSION);
  display.display();
  

}
void pantallaTIP(int tip, int valor) {

  if(estadoS){ //si estamos en panatalla negra no dibujamos ni los tips
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

}




void grabaEEprom() {
  int eeAddress = 0;
  EEPROM.write( eeAddress, Tcal );

  eeAddress += sizeof(int);
  EEPROM.write( eeAddress, Tempacs );

}

void pantalla(void) {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 9);
  display.print("Caldera: ");

  display.setCursor(26, 29);
  display.print("Caudal: ");

  display.setCursor(43, 49);
  display.print("ACS: ");

  if (temps[0]<-99){
    //-xxx
    display.setCursor(118, 3);
  }else if (temps[0]<-9){
    //-xx
    display.setCursor(106, 3);
  }else if (temps[0]<0){
    //-x
    display.setCursor(94, 3);
  }else if (temps[0]>9){
    //xx   
    display.setCursor(94, 3);
  }else{
    //x
    display.setCursor(82, 3);
  }
  //display.setCursor(120-26, 3);
  display.print("o");

    if (temps[1]<-99){
    //-xxx
    display.setCursor(118, 23);
  }else if (temps[1]<-9){
    //-xx
    display.setCursor(106, 23);
  }else if (temps[1]<0){
    //-x
    display.setCursor(94, 23);
  }else if (temps[1]>9){
    //xx   
    display.setCursor(94, 23);
  }else{
    //x
    display.setCursor(82, 23);
  }
  //display.setCursor(120-2, 23);
  display.print("o");
    if (temps[2]<-99){
    //-xxx
    display.setCursor(118, 43);
  }else if (temps[2]<-9){
    //-xx
    display.setCursor(106, 43);
  }else if (temps[2]<0){
    //-x
    display.setCursor(94, 43);
  }else if (temps[2]>9){
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
  display.print(temps[0]);
  display.setCursor(70, 25);
  display.print(temps[1]);
  display.setCursor(70, 45);
  display.print(temps[2]);
  pantallaTIP(0, estadoQ);
  pantallaTIP(1, estadoB);
  pantallaTIP(2, estadoT);
  
  display.display();
  
}

void alarm(void){
  //estamos en estado de alarma
  //apagamos quemador
        digitalWrite(QUEMADOR, HIGH);

        estadoQ=0;

    
        pantallaTIP(0, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
     
        Serial.println("Quemador off");

   //apagamos bomba recirculación

        digitalWrite(BOMBA, LOW);

        pantallaTIP(1, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías

        Serial.println("Bomba off");

    //ponemos vávula de 3 vías en ACS (reposo, apagada)

          digitalWrite(VALVULA, HIGH);
                
          pantallaTIP(2, 0); //(tip,valor) tip 0=quemador 1=bomba 2=válvula 3 vías
    
          Serial.println("Válvula en ACS");
      
}

void plog(void){
  
        Serial.print ("Temp Caldera: ");
        Serial.print (temps[0]);
        Serial.print (" * setTemp caldera: ");
        Serial.print (Tcal);
        Serial.print (" * Temp caudal: ");
        Serial.print (temps[1]);
        Serial.print (" * TempACS: ");
        Serial.print (temps[2]);
        Serial.print (" * setTempACS: ");
        Serial.println (Tempacs);

          if (!estadoT) {
      
            Serial.print("Válvula en ACS  -  ");
            
          } else {
            
            Serial.print("Válvula en calefacción  -  ");
            
          }

          if (!estadoB) {
                    
              Serial.print("Bomba off  -  ");
              
          } else {
            
               Serial.print("Bomba on  -  ");
       
          }

          if (!estadoQ){
            
            Serial.println("Quemador off");
            
          } else {
            
            Serial.println("Quemador on");
          }
}
