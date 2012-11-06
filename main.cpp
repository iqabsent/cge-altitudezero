#define GLEW_STATIC
#include "include/GL/glew.h"
#define FREEGLUT_STATIC
#include "include/GL/glut.h"

// just use andy thomason's vector and matrix classes
// think they need math
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "include/file_manager.h"
#include "include/texture_manager.h"
#include "include/vector.h"
#include "include/matrix.h"

// my shader program class
// .program returns a compiled & linked shader program
#include "include/shader.h"

ShaderProg shaderProg;
GLuint program;
GLuint texture_;
mat4 modelToProjection_;
mat4 cameraToWorld;
mat4 modelToWorld;
const int viewport_width_ = 640, viewport_height_ = 640;
char keys[256];

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
  float speed = 10.0f / 30;
  if (keys['f']) modelToWorld.translate(0.5f,0.0f,0.0f);
  if (keys['s']) modelToWorld.translate(-0.5f,0.0f,0.0f);
  if (keys['d']) modelToWorld.translate(0.0f,-0.5f,0.0f);
  if (keys['e']) modelToWorld.translate(0.0f,0.5f,0.0f);
}

void set_key(int key, int value) {
  keys[key & 0xff] = value;
}

void render()
{

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shaderProg.render();

  //glUseProgram(program);

  //vec4 color(1, 1, 0, 1);
  //vec4 light_direction(-1, 1, 1, 1);
  //shaderProg.setSomeStuff(color, light_direction);
  shaderProg.setMatrixStuff(modelToProjection_);
  
  glClearColor(0, 0, 0, 1);
  glViewport(0, 0, viewport_width_, viewport_height_);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  float vertices[3*3] = {
    -1, -1, 0,
    1, -1, 0,
    0,  1, 0,
  };

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);

  updateView();

  glDrawArrays(GL_TRIANGLES, 0, 3);
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

  texture_ = texture_manager::new_texture("assets/textures.tga", 0, 128, 128, 128);

  shaderProg.init();

  program = shaderProg.program();

  build_camera_matrix();

  //function for rendering
  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutKeyboardUpFunc(key_up);
  glutTimerFunc(30, timer, 1);

  //call main loop
  glutMainLoop(); //Start the main loop
  //display();
 
  /*/ Add or remove * between preceding // to toggle this block
  GLfloat color[] = {1, 1, 0, 1};
  GLuint colorIndex = glGetUniformLocation(program, "color");
  glUniform4fv(colorIndex, 1, color);
  //*/

}