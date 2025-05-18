#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <OSCMessage.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define RLDR_PIN A0

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP Udp;

const char* ssidList[] = {"G52DT", "Appartamento II Piano"};
const char* passList[] = {"GDThotspot", "benvenuti!"};
const int numNetworks = 2;

IPAddress remoteIP(192, 168, 123, 50);
const unsigned int remotePort = 57120;

int temp = 0;
int humi = 0;
int rldr = 0;
int triggerBar = 0;
unsigned long durBar = random(5000, 15000);
unsigned long barStart = 0;

void connectToWiFi() {
  for (int i = 0; i < numNetworks; i++) {
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 10) {
      WiFi.begin(ssidList[i], passList[i]);
      delay(1000);
      tries++;
    }
    if (WiFi.status() == WL_CONNECTED) return;
  }
}

void setup() {
  dht.begin();
  analogReadResolution(12);
  randomSeed(analogRead(A1));
  connectToWiFi();
  Udp.begin(8888);
  barStart = millis();
}

void loop() {
  unsigned long now = millis();

  // === Controllo se è ora di iniziare una nuova battuta ===
  if (now - barStart >= durBar) {
    triggerBar = 1;
    durBar = random(5000, 15000);  
    barStart = now;               
  } else {
    triggerBar = 0;
  }

  // === Lettura sensori ===
  temp = 100 * dht.readTemperature();
  humi = 100 * dht.readHumidity();
  rldr = analogRead(RLDR_PIN);

  // === Invio dati OSC ===
  sendOSC("/sensors/temp", temp);
  sendOSC("/sensors/humi", humi);
  sendOSC("/sensors/rldr", rldr);
  sendOSC("/trigger/tbar", triggerBar);
  sendOSC("/duratab/dbar", durBar);

  delay(200);
}

void sendOSC(const char* address, int value) {
  OSCMessage msg(address);
  msg.add(value);
  Udp.beginPacket(remoteIP, remotePort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}
