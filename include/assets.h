// Holds static enums for asstes, sets up assets and contains texture refs

class Asset {

  bool init_;
  GLuint textures_[5];

  Asset(){};  
  Asset(Asset const&);
  void operator=(Asset const&);

public:
  static enum textures { TX_FIGHTER, TX_FIGHTER_LEFT, TX_FIGHTER_RIGHT, TX_ENEMY, TX_BULLET }; // no more than textures_[] size
  static enum sounds { SND_NOTING, SND_SHOT, SND_RICO, SND_RICO2, SND_RICO3, SND_EXPLODE };

  void init() {
    // set up textures
    textures_[TX_FIGHTER] = texture_manager::new_texture("assets/spaceship_sprites_by_arboris.tga", 41, 132, 40, 40);
    textures_[TX_FIGHTER_LEFT] = texture_manager::new_texture("assets/spaceship_sprites_by_arboris.tga", 1, 132, 40, 40);
    textures_[TX_FIGHTER_RIGHT] = texture_manager::new_texture("assets/spaceship_sprites_by_arboris.tga", 81, 132, 40, 40);
    textures_[TX_ENEMY] = texture_manager::new_texture("assets/enemy_sprite_by_skorpio.tga", 1, 1, 36, 36);
    textures_[TX_BULLET] = texture_manager::new_texture("assets/spaceship_sprites_by_arboris.tga", 4, 9, 6, 8);
   
    // set up sounds
    sound_manager::add_buffers("assets/q3sounds.txt", "assets/q3sounds.wav");

    init_ = true;
  }

  GLuint texture(int index) {
    if(!init_) init();
    return textures_[index];
  }

  static Asset& get()
  {
      static Asset instance;
      return instance;
  }
};