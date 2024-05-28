# Odd-even

This is an over-engineered peice of art. Its a simple browser based 2 player game that has the following rules.

## Rules

1. If you are orange you are in team even, blue is team odd.
2. Move your player's dot by moving your cursor.
3. There will be pickups on screen that you can go to and get 1 bullet from them. You can shoot them immediately, or hold them for as long as you want.
4. To shoot a bullet, click. Once shot the bullet will bounce around forever unless they hit a player.
5. If the screen has odd number of bullets, they can all harm the even player, the odd player is invincible. If the bullets are even, the opposite.
6. The probability of the pickup appearing near you is inversely propotional to how many bullets you hold.
7. Once each player holds more than 5 bullets, pickups stop spawning.

## And then god said, let there be WASM!

Now at this point you must be wondering, this is simple, pick one of the million 2D js physics frameworks, and go to town, it can be done in a day! Thats where you were wrong. You expected me not to have a need at an atomic level to prove myself as the smartest person in the room. And as the smartest person in the room, I know JS is too high level.

For the un-initiated, WASM is a binary instruction format for a stack-based virtual machine.Ofcourse I totally understand what that means, but in a nutshell, it lets you call C++ from JS and flex on frontend plebs.

Now I hear you say, ok fine, go low level for some reason, pick Bevy or the thousand other RUST/GO/C++ game engines that target WASM. Here is where you are wrong again. That is what a smart person in the room would do. You are forgetting, I am the smartest. To hell with you and your speed to market. We will write this is C++, and from SCRATCH (🎶dun dun daaa🎶).

## Some more rules

1. For simplicity the playing feild will be 800px by 800px
2. It will be divided into cells of 10px by 10px (so 80 cells by 80 cells)
3. Each object will be exactly one pixel, ofcourse I can figure out the complex physics of multi-cell rigid body, but I dont want to 😤

# Physics

## Basic Movement

The basic movement in a C++ class called `physics::Entity`. It has the following function to update the velocity vector, acc vector, position vector based on force:

```C++
  void compute_position() {
    this->acceleration.x = this->force.x / this->mass;
    this->acceleration.y = this->force.y / this->mass;
    this->velocity.x += this->acceleration.x;
    this->velocity.y += this->acceleration.y;
    this->position.x += this->velocity.x;
    this->position.y += this->velocity.y;
  }
```

Every n ms every entities compute position is called, and their position is update! n is dependent on JS, which calls the C++ function responsible for updating all the entities positions. Every time the positions are updated its called a `tick`.

### A Spring!

The player entity will follow the cursor of the user. But I don't want the user to be able to stand in one place and avoid all the bullets. So to create chaos, the player entity will follow a spring motion around the cursor. The spring is interesting because:

1. Its smart! Only a smart person can figure out how to multiply a constant with a difference of 2 position vectors. And I can brag!
2. Its not as smart as a pendulum so it won't make me feel dumb. Winning. :)

Now spring is simple. Set a origin and at every `tick` update the entities force with: `k * (position - origin)`.

Some good old fashioned inheritence:

```C++
class Spring : public Entity {
  private:
  Vector origin;
  double k;

  public:
  Spring(Vector origin, double k) {
      this->origin = origin;
      this->k = k;
  }

  Vector get_force(Vector position) {
      Vector displacement = position - origin;
      return displacement * -k;
  }

  void tick() {
      update_force(get_force(get_position()));

      this->compute_position();
  }
};
```

So far, so good:
![spring demo](/brags/physics.gif)
