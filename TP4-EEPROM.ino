#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Preferences.h>

//defino estados
#define PANTALLA1 1
#define CONFIRM_PANTALLA1 2
#define PANTALLA2 3
#define CONFIRM_PANTALLA2 4
#define SUMA 5
#define CONFIRMACION_SUMA 6
#define RESTA 7
#define CONFIRMACION_RESTA 8

//configuación de la pantalla
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 21
#define SCL_PIN 22
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//dht
#define DHTPIN 23
#define DHTTYPE DHT11 //tipo de sensor
DHT_Unified dht(DHTPIN, DHTTYPE);

//defino pines
#define SW1_PIN 34
#define SW2_PIN 35

//variables globales
float VU = 0.0; //valor umbral
int estadoActual = PANTALLA1;
unsigned long contando = 0;
Preferences preferences;

void setup() {
  Serial.begin(115200);
  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  preferences.begin("valor-VU", false);
  VU = preferences.getFloat("umbral", 0);
  preferences.end();
}


void loop() {

  float VA = dht.readTemperature(); //temperatura actual
  switch (estadoActual) {
    case PANTALLA1:
      
      if (digitalRead(SW1_PIN) == LOW){
        contando = millis();
        estadoActual = CONFIRM_PANTALLA1;
      }
      
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10);
      display.print("VALOR ACTUAL: ");
      display.print(VA);
      display.print(" C");
      display.print("VALOR UMBRAL: ");
      display.print(VU);
      display.print(" C");
      display.display(); 
      break;

    case CONFIRM_PANTALLA1:

      if ((millis() - contando) >= 5000 && digitalRead(SW1_PIN) == HIGH) {
        contando = 0;
        estadoActual = PANTALLA2;
      }
      else {
        estadoActual = PANTALLA1;
      }
      break;
    
    case PANTALLA2:

      if (digitalRead(SW2_PIN) == LOW) {
        estadoActual = CONFIRM_PANTALLA2;
      }

      if (digitalRead(SW1_PIN) == LOW) {
        estadoActual = SUMA;
      }

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10); 
      display.print("VALOR UMBRAL: ");
      display.print(VU);
      display.display();
      break;
    
    case CONFIRM_PANTALLA2:

      if ((millis() - contando >= 5000) && digitalRead(SW2_PIN) == HIGH) {
        contando = 0;
        preferences.begin("valor-VU", false);
        preferences.putFloat("umbral", VU);
        preferences.end();
        Serial.print("nuevo umbral: ");
        Serial.println(VU);
        }
        estadoActual = PANTALLA1;  
      }

      if ((millis() - contando) <= 5000) && digitalRead(SW2_PIN) == HIGH) {
        estadoActual = RESTA;
      }
      break;

    case SUMA:

      if (digitalRead(SW1_PIN) == HIGH) {
        VU += 1;
        estadoActual = PANTALLA2;
      }

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10); 
      display.print("VALOR UMBRAL: ");
      display.print(VU);
      display.display();
      break;

    case RESTA:

      if (digitalRead(SW2_PIN) == HIGH) {
        VU -= 1;
        estadoActual = PANTALLA2;
      }

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10); 
      display.print("VALOR UMBRAL: ");
      display.print(VU);
      display.display();
      break;
  }
