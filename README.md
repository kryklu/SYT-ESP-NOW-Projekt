# SYT-Projekt: Drahtlose Sensor-Erfassung & Smart-Aktorik via ESP-NOW

**Verfasser:** Ali Tanriut, Djordje Stojanovic  
**Datum:** 26.05.2026

---

## 1. Einführung
In der modernen Automatisierungstechnik und im Internet of Things (IoT) spielt die effiziente, drahtlose Datenübertragung zwischen Kleinstgeräten eine zentrale Rolle. Häufig wird hierfür auf klassische WLAN-Verbindungen über einen zentralen Router zurückgegriffen, was jedoch zu Latenzen und erhöhtem Energieverbrauch führen kann. 

Das von Espressif entwickelte **ESP-NOW-Protokoll** bietet hier eine ressourcenschonende Alternative, da es eine direkte Peer-to-Peer-Kommunikation ohne Accesspoint ermöglicht. Im Rahmen dieses Projekts wird eine Sensor-Aktor-Infrastruktur auf Basis zweier ESP32-Mikrocontroller realisiert, welche Umgebungshelligkeit und Objekterkennung im Raum überwacht und via Webinterface steuerbar macht.

---

## 2. Projektbeschreibung
Es wurde ein verteiltes IoT-System realisiert, bei dem zwei ESP32-Module drahtlos über ESP-NOW miteinander kommunizieren:

* **Die Sender-Station (ESP1):** Erfasst zyklisch die Werte eines Lichtsensors (LDR) sowie eines Infrarot-Näherungssensors (IR). Ein Software-Filter bereinigt das Signal des Lichtsensors. Zudem steuert der Sender direkt eine RGB-LED zur lokalen Statusanzeige an.
* **Die Empfänger-Station (ESP2):** Empfängt die strukturierten Sensordaten, verarbeitet diese und stellt die Messwerte über ein responsives HTTP-Webinterface zur Verfügung. Das Webinterface bietet neben der Live-Überwachung einen automatischen sowie manuellen Steuerungsmodus mit dynamischer UI-Anpassung (Hell/Dunkel-Theme).

---

## 3. Theorie

### ESP-NOW Protokoll
ESP-NOW ist ein von Espressif entwickeltes verbindungsloses Kommunikationsprotokoll, das auf der 2,4-GHz-Frequenz arbeitet. Es überspringt den zeit- und energieaufwendigen Handshake-Prozess eines Standard-Wi-Fi-Verbindungsaufbaus. Dadurch können Datenpakete extrem schnell und mit minimalem Strombedarf direkt von einem Mikrocontroller zum anderen übertragen werden.

### Sensorik und Signalverarbeitung
1. **LDR-Lichtsensor (Fotowiderstand):** Liefert ein analoges Signal basierend auf der Umgebungshelligkeit. Um Signalrauschen zu minimieren, wird im Code ein gleitender Mittelwertfilter über 10 Samples angewendet.
2. **Infrarot-Näherungssensor (IR):** Arbeitet digital als *Active-Low*-Sensor. Sobald sich ein Objekt im Sichtfeld befindet, schaltet der Pin auf `LOW`.
3. **RGB-LED (Common Anode):** Da die LED eine gemeinsame Anode besitzt, wird sie invertiert angesteuert (`255 - Farbwert`).

---

## 4. Arbeitsschritte & Hardware-Aufbau

### Pinbelegung

* **Sender-Knoten (ESP1):**
  * Analoger Lichtsensor &rarr; **GPIO 34** (Analog INPUT)
  * Infrarot-Sensor (IR) &rarr; **GPIO 27** (Digital INPUT)
  * RGB-LED (Rot) &rarr; **GPIO 22** (OUTPUT)
  * RGB-LED (Grün) &rarr; **GPIO 21** (OUTPUT)
  * RGB-LED (Blau) &rarr; **GPIO 16** (OUTPUT)
* **Empfänger-Knoten (ESP2):**
  * Konfiguriert als Soft-AP (Hotspot) zur Bereitstellung des Webservers.

---

## 5. Systemfunktionen & UI-Betrieb

Das System arbeitet hochgradig adaptiv. Die Webseite passt ihr Farbschema automatisch der realen Umgebung oder der manuellen Benutzervorgabe an:

### Lokale Aktorik-Logik (Sender-Knoten)
* **Zustand "Hell":** Die RGB-LED leuchtet dauerhaft **Gelb**.
* **Zustand "Dunkel" (Lichtwert &ge; 200):** Die RGB-LED wechselt auf **Blau**.
* **Ereignis "Objekt erkannt":** Unabhängig von der Helligkeit unterbricht die LED ihren Zustand und **blinkt zweimal kurz**, um Aufmerksamkeit zu generieren.

### Webinterface-Ansichten (Empfänger-Knoten)
* **Automatischer Modus:** Der Webserver wertet den Flag `dunkel` aus. Ist es dunkel, schaltet die Seite ins dunkle CSS-Theme (`#0f172a`), ist es hell, wechselt das Dashboard in das helle CSS-Theme (`#f1f5f9`).
* **Manueller Modus:** Über die Buttons `NACHT EIN` und `NACHT AUS` kann das Verhalten der Anzeige vom User remote überschrieben werden.

---

## 6. Komponenten & Prototypen-Aufbau

### Komponentenliste
* 2x ESP32 NodeMCU Modul
* 1x Infrarot-Näherungssensor (Active-Low)
* 1x LDR-Lichtsensor-Modul
* 1x RGB-LED (Common Anode)
* 3x Vorwiderstände für die LED-Kanäle
* Breadboards und Jumper-Kabel

### Fotos vom Versuchsaufbau
> Die folgenden Bilder zeigen den physischen Breadboard-Aufbau des Senders mit der Verkabelung der Sensoren und der RGB-Status-LED.

#### Hardware-Aufbau (Top-View)
![Hardware Aufbau Top](hardware_top.jpeg)

#### Hardware-Aufbau (Front-View)
![Hardware Aufbau Front](hardware_front.jpeg)
