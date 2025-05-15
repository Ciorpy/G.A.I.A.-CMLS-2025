import oscP5.*;
import netP5.*;
import controlP5.*;

ControlP5 cp5;
OscP5 oscP5;
NetAddress juceApp;
DropdownList menu;

float delayVal = 0.0;
float distortionVal = 0.0;
float reverbVal = 0.0;

int w = 800;
int h = 600;
int radius = w / 8;
int spacing = 250;

int juceDOOR = 9002;
  
void settings() {
  size(w, h);
}

void setup() {

  // Setup OSC
  oscP5 = new OscP5(this, 8000);  // porta locale
  juceApp = new NetAddress("127.0.0.1", juceDOOR);  // plugin JUCE

  cp5 = new ControlP5(this);

  // Knob: Delay
  cp5.addKnob("delayVal")
     .setRange(0, 1)
     .setValue(0.0)
     .setPosition(width / 2 - radius - spacing, 170)
     .setRadius(radius)
     .setCaptionLabel("")
     .setColorForeground(color(0, 150, 255))
     .setColorActive(color(0, 100, 200));

  // Knob: Distorsione
  cp5.addKnob("distortionVal")
     .setRange(0, 1)
     .setValue(0.0)
     .setPosition(width / 2 - radius, 170)
     .setRadius(radius)
     .setCaptionLabel("")
     .setColorForeground(color(255, 100, 100))
     .setColorActive(color(200, 50, 50));

  // Knob: Riverbero
  cp5.addKnob("reverbVal")
     .setRange(0, 1)
     .setValue(0.0)
     .setPosition(width / 2 - radius + spacing, 170)
     .setRadius(radius)
     .setCaptionLabel("")
     .setColorForeground(color(100, 255, 150))
     .setColorActive(color(50, 200, 100));
     
     
  //menu = cp5.addDropdownList("Seleziona una forma d'onda")
  //          .setPosition(50, 50)
  //          .setSize(200, 300)  // larghezza e altezza dell'elenco
  //          .setItemHeight(40)
  //          .setBarHeight(40)
  //          .setValue(0)
  //          .close();

  // Aggiungi voci al menu
  //menu.addItem("Option A", 0);
  //menu.addItem("Option B", 1);
  //menu.addItem("Option C", 2);
  //menu.setValue(0);
  
  cp5.addSlider("octaverVal")
   .setRange(-2, 2)
   .setValue(0)
   .setNumberOfTickMarks(5)
   .setPosition(width / 2 - 200, 450)
   .setSize(400, 100)
   .setSliderMode(Slider.FLEXIBLE)
   .setCaptionLabel("")
   .setColorForeground(color(255, 200, 0))
   .setColorActive(color(200, 150, 0));
}

void draw() {
  background(50);

  // Titolo del synth
  textAlign(CENTER);
  textSize(80);
  fill(255, 140, 0);  // arancione
  text("G.A.I.A.", width / 2, 100);

   // Etichette manopole abbinate per colore
  textSize(30);
  textAlign(CENTER);

  fill(0, 150, 255); // blu chiaro (delay)
  text("Delay", width / 2 - spacing, 150);

  fill(255, 100, 100); // rosso (distorsione)
  text("Distortion", width / 2, 150);

  fill(100, 255, 150); // verde (riverbero)
  text("Reverb", width / 2 + spacing, 150);
  
    fill(100, 255, 150); // verde (riverbero)
  text("Octaver", width / 2, 415);
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

void octaverVal(int val) {
  sendOSC("/octaver", val);
}

void sendOSC(String address, float val) {
  OscMessage msg = new OscMessage(address);
  msg.add(val);
  oscP5.send(msg, juceApp);
}
