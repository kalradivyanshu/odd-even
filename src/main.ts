import InitWasm from "./cc/compiled/wasm_core";

function coordinate(event: any, rect: DOMRect) {
  let x = event.clientX;
  let y = event.clientY;
  console.log("po!!", { x: x - rect.left, y: y - rect.top });
}

// Get the canvas element and its context
const canvas = document.getElementById("gridCanvas") as HTMLCanvasElement;
let rect = canvas.getBoundingClientRect();

canvas.onmousemove = (event) => coordinate(event, rect);

window.onload = async function () {
  let wasm = await InitWasm();
  let world = wasm.new_world();

  const ctx = canvas.getContext("2d")!;

  // paint the entire canvas black
  ctx.fillStyle = "black";
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Define the size of each cell
  const cellSize = 10;

  // Set the fill style to white for the squares
  ctx.fillStyle = "white";
  setInterval(() => {
    wasm.tick(world);
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "white";
    let graphics = wasm.get_graphics(world);
    let arr = new Uint8Array(wasm.HEAPU8.buffer, graphics, 80 * 80);
    for (let x = 0; x < canvas.width; x += cellSize) {
      for (let y = 0; y < canvas.height; y += cellSize) {
        let index = x / cellSize + (y / cellSize) * 80;
        if (index >= 6400) {
          throw new Error("Index out of bounds");
        }
        if (arr[index] == 0) {
          ctx.fillStyle = "black";
        }
        if (arr[index] == 1) {
          ctx.fillStyle = "gray";
        }
        if (arr[index] == 2) {
          ctx.fillStyle = "yellowgreen";
        }
        if (arr[index] == 3) {
          ctx.fillStyle = "orange";
        }
        if (arr[index] == 4) {
          ctx.fillStyle = "red";
        }
        if (arr[index] != 0) {
          ctx.fillRect(x, y, cellSize, cellSize);
        }
      }
    }
  }, 100);
  // Draw the grid
};
