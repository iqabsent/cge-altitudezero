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
  float center_offset_x, center_offset_y, center_x, center_y, width, height, radius;
};

#include "include/render.h"
#include "include/gameobj.h"
#include "include/collision.h"
#include "include/spawner.h"

Renderer renderer;

GLuint fighter_texture;
GLuint enemy_texture;
GLuint bullet_texture;

const int viewport_width_ = 800, viewport_height_ = 800;
char keys[256];

Projectile bullet;
Fighter player1;
Enemy enemy;

void collision_detection() {
  
  // for each friendly bullet, check hit on each enemy
  // for player fighter craft, check hit on each enemy and each enemy bullet

  Projectile * friendly_bullets = Spawn::get().getFriendlyBullets();
  Enemy * enemies = Spawn::get().getEnemies();

  int enemy_max = Spawn::get().getEnemyMax();
  int friendly_bullets_max = Spawn::get().getFriendlyBulletMax();

  for(int i = 0; i < friendly_bullets_max; i++) {
    if(friendly_bullets[i].isActive()) {
      for(int j = 0; j < enemy_max; j++) {
        if(enemies[j].isActive()) {
          if(Collision::checkHit(friendly_bullets[i].getCollisionData(), enemies[j].getCollisionData())) {
            // hit!
            enemies[j].damage(friendly_bullets[i].getDamage());
            Spawn::get().spawnSound(Spawn::sounds::SND_RICO);
            friendly_bullets[i].deactivate();
            if(!enemies[j].isActive()) {
              // damage killed it
              // spawn explosion effect!! .. in the near future :/
              Spawn::get().spawnSound(Spawn::sounds::SND_EXPLODE);
            }
          }
        }
      }
    }
  }
}

void simulate() {

  Spawn::get().cooldown();

  float speed = 10.0f / 30;
  if (keys['f']) player1.setThrust(0.06f,0.0f);
  if (keys['s']) player1.setThrust(-0.06f,0.0f);
  if (keys['d']) player1.setThrust(0.0f,-0.06f);
  if (keys['e']) player1.setThrust(0.0f,0.06f);
  if (keys['c']) {
    Spawn::get().spawnFriendlyBullet(player1);
  };

  player1.simulate();

  // simulate all friendly bullets
  Projectile * friendly_bullets = Spawn::get().getFriendlyBullets();
  int friendly_bullets_max = Spawn::get().getFriendlyBulletMax();
  for(int i = 0; i < friendly_bullets_max; i++) {
    if(friendly_bullets[i].isActive()) friendly_bullets[i].simulate();
  }

  // simulate all enemy bullets
  Projectile * enemy_bullets = Spawn::get().getEnemyBullets();
  int enemy_bullets_max = Spawn::get().getEnemyBulletMax();
  for(int i = 0; i < enemy_bullets_max; i++) {
    if(enemy_bullets[i].isActive()) enemy_bullets[i].simulate();
  }

  // simulate all enemies
  Enemy * enemies = Spawn::get().getEnemies();
  int enemies_max = Spawn::get().getEnemyMax();
  for(int i = 0; i < enemies_max; i++) {
    if(enemies[i].isActive()) enemies[i].simulate();
  }

  collision_detection();
}

void set_key(int key, int value) {
  keys[key & 0xff] = value;
}

void render()
{
  renderer.preRender();

  // crappy implimentation .. to render all friendly bullets
  Projectile * friendly_bullets = Spawn::get().getFriendlyBullets();
  int friendly_bullets_max = Spawn::get().getFriendlyBulletMax();
  for(int i = 0; i < friendly_bullets_max; i++) {
    if(friendly_bullets[i].isActive()) renderer.renderObject(friendly_bullets[i].getRenderData());
  }

  // .. and render all enemy bullets
  Projectile * enemy_bullets = Spawn::get().getEnemyBullets();
  int enemy_bullets_max = Spawn::get().getEnemyBulletMax();
  for(int i = 0; i < enemy_bullets_max; i++) {
    if(enemy_bullets[i].isActive()) renderer.renderObject(enemy_bullets[i].getRenderData());
  }

  // .. and all enemies
  Enemy * enemies = Spawn::get().getEnemies();
  int enemy_max = Spawn::get().getEnemyMax();
  for(int i = 0; i < enemy_max; i++) {
    if(enemies[i].isActive()) renderer.renderObject(enemies[i].getRenderData());
  }

  //render player
  renderer.renderObject(player1.getRenderData());

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

  renderer.init();
  Spawn::get().init();
  
  Spawn::get().spawnPlayer1();
  player1 = Spawn::get().getPlayer1();
  
  Spawn::get().spawnEnemy(0,10);
  Spawn::get().spawnEnemy(5,10);
    
  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutKeyboardUpFunc(key_up);
  glutTimerFunc(10, timer, 1);

  //call main loop
  glutMainLoop(); //Start the main loop
}