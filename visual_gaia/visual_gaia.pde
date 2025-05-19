import oscP5.*;
import netP5.*;

OscP5 oscP5;

float temp = 0.5;   // Colore
float hum = 0.5;    // Lifespan
float light = 0.5;  // Accelerazione

ParticleSystem ps;
int Nparticles = 500;

void setup(){
  size(1280, 720, P2D);
  oscP5 = new OscP5(this, 57130, OscP5.UDP);

  ps = new ParticleSystem();
  for(int i = 0; i < Nparticles; i++){
    ps.addParticle();
  }
  background(0);
}

void draw(){
  blendMode(ADD);
  background(0);
  ps.followEnv(light, light*100, light*100);
  ps.draw();
}

// Ricezione OSC da SuperCollider
void oscEvent(OscMessage msg) {
  println("MSG ARRIVATO:");
  println("address: " + msg.addrPattern());
  println("typetag: " + msg.typetag());

  try {
    if (msg.checkAddrPattern("/sensors/temp") && msg.checkTypetag("i")) {
      temp = msg.get(0).intValue();
    }

    if (msg.checkAddrPattern("/sensors/humi") && msg.checkTypetag("i")) {
      hum = msg.get(0).intValue();
    }

    if (msg.checkAddrPattern("/sensors/rldr") && msg.checkTypetag("i")) {
      light = msg.get(0).intValue() / 4095.0;
    }
    println("Valori aggiornati:");
    println("temp: " + temp + ", hum: " + hum + ", light: " + light);
  } catch(Exception e) {
    println("Errore nel parsing OSC: " + e.getMessage());
  }
}
