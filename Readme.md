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

Now I hear you say, ok fine, go low level for some reason, pick Bevy or the thousand other RUST/GO/C++ game engines that target WASM. Here is where you are wrong again. That is what a smart person in the room would do. You are forgetting, I am the smartest. To hell with you and your speed to market. We will write this is C++, and from SCRATCH (🎶dun dun daaa🎶).

## The basics
