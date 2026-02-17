import InitWasm from "./cc/compiled/wasm_core";
import { wasm_ty } from "./cc/compiled/wasm_types";

// Define the size of each cell
const cellSize = 5;

function coordinate(x: number, y: number, rect: {left: number, top: number}) {
  //@ts-ignore
  let wasm: wasm_ty = self.wasm;
  //@ts-ignore
  let world: number = self.world;
   
  wasm.setup_spring_center(world, (x - rect.left) / cellSize, (y - rect.top) / cellSize);
}

let bullets = 0;

async function shoot(x: number, y: number, rect: {left: number, top: number}) {
  for(let i = 0; i < 10; i++) {
    
    //@ts-ignore
    let wasm: wasm_ty = self.wasm;
    //@ts-ignore
    let world: number = self.world;

    wasm.shoot(world, (x - rect.left) / 10, (y - rect.top) / 10);
    await new Promise(resolve => setTimeout(resolve, 200));
    bullets++;
  }
}



// static Color from_color_u8(uint8_t color) {
//   return Color((color & 0b00110000) << 4, (color & 0b00001100) << 2, color & 0b00000011);
// }

function from_color_u8(color: number): string {
  let r = (color & 0b00110000) >> 4;
  let g = (color & 0b00001100) >> 2;
  let b = color & 0b00000011;
  return `rgb(${r * 85}, ${g * 85}, ${b * 85})`;
}


const initialize = async function (ctx: OffscreenCanvasRenderingContext2D, width: number, height: number) {
  let wasm = await InitWasm();
  let world = wasm.new_world();

  //@ts-ignore
  self.world = world;
  //@ts-ignore
  self.wasm = wasm;

  const worldSize = wasm.get_world_size();



  // paint the entire canvas black
  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, width, height);

  // Set the fill style to white for the squares
  ctx.fillStyle = "white";
  setInterval(() => {
    //@ts-ignore
    self.message_port.postMessage({ type: "fps", fps: wasm.get_average_fps(world), bullets: bullets });
  }, 1000);

  setInterval(() => {
    wasm.tick(world);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, width, height);
    ctx.fillStyle = "white";
    let graphics = wasm.get_graphics(world);
    let arr = new Uint8Array(wasm.HEAPU8.buffer, graphics, worldSize * worldSize);
    for (let x = 0; x < width; x += cellSize) {
      for (let y = 0; y < height; y += cellSize) {
        let index = x / cellSize + (y / cellSize) * worldSize;
        if (index >= worldSize * worldSize) {
          throw new Error("Index out of bounds");
        }
        if (arr[index] == 0) {
          ctx.fillStyle = "black";
        } else {
          ctx.fillStyle = from_color_u8(arr[index]);
        }
        if (arr[index] != 0) {
          ctx.fillRect(x, y, cellSize, cellSize);
        }
      }
    }
    const compressed_size = wasm.compress_graphics(world);
    const compressed_graphics = wasm.get_compressed_graphics(world);
    const compressed_arr = new Uint8Array(wasm.HEAPU8.buffer, compressed_graphics, compressed_size);
    (self as any).data_channel.send(compressed_arr);
  }, 33);
  // Draw the grid
};

const remote_initialize = async function (ctx: OffscreenCanvasRenderingContext2D, width: number, height: number, data_channel: RTCDataChannel) {
  let wasm = await InitWasm();
  let world = wasm.new_world();

  //@ts-ignore
  self.world = world;
  //@ts-ignore
  self.wasm = wasm;

  const worldSize = wasm.get_world_size();
  // paint the entire canvas black
  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, width, height);

  // Set the fill style to white for the squares
  ctx.fillStyle = "white";

  let total_bytes_received = 0;
  let last_time = Date.now();
  let average_bandwidth = 0;

  setInterval(() => {
    const current_time = Date.now();
    const time_diff = (current_time - last_time) / 1000;
    last_time = current_time;
    const current_bandwidth = total_bytes_received * 8 / (time_diff * 1000);
    average_bandwidth = (average_bandwidth * 0.9) + (current_bandwidth * 0.1);
    (self as any).message_port.postMessage({ type: "network", bandwidth: average_bandwidth });
    total_bytes_received = 0;

  }, 1000);

  data_channel.onmessage = (event) => {
    const compressed_ptr = wasm.get_compressed_graphics(world);
    const compressed_arr = new Uint8Array(event.data);
    wasm.HEAPU8.set(compressed_arr, compressed_ptr);
    wasm.decompress_graphics(world, event.data.byteLength);
    let graphics = wasm.get_graphics(world);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, width, height);
    ctx.fillStyle = "white";
    let arr = new Uint8Array(wasm.HEAPU8.buffer, graphics, worldSize * worldSize);
    for (let x = 0; x < width; x += cellSize) {
      for (let y = 0; y < height; y += cellSize) {
        let index = x / cellSize + (y / cellSize) * worldSize;
        if (index >= worldSize * worldSize) {
          throw new Error("Index out of bounds");
        }
        if (arr[index] == 0) {
          ctx.fillStyle = "black";
        } else {
          ctx.fillStyle = from_color_u8(arr[index]);
        }
        if (arr[index] != 0) {
          ctx.fillRect(x, y, cellSize, cellSize);
        }
      }
    }
    total_bytes_received += event.data.byteLength;
  }
};

self.onmessage = async (event) => {
    switch(event.data.type) {
        case "initialize":
            const canvas: OffscreenCanvas = event.data.ctx;
            const ctx = canvas.getContext("2d")!;
            //@ts-ignore
            self.message_port = event.data.port;
            //@ts-ignore
            self.data_channel = event.data.data_channel;
            await new Promise<void>((resolve) => {
                (self as any).data_channel!.onopen = () => {
                  resolve();
                };
            });
            initialize(ctx, event.data.width, event.data.height);
            break;
        case "remote_initialize":
            const data_channel = event.data.data_channel;
            const canvas_: OffscreenCanvas = event.data.ctx;
            const ctx_ = canvas_.getContext("2d")!;
            //@ts-ignore
            self.message_port = event.data.port;
            await new Promise<void>((resolve) => {
                data_channel!.onopen = () => {
                  resolve();
                };
            });
            remote_initialize(ctx_, event.data.width, event.data.height, data_channel);
            break;
        case "mousemove":
            coordinate(event.data.x, event.data.y, event.data.rect);
            break;
        case "click":
            shoot(event.data.x, event.data.y, event.data.rect);
            break;
    }


}