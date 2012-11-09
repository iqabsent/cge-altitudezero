class GameObj {
  bool active_;
  vec4 pos_;
  vec4 speed_;
  vec4 thrust_;
  vec4 max_;
  float friction_;

  RenderData render_data_;
  // FOR REFERENCE
  //struct RenderData {
  //  GLuint vertex_count;
  //  GLuint texture;
  //  mat4 model_to_world;
  //  float * vertices; //didn't have []
  //};

public:
  GameObj(){};

  void init(float x, float y, float vertices[], int vertex_count) {

    active_ = true;
    friction_ = 0;

    pos_ = vec4(x,y,0,0);
    max_ = vec4(1,1,0,0);
    render_data_.vertices = vertices;
    render_data_.vertex_count = vertex_count;
    render_data_.model_to_world.loadIdentity();
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

  void setMax(float max) {
    max_ = vec4(max, max, 0, 0);
  }

  void simulate() {
    // position
    if(!active_) return;
    speed_ = speed_ + thrust_;
    thrust_ = vec4(0,0,0,0);
    // friction
    float * speed = speed_.get();
    for (int i = 0; i < 2; i++) {
      if(speed[i] / friction_ < 1 && speed[i] / friction_ > -1) speed[i] = 0; //would be better to get abs and compare magnitude against friciton
      if(speed[i] > 0 && speed[i] > friction_) speed[i] -= friction_;
      if(speed[i] < 0 && speed[i] < -friction_) speed[i] += friction_;
      // limit max speed
      float * max = max_.get();
      if(speed[i] > 0 && speed[i] > max[i]) speed[i] = max[i];
      if(speed[i] < 0 && speed[i] < -max[i]) speed[i] = -max[i];
    }
    speed_ = vec4(speed[0], speed[1], 0, 0);

    // translation vector .. kinda
    pos_ = pos_ + speed_;

    // matrix
    float * pos = pos_.get();
    render_data_.model_to_world.loadIdentity();
    render_data_.model_to_world.translate(pos[0], pos[1], 0);
  }

  RenderData renderData() { return render_data_; }
};