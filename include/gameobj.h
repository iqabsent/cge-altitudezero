class GameObj {
  bool active_;
  vec4 pos_;
  vec4 speed_;
  vec4 thrust_;
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

  void simulate() {
    //position
    if(!active_) return;
    speed_ = speed_ + thrust_;
    thrust_ = vec4(0,0,0,0);
    //speed_ = speed_ - vec4(friction_,friction_,0,0); // ...that's not how friction works :/
    pos_ = pos_ + speed_;

    //matrix
    float * pos = pos_.get();
    render_data_.model_to_world.loadIdentity();
    render_data_.model_to_world.translate(pos[0], pos[1], 0);
  }

  RenderData renderData() { return render_data_; }
};