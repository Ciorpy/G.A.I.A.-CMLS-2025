#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <OSCMessage.h> // ← nuova libreria

#define DHTPIN 2
#define DHTTYPE DHT22
#define rldr_PIN A0

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP Udp;

// Access point credentials
const char* ssidList[] = {"G52DT", "Appartamento II Piano"};
const char* passList[] = {"GDThotspot", "benvenuti!"};
const int numNetworks = 2;

IPAddress remoteIP(192, 168, 123, 50); // IP del computer con SC
const unsigned int remotePort = 57120;

unsigned long lastSend = 0;
const unsigned long sendInterval = 200;

int temp = 0;
int humi = 0;
int rldr = 0;

void connectToWiFi() {
  for (int i = 0; i < numNetworks; i++) {
    Serial.print("Trying to connect to: ");
    Serial.println(ssidList[i]);
    WiFi.begin(ssidList[i], passList[i]);

    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 10) {
      delay(1000);
      Serial.print(".");
      tries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      Serial.print("SSID: "); Serial.println(WiFi.SSID());
      Serial.print("IP: "); Serial.println(WiFi.localIP());
      return;
    }
    Serial.println("\nFailed to connect to: " + String(ssidList[i]));
  }

  Serial.println("Could not connect to any known network.");
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  analogReadResolution(12);

  connectToWiFi();
  Udp.begin(8888);
}

void loop() {
  unsigned long now = millis();
  if (now - lastSend >= sendInterval) {
    lastSend = now;

    temp = 100 * dht.readTemperature();
    humi = 100 * dht.readHumidity();
    rldr = analogRead(rldr_PIN);

    if (!isnan(temp) && !isnan(humi)) {
      sendOSC("/sensors/temp", temp);
      sendOSC("/sensors/humi", humi);
    }

    sendOSC("/sensors/rldr", rldr);
  }
}

void sendOSC(const char* address, int value) {
  OSCMessage msg(address);
  msg.add(value);
  Udp.beginPacket(remoteIP, remotePort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}
