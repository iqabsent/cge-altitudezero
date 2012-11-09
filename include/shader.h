class ShaderProg {
  
  GLuint program_;
  GLuint modelToProjection_index;
  GLuint diffuse_color_index;
  GLuint light_direction_index;
  GLuint textureIndex_;

public:
  ShaderProg() {}

  void init() {

  /*
    char * vertex_shader_code =
      "varying vec4 color_;"
      "attribute vec4 pos;"
      "attribute vec3 normal;"
      "uniform vec4 diffuse_color;"
      "uniform vec3 light_direction;"
      "uniform mat4 modelToProjection;"
      ""
      "void main() {"
      "  float light_factor = dot(light_direction, normal);"
      "  color_ = max(light_factor, 0.2) * diffuse_color;"
      "  gl_Position = modelToProjection * pos;"
      "}";

    char * fragment_shader_code =
      "varying vec4 color_;"
      "void main() {gl_FragColor = color_;}";
    */

    char * vertex_shader_code =
      "varying vec2 uv_;"
      "attribute vec3 pos;"
      "attribute vec2 uv;"
      "uniform mat4 modelToProjection;"
      "void main() { gl_Position = modelToProjection * vec4(pos, 1); uv_ = uv; }";

    char * fragment_shader_code =
      "varying vec2 uv_;"
      "uniform sampler2D texture;"
      "void main() { gl_FragColor = texture2D(texture, uv_); }";

    GLuint vertex_shader = makeShader(GL_VERTEX_SHADER, vertex_shader_code);
    GLuint fragment_shader = makeShader(GL_FRAGMENT_SHADER, fragment_shader_code);
    
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);

    glBindAttribLocation(program_, 0, "pos");
    glBindAttribLocation(program_, 2, "uv");
    glBindAttribLocation(program_, 1, "normal");

    glLinkProgram(program_);

    modelToProjection_index = glGetUniformLocation(program_, "modelToProjection");
    textureIndex_ = glGetUniformLocation(program_, "texture");
    diffuse_color_index = glGetUniformLocation(program_, "diffuse_color");
    light_direction_index = glGetUniformLocation(program_, "light_direction");
  }

  void setSomeStuff(const vec4 &diffuse_color, const vec4 &light_direction) {
    
    glUseProgram(program_);

    glUniform4fv(diffuse_color_index, 1, diffuse_color.get());
    glUniform3fv(light_direction_index, 1, light_direction.get());
  }

  void setMatrixStuff(const mat4 &modelToProjection) {
    
    glUseProgram(program_);
    
    glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get());
  }

  GLuint makeShader (int type, char * code) {
    GLsizei length;
    GLchar buf[256];
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char **)&code, NULL);
    glCompileShader(shader);
    glGetShaderInfoLog(shader, sizeof(buf), &length, buf);
    puts(buf);

    modelToProjection_index = glGetUniformLocation(program_, "modelToProjection");
    diffuse_color_index = glGetUniformLocation(program_, "diffuse_color");
    light_direction_index = glGetUniformLocation(program_, "light_direction");

    return shader;
  }

  GLuint program() {
    return program_;
  }

  void render() {
    glUseProgram(program_);
    glUniform1i(textureIndex_, 0);
  }

  mat4 setProjection(mat4 modelToWorld)
  {
    mat4 cameraToWorld;
    cameraToWorld.loadIdentity();
    cameraToWorld.translate(0, 0, 10);

    // flip it around to transform from world to camera
    mat4 worldToCamera;
    cameraToWorld.invertQuick(worldToCamera);

    // build a projections matrix to add perspective
    mat4 cameraToProjection;
    cameraToProjection.loadIdentity();
    float n = 0.1f, f = 200;
    cameraToProjection.frustum(-n, n, -n, n, n, f);

    // model -> world -> camera -> projection
    return modelToWorld * worldToCamera * cameraToProjection;
  }

  void renderObject(GLuint texture, float vertices[], int vertex_count, mat4 modelToWorld) {

    mat4 modelToProjection = setProjection(modelToWorld);

    glUniform1i(textureIndex_, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get());
  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2); 

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
  }

  void renderObject(RenderData render_data) {
    renderObject(render_data.texture, render_data.vertices, render_data.vertex_count, render_data.model_to_world);
  }

};