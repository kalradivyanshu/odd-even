export type wasm = {
  HEAPU8: Uint8Array;
  new_world: () => number;
  tick: (world_ptr: number) => void;
  get_graphics: (world_ptr: number) => number;
  setup_spring_center: (world_ptr: number, x: number, y: number) => void;
  shoot: (world_ptr: number, x: number, y: number) => void;
  get_average_fps: (world_ptr: number) => number;
  get_world_size: () => number;
  get_top_info_bar_height: () => number;
  get_compressed_graphics: (world_ptr: number) => number;
  compress_graphics: (world_ptr: number) => number;
  decompress_graphics: (world_ptr: number, compressed_size: number) => void;
};

type init = () => Promise<wasm>;

declare const Init: init;

export default Init;
