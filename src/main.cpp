#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ws2812.h"
#include "system.h" // Contiene las variables del sistema
#include "multitask.h"


const char* ssid = "WIFI_8K_WAFF1E2";
// const char* ssid = "ROBOTICSNEXTGY";
// const char* password = "Sunnytop2022";

const int group1Pins[4] = {19, 18, 12, 14};
const int group2Pins[4] = {26, 32, 33, 35};

const int relay1 = RL1;
const int relay2 = RL2;
const int buzzer = BUZZER;

int group1Count = 0;
int group2Count = 0;

unsigned long lastSensorUpdateTime = 0;
const unsigned long sensorUpdateInterval = 2; // Cambiado a 10ms

void actualizarSensores() {
  group1Count = 0;
  group2Count = 0;

  for (int i = 0; i < 4; i++) {
    group1Count += digitalRead(group1Pins[i]) == HIGH;
    group2Count += digitalRead(group2Pins[i]) == HIGH;
  }

  bool relay1Active = (group1Count >= 4);
  bool relay2Active = (group2Count >= 4);

  digitalWrite(relay1, relay1Active ? HIGH : LOW);
  digitalWrite(relay2, relay2Active ? LOW : HIGH);

  if (relay1Active || relay2Active) {
    digitalWrite(buzzer, LOW);
  } else {
    digitalWrite(buzzer, HIGH);
  }
  //printf("datos enviados\n");

}

// void controlarRelesYBuzzer() {
//   bool relay1Active = (group1Count >= 4);
//   bool relay2Active = (group2Count >= 4);

//   digitalWrite(relay1, relay1Active ? HIGH : LOW);
//   digitalWrite(relay2, relay2Active ? LOW : HIGH);

//   if (relay1Active || relay2Active) {
//     digitalWrite(buzzer, LOW);
//   } else {
//     digitalWrite(buzzer, HIGH);
//   }
//   printf("datos enviados\n");
// }


void setup() {

  MultitaskInit();
  Serial.begin(115200);

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, LOW);

  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);

  for (int i = 0; i < 4; i++) {
    pinMode(group1Pins[i], INPUT);
    pinMode(group2Pins[i], INPUT);
  }

  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());


}

void loop() {
  
}

void indicaciones(void *arg) {
  unsigned long previousMillis = 0;
  const unsigned long indicacionesInterval = 10; // Intervalo de 100 ms para actualizar sensores

  for (;;) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= indicacionesInterval) {
      actualizarSensores();
      previousMillis = currentMillis;
    }
  }
}


void ModbusTransmission(void *arg){
  
  unsigned long previousMillis = 0;
  const unsigned long modbusInterval = 1000; // Intervalo de 1000 ms para la transmisión Modbus

  for(;;){
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= modbusInterval) {

    if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    // String datos_a_enviar = "senFrontales=" + (String)group1Count + "&senTraseros=" + (String)group2Count;
    String datos_a_enviar = "sen1del=" + (String)digitalRead(group1Pins[0])  + "&sen2del=" + (String)digitalRead(group1Pins[1]) + "&sen3del=" + (String)digitalRead(group1Pins[2]) + "&sen4del=" + (String)digitalRead(group1Pins[3]) + "&sen1atras=" + (String)digitalRead(group2Pins[0]) + "&sen2atras=" + (String)digitalRead(group2Pins[1]) + "&sen3atras=" + (String)digitalRead(group2Pins[2]) + "&sen4atras=" + (String)digitalRead(group2Pins[3]);
    //http.begin("http://192.168.0.175/Servidor/insertar/guardarDatos.php");        //Indicamos el destino- 
    http.begin("http://192.168.209.128/Servidor/insertar/guardarDatos.php");        //Indicamos el destino IP DE LA CAMWIFI
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Preparamos el header text/plain si solo vamos a enviar texto plano sin un paradigma llave:valor.

    int codigo_respuesta = http.POST(datos_a_enviar);


    if(codigo_respuesta>0){
      Serial.println("Código HTTP ► " + String(codigo_respuesta));   //Print return code

      if(codigo_respuesta == 200){
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondió ▼ ");
        Serial.println(cuerpo_respuesta);

      }

    }else{

     Serial.print("Error enviando POST, código: ");
     Serial.println(codigo_respuesta);

    }

    http.end();  //libero recursos

        previousMillis = currentMillis;
      } else {
        Serial.println("Error en la conexión WIFI");
      }
    }
  }
}




