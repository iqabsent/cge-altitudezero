// Spawns GameObjs, sounds, effects.. etc.
// Must know about sounds and textures
// ..and it's a Singleton .. just because it is

//global variable(s) ... it made sense at the time
float craft_vertices[3*6] = { -0.5f, 0.5f, 0, -0.5f, -0.5f, 0, 0.5f, 0.5f, 0, 0.5f, -0.5f, 0, -0.5f, -0.5f, 0, 0.5f, 0.5f, 0 };
float craft_uv[3*6] = { 0.0f, 1.0f, 0, 0.0f, 0.0f, 0, 1.0f, 1.0f, 0, 1.0f, 0.0f, 0, 0.0f, 0.0f, 0, 1.0f, 1.0f, 0 };
float enemy_craft_vertices[3*6] = { -0.6f, 0.6f, 0, -0.6f, -0.6f, 0,  0.6f, 0.5f, 0, 0.6f, -0.6f, 0, -0.6f, -0.6f, 0, 0.6f, 0.6f, 0 };
float enemy_craft_uv[3*6] = { 0.0f, 1.2f, 0, 0.0f, 0.0f, 0, 1.2f, 1.2f, 0, 1.2f, 0.0f, 0, 0.0f, 0.0f, 0, 1.2f, 1.2f, 0 };
float bullet_vertices[3*6] = { -0.06f, 0.08f, 0, -0.06f, -0.08f, 0, 0.06f, -0.08f, 0, 0.06f, 0.08f, 0, 0.06f, -0.08f, 0, -0.06f, 0.08f, 0 };
float bullet_uv[3*6] = { 0.0f, 0.15f, 0, 0.0f, 0.15f, 0, 0.15f, 0.0f, 0, 0.15f, 0.15f, 0, 0.15f, 0.0f, 0, 0.0f, 0.15f, 0 };

class Spawn {

  static const int max_enemy_bullets_ = 50;
  static const int max_friendly_bullets_ = 8;
  static const int max_enemies_ = 20;

  int friendly_bullet_cooldown_;
  int enemy_cooldown_;

  Fighter player1_;
  Fighter player2_;
  Projectile enemy_bullets_[max_enemy_bullets_];
  Projectile friendly_bullets_[max_friendly_bullets_];
  Enemy enemies_[max_enemies_];

  // these prevent instancing .. somehow
  Spawn(){}; // this one because it's the constructor .. and it's private
  Spawn(Spawn const&); // these two because they're private .. and .. ?
  void operator=(Spawn const&); // private overridden assignment operator

public:

  void init() {
    friendly_bullet_cooldown_ = 0;
    enemy_cooldown_ = 500;
  }

  void spawnPlayer1() {
    player1_.init(0, -5, craft_vertices, 6);
    player1_.setTexture(Asset::get().texture(Asset::TX_FIGHTER));
    player1_.setFriction(0.01f);
    player1_.setMaxVelocity(0.20f);
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
      friendly_bullets_[i].setTexture(Asset::get().texture(Asset::TX_BULLET));
      friendly_bullets_[i].setMaxVelocity(0.2f);
      friendly_bullets_[i].setThrust(0.0f, 0.2f);
      // make sound :)
      spawnSound(Asset::SND_SHOT);
      // set cooldown
      friendly_bullet_cooldown_ = 10;
    }
  }

  void spawnEnemy(float x, float y) {
    int i;
    for (i = 0; i < max_enemies_; i++) {
      if(!enemies_[i].isActive()) break;
    }

    if(!enemies_[i].isActive()) {
      enemies_[i].init(x, y, craft_vertices, 6);
      enemies_[i].setTexture(Asset::get().texture(Asset::TX_ENEMY));
      enemies_[i].setMaxVelocity(0.02f);
      enemies_[i].setThrust(0.0f, -0.02f);
    }
  }

  // hacked to spawn different variables based on type. can handle this better..
  void spawnEnemy(float x, float y, int type) {
    int i;
    for (i = 0; i < max_enemies_; i++) {
      if(!enemies_[i].isActive()) break;
    }

    if(!enemies_[i].isActive()) {
      enemies_[i].init(x, y, craft_vertices, 6);
      enemies_[i].setTexture(Asset::get().texture(Asset::TX_ENEMY + type - 1));
      enemies_[i].setMaxVelocity(0.02f * type);
      enemies_[i].setThrust(0.0f, -0.02f * type);
      enemies_[i].setHealth(100 / (type + 1));
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

  // somehow return all GameObjs as single array
  // would be useful for rendering and simulating all Objs in one go
  //GameObj * getGameObjects() {} // ... ?

  void cooldown() {
    if(friendly_bullet_cooldown_) friendly_bullet_cooldown_--;
    if(enemy_cooldown_) enemy_cooldown_--;
  }

  void setEnemyCooldown(int cooldown) {
    enemy_cooldown_ = cooldown;
  }

  int getEnemyCooldown() {
    return enemy_cooldown_;
  }

  static float * getBulletUV() {
    return bullet_uv;
  }
  static float * getCraftUV() {
    return craft_uv;
  }

  static Spawn& get()
  {
      static Spawn instance;
      return instance;
  }
};