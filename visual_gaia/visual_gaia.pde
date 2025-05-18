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
  oscP5 = new OscP5(this, 57121); // Porta in ascolto

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
void oscEvent(OscMessage msg){
  if (msg.checkAddrPattern("/env") && msg.checkTypetag("fff")) {
    temp = msg.get(0).floatValue();
    hum  = msg.get(1).floatValue();
    light= msg.get(2).floatValue();
  }
}
