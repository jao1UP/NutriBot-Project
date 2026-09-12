#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <Wire.h> 

#include "RobotEyes.h" // Importa a sua biblioteca de olhos limpa
// === Sensor Setup ===
// === Sensor Setup ===
const int proxSensorPin = 5; // Pino do sensor de proximidade (D5)

// === Servo setup ===
Servo leftServo;
Servo rightServo;
const int leftPin = 6;
const int rightPin = 7;

// === OLED setup ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Inicializa a classe de olhos passando o seu display Adafruit
RobotEyes eyes(&display);

// === Server & Captive Portal ===
AsyncWebServer server(80);
DNSServer dnsServer;
Preferences preferences;
const byte DNS_PORT = 53;
bool isAPMode = false;

// === State ===
bool started = false;
bool forwardActive = false;
bool backwardActive = false;
bool leftActive = false;
bool rightActive = false;
bool wiggleActive = false;

// Variável para animar o texto no OLED no modo setup
unsigned long lastOledUpdate = 0;
bool toggleDot = false;

// === Helper Functions ===
void stopServos() {
  leftServo.write(90);
  rightServo.write(90);
}

void updateCar() {
  if (forwardActive) { leftServo.write(0); rightServo.write(180); }
  else if (backwardActive) { leftServo.write(180); rightServo.write(0); }
  else if (leftActive) { leftServo.write(180); rightServo.write(180); }
  else if (rightActive) { leftServo.write(0); rightServo.write(0); }
  else if (wiggleActive) {
    leftServo.write(180); rightServo.write(180);
    delay(50); leftServo.write(0); rightServo.write(0); delay(50);
    leftServo.write(180); rightServo.write(180);
    delay(50); leftServo.write(0); rightServo.write(0); delay(50);
  }
  else stopServos();
}

// === Lógica dos Olhos usando a sua biblioteca ===
void updateEyes() {
  bool isScared = (digitalRead(proxSensorPin) == LOW);

  if (isScared) {
    eyes.setExpression(EXPR_SCARED); // Fica assustado/bravo se o sensor achar obstáculo
  } else {
    eyes.setExpression(EXPR_HAPPY); // Olha normal e pisca sozinho com animação suave
  }

  eyes.update(); // Executa o cálculo e o desenho automático
}

