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

## Collisions

### Detections

The interesting thing about this setup is that updates to position can happen with mutliple steps within a tick. This means: P and Q can move like this: from p1 to p2 and q1 to q2.

![alt text](brags/collision.png)

So if we just compare exact cell positions to detect collisions, we will never detect any b/w P and Q. So we need to improvise. Thankfully what we can do is make a list of position before and after the tick and use line segments between old and new position and see if any intersect! Is there a better way? Probably, but I won't google it, ofc mine is the best solution. What do you mean arrogance?

Some basic algebra is needed to detect the intersection of the two update vectors:

#### Two point formula of a line:

Given a line going through $(x_1, y_1)$ and $(x_2, y_2)$ the two point formula of a line is:
$\frac{(y - y_1)}{(x - x_1)} = \frac{(y_2 - y_1)}{(x_2 - x_1)}$ so $$\frac{(y - y_1)}{(x - x_1)} - \frac{(y_2 - y_1)}{(x_2 - x_1)} = 0$$ (basically the slope b/w the two points will always be equal to the slope b/w any other two points).

#### Do two points lie on the same side of a line?

Given two arbitary points $(x_a, y_a)$ and $(x_b, y_b)$ they lie on the same side of the line if and only if:
$$\frac{(y_a - y_1)}{(x_a - x_1)} - \frac{(y_2 - y_1)}{(x_2 - x_1)},  \frac{(y_b - y_1)}{(x_b - x_1)} - \frac{(y_2 - y_1)}{(x_2 - x_1)}$$
have the same sign.

So now we just need to check if the old and new position of the entity we are checking lies on the same side of the old and new position entity we are checking against.

#### Then god said, f\*\*\* u, have an edge case.

I tried making it work for this:

![alt text](/brags/physics.gif)

and it always returns collided. Why is that, lets revisit the line thingy if one of the objects is static:

![alt text](brags/collision_edge.png)

In all three cases, p is on the line that connects $q_1$ and $q_2$, so we actually need four cases:

1. If $p$ and $q$ are both moving but along the same line
2. If $p$ and $q$ are both moving but along different lines
3. If one of $p$ and $q$ is moving, one is static.
4. If both $p$ and $q$ are static.

In 2 our method works, for 1 it has collided if both one of the position points are b/w the position points of the other one. For 3 it has collided if the static one lies on the same line and it is b/w the position updates of the one moving. For 4, only collided if both positions are the same.

And, well it works (atleast for case 3, remember kids, its not testing in production, its distributed and decentralised edge testing.)

![collision working*](brags/collision.gif)

### Elastic Collisions

We will assume that all collisions are perfectly elastic. So that simplifies things, not that I can't figure out dampnings and whatnots, ofc I can, but I rather not.

During elastic collisions 2 things are conserved: total momentum and total kinetic energy. We can just google the final equation lol. So when 2 items $a$, $b$ collide with a velocity of $v_a$ and $v_b$ the new velocities $v_a^1$ and $v_b^1$ are:

$$
v_a^1 = \frac{m_a - m_b}{m_a + m_b}*v_a + \frac{2m_b}{m_a + m_b}*v_b
$$

$$
v_b^1 = \frac{m_a - m_b}{m_a + m_b}*v_b + \frac{2m_a}{m_a + m_b}*v_a
$$

And easy peezy:
![collision bug*](brags/elastic_bug.gif)

Wait, thats not right.

After scratching my head, I realised that my ticking function is broken, I detect collision and change the velocity after the objects have gone through each other, then they go back, and collide again, getting stuck in a loop. I need to break ticking into proposed position update and position updates, then before they collide I change their velocity. Over-engineered? WHAT EVEN DO YOU MEAN? I will die before I learn better way to do things than the first instinct I have, good day to you sir!

Ok, I patched it up by keeping a map of all the collisions, and ignoring collisions that happen b/w two objects that collided within the last 2 ticks. Will this create edge case? Ofcourse, but that is future me's problem, and I really don't care about that guy. Screw him, it works! Yay!

![collision fix](brags/ellastic_fix.gif)

## Problems

As any one who has ever worked with me, or if you read the above section and have 2 working brain cell, can guess, I made this way too complicated. And it doesn't work. AT ALL.

![collision broken](brags/ellastic_broke.gif)
Like wtf is even this? This is so broken its not even funny.

I procastinated fixing this for 4 days, and then today realised that I have been missing something fundamental. Time! (somewhere christopher nolan has suddenly started paying attention.)

See in the tick loop:

```c++
void compute_position() {
  this->last_position.x = this->position.x;
  this->last_position.y = this->position.y;

  this->acceleration.x = this->force.x / this->mass;
  this->acceleration.y = this->force.y / this->mass;
  this->velocity.x += this->acceleration.x;
  this->velocity.y += this->acceleration.y;
  this->position.x += this->velocity.x;
  this->position.y += this->velocity.y;
  this->bound_collisions();
}
```

This just assumes that one tick means the exact same everytime, and also that one tick is the exact unit of the velocity and force values. But that is not true. If you have ever used JavaScript you know no timer in JS is accurate. Ever. If the user keeps your tab in the background and their device is not connected to a charger, the 30ms `setInterval` will bloat to few seconds in the worse case.

So how do we fix this and the collision overengineering?

1. A tick must be 1ms always, if tick is called after 45ms, we will internally call tick 45 times.
2. In one tick, a box must not move more than 1 cell, hence velocity has a upper cap of 1000 cell/sec.

Since we have 2, we can delete the BS code of checking if the vectors overlap, and just say collided if the two boxes are in the same cell!

And deleting 100 lines later:
![collision fixed finally!](brags/elastic_fix_final.gif)

## Player

The player will follow the mouse, as a spring as defined before, but to ensure the player doesn't keep oscillating we need damping! It seems straight forward, damping is just a force propotional to the velocity of the object, but in the opposite direction. `auto damping_force = this->get_velocity() * -1. * damping;` and done!

![player motion](brags/player.gif)

## Bullet

When the player clicks, a bullet needs to be fired in the direction of the click.

![bullet triangle](brags/bullet_triangle.png)

If the user clicks at C, and the player is at P, the bullet should have a velocity in the direction of $C - p$. But if we just set the velocity of bullet to $C - p$, the further the player is from the click, the faster the bullet, we don't want that.

Our friend polar coordinate come into play:

![polar coordinate](brags/polar_cartesian.png)

So:

$$v_y = r * sin(\beta) = r * \frac{(C_y - p_y)}{H}$$

$$v_x = r * cos(\beta) = r * \frac{(C_x - p_x)}{H}$$

$$H = \sqrt([C_y - p_y]^2 + [C_x - p_x]^2)$$

And voila!

![bullets](brags/bullets.gif)
