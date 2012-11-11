// Spawns GameObjs, sounds, effects.. etc.
// Must know about sounds and textures
// ..and it's a Singleton .. just because it is

//global variable(s) ... it made sense at the time
float craft_vertices[] = {
  -0.3f, -0.3f, 0,
  0.3f, -0.3f, 0,
  0,  0.3f, 0
};

float bullet_vertices[3*6] = {
  -0.02f, 0.02f, 0,
  -0.02f, -0.02f, 0,
  0.02f, -0.02f, 0,
  0.02f, 0.02f, 0,
  0.02f, -0.02f, 0,
  -0.02f, 0.02f, 0
};

class Spawn {

  static const int max_enemy_bullets_ = 50;
  static const int max_friendly_bullets_ = 8;
  static const int max_enemies_ = 20;

  int friendly_bullet_cooldown_;

  Fighter player1_;
  Fighter player2_;
  Projectile enemy_bullets_[max_enemy_bullets_];
  Projectile friendly_bullets_[max_friendly_bullets_];
  Enemy enemies_[max_enemies_];
  GLuint textures_[3];

  // these prevent instancing .. somehow
  Spawn(){}; // this one because it's the constructor .. and it's private
  Spawn(Spawn const&); // these two because they're private .. and .. ?
  void operator=(Spawn const&);

public:

  static enum textures { TX_FIGHTER, TX_ENEMY, TX_BULLET }; // no more than textures_ array size
  static enum sounds { SND_NOTING, SND_SHOT, SND_RICO, SND_RICO2, SND_RICO3, SND_EXPLODE };

  void init() {
    friendly_bullet_cooldown_ = 0;

    // set up textures
    textures_[TX_FIGHTER] = texture_manager::new_texture("assets/texture.tga", 0, 1, 256, 180);
    textures_[TX_ENEMY] = texture_manager::new_texture("assets/texture.tga", 100, 100, 56, 80);
    textures_[TX_BULLET] = texture_manager::new_texture("assets/texture.tga", 2, 0, 1, 1);
   
    // set up sounds
    sound_manager::add_buffers("assets/q3sounds.txt", "assets/q3sounds.wav");
  }

  void spawnPlayer1() {
    player1_.init(0,-5,craft_vertices,3);
    player1_.setTexture(textures_[TX_FIGHTER]);
    player1_.setFriction(0.03f);
    player1_.setMaxSpeed(0.24f);
  }

  void spawnFriendlyBullet(Fighter player) {
    // enforce cooldown period
    if (friendly_bullet_cooldown_) { return; }
    // find first inactive
    int i;
    for (i = 0; i < max_friendly_bullets_; i++) {
      if(!friendly_bullets_[i].isActive()) break;
    } // will return last bullet if none are inactive

    if(!friendly_bullets_[i].isActive()) {
      // initialise new bullet
      friendly_bullets_[i].init(
        player.getCollisionData().center_x,
        player.getCollisionData().center_y,
        bullet_vertices, 6, 25
      );
      friendly_bullets_[i].setTexture(textures_[TX_BULLET]);
      friendly_bullets_[i].setMaxSpeed(0.2f);
      friendly_bullets_[i].setThrust(0.0f, 0.2f);
      // make sound :)
      spawnSound(SND_SHOT);
      // set cooldown
      friendly_bullet_cooldown_ = 10;
    }
  }

  void spawnEnemy(float x, float y) {
    int i;
    for (i = 0; i < max_enemies_; i++) {
      if(!enemies_[i].isActive()) break;
    } // will return last enemy if none are inactive

    if(!enemies_[i].isActive()) {
      enemies_[i].init(x, y, craft_vertices, 6);
      enemies_[i].setTexture(textures_[TX_ENEMY]);
      enemies_[i].setMaxSpeed(0.02f);
      enemies_[i].setThrust(0.0f, -0.02f);
    }
  }

  void spawnSound(int sound) {
    char name[2];
    name[0] = sound + '0';
    name[1] = 0;

    sound_manager::play(vec4(0, 0, 0, 0), name);
  }

  Fighter getPlayer1() {
    return player1_;
  }

  Projectile * getFriendlyBullets(){ return friendly_bullets_; }
  Projectile * getEnemyBullets(){ return enemy_bullets_; }
  Enemy * getEnemies(){ return enemies_; }
  int getFriendlyBulletMax(){ return max_friendly_bullets_; }
  int getEnemyBulletMax(){ return max_enemy_bullets_; }
  int getEnemyMax(){ return max_enemies_; }

  //GameObj * getGameObjects() {}

  void cooldown() {
    if(friendly_bullet_cooldown_) friendly_bullet_cooldown_--;
  }

  static Spawn& get()
  {
      static Spawn instance;
      return instance;
  }
};