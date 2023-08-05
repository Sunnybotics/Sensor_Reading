#include  <Arduino.h>
#include "system.h" //contiene las variables del sistema
#include "multitask.h"
#include "ws2812.h" 



void setup() 
{ 
  pinMode(RL1, OUTPUT);digitalWrite(RL1, LOW);
  pinMode(BUZZER, OUTPUT);digitalWrite(BUZZER, LOW);
  //pinMode(prueba, OUTPUT);digitalWrite(prueba, LOW );
  delay(200);
  MultitaskInit();
  delay(1000);
  digitalWrite(RL1, HIGH); digitalWrite(BUZZER, HIGH);
  pinMode(S1, INPUT);pinMode(S2, INPUT);
  pinMode(S3, INPUT);pinMode(S4, INPUT);
  pinMode(S5, INPUT);pinMode(S6, INPUT);
  pinMode(S7, INPUT);pinMode(S8, INPUT);
  pinMode(S9, INPUT);pinMode(S10, INPUT);
  pinMode(S11, INPUT);pinMode(S12, INPUT);
}

void ModbusTransmission(void *arg){
  // Serial.print("Tarea para lectura del control y comunicación modus con los drivers");
  int count = 0; 
  for(;;){
    
    //MostrarValores(); //para imprimir los valores de todos los canales del control
    // delay(1);
    //digitalWrite(prueba, !digitalRead(prueba));

  } 
  
}

void indicaciones(void *arg){
  for(;;){
    statusled(status);
  }
}


void loop() 
{

}

