// Se configura el NODE MCU como servidor 
// Se usa parte del ejemplo DHTTester de la libreria <DHT.h>

// Son necesarias las siguientes librerias:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <ESP8266WiFi.h>    // Es el core Arduino para el ESP8266
#include "DHT.h"
#include "arduino_secrets.h"

#define DHTPIN D2     // Pin digital del ESP8266 conectado al sensor DHT
#define DHTTYPE DHT11   // Se usa el modelo DHT 11

// Se usa el DHT11 integrado, con tres conexiones:
// El pin1 se conecta a la alimentacion de 3.3V de la placa NODE MCU
// El pin2 se conecta al pin D2 de la placa NODE MCU
// El pin3 se conecta a tierra de la placa NODE MCU

// Inicialización del sensor
DHT dht(DHTPIN, DHTTYPE);

float t;  // Variable que recoge la temperatura
float h;  // Variable que recoge la humedad
 
// Parámetros de la conexión WiFi
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS ;

// Declaración de servidor en el puerto 80
WiFiServer server(80);

void setup() {
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
            "<html>" + "La temperatura es " + t + "ºC y la humedad es del " + h +"%" +    // Envia la información de temperatura y humedad
            "</html>" +
            "\r\n";
  return htmlPage;
}

/////////////////////////////////////////////////////////////////////////// BUCLE /////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  // Espera entre medidas
  delay(2000);

  // Las lecturas del sensor pueden necesitar  hasta 2 segundos (es un sensor muy lento)
  // Lectura de la humedad en la variable "h"
  h = dht.readHumidity();             
  // Lectura de la temperatura en la variable "t"
  t = dht.readTemperature();


  // Comprueba si alguna de las lecturas ha fallado y abandona el bucle para volver a intentarlo
  if (isnan(h) || isnan(t)) {
   // Serial.println(F("Error al leer el sendor DHT!"));
    return;
  }

  /*
  Serial.print(F("Hay una temperatura de: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("y una Humedad del: "));
  Serial.print(h);
  Serial.println(F("%"));
  */

   WiFiClient client = server.available();
  // Espera la conexión del cliente
  if (client)
  {
    Serial.println("\n[Cliente conectado]");
    while (client.connected())
    {
      // Lee linea por linea la petición del cliente
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // Espera al final de la petición, que está indicada por una línea vacía
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(preparaHtmlPage());
          break;
        }
      }
    }
    delay(1); // Tiempo de espera para que el navegador reciba los datos

    // Cierre de la conexión
    client.stop();
    Serial.println("[Cliente desconectado]");
  }
  
}
