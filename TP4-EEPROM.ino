#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

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
float VU = 0; //valor umbral
float VA = dht.readTemperature(); //temperatura actual
int estadoActual = PANTALLA1;

//variables antirebote 
bool ARBOT1 = HIGH;
bool ARBOT2 = HIGH;
unsigned long tiempoBotones = 0;
unsigned long intervaloBotones = 200;

//variables delay 5 segundos
unsigned long TimeUltimoCambio = 0;
const long intervalo5seg = 5000;


void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

bool delay5seg () {
  unsigned long TiempoAtual = millis();
  if (TiempoActual - TimeUltimoCambio >= intervalo5seg) {
    TimeUltimoCambio = TiempoActual;
    return true;
  }
  return false;
}

void antiRebote () {
  unsigned long tiempoAhora = millis();
  if (tiempoAhora - tiempoBotones >= intervaloBotones) {
    ARBOT1 = digitalRead(SW1_PIN);
    ARBOT2 = digitalRead(SW2_PIN);
    tiempoBotones = tiempoAhora;
  }
}


void loop() {
  switch (estadoActual) {
    case PANTALLA1:
      antiRebote();
      
      if (ARBOT1 == LOW){
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
      antiRebote();

      if (ARBOT == HIGH && delay5seg()) {
        estadoActual = PANTALLA2;
      }

      if (ARBOT == HIGH < delay5seg()) {
        estadoActual = PANTALLA1;
      }
      break;
    
    case PANTALLA2:
      antiRebote();

      if (ARBOT2 == LOW) {
        estadoActual = CONFIRM_PANTALLA2;
      }

      if (ARBOT1 == LOW) {
        estadoActual = SUMA;
      }

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10);
      display.print("VALOR UMBRAL: ");
      display.print(VU);
      display.print(" C");
      display.display();
      break;
    
    case CONFIRM_PANTALLA2:
      antiRebote();

      if (ARBOT2 == HIGH < delay5seg()) {
        estadoActual = RESTA;  
      }

      if (ARBOT2 == HIGH && delay5seg()) {
        estadoActual = PANTALLA1;
      }
      break;

    case SUMA:
      antiRebote();

      if (ARBOT1 == HIGH) {
        VU += 1;
        estadoActual = PANTALLA2;
      }
      break;

    case RESTA:
      antiRebote();

      if (ARBOT2 == HIGH) {
        VU -= 1;
        estadoActual = PANTALLA2;
      }
      break;

  }













