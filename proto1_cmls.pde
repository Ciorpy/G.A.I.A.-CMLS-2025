import oscP5.*;
import netP5.*;
import controlP5.*;

ControlP5 cp5;
OscP5 oscP5;
NetAddress juceApp;

float delayVal = 0.0;
float distortionVal = 0.0;
float reverbVal = 0.0;

void setup() {
  size(400, 280);

  // Setup OSC
  oscP5 = new OscP5(this, 8000);  // porta locale
  juceApp = new NetAddress("127.0.0.1", 9000);  // plugin JUCE

  cp5 = new ControlP5(this);

  // Knob: Delay
  cp5.addKnob("delayVal")
     .setRange(0, 1)
     .setValue(0.5)
     .setPosition(50, 120)
     .setRadius(40)
     .setCaptionLabel("")
     .setColorForeground(color(0, 150, 255))
     .setColorActive(color(0, 100, 200));

  // Knob: Distorsione
  cp5.addKnob("distortionVal")
     .setRange(0, 1)
     .setValue(0.5)
     .setPosition(160, 120)
     .setRadius(40)
     .setCaptionLabel("")
     .setColorForeground(color(255, 100, 100))
     .setColorActive(color(200, 50, 50));

  // Knob: Riverbero
  cp5.addKnob("reverbVal")
     .setRange(0, 1)
     .setValue(0.5)
     .setPosition(270, 120)
     .setRadius(40)
     .setCaptionLabel("")
     .setColorForeground(color(100, 255, 150))
     .setColorActive(color(50, 200, 100));
}

void draw() {
  background(50);

  // Titolo del synth
  textAlign(CENTER);
  textSize(20);
  fill(255, 140, 0);  // arancione
  text("My Synth Plugin", width / 2, 40);

   // Etichette manopole abbinate per colore
  textSize(14);
  textAlign(CENTER);

  fill(0, 150, 255); // blu chiaro (delay)
  text("Delay", 90, 110);

  fill(255, 100, 100); // rosso (distorsione)
  text("Distortion", 200, 110);

  fill(100, 255, 150); // verde (riverbero)
  text("Reverb", 310, 110);
}

// Invio OSC
void delayVal(float val) {
  sendOSC("/delay", val);
}
void distortionVal(float val) {
  sendOSC("/distortion", val);
}
void reverbVal(float val) {
  sendOSC("/reverb", val);
}

void sendOSC(String address, float val) {
  OscMessage msg = new OscMessage(address);
  msg.add(val);
  oscP5.send(msg, juceApp);
}
