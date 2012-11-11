#define GLEW_STATIC
#include "include/GL/glew.h"
#define FREEGLUT_STATIC
#include "include/GL/glut.h"
#include "include/AL/al.h"
#include "include/AL/alc.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "include/vector.h"
#include "include/matrix.h"
#include "include/file_manager.h"
#include "include/texture_manager.h"
#include "include/sound_manager.h"
#include "include/structs.h"
#include "include/assets.h"
#include "include/render.h"
#include "include/gameobj.h"
#include "include/collision.h"
#include "include/spawner.h"

const int viewport_width_ = 800, viewport_height_ = 800;
int friendly_bullets_max_ = Spawn::get().getFriendlyBulletMax();
int enemy_bullets_max_ = Spawn::get().getEnemyBulletMax();
int enemy_max_ = Spawn::get().getEnemyMax();
int kills_ = 0;

char keys[256];

Renderer renderer; // could also be a singleton ...

Fighter player1;
Projectile * friendly_bullets_;
Projectile * enemy_bullets_;
Enemy * enemies_;

void collision_detection() {
  
  // for each friendly bullet, check hit on each enemy
  // for player fighter craft, check hit on each enemy and each enemy bullet

  for(int i = 0; i < friendly_bullets_max_; i++) {
    if(friendly_bullets_[i].isActive()) {
      for(int j = 0; j < enemy_max_; j++) {
        if(enemies_[j].isActive()) {
          if(Collision::checkHit(friendly_bullets_[i].getCollisionData(), enemies_[j].getCollisionData())) {
            // hit!
            Spawn::get().spawnSound(rand() % 3 + 2);
            enemies_[j].damage(friendly_bullets_[i].getDamage());
            friendly_bullets_[i].deactivate();
            if(!enemies_[j].isActive()) {
              // damage killed it
              // spawn explosion effect!! .. in the near future :/
              Spawn::get().spawnSound(Asset::SND_EXPLODE);
              kills_++;
            }
          }
        }
      }
    }
  }

  for(int i = 0; i < enemy_max_; i++) {
    if(enemies_[i].isActive()) {
      if(Collision::checkHit(enemies_[i].getCollisionData(), player1.getCollisionData())) {
        // hit!
        Spawn::get().spawnSound(Asset::SND_EXPLODE);
        enemies_[i].deactivate();
        player1.damage(enemies_[i].getHealth());
        if(!player1.isActive()) {
          //player died
          player1.die();
          Spawn::get().spawnPlayer1();
          player1 = Spawn::get().getPlayer1();
        }
      }
    }
  }

}

void simulate() {

  Spawn::get().cooldown();

  // spawn enemies ... much room for improvement
  // ..can spawn enemies with different speed, health, etc.
  if(!Spawn::get().getEnemyCooldown()) {
    float x = rand() % 18 - 9;
    float y = 10;
    int type = rand() % 3 + 1;
    Spawn::get().spawnEnemy(x, y, type);
    Spawn::get().setEnemyCooldown((int)(500 / (kills_ + 1)));
  }

  float speed = 10.0f / 30;
  if (keys['f']) player1.setThrust(0.02f,0.0f);
  if (keys['s']) player1.setThrust(-0.02f,0.0f);
  if (keys['d']) player1.setThrust(0.0f,-0.02f);
  if (keys['e']) player1.setThrust(0.0f,0.02f);
  if (keys['c']) {
    Spawn::get().spawnFriendlyBullet(player1);
  };

  player1.simulate();

  // simulate all friendly bullets
  for(int i = 0; i < friendly_bullets_max_; i++) {
    if(friendly_bullets_[i].isActive()) friendly_bullets_[i].simulate();
  }
  // simulate all enemy bullets
  for(int i = 0; i < enemy_bullets_max_; i++) {
    if(enemy_bullets_[i].isActive()) enemy_bullets_[i].simulate();
  }
  // simulate all enemies
  for(int i = 0; i < enemy_max_; i++) {
    if(enemies_[i].isActive()) enemies_[i].simulate();
  }

  collision_detection();
}

void set_key(int key, int value) {
  keys[key & 0xff] = value;
}

void render()
{
  renderer.preRender();

  // crappy implimentation .. to render all GameObjs
  for(int i = 0; i < friendly_bullets_max_; i++) {
    if(friendly_bullets_[i].isActive()) renderer.renderObject(friendly_bullets_[i].getRenderData(), Spawn::getBulletUV());
  }
  for(int i = 0; i < enemy_bullets_max_; i++) {
    if(enemy_bullets_[i].isActive()) renderer.renderObject(enemy_bullets_[i].getRenderData(), Spawn::getBulletUV());
  }
  for(int i = 0; i < enemy_max_; i++) {
    if(enemies_[i].isActive()) renderer.renderObject(enemies_[i].getRenderData(), Spawn::getCraftUV());
  }

  //render player
  renderer.renderObject(player1.getRenderData(), Spawn::getCraftUV());
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

  enemies_ = Spawn::get().getEnemies();
  enemy_bullets_ = Spawn::get().getEnemyBullets();
  friendly_bullets_ = Spawn::get().getFriendlyBullets();
  
  Spawn::get().spawnEnemy(0,10);
  Spawn::get().spawnEnemy(5,10);
    
  glutDisplayFunc(display);
  glutKeyboardFunc(key_down);
  glutKeyboardUpFunc(key_up);
  glutTimerFunc(10, timer, 1);

  //call main loop
  glutMainLoop(); //Start the main loop
}