#define GLEW_STATIC
#include "include/GL/glew.h"
#define FREEGLUT_STATIC
#include "include/GL/glut.h"

#include "include/AL/al.h"
#include "include/AL/alc.h"

// just use andy thomason's vector and matrix classes
// think they need math
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "include/vector.h"
#include "include/matrix.h"

#include "include/file_manager.h"
#include "include/texture_manager.h"
#include "include/sound_manager.h"

struct RenderData {
  GLuint vertex_count;
  GLuint texture;
  mat4 model_to_world;
  float * vertices; //didn't have []
};

// my shader program class
// .program returns a compiled & linked shader program
#include "include/shader.h"
#include "include/gameobj.h"

ShaderProg shaderProg;
GLuint program;
GLuint texture_;

GLuint texture2;

mat4 modelToProjection_;
mat4 cameraToWorld;
mat4 modelToWorld;
const int viewport_width_ = 800, viewport_height_ = 600;
char keys[256];

GameObj bullet;

void build_camera_matrix()
{
  // this matrix can be used to move objects around in the scene
  modelToWorld.loadIdentity();

  // this is the transform from the camera to the world
  cameraToWorld.loadIdentity();
  cameraToWorld.translate(0, 0, 10);
}

void updateView()
{
  // flip it around to transform from world to camera
  mat4 worldToCamera;
  cameraToWorld.invertQuick(worldToCamera);

  // build a projections matrix to add perspective
  mat4 cameraToProjection;
  cameraToProjection.loadIdentity();
  float n = 0.1f, f = 200;
  cameraToProjection.frustum(-n, n, -n, n, n, f);

  // model -> world -> camera -> projection
  modelToProjection_ = modelToWorld * worldToCamera * cameraToProjection;
}

void simulate() {

//move this elsewhere..
char name[2];
name[0] = 1 + '0';
name[1] = 0;

  float speed = 10.0f / 30;
  if (keys['f']) modelToWorld.translate(0.5f,0.0f,0.0f);
  if (keys['s']) modelToWorld.translate(-0.5f,0.0f,0.0f);
  if (keys['d']) modelToWorld.translate(0.0f,-0.5f,0.0f);
  if (keys['e']) modelToWorld.translate(0.0f,0.5f,0.0f);
  if (keys['c']) sound_manager::play(vec4(0, 0, 0, 0), name);

  bullet.simulate();
}

void set_key(int key, int value) {
  keys[key & 0xff] = value;
}

void render()
{
  glClearColor(0, 0, 0, 1);
  glClear (GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(program);

  float vertices[3*3] = {
    -1, -1, 0,
    1, -1, 0,
    0,  1, 0
  };

  mat4 modelToWorld2;
  modelToWorld2.loadIdentity();

  shaderProg.renderObject(texture_, vertices, 3, modelToWorld);
  shaderProg.renderObject(bullet.renderData());
}

static void display() { simulate(); render(); }
static void key_down( unsigned char key, int x, int y) { set_key(key, 1); }
static void key_up( unsigned char key, int x, int y) { set_key(key, 0); }
static void timer(int value) { glutTimerFunc(30, timer, 1); glutPostRedisplay(); }

void main(int argc, char** argv) {

//these should be set / overiden by args

  glutInit(&argc, argv);
  glutInitWindowSize(viewport_width_, viewport_height_);
  glutCreateWindow("AltitudeZero - Scroller game..");
  glewInit();

  sound_manager::add_buffers("assets/labels.txt", "assets/poing.wav");

  texture_ = texture_manager::new_texture("assets/texture.tga", 0, 1, 256, 180);
  texture2 = texture_manager::new_texture("assets/texture.tga", 2, 0, 1, 1);

  shaderProg.init();
  program = shaderProg.program();

  build_camera_matrix();

  float bullet_vertices[3*6] = {
    -0.1f, 0.1f, 0,
    -0.1f, -0.1f, 0,
    0.1f, -0.1f, 0,
    0.1f, 0.1f, 0,
    0.1f, -0.1f, 0,
    -0.1f, 0.1f, 0
  };

  bullet.init(1,1,bullet_vertices,6);
  bullet.setTexture(texture2);
  bullet.setThrust(0.0f, 0.1f);

  //function for rendering
  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutKeyboardUpFunc(key_up);
  glutTimerFunc(30, timer, 1);

  //call main loop
  glutMainLoop(); //Start the main loop
  //display();

}