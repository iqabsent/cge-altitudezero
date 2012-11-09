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
  float * vertices;
};

// my shader program class
// .program returns a compiled & linked shader program
#include "include/shader.h"
#include "include/gameobj.h"

Renderer renderer;

GLuint fighter_texture;
GLuint bullet_texture;

const int viewport_width_ = 800, viewport_height_ = 800;
char keys[256];

Projectile bullet;
Fighter fighter;

void simulate() {

  //move this elsewhere..
  char name[2];
  name[0] = 1 + '0';
  name[1] = 0;

  float speed = 10.0f / 30;
  if (keys['f']) fighter.setThrust(0.06f,0.0f);
  if (keys['s']) fighter.setThrust(-0.06f,0.0f);
  if (keys['d']) fighter.setThrust(0.0f,-0.06f);
  if (keys['e']) fighter.setThrust(0.0f,0.06f);
  if (keys['c']) sound_manager::play(vec4(0, 0, 0, 0), name);

  fighter.simulate();
  bullet.simulate();
}

void set_key(int key, int value) {
  keys[key & 0xff] = value;
}

void render()
{
  renderer.preRender();

  renderer.renderObject(fighter.renderData());
  if(bullet.active()) renderer.renderObject(bullet.renderData());
}

static void display() { simulate(); render(); }
static void key_down( unsigned char key, int x, int y) { set_key(key, 1); }
static void key_up( unsigned char key, int x, int y) { set_key(key, 0); }
static void timer(int value) { glutTimerFunc(10, timer, 1); glutPostRedisplay(); }

void main(int argc, char** argv) {

  glutInit(&argc, argv);
  glutInitWindowSize(viewport_width_, viewport_height_);
  glutCreateWindow("AltitudeZero - Scroller game..");
  glewInit();

  sound_manager::add_buffers("assets/labels.txt", "assets/poing.wav");

  fighter_texture = texture_manager::new_texture("assets/texture.tga", 0, 1, 256, 180);
  bullet_texture = texture_manager::new_texture("assets/texture.tga", 2, 0, 1, 1);

  renderer.init();

  float bullet_vertices[3*6] = {
    -0.1f, 0.1f, 0,
    -0.1f, -0.1f, 0,
    0.1f, -0.1f, 0,
    0.1f, 0.1f, 0,
    0.1f, -0.1f, 0,
    -0.1f, 0.1f, 0
  };

  bullet.init(1,1,bullet_vertices,6);
  bullet.setTexture(bullet_texture);
  bullet.setThrust(0.0f, 0.2f);

  float fighter_vertices[3*3] = {
    -1, -1, 0,
    1, -1, 0,
    0,  1, 0
  };

  fighter.init(0,0,fighter_vertices,3);
  fighter.setTexture(fighter_texture);
  fighter.setFriction(0.03f);
  fighter.setMax(0.24f);

  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutKeyboardUpFunc(key_up);
  glutTimerFunc(10, timer, 1);

  //call main loop
  glutMainLoop(); //Start the main loop
}