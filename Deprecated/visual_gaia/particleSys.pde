float clip(float val, float min_val, float max_val){
  return max(min(val, max_val), min_val);
}

class ParticleSystem {
  ArrayList<Particle> particles;
  PVector origin;
  float forceScale = 1;
  int particleHue = 255;
  float lifespanBase = 255;

  ParticleSystem(){
    this.particles = new ArrayList<Particle>();
    this.origin = new PVector(width/2, height/2);
  }

  void addParticle(){
  PVector randomPos = new PVector(random(width), random(height));
  this.particles.add(new Particle(randomPos, 10, random(0,255)));
}


  void followEnv(float temperature, float humidity, float lightness){
    forceScale = lightness * 2.0; // mappa luce → forza
    particleHue = (int)(temperature * 255); // mappa temp → colore
    lifespanBase = clip(humidity * 255, 50, 255); // mappa umidità → vita base
  }

  void draw(){
    Particle p;
    float small_force = 0.05;
    PVector random_force = new PVector(0,0);

    for(int i = this.particles.size() - 1; i >= 0; i--){
      p = this.particles.get(i);
      random_force.x = forceScale * random(-small_force, small_force);
      random_force.y = forceScale * random(-small_force, small_force);
      p.applyForce(random_force);
      p.update();
      p.draw(particleHue);
      p.lifespan -= 0.5;
      if(p.isDead()){
        particles.remove(i);
        this.addParticle();
      }
    }
  }
}
