//Importación de librerias
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "SR04.h"

//Asignación de nombre a valores constante (antes de compilar el programa)
//Credenciales base de datos en tiempo reañ
#define FIREBASE_HOST "iot-apps-192d1.firebaseio.com"
#define FIREBASE_AUTH "oyzULKFkQS38Z7KpINJ54GFWERAaXXjsk9BT52eV"
//Credenciales de WiFi
#define WIFI_SSID "NASA_5.2"
#define WIFI_PASSWORD "4488567UT"
//Asignaciones usadas en sensor ultrasonico
#define TRIG_PIN 27
#define ECHO_PIN 26

//Variable de tamaño extendido, uso de sensor ultrasonico
long distancia;

//Variable constantes para diferentes sensores y actuadores
const int LED_COCINA=18;
const int LED_SALA=19;
const int LED_MIRADOR = 32;
const int LED_MOVIMIENTO = 33;
const int ALARMA = 22;
const int MICROFONO_PIN = 23;
const int MOVIMIENTO = 35;

//Instancia de objecto
FirebaseData firebaseData;
//Instancia de objecto con sus debido parametros
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);

void setup() {  
  //Configuración de los pines (Salida o entrada)
  pinMode(LED_COCINA, OUTPUT);
  pinMode(LED_SALA, OUTPUT);
  pinMode(LED_MIRADOR, OUTPUT);
  pinMode(LED_MOVIMIENTO, OUTPUT);
  pinMode(ALARMA,OUTPUT);
  pinMode(MICROFONO_PIN, INPUT);

  //Velocidad de datos en bits por segundo (baudios) para la transmisión de datos en serie.
  Serial.begin(9600);

  //Conexión a WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  //Conexión a FireBase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Inserción de datos en la BD, campo:valor
  Firebase.setString(firebaseData, "/led_cocina_status", "Apagado");
  Firebase.setString(firebaseData, "/led_sala_status", "Apagado");
  Firebase.setString(firebaseData, "/led_mirador_status", "Apagado");
  Firebase.setString(firebaseData, "/led_pasillo_status", "Apagado");
  Firebase.setString(firebaseData, "/movimiento_status", "No hay");
  Firebase.setInt(firebaseData, "/microfono_status", 0);
  Firebase.setFloat(firebaseData, "/distanciaUltrasonico", 0);
}

//Variable tipo int, inicializadas
int led_cocina_status = 0;
int led_sala_status = 0;
int led_mirador_status = 0;
int led_pasillo_status = 0;
int movimiento_status = 0;
int alarma_status = 0;

void loop() {
  //Tipo de dato sin signo
  unsigned char i;

  //Lectura del sensor ultrasonico
  distancia=sr04.Distance();
  Serial.print("DISTANCIA ");
  Serial.println(distancia);
  //Lectura del sensor de sonido
  bool sonidoDetectado = digitalRead(MICROFONO_PIN);
  //Lectura del sensor de movimiento
  int movimientoLectura = digitalRead(MOVIMIENTO);

  //Actualizacion de datos en la BD con valor de las lecturas
  Firebase.setInt(firebaseData, "/microfono_status", sonidoDetectado);
  Firebase.setInt(firebaseData, "/movimiento_status", movimientoLectura);
  Firebase.setFloat(firebaseData, "/distanciaUltrasonico", distancia);

  //Conseguir la informacion de la base de datos y asignarla a las variables definidas
  led_cocina_status = Firebase.getInt(firebaseData, "/led_cocina_status");
  led_sala_status = Firebase.getInt(firebaseData, "/led_sala_status");
  led_mirador_status = Firebase.getInt(firebaseData, "/led_mirador_status");
  led_pasillo_status = Firebase.getInt(firebaseData, "/led_pasillo_status");
  movimiento_status = Firebase.getInt(firebaseData, "/movimiento_status");
  alarma_status = Firebase.getInt(firebaseData, "/alarma_status");
  
  Serial.print("LED COCINA ");
  Serial.println(led_cocina_status);
  if (led_cocina_status == 1) {
    digitalWrite(LED_COCINA,HIGH);  
    delay(10);
  } else {
    digitalWrite(LED_COCINA,LOW);
  }
  Serial.print("LED SALA ");
  Serial.println(led_sala_status);
  if (led_sala_status == 1) {
    digitalWrite(LED_SALA,HIGH);  
    delay(10);
  } else {
    digitalWrite(LED_SALA,LOW);
  }
  Serial.print("LED MIRADOR ");
  Serial.println(led_mirador_status);
  if (led_mirador_status == 1) {
    digitalWrite(LED_MIRADOR,HIGH);  
    delay(10);
  } else {
    digitalWrite(LED_MIRADOR,LOW);
  }
  Serial.print("LED PASILLO");
  Serial.println(led_pasillo_status);
  if (led_pasillo_status == 1) {
    digitalWrite(LED_MOVIMIENTO,HIGH);  
    delay(10);
  } else {
    digitalWrite(LED_MOVIMIENTO,LOW);
  }
  
  if (alarma_status == 1) {
    for(i=0;i<80;i++) {
      digitalWrite(ALARMA,HIGH);
      delay(1);
      digitalWrite(ALARMA,LOW);
      delay(1);//wait for 1ms
    }
    for(i=0;i<100;i++) {
      digitalWrite(ALARMA,HIGH);
      delay(2);//wait for 2ms
      digitalWrite(ALARMA,LOW);
      delay(2);
    }
  }
}