// #include <Arduino.h>
// #include <WiFi.h>
// #include <WebServer.h>
// #include "ws2812.h"
// #include "system.h" //contiene las variables del sistema
// #include "multitask.h"
// // Definiciones de red WiFi
// const char* ssid = "ROBOTICSNEXTGY";
// const char* password = "Sunnytop2022";
// // Pines de los sensores agrupados
// const int group1Pins[4] = {19, 18, 12, 14};  // Grupo 1, adelante
// const int group2Pins[4] = {26, 32, 33, 35};  // Grupo 2, atras
// // Pines del relé y el buzzer
// const int relay1 = RL1;
// const int buzzer = BUZZER;
// WebServer server(80);
// void setup() {
//   Serial.begin(115200);
//   // Configurar pines de relé y buzzer
//   pinMode(relay1, OUTPUT);
//   digitalWrite(relay1, HIGH); // Inicialmente apagado
//   pinMode(buzzer, OUTPUT);
//   digitalWrite(buzzer, HIGH); // Inicialmente apagado
//   // Configurar pines de los sensores
//   for (int i = 0; i < 4; i++) {
//     pinMode(group1Pins[i], INPUT);
//     pinMode(group2Pins[i], INPUT);
//   }
//   // Configuración de la red WiFi
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Conectado a ");
//   Serial.println(ssid);
//   Serial.print("Dirección IP: ");
//   Serial.println(WiFi.localIP());
//   // Configuración de las rutas para el servidor web
//   server.on("/", HTTP_GET, [](){
//     String html = "<html><body>";
//     html += "<h1>Estado de Sensores</h1>";
//     int group1Count = 0;
//     int group2Count = 0;
//     // Leer el estado de los sensores
//     for (int i = 0; i < 4; i++) {
//       group1Count += digitalRead(group1Pins[i]) == HIGH;
//       group2Count += digitalRead(group2Pins[i]) == HIGH;
//     }
//     // Controlar el relé según el estado de los sensores
//     bool relay1Active = (group1Count >= 4);
//     // Controlar el Buzzer para enviar un alto cuando se active el grupo 2
//     bool buzzerActive = (group2Count >= 4);
//     // Mostrar estado de los sensores en la página web
//     html += "<p>Grupo 1 Sensores Activos ADELANTE: ";
//     html += group1Count;
//     html += "</p>";
//     html += "<p>Grupo 2 Sensores Activos ATRAS: ";
//     html += group2Count;
//     html += "</p>";
//     // Mostrar estado del relé y el buzzer en la página web
//     html += "<h2>FRONTAL: ";
//     html += relay1Active ? " ROBOT DETENIDO" : " DENTRO DEL AREA DE TRABAJO";
//     html += "</h2>";
//     html += "<h2>TRASERA: ";
//     html += buzzerActive ? " ROBOT DETENIDO" : " DENTRO DEL AREA DE TRABAJO";
//     html += "</h2>";
//     html += "</body></html>";
//     server.send(200, "text/html", html);
//     // Mostrar estado de los sensores en el monitor serial
//     Serial.print("Grupo 1 Sensores Activos ADELANTE: ");
//     Serial.println(group1Count);
//     Serial.print("Grupo 2 Sensores Activos ATRAS: ");
//     Serial.println(group2Count);
//   });
//   server.begin();
// }
// void loop() {
//   server.handleClient();
// }