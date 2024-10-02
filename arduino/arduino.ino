#include <LiquidCrystal.h>
#include "HX711.h"
#include <NewPing.h>
#include <Servo.h>

// pinos do display lcd 16x2
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(43, 41, 37, 35, 33, 31);
int pwm = 13;


Servo myservo;  // Cria um objeto baseado na biblioteca Servo

// Define os pinos para o sensor ultrassônico e a distância máxima para medir (em centímetros)
#define TRIGGER_PIN  24
#define ECHO_PIN     26
#define MAX_DISTANCE 200
#define RXD2 16
#define TXD2 17


// cria uma instancia da biblioteca HX711 na variavel escala para o sensor da balança
HX711 escala;

// Define os pinos para o sensor da balança
#define SCK A0
#define DT A1

// Cria uma instância do objeto NewPing para o ultrassonico
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// variaveis globais e de configuração
unsigned long ultimoDebounce = 0;
const long debounceDelay = 150;
int pesoMaximo = 0;
int alturaMaxima = 0;
String receivedData = "";

void setup()
{
  //seta os pinos dos leds
  for(int i = 2; i <= 6; i++)
  {
    pinMode(i, OUTPUT);
  }

  // desliga os leds
  for(int i = 2; i <= 6; i++)
  {
    digitalWrite(i, HIGH);
  }
  Serial.begin(115200);
  Serial1.begin(115200);
  lcd.begin(16, 2);
  pinMode(pwm, INPUT);
  analogWrite(pwm, 130);
  atualizarDisplay();

  // inicia o servo na porta 9
  myservo.attach(9);

  // inicia a instancia 'escala'
  escala.begin (DT, SCK);
  // começa a leitura e calculos para iniciar a pesagem
  Serial.print("Leitura do Valor ADC:  ");
  Serial.println(escala.read());   // Aguarda até o dispositivo estar pronto
  Serial.println("Nao coloque nada na balanca!");
  Serial.println("Iniciando...");
  escala.set_scale(765991.129);     // Substituir o valor encontrado para escala
  escala.tare(20);
  Serial.println("Balança pronta!");

  // seta a posição inicial do servo
  myservo.write(45);
}

void loop()
{
  if (Serial1.available() > 0) {
    receivedData = Serial1.readString();
  }

  if (receivedData.startsWith("PESO:")) {
    String peso = receivedData.substring(5);
    setarPeso(peso.toInt());
  } else if (receivedData.startsWith("ALT:")) {
    String alt = receivedData.substring(4);
    setarAltura(alt.toInt());
  } else if (receivedData.startsWith("START")) {
    receivedData = "";
    Serial.println("START");
    executaRotina();
  }



  delay(1000);
}

void atualizarDisplay()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parado");
  Serial.println("STOP");
}


void setarPeso(int peso)
{
  pesoMaximo = peso;
  Serial.println("SETPESO");
  receivedData = "";
}

void setarAltura(int altura)
{
  alturaMaxima = altura;
  Serial.println("SETALT");
  receivedData = "";
}

void executaRotina()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Executando rotina");
  delay(1000);
  while (true)
  {
    if (Serial1.available() > 0) {
      receivedData = Serial1.readString();
    }
    if (receivedData.startsWith("STOP")) {
      break;
    }


    desligaLeds();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Aguardando...");
    delay(500);
    if (getPeso() > 1)
    {
      if (getAltura() > alturaMaxima || getPeso() > pesoMaximo)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("produto descartado");
        delay(500);
        myservo.write(170);
        delay(2000);
        myservo.write(45);
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("produto aceito");
        ligaLeds();
        delay(2000);
      }
    }
  }
  atualizarDisplay();
}

int getPeso()
{
  return escala.get_units(20) * 1000;
}

int getAltura()
{
  unsigned int distance = sonar.ping_cm();
  int distancia = 10 - distance;
  return distancia;
}

void desligaLeds()
{
  for(int i = 2; i <= 6; i++)
  {
    digitalWrite(i, HIGH);
  }
}

void ligaLeds()
{
  for(int i = 2; i <= 6; i++)
  {
    digitalWrite(i, LOW);
  }
}