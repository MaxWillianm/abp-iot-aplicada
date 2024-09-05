/* Inclusão de Bibliotecas */
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

/* Variáveis Globais */
const char* ssid = "SATC IOT";
const char* password = "IOT2024@#";

String ledState = "OFF";
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "GPIO_STATE"){
    if(digitalRead(ledPin)){
      ledState = "OFF";
    }
    else{
      ledState = "ON";
    }
    Serial.print(ledState);
  }
  return ledState;
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/led2on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/led2off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}

void loop() {}
