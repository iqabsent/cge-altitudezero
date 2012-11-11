// Separating out structs to keep main clean..

struct RenderData {
  GLuint vertex_count;
  GLuint texture;
  mat4 model_to_world;
  float * vertices;
};

struct CollisionData {
  float center_offset_x, center_offset_y, center_x, center_y, width, height, radius;
};