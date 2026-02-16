import InitWasm from "./cc/compiled/wasm_core";
import { wasm_ty } from "./cc/compiled/wasm_types";

// Define the size of each cell
const cellSize = 5;

function coordinate(event: any, rect: DOMRect) {
  let x = event.clientX;
  let y = event.clientY;
  //@ts-ignore
  let wasm: wasm_ty = window.wasm;
  //@ts-ignore
  let world: number = window.world;
   
  wasm.setup_spring_center(world, (x - rect.left) / cellSize, (y - rect.top) / cellSize);
}

let bullets = 0;

async function shoot(event: any, rect: DOMRect) {
  for(let i = 0; i < 10; i++) {
    let x = event.clientX;
    let y = event.clientY;
    
    //@ts-ignore
    let wasm: wasm_ty = window.wasm;
    //@ts-ignore
    let world: number = window.world;

    wasm.shoot(world, (x - rect.left) / 10, (y - rect.top) / 10);
    await new Promise(resolve => setTimeout(resolve, 200));
    bullets++;
  }
}

// Get the canvas element and its context
const canvas = document.getElementById("gridCanvas") as HTMLCanvasElement;
let rect = canvas.getBoundingClientRect();

canvas.onmousemove = (event) => coordinate(event, rect);
canvas.onclick = (event) => shoot(event, rect);

// static Color from_color_u8(uint8_t color) {
//   return Color((color & 0b00110000) << 4, (color & 0b00001100) << 2, color & 0b00000011);
// }

function from_color_u8(color: number): string {
  let r = (color & 0b00110000) >> 4;
  let g = (color & 0b00001100) >> 2;
  let b = color & 0b00000011;
  return `rgb(${r * 85}, ${g * 85}, ${b * 85})`;
}


window.onload = async function () {
  let wasm = await InitWasm();
  let world = wasm.new_world();

  //@ts-ignore
  window.world = world;
  //@ts-ignore
  window.wasm = wasm;

  const worldSize = wasm.get_world_size();


  const ctx = canvas.getContext("2d")!;

  // paint the entire canvas black
  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Set the fill style to white for the squares
  ctx.fillStyle = "white";
  setInterval(() => {
    let average_fps = wasm.get_average_fps(world);
    document.getElementById("averageFps")!.innerHTML = `Average FPS: ${average_fps.toFixed(2)}. Bullets shot: ${bullets}`;
  }, 1000);

  setInterval(() => {
    wasm.tick(world);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "white";
    let graphics = wasm.get_graphics(world);
    let arr = new Uint8Array(wasm.HEAPU8.buffer, graphics, worldSize * worldSize);
    for (let x = 0; x < canvas.width; x += cellSize) {
      for (let y = 0; y < canvas.height; y += cellSize) {
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
  }, 33);
  // Draw the grid
};
