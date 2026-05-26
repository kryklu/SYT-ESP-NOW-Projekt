#include <WiFi.h>
#include <esp_now.h>
#include <WebServer.h>

// =========================================================================
// WLAN ACCESS POINT KONFIGURATION
// =========================================================================
const char* ssid = "ESP32-Monitor";
const char* password = "12345678";

WebServer server(80);

// Globale Variablen für die Sensorwerte
int licht = 0;
bool objekt = false;
bool dunkel = false;
int sleepZeit = 0;

// Steuerungs-Flags für das Webinterface
bool autoMode = true;
bool manualNightMode = false;

// =========================================================================
// DATENSTRUKTUR (Muss beim Sender exakt identisch sein)
// =========================================================================
typedef struct {
  int licht;
  bool objekt;
  bool dunkel;
  unsigned long sleepTime;
} message_t;

message_t incomingData;

// Callback-Funktion, die bei eingehenden ESP-NOW Daten aufgerufen wird
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  
  licht = incomingData.licht;
  objekt = incomingData.objekt;
  dunkel = incomingData.dunkel;
  sleepZeit = incomingData.sleepTime;
  
  Serial.println("NEUE DATEN VIA ESP-NOW EMPFANGEN");
}

// Generierung und Bereitstellung der HTML-Webseite
void handleRoot() {
  bool nightMode = autoMode ? dunkel : manualNightMode;

  // Dynamisches Theme-Styling basierend auf Tag/Nacht-Modus
  String bg = nightMode ? "#0f172a" : "#f1f5f9";
  String card = nightMode ? "#1e293b" : "#ffffff";
  String text = nightMode ? "#ffffff" : "#111827";

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='2'>"; // Automatischer Reload alle 2 Sekunden
  html += "<style>";
  html += "body{font-family:Arial; background:" + bg + "; color:" + text + "; padding:20px; transition:0.3s;}";
  html += ".card{background:" + card + "; padding:20px; border-radius:20px; max-width:420px; margin:auto; box-shadow:0 0 20px rgba(0,0,0,0.2);}";
  html += ".item{margin:15px 0; font-size:20px;}";
  html += ".badge{padding:6px 12px; border-radius:10px; color:white; font-weight:bold;}";
  html += ".red{background:#dc2626;} .green{background:#16a34a;} .blue{background:#2563eb;} .orange{background:#f59e0b;}";
  html += ".btn{display:inline-block; padding:12px 18px; margin:8px; border-radius:12px; text-decoration:none; color:white; font-weight:bold;}";
  html += ".on{background:#2563eb;} .off{background:#f59e0b;} .auto{background:#16a34a;}";
  html += "</style></head><body>";

  html += "<div class='card'><h1>ESP32 SENSOR</h1>";
  html += "<div class='item'>Lichtwert: <b>" + String(licht) + "</b></div>";
  
  html += "<div class='item'>Objekt erkannt: ";
  html += objekt ? "<span class='badge red'>JA</span>" : "<span class='badge green'>NEIN</span>";
  html += "</div>";

  html += "<div class='item'>Modus: ";
  html += nightMode ? "<span class='badge blue'>NACHT</span>" : "<span class='badge orange'>HELL</span>";
  html += "</div>";

  html += "<div class='item'>Steuerung: ";
  html += autoMode ? "<span class='badge green'>AUTO</span>" : "<span class='badge blue'>MANUELL</span>";
  html += "</div>";

  // Buttons für die Web-Interaktion
  html += "<div class='item'>";
  html += "<a class='btn auto' href='/auto'>AUTO</a>";
  html += "<a class='btn on' href='/nighton'>NACHT EIN</a>";
  html += "<a class='btn off' href='/nightoff'>NACHT AUS</a>";
  html += "</div>";

  html += "<div class='item'>Sleep: " + String(sleepZeit) + " s</div></div></body></html>";

  server.send(200, "text/html", html);
}

// Webserver Routen-Handler
void handleAuto() { autoMode = true; server.sendHeader("Location", "/"); server.send(303); }
void handleNightOn() { autoMode = false; manualNightMode = true; server.sendHeader("Location", "/"); server.send(303); }
void handleNightOff() { autoMode = false; manualNightMode = false; server.sendHeader("Location", "/"); server.send(303); }

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Wi-Fi Access Point und Station gleichzeitig starten
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  
  Serial.print("Access Point gestartet. SSID: "); Serial.println(ssid);
  Serial.print("IP-Adresse für Webinterface: "); Serial.println(WiFi.softAPIP());
  Serial.print("Empfänger MAC-Adresse: "); Serial.println(WiFi.macAddress());

  // ESP-NOW initialisieren
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW FEHLER BEI INITIALISIERUNG");
    return;
  }

  // Callback für Datenempfang registrieren
  esp_now_register_recv_cb(onReceive);
  
  // HTTP-Routen zuweisen
  server.on("/", handleRoot);
  server.on("/auto", handleAuto);
  server.on("/nighton", handleNightOn);
  server.on("/nightoff", handleNightOff);
  
  // Webserver starten
  server.begin();
  Serial.println("WEBSERVER ERFOLGREICH GESTARTET");
}

void loop() {
  server.handleClient();
  delay(2); // Kleine Entlastungspause für die CPU
}
