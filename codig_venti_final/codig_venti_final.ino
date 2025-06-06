#define TRIG 2
#define ECHO A0
#define PWM_PIN 10

int ultimaDistancia = 20; // arranca frenado

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PWM_PIN, OUTPUT);
  digitalWrite(TRIG, LOW);
}

void loop() {
  int distancia = promedioDistancia();

  // si la lectura es inválida (0 o muy chica), usar última válida
  if (distancia <= 1 || distancia > 300) {
    distancia = ultimaDistancia;
  } else {
    ultimaDistancia = distancia; // actualizar
  }

  // Limitar a 0–20 cm
  distancia = constrain(distancia, 0, 20);

  //múltiplos de 2 para que lea cada 2cm
  int escalon = (distancia / 2) * 2;

  // Reducir velocidad máxima a 150
  int pwmValor = map(escalon, 0, 20, 100, 0);

  analogWrite(PWM_PIN, pwmValor);

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.print(" cm (escalón ");
  Serial.print(escalon);
  Serial.print("), PWM: ");
  Serial.println(pwmValor);

  delay(300);
}

int sensorDistancia(int trigPin, int echoPin) {
  long duracion;
  int distancia;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duracion = pulseIn(echoPin, HIGH, 30000); // lee cuanto tarda en volver el eco que mandó el trig, osea el tiempo que el ECHO estuvo en HIGH. el pulseIn mide ese tiempo en microsegundos

  if (duracion == 0) return 0;

  distancia = duracion * 0.034 / 2; //lo pasa a cm
  return distancia;
}

int promedioDistancia() {
  int suma = 0;
  int lecturasValidas = 0;
//esto es porque si la mano está muy cerca no detecta nada
  for (int i = 0; i < 5; i++) { //toma 5 lecturas
    int lectura = sensorDistancia(TRIG, ECHO);

    if (lectura > 1 && lectura <= 300) { //si la lectura es entre 1 y 300
      suma += lectura;
      lecturasValidas++; //se incrementa la suma y las lectura validas
    }

    delay(30);
  }

  if (lecturasValidas == 0) return 0;
  return suma / lecturasValidas; //hace el promedio
}