void showMessage(const char* line1, const char* line2 = "", const char* line3 = "") {
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(line1);
  if (line2[0] != '\0') {
    display.setCursor(0, 20);
    display.println(line2);
  }
  if (line3[0] != '\0') {
    display.setCursor(0, 40);
    display.println(line3);
  }
  display.display();
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  pinMode(proxSensorPin, INPUT);

  leftServo.attach(leftPin);
  rightServo.attach(rightPin);
  stopServos();

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("FALHA: OLED nao encontrado no endereco 0x3C!"));
  } else {
    Serial.println(F("OLED Inicializado com sucesso!"));
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  showMessage("Iniciando Sistema...");
  delay(1000);

  // 1. Inicia a memória flash (Preferences)
  preferences.begin("wifi_config", false);
  String savedSSID = preferences.getString("ssid", "");
  String savedPass = preferences.getString("pass", "");

  showMessage("Lendo Memoria:", savedSSID != "" ? savedSSID.c_str() : "Nenhuma rede salva");
  delay(1500);

  WiFi.mode(WIFI_STA);
  
  // 2. Tenta conectar na rede salva
  if (savedSSID != "") {
    showMessage("Conectando a:", savedSSID.c_str(), "Aguarde...");
    WiFi.begin(savedSSID.c_str(), savedPass.c_str());
    
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
      delay(500);
      Serial.print(".");
      retries++;
    }
  }

  // 3. Se falhou ou não tem rede, abre o Captive Portal com Scan de Redes
  if (WiFi.status() != WL_CONNECTED) {
    isAPMode = true;
    
    showMessage("Buscando Redes", "Wi-Fi disponiveis", "Aguarde...");
    WiFi.mode(WIFI_AP_STA); 
    WiFi.disconnect();
    delay(100);
    
    int n = WiFi.scanNetworks();
    String wifiOptions = "<option value='' disabled selected>Selecione sua rede...</option>";
    if (n == 0) {
      wifiOptions += "<option value='' disabled>Nenhuma rede encontrada</option>";
    } else {
      for (int i = 0; i < n; ++i) {
        wifiOptions += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>";
      }
    }
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Robo-Config"); 
    
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP()); 
    
    showMessage("MODO SETUP!", "Conecte no Wi-Fi:", "Rede: Robo-Config");
    Serial.println("Modo AP (Captive Portal) Iniciado.");

    server.on("/", HTTP_GET, [wifiOptions](AsyncWebServerRequest *request){
      String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
      html += "<style>body{font-family:sans-serif; text-align:center; background:#111; color:white;} ";
      html += "select, input{width:80%; padding:10px; margin:10px; border-radius:5px; font-size:16px;} ";
      html += "input[type=submit]{background:#0a74da; color:white; border:none; font-size:20px; font-weight:bold;}</style></head><body>";
      html += "<h2>Configurar Robozinho</h2>";
      html += "<form action='/save' method='POST'>";
      html += "<select name='ssid' required>" + wifiOptions + "</select><br>";
      html += "<input type='password' name='pass' placeholder='Senha da Rede' required><br>";
      html += "<input type='submit' value='Salvar e Conectar'>";
      html += "</form></body></html>";
      request->send(200, "text/html", html);
    });

    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
      if(request->hasParam("ssid", true) && request->hasParam("pass", true)) {
        String newSSID = request->getParam("ssid", true)->value();
        String newPass = request->getParam("pass", true)->value();
        preferences.putString("ssid", newSSID);
        preferences.putString("pass", newPass);
        
        request->send(200, "text/html", "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>body{font-family:sans-serif; text-align:center; background:#111; color:white;}</style></head><body><h2>Salvo! O robo vai reiniciar.</h2></body></html>");
        delay(1000);
        ESP.restart(); 
      }
    });

    server.onNotFound([](AsyncWebServerRequest *request){
      request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    });
  } 
  // 4. Se conectou com sucesso, inicia o modo Normal
  else {
    isAPMode = false;
    
    String ipStr = WiFi.localIP().toString();
    showMessage("Conectado!", "IP do Robo:", ipStr.c_str());
    delay(3000); 
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
      html += "<style>body { font-family: sans-serif; text-align: center; background:#111; color:white; }";
      html += ".btn { display: block; width: 80%; height: 60px; margin: 10px auto; font-size: 24px; background: #0a74da; color: white; line-height: 60px; border-radius:15px; user-select:none; }</style></head><body>";
      html += "<h2>ESP32 Car Control</h2><div class='btn' onclick='fetch(\"/start\")'>Start</div>";
      html += "<div class='btn' ontouchstart='fetch(\"/forward/on\")' ontouchend='fetch(\"/forward/off\")'>Forward</div>";
      html += "<div class='btn' ontouchstart='fetch(\"/backward/on\")' ontouchend='fetch(\"/backward/off\")'>Backward</div>";
      html += "<div class='btn' ontouchstart='fetch(\"/left/on\")' ontouchend='fetch(\"/left/off\")'>Left</div>";
      html += "<div class='btn' ontouchstart='fetch(\"/right/on\")' ontouchend='fetch(\"/right/off\")'>Right</div>";
      html += "<div class='btn' ontouchstart='fetch(\"/wiggle/on\")' ontouchend='fetch(\"/wiggle/off\")'>Wiggle</div>";
      html += "<br><br><div class='btn' style='background:#d32f2f;' onclick='if(confirm(\"Esquecer Wi-Fi?\")) fetch(\"/resetwifi\")'>Reset Wi-Fi</div>";
      html += "</body></html>";
      request->send(200, "text/html", html);
    });

    server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){ started = true; request->send(200, "text/plain", "Started!"); });
    server.on("/forward/on", HTTP_GET, [](AsyncWebServerRequest *request){ forwardActive = true; request->send(200,"text/plain","ok");});
    server.on("/forward/off", HTTP_GET, [](AsyncWebServerRequest *request){ forwardActive = false; request->send(200,"text/plain","ok");});
    server.on("/backward/on", HTTP_GET, [](AsyncWebServerRequest *request){ backwardActive = true; request->send(200,"text/plain","ok");});
    server.on("/backward/off", HTTP_GET, [](AsyncWebServerRequest *request){ backwardActive = false; request->send(200,"text/plain","ok");});
    server.on("/left/on", HTTP_GET, [](AsyncWebServerRequest *request){ leftActive = true; request->send(200,"text/plain","ok");});
    server.on("/left/off", HTTP_GET, [](AsyncWebServerRequest *request){ leftActive = false; request->send(200,"text/plain","ok");});
    server.on("/right/on", HTTP_GET, [](AsyncWebServerRequest *request){ rightActive = true; request->send(200,"text/plain","ok");});
    server.on("/right/off", HTTP_GET, [](AsyncWebServerRequest *request){ rightActive = false; request->send(200,"text/plain","ok");});
    server.on("/wiggle/on", HTTP_GET, [](AsyncWebServerRequest *request){ wiggleActive = true; request->send(200,"text/plain","ok");});
    server.on("/wiggle/off", HTTP_GET, [](AsyncWebServerRequest *request){ wiggleActive = false; request->send(200,"text/plain","ok");});

    server.on("/resetwifi", HTTP_GET, [](AsyncWebServerRequest *request){
      preferences.clear();
      request->send(200, "text/plain", "Wi-Fi esquecido! Reiniciando...");
      delay(1000);
      ESP.restart();
    });
  }

  server.begin();
}

// === Loop ===
void loop() {
  if (isAPMode) {
    dnsServer.processNextRequest();
    
    unsigned long currentMillis = millis();
    if (currentMillis - lastOledUpdate > 1000) {
      lastOledUpdate = currentMillis;
      toggleDot = !toggleDot;
      if (toggleDot) {
        showMessage("MODO SETUP", "Conecte no Wi-Fi:", ">> Robo-Config <<");
      } else {
        showMessage("MODO SETUP", "Conecte no Wi-Fi:", "   Robo-Config   ");
      }
    }
  } 
  else {
    if (!started) {
      String ipStr = "IP: " + WiFi.localIP().toString();
      showMessage("Pronto!", "Acesse no celular:", ipStr.c_str());
      delay(500);
    } else {
      updateEyes(); 
      updateCar();  
    }
  }
}
