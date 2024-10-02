/* InclusÃ£o de Bibliotecas */
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>

/* VariÃ¡veis Globais */
const char* ssid = "Wifi_Max";
const char* password = "19142020";
#define RXD2 16
#define TXD2 17

String receivedData = "";
String ledState = "OFF";
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  // Criação do WebSocket na rota "/ws"

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
  } else if (type == WS_EVT_DATA) {
    String message = "";
    for (size_t i = 0; i < len; i++) {
      message += (char) data[i];
    }
    Serial.printf(message.c_str());
  }
}

void listFiles() {
  Serial.println("Listing files stored in LittleFS:");
  File root = LittleFS.open("/");
  File file = root.openNextFile();

  if (!file) {
    Serial.println("No files found.");
  }

  while (file) {
    Serial.print("File: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
}

void initWebSocket() {
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Tentar montar LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An error occurred while mounting LittleFS. Formatting...");
    // Tentar formatar LittleFS, caso esteja corrompido
    if (LittleFS.format()) {
      Serial.println("LittleFS formatted successfully.");
    } else {
      Serial.println("Failed to format LittleFS.");
    }
    return;
  } else {
    Serial.println("LittleFS mounted successfully.");
  }

  listFiles();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  if (LittleFS.exists("/index.html")) {
    Serial.println("Serving index.html");
    request->send(LittleFS, "/index.html", String());
  } else {
    Serial.println("index.html not found");
    request->send(404, "text/plain", "File Not Found");
  }
});

  // Start server
  server.begin();
}

void loop() {
  if (Serial2.available() > 0) { // Verifica se há dados no buffer serial
    receivedData = Serial2.readString(); // Lê a string completa
  }

  if (receivedData.startsWith("SETPESO")) {
    receivedData = "";
    ws.textAll("SETPESO");
  } else if (receivedData.startsWith("SETALT")) {
    receivedData = "";
    ws.textAll("SETALT");
  } else if(receivedData.startsWith("START")) {
    receivedData = "";
    ws.textAll("START");
  } else if (receivedData.startsWith("STOP")) {
    receivedData = "";
    ws.textAll("STOP");
  } else if(receivedData.startsWith("DESC")) {
    receivedData = "";
    ws.textAll("DESC");
  } else if(receivedData.startsWith("ACEIT")) {
    receivedData = "";
    ws.textAll("ACEIT");
  }

  delay(500);

}