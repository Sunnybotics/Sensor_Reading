#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ws2812.h" 
#include "system.h" //contiene las variables del sistema
#include "multitask.h"

// Definiciones de red WiFi
const char* ssid = "ROBOTICSNEXTGY";
const char* password = "Sunnytop2022";

// Pines de los sensores agrupados
const int group1Pins[4] = {19, 18, 12, 14};  // Grupo 1, adelante
const int group2Pins[4] = {26, 32, 33, 35};  // Grupo 2, atras

// Pines del relé y el buzzer
const int relay1 = RL1; 
const int relay2 = RL2; 
const int buzzer = BUZZER; 

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Configurar pines de relé y buzzer
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH); // Inicialmente apagado
  
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH); // Inicialmente apagado

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH); // Inicialmente apagado

  // Configurar pines de los sensores
  for (int i = 0; i < 4; i++) {
    pinMode(group1Pins[i], INPUT);
    pinMode(group2Pins[i], INPUT);
  }

  // Configuración de la red WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configuración de las rutas para el servidor web
  server.on("/", HTTP_GET, [](){
    String html = "<html><body>";
    html += "<h2>";
    html += "<span style='color:rgb(252, 191, 0); font-size: 50px;'> Estado de </span>";
    html +=  "<span style='color:rgb(236, 105, 7); font-size: 50px;'> Sensores </span>";
    html += "</h2>";

    int group1Count = 0;
    int group2Count = 0;

    // Leer el estado de los sensores
    for (int i = 0; i < 4; i++) {
      group1Count += digitalRead(group1Pins[i]) == HIGH;
      group2Count += digitalRead(group2Pins[i]) == HIGH;
    }

    // Controlar el relé según el estado de los sensores
    bool relay1Active = (group1Count >= 4);
    bool relay2Active = (group2Count >= 4);
   
    digitalWrite(relay1, relay1Active ? LOW : HIGH); // Grupo 1
    digitalWrite(relay2, relay2Active ? LOW : HIGH); // Grupo 2

    // Mostrar estado de los sensores en la página web
    html += "<p>Grupo 1 Sensores Activos ADELANTE: ";
    html += group1Count;
    html += "</p>";

    html += "<p>Grupo 2 Sensores Activos ATRAS: ";
    html += group2Count;
    html += "</p>";

    // Mostrar estado del relé y el buzzer en la página web
  html += "<h2>FRONTAL: ";
  html += relay1Active ? "<span style='color:red;'>ROBOT DETENIDO</span>" : "<span style='color:green;'>DENTRO DEL AREA DE TRABAJO</span>";
  html += "</h2>";

  html += "<h2>TRASERA: ";
  html += relay2Active ? "<span style='color:red;'>ROBOT DETENIDO</span>" : "<span style='color:green;'>DENTRO DEL AREA DE TRABAJO</span>";
  html += "</h2>";

    html += "</body></html>";
    server.send(200, "text/html", html);

    // Mostrar estado de los sensores en el monitor serial
    Serial.print("Grupo 1 Sensores Activos ADELANTE: ");
    Serial.println(group1Count);
    Serial.print("Grupo 2 Sensores Activos ATRAS: ");
    Serial.println(group2Count);

    // Mostrar mensaje si el relé 1 está activo
    if (relay1Active) {
      Serial.println("ROBOT DETENIDO: ADELANTE FUERA DEL AREA DE TRABAJO");
    }
    // Mostrar mensaje si el relé 2 está activo
    if (relay2Active) {
      Serial.println("ROBOT DETENIDO: ATRAS FUERA DEL AREA DE TRABAJO");
    }
    
  });

  server.begin();
}

void loop() {
  server.handleClient();
  // Verificar si el rele 1 o el rele 2 están activos
  if (digitalRead(relay1) == LOW || digitalRead(relay2) == LOW) {
    // Si cualquiera de los relés está activo, activar el buzzer
    digitalWrite(buzzer, LOW);
  } else {
    // Si ninguno de los reles está activo, mantener el buzzer apagado
    digitalWrite(buzzer, HIGH);
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