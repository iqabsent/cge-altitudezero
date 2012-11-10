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

struct CollisionData {
  //GLuint texture; // for more precise collision detection .. much later .. maybe
  float width;
  float height;
  float radius;
};

// my shader program class
// compiles and links a standard shader for use in rendering RenderData
#include "include/render.h"
#include "include/gameobj.h"

Renderer renderer;

GLuint fighter_texture;
GLuint enemy_texture;
GLuint bullet_texture;

const int viewport_width_ = 800, viewport_height_ = 800;
char keys[256];

Projectile bullet;
Fighter fighter;
Enemy enemy;

void collision_detection() {
  // very basic
  // - only works on geometry centered on x=0, y=0 in its own model space
  // - only works in 2D
  // - only to radius precision
  // .. can be improved to bounding-box precision
  // .. can be improved to check texture at picked point for pixel colour
  // * but that would be silly, just use bullet instead 0_0

  CollisionData enemy_collision_data = enemy.getCollisionData();
  float enemy_xy[2] = {0,0};
  float bullet_xy[2] = {0,0};
  float distance = 0;
  float delta_x = 0;
  float delta_y = 0;

  enemy.xy((float *)&enemy_xy);
  bullet.xy((float *)&bullet_xy);

  delta_x = enemy_xy[0] - bullet_xy[0];
  delta_y = enemy_xy[1] - bullet_xy[1];

  distance = sqrt(delta_x*delta_x + delta_y*delta_y);

  if (distance < enemy_collision_data.radius) {
    // hit possible, check bounding box
    // for now assume hit .. no time :/
    enemy.damage(bullet.getDamage());
  }
}

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
  enemy.simulate();
  bullet.simulate();

  collision_detection();
}

void set_key(int key, int value) {
  keys[key & 0xff] = value;
}

void render()
{
  renderer.preRender();

  renderer.renderObject(fighter.getRenderData());
  if(enemy.isActive()) renderer.renderObject(enemy.getRenderData());
  if(bullet.isActive()) renderer.renderObject(bullet.getRenderData());
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

  sound_manager::add_buffers("assets/q3machg.txt", "assets/q3machg.wav");

  fighter_texture = texture_manager::new_texture("assets/texture.tga", 0, 1, 256, 180);
  enemy_texture = texture_manager::new_texture("assets/texture.tga", 100, 100, 56, 80);
  bullet_texture = texture_manager::new_texture("assets/texture.tga", 2, 0, 1, 1);

  renderer.init();

  float bullet_vertices[3*6] = {
    -0.05f, 0.05f, 0,
    -0.05f, -0.05f, 0,
    0.05f, -0.05f, 0,
    0.05f, 0.05f, 0,
    0.05f, -0.05f, 0,
    -0.05f, 0.05f, 0
  };

  bullet.init(0, 1, bullet_vertices, 6, 100);
  bullet.setTexture(bullet_texture);
  bullet.setThrust(0.0f, 0.2f);

  float craft_vertices[3*3] = {
    -0.3f, -0.3f, 0,
    0.3f, -0.3f, 0,
    0,  0.3f, 0
  };

  fighter.init(0,0,craft_vertices,3);
  fighter.setTexture(fighter_texture);
  fighter.setFriction(0.03f);
  fighter.setMaxSpeed(0.24f);

  enemy.init(0,10,craft_vertices,3);
  enemy.setTexture(enemy_texture);
  enemy.setThrust(0, -0.05f);

  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutKeyboardUpFunc(key_up);
  glutTimerFunc(10, timer, 1);

  //call main loop
  glutMainLoop(); //Start the main loop
}