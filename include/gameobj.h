// GameObj, used as base for any object in game
// This file also contains extended classes Craft(abstract), Fighter, Enemy and Projectile

class GameObj {

  bool active_;
  float friction_;
  vec4 thrust_;
  RenderData render_data_;        // struct defined in structs.h
  CollisionData collision_data_;  // struct defined in structs.h

protected:
  vec4 pos_;
  vec4 speed_;
  vec4 max_velocity_;

public:
  GameObj(){
    active_ = false;
  };

  void init(float x, float y, float vertices[], int vertex_count) {

    active_ = true;
    friction_ = 0;

    pos_ = vec4(x,y,0,0);
    max_velocity_ = vec4(1,1,0,0);
    render_data_.vertices = vertices;
    render_data_.vertex_count = vertex_count;
    render_data_.model_to_world.loadIdentity();

    // check bounding box dimensions
    float vertex_x = vertices[0];
    float vertex_y = vertices[1];
    float bounds[4] = {vertex_x, vertex_y, vertex_x, vertex_y}; //{min_x, min_y, max_x, max_y}
    for (int i = 1; i < vertex_count; i++) {
      vertex_x = vertices[i*3];
      vertex_y = vertices[i*3 + 1];
      if(vertex_x < bounds[0]) bounds[0] = vertex_x;
      if(vertex_x > bounds[2]) bounds[2] = vertex_x;
      if(vertex_y < bounds[1]) bounds[1] = vertex_y;
      if(vertex_y > bounds[3]) bounds[3] = vertex_y;
    }

    float width = bounds[2] - bounds[0];
    float height = bounds[3] - bounds[1];

    // find center of geometry and record radius
    collision_data_.center_offset_x = bounds[0] + width / 2;
    collision_data_.center_offset_y = bounds[1] + height / 2;
    collision_data_.center_x = bounds[0] + width / 2;
    collision_data_.center_y = bounds[1] + height / 2;
    collision_data_.width = width;
    collision_data_.height = height; 
    collision_data_.radius = sqrt(width*width + height*height);
  }

  void activate(float x, float y) {
    pos_ = vec4(x,y,0,0);
    active_ = true;
  }

  void deactivate() {
    active_ = false;
  }

  void setTexture(GLuint texture) {
    render_data_.texture = texture;
  }

  void setThrust(float x, float y) {
    thrust_ = thrust_ + vec4(x, y, 0, 0);
  }

  void setFriction(float friction) {
    friction_ = friction;
  }

  void setMaxVelocity(float max) {
    max_velocity_ = vec4(max, max, 0, 0);
  }

  virtual void boundaryChecks() {};

  void simulate() {
    // position
    if(!active_) return;
    speed_ = speed_ + thrust_;
    thrust_ = vec4(0,0,0,0);
    // friction
    float * speed = speed_.get();
    for (int i = 0; i < 2; i++) {
      if(speed[i] / friction_ <= 1 && speed[i] / friction_ >= -1) speed[i] = 0; //would be better to get abs and compare magnitude against friciton
      if(speed[i] > 0 && speed[i] > friction_) speed[i] -= friction_;
      if(speed[i] < 0 && speed[i] < -friction_) speed[i] += friction_;
      // limit max speed
      float * max = max_velocity_.get();
      if(speed[i] > 0 && speed[i] > max[i]) speed[i] = max[i];
      if(speed[i] < 0 && speed[i] < -max[i]) speed[i] = -max[i];
    }
    speed_ = vec4(speed[0], speed[1], 0, 0);

    // update translation vector .. kinda
    pos_ = pos_ + speed_;

    // update collision data
    float * pos = pos_.get();
    collision_data_.center_x = pos[0] + collision_data_.center_offset_x;
    collision_data_.center_y = pos[1] + collision_data_.center_offset_y;

    // boundary checks
    boundaryChecks();

    // matrix
    render_data_.model_to_world.loadIdentity();
    render_data_.model_to_world.translate(pos[0], pos[1], 0);
  }

  bool isActive() { return active_; }
  RenderData getRenderData() { return render_data_; }
  CollisionData getCollisionData() { return collision_data_; }
  vec4 pos() {return pos_; }
};

/////////////////////////////////////////
// derived from GameObj, used for bullets
class Projectile : public GameObj {

  float damage_;

public:
  void init(float x, float y, float vertices[], int vertex_count, float damage) {
    GameObj::init(x, y, vertices, vertex_count);
    damage_ = damage;
    speed_ = vec4(0, 0, 0, 0);
  }

  void boundaryChecks() {
    float * pos = pos_.get();
    if(abs(pos[0]) > 10 || abs(pos[1]) > 10) deactivate();
  }

  void setDamage(float damage){ damage_ = damage; }
  float getDamage(){ return damage_; }
};

////////////////////////////////////////////////////////////////
// derived from GameObj used, for player fighter and enemy craft
class Craft : public GameObj {

protected:
  float health_;
  Craft() {}; // don't want anyone to instantiate one of these

public:
  void damage(float points) {
    health_ = health_ - points;
    if(health_ <= 0) {
      deactivate();
    }
  }

  void heal(float points) { health_ = health_ + points; }
  void setHealth(int points) {health_ = points; }
  float getHealth() { return health_; }

};

////////////////////////////////////////////////////
// derived from Craft, used for player fighter craft
class Fighter : public Craft {

public:
  void init(float x, float y, float vertices[], int vertex_count) {
    Craft::init(x, y, vertices, vertex_count);
    health_ = 100;
  }

  void simulate() {
    Craft::simulate();
    float * speed = speed_.get();
    float * max = max_velocity_.get();
    if(speed[0] < -max[0]/2) {
      setTexture(Asset::get().texture(Asset::TX_FIGHTER_LEFT));
    } else if(speed[0] > max[0]/2) {
      setTexture(Asset::get().texture(Asset::TX_FIGHTER_RIGHT));
    } else {
      setTexture(Asset::get().texture(Asset::TX_FIGHTER));
    }
  }

  void boundaryChecks() {
    float * pos = pos_.get();
    if(pos[0] > 10) pos[0] = 10;
    if(pos[0] < -10) pos[0] = -10;
    if(pos[1] > 10) pos[1] = 10;
    if(pos[1] < -10) pos[1] = -10;
    pos_ = vec4(pos[0], pos[1], 0, 0);
  }

  void die() {
    // cry?
  }
};

///////////////////////////////////////////////////
// derived from Craft, used for enemy fighter craft
class Enemy : public Craft {

public:

  void init(float x, float y, float vertices[], int vertex_count) {
    Craft::init(x, y, vertices, vertex_count);
    health_ = 100;
  }

  void boundaryChecks() {
    float * pos = pos_.get();
    if(abs(pos[0]) > 10 || abs(pos[1]) > 10) deactivate();
  }

  void behaviour() {
    // AI ! .. the part I'm really interested in
    // completely randomly fire?
    int hold_fire = rand() % 20;
    //if(!hold_fire) ... $#%&^! ... can't spawn things from here :/
    //pass function pointer to function in main.cpp as spawn proxy?
  }

  void simulate() {
    behaviour();
    Craft::simulate();
  }
};