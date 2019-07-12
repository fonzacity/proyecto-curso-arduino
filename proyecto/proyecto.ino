// Se configura el NODE MCU como servidor y como cliente para su control desde el navegador

// Son necesarias las siguientes librerias:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <ESP8266WiFi.h>    // Es el core Arduino para el ESP8266
#include <DHT.h>
#include "arduino_secrets.h"

#define DHTPIN D2     // Pin digital del ESP8266 conectado al sensor DHT11 en circuito integrado 
#define DHTTYPE DHT11   // Se usa el modelo DHT 11
// Se usa el DHT11 integrado, con tres conexiones:
// El pin1 se conecta a la alimentacion de 3.3V de la placa NODE MCU
// El pin2 se conecta al pin D2 de la placa NODE MCU
// El pin3 se conecta a tierra de la placa NODE MCU

#define calefaccion D3
#define luces D4
#define alarma  D5
#define pir D1

// Inicialización del sensor
DHT dht(DHTPIN, DHTTYPE);

float t;  // Variable que recoge la temperatura actual

int temp_des = 24;    // Es el valor de temperatura que se desea

int intrusion = LOW; // Variable que detecta el estado de intrusión

String estado_luces = "APAGADAS";  // Indica el estado de las luces
String estado_alarma = "DESACTIVADA";  // Indica el estado de la alarma

String line, data="";

// Parámetros de la conexión WiFi
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS ;

// Declaración de servidor en el puerto 80
WiFiServer server(80);

// Funciones de control de la placa
void Subir_temperatura(void);
void Bajar_temperatura(void); 
void Activar_luces(void); 
void Activar_alarma(void); 


void setup() {

  pinMode(calefaccion, OUTPUT);   // Configura los pines conectados a actuadores como salidas
  pinMode(luces, OUTPUT);
  pinMode(alarma, OUTPUT);

  pinMode(pir, INPUT);            // Configura el detector de presencia como entrada

  Serial.begin(115200);
  
  // Inicia la conexión con la red WiFi
  Serial.printf("Conectando a %s", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ¡Conectado!");

  // Inicia el servidor y devuelve la dirección IP del NODE MCU
  server.begin();
  Serial.printf("Servidor Web Iniciado en la dirección IP %s\n", WiFi.localIP().toString().c_str());

  // Inicia el sensor de humedad y temperatura
  dht.begin();

}
 
/////////////////////////////////////////////////////////////////////////// Diseño de la página web del servidor /////////////////////////////////////////////////////////////////
String preparaHtmlPage()
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +                                                      // La conexión se cuerra tras la respuesta
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "<head>" +
            "<title>Control domotico por WiFi</title>" +  // Texto de la pestaña del navegador
            "</head>" +
            "<body>" +
            "<center>" +
            "<h1> PROYECTO - SISTEMA DOMOTICO</h1>" +
            "<h2>La temperatura actual es " + t + "ºC</h2>" +
            "<h2>La temperatura deseada es " + temp_des + "ºC</h2>" +
            "<h2>Las luces estan " + estado_luces + "</h2>" +
            "<h2>La alarma esta " + estado_alarma + "</h2>" +
            "<br><br>" +
            "<hr>" +

             /* Botones */
            "<br><br>" +
            "<a href=\"/subir\"\"><button>SUBIR TEMPERATURA </button></a><br />" +
            "<br><br>" +
            "<a href=\"/bajar\"\"><button>BAJAR TEMPERATURA </button></a><br />" +
            "<br><br>" +
            "<a href=\"/luces\"\"><button>ACTIVAR/DESACTIVAR LAS LUCES </button></a><br />" +
            "<br><br>" +
            "<a href=\"/alarma\"\"><button>ACTIVAR/DESACTIVAR LA ALARMA </button></a><br />" +
            "</center>" +
            "</body>" +
            "</html>" +
            "\r\n";
  return htmlPage;
}

/////////////////////////////////////////////////////////////////////////// BUCLE /////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
    // Espera entre medidas
    delay(2000);

    // Las lecturas del sensor pueden necesitar  hasta 2 segundos (es un sensor muy lento)
    // Lectura de la temperatura en la variable "t"
    t = dht.readTemperature();

    // Ajustar la calefacción
    if (t<temp_des)
      digitalWrite(calefaccion, HIGH);
    else
      digitalWrite(calefaccion, LOW);

    // Ajustar la intrusión
    int valor_pir=digitalRead(pir);
 
    if ((estado_alarma == "ACTIVADA")&&(valor_pir == HIGH))
      intrusion = HIGH;

    if ((estado_alarma == "DESACTIVADA")&&(intrusion == HIGH))
      intrusion = LOW;

    Serial.print("El detector PIR detecta ");
    Serial.print(valor_pir);
    Serial.print(" y se está produciendo una intrusión");
    Serial.println(intrusion);
    
    if(intrusion == HIGH)
      digitalWrite(alarma, HIGH);
    else
      digitalWrite(alarma, LOW);

    WiFiClient client = server.available();
    if (!client) return; 
    data = client.readStringUntil('\r');
    client.flush();

    if (data.indexOf("subir") != -1) Subir_temperatura();
    else if (data.indexOf("bajar") != -1)Bajar_temperatura();
    else if (data.indexOf("luces") != -1) Activar_luces();
    else if (data.indexOf("alarma") != -1) Activar_alarma();

    client.println(preparaHtmlPage());
    
    // Cierre de la conexión
    client.stop();
  
}

void Subir_temperatura(void)   
{
  temp_des = temp_des + 1;
}

void Bajar_temperatura(void)   
{
  temp_des = temp_des - 1;
}

void Activar_luces(void)   
{
  if(estado_luces == "APAGADAS"){
    estado_luces="ENCENDIDAS";
    digitalWrite(luces, HIGH);
  }
  else{
     estado_luces="APAGADAS";
     digitalWrite(luces, LOW);
  } 
}

void Activar_alarma(void)   
{
  if(estado_alarma == "DESACTIVADA"){
    estado_alarma="ACTIVADA";
  }
  else{
     estado_alarma="DESACTIVADA";
  }
}
