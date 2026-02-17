export type wasm_ty = {
  HEAPU8: Uint8Array;
  new_world: () => number;
  tick: (world_ptr: number) => void;
  get_graphics: (world_ptr: number) => number;
  move_p1: (world_ptr: number, x: number, y: number) => void;
  move_p2: (world_ptr: number, x: number, y: number) => void;
  shoot_p1: (world_ptr: number, x: number, y: number) => void;
  shoot_p2: (world_ptr: number, x: number, y: number) => void;
  get_average_fps: (world_ptr: number) => number;
  get_world_size: () => number;
  get_top_info_bar_height: () => number;
  get_compressed_graphics: (world_ptr: number) => number;
  compress_graphics: (world_ptr: number) => number;
  decompress_graphics: (world_ptr: number, compressed_size: number) => void;
};
