class Collision {

  // FOR REFERENCE
  //struct CollisionData {
  //  float center_offset_x, center_offset_y, center_x, center_y, width, height, radius;
  //};

public:
  Collision(){};

  void init() {
  }

  static bool checkHit(CollisionData hitting, CollisionData being_hit) {
    // trigger hit funciton on being_hit ... ?

    // very basic
    // - only works in 2D
    // - only to radius precision
    // .. can be improved to bounding-box precision
    // .. can be improved to check texture at picked point for pixel colour
    // * but that would be silly, just use bullet instead 0_0

    bool hit_possible = false;
    float distance, delta_x, delta_y = 0;

    delta_x = hitting.center_x - being_hit.center_x;
    delta_y = hitting.center_y - being_hit.center_y;

    distance = sqrt(delta_x*delta_x + delta_y*delta_y);

    hit_possible = distance < hitting.radius + being_hit.radius;
    // hit possible, check bounding box
    // for now assume hit .. no time :/

    return hit_possible;
  }
};