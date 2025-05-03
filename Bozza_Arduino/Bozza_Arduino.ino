#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define LDR_PIN A0

DHT dht(DHTPIN, DHTTYPE);

// Reading intervals (milliseconds)
const unsigned long dhtInterval = 2000;
const unsigned long ldrInterval = 200;

// Timers
unsigned long lastDHT = 0;
unsigned long lastLDR = 0;

// Raw sensor data
float temp = 0.0;
float hum = 0.0;
int ldrValue = 0;

// Normalized and transformed values
float tempNorm = 0.0, humNorm = 0.0, ldrNorm = 0.0;
float tempTransf = 0.0, humTransf = 0.0, ldrTransf = 0.0;

// Generative drift parameters
float tempDrift = 0.0, humDrift = 0.0, ldrDrift = 0.0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  dht.begin();
  analogReadResolution(12);  // MKR1010 ADC uses 12-bit resolution
  randomSeed(analogRead(A1));  // Seed randomness using a floating analog pin
}

void loop() {
  unsigned long now = millis();

  // Read DHT22 every 2000 ms
  if (now - lastDHT >= dhtInterval) {
    lastDHT = now;
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      temp = t;
      hum = h;
    }
  }

  // Read LDR every 200 ms and send data
  if (now - lastLDR >= ldrInterval) {
    lastLDR = now;

    ldrValue = analogRead(LDR_PIN);

    // --- Linear normalization ---
    tempNorm = constrain((temp - 10.0) / 60.0, 0.0, 1.0);   // 10-70°C
    humNorm  = constrain((hum - 10.0) / 80.0, 0.0, 1.0);    // 10-90%
    ldrNorm  = ldrValue / 4095.0;

    // --- Musical transformations ---
    tempTransf = pow(tempNorm, 2);             
    humTransf  = pow(humNorm, 1.5);            
    ldrTransf  = log10(ldrNorm * 9 + 1);       

    // --- Generative drift ---
    tempDrift = tempTransf + random(-5, 6) * 0.005;   // Small random variation [-0.025, +0.025]
    humDrift  = humTransf  + random(-5, 6) * 0.005;
    ldrDrift  = ldrTransf  + random(-5, 6) * 0.005;

    // Keep drift values within [0.0, 1.0]
    tempDrift = constrain(tempDrift, 0.0, 1.0);
    humDrift  = constrain(humDrift, 0.0, 1.0);
    ldrDrift  = constrain(ldrDrift, 0.0, 1.0);

    // --- Serial output ---
    Serial.print("DATA,");
    Serial.print(temp, 2); Serial.print(",");
    Serial.print(hum, 2); Serial.print(",");
    Serial.print(ldrValue); Serial.print(",");

    Serial.print(tempNorm, 3); Serial.print(",");
    Serial.print(humNorm, 3); Serial.print(",");
    Serial.print(ldrNorm, 3); Serial.print(",");

    Serial.print(tempTransf, 3); Serial.print(",");
    Serial.print(humTransf, 3); Serial.print(",");
    Serial.print(ldrTransf, 3); Serial.print(",");

    Serial.print(tempDrift, 3); Serial.print(",");
    Serial.print(humDrift, 3); Serial.print(",");
    Serial.println(ldrDrift, 3);
  }
}
