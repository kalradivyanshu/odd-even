export type wasm_ty = {
  HEAPU8: Uint8Array;
  new_world: () => number;
  tick: (world_ptr: number) => void;
  get_graphics: (world_ptr: number) => number;
  setup_spring_center: (world_ptr: number, x: number, y: number) => void;
  shoot: (world_ptr: number, x: number, y: number) => void;
  get_average_fps: (world_ptr: number) => number;
  get_world_size: () => number;
};
