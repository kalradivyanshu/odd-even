// uintptr_t new_world() {
//   auto world = new world::World();
//   auto spring = physics::Spring(physics::Vector(40, 40), 0.1);
//   world->add_entity(spring);
//   return (uintptr_t)(world);
// }

// void tick(uintptr_t world) {
//   ((world::World*)world)->tick();
// }

type wasm = {
  HEAPU8: Uint8Array;
  new_world: () => number;
  tick: (world_ptr: number) => void;
  get_graphics: (world_ptr: number) => number;
};

type init = () => Promise<wasm>;

declare const Init: init;

export default Init;
