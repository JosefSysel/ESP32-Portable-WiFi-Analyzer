#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Display settings ----------
// Button pin used for controlling the analyzer.
// ESP32-S2 Mini uses the built-in BOOT button on GPIO 0.
// If an external button is used, change this value to the GPIO pin
// where the new button is connected.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SDA_PIN 9
#define SCL_PIN 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Button settings ----------
#define BUTTON_PIN 0
const unsigned long DEBOUNCE_MS = 50;

bool lastRawState = HIGH;      // last raw pin reading
bool stableState = HIGH;       // confirmed (debounced) state
unsigned long lastDebounceTime = 0;

// ---------- Behavior settings ----------
const unsigned long RESCAN_INTERVAL = 15000;   // automatic rescan (ms)

int networkCount = 0;
int currentIndex = 0;
unsigned long lastScan = 0;

String encTypeToStr(wifi_auth_mode_t enc) {
  switch (enc) {
    case WIFI_AUTH_OPEN:            return "OPEN";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ENT";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
    default:                        return "?";
  }
}

void doScan() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Scanning networks...");
  display.display();

  networkCount = WiFi.scanNetworks();
  currentIndex = 0;
  lastScan = millis();

  display.clearDisplay();
  display.setCursor(0, 0);
  if (networkCount == 0) {
    display.println("No networks found");
  } else {
    display.print("Networks found: ");
    display.println(networkCount);
  }
  display.display();
  delay(1000);
}

void showNetwork(int idx) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print(idx + 1);
  display.print("/");
  display.println(networkCount);
  display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

  String ssid = WiFi.SSID(idx);
  if (ssid.length() == 0) ssid = "(hidden network)";
  display.setCursor(0, 14);
  display.println(ssid);

  int rssi = WiFi.RSSI(idx);
  int ch   = WiFi.channel(idx);
  String enc = encTypeToStr(WiFi.encryptionType(idx));

  display.setCursor(0, 28);
  display.print("Signal: ");
  display.print(rssi);
  display.println(" dBm");

  display.setCursor(0, 40);
  display.print("Channel: ");
  display.println(ch);

  display.setCursor(0, 52);
  display.print("Security: ");
  display.println(enc);

  display.display();
}

// Returns true if the button was just pressed (debounced)
bool buttonPressed() {
  bool reading = digitalRead(BUTTON_PIN);
  bool pressed = false;

  if (reading != lastRawState) {
    lastDebounceTime = millis();   // signal changed, restart debounce timer
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    // signal has been stable for longer than DEBOUNCE_MS
    if (reading != stableState) {
      stableState = reading;
      if (stableState == LOW) {
        pressed = true; // falling edge = press
      }
    }
  }

  lastRawState = reading;
  return pressed;
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Failed to initialize SSD1306");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WiFi Scanner");
  display.println("starting...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  doScan();
  if (networkCount > 0) showNetwork(currentIndex);
}

void loop() {
  unsigned long now = millis();

  // Automatic rescan after interval elapses
  if (now - lastScan > RESCAN_INTERVAL) {
    doScan();
    if (networkCount > 0) showNetwork(currentIndex);
    return;
  }

  // Manual switch via button
  if (networkCount > 0 && buttonPressed()) {
    currentIndex = (currentIndex + 1) % networkCount;
    showNetwork(currentIndex);
  }

  delay(10);
}
