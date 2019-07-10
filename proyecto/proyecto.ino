// Primera prueba de uso del sensor DHT11 con el NODE MCU V1.0 / V2
// Emplea el ejemplo DHTTester de la libreria <DHT.h>

// Son necesarias las siguientes librerias:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN D2     // Pin digital del ESP8266 conectado al sensor DHT

#define DHTTYPE DHT11   // Se usa el modelo DHT 11

// Se usa el DHT11 integrado, con tres conexiones:
// El pin1 se conecta a la alimentacion de 3.3V de la placa NODE MCU
// El pin2 se conecta al pin D2 de la placa NODE MCU
// El pin3 se conecta a tierra de la placa NODE MCU

// Inicialización del sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  // Espera entre medidas
  delay(2000);

  // Las lecturas del sensor pueden necesitar  hasta 2 segundos (es un sensor muy lento)
  // Lectura de la humedad en la variable "h"
  float h = dht.readHumidity();             
  // Lectura de la temperatura en la variable "t"
  float t = dht.readTemperature();

  // Comprueba si alguna de las lecturas ha fallado y abandona el bucle para volver a intentarlo
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Error al leer el sendor DHT!"));
    return;
  }

 
  Serial.print(F("Hay una temperatura de: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("y una Humedad del: "));
  Serial.print(h);
  Serial.println(F("%"));
  
}
