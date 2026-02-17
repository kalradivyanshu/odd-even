import { WRTCHandshake } from './wrtc.ts';
import { Deferred } from './deferred.ts';
import InitWasm from "./cc/compiled/wasm_core";
import { wasm_ty } from "./cc/compiled/wasm_types";

const cellSize = 5;

function from_color_u8(color: number): string {
  let r = (color & 0b00110000) >> 4;
  let g = (color & 0b00001100) >> 2;
  let b = color & 0b00000011;
  return `rgb(${r * 85}, ${g * 85}, ${b * 85})`;
}

window.onload = async function () {
  const canvas = document.getElementById("gridCanvas") as HTMLCanvasElement;
  const ctx = canvas.getContext("2d")!;
  const width = canvas.width;
  const height = canvas.height;

  //get parameters from url
  const url = new URL(window.location.href);
  const params = new URLSearchParams(url.search);
  const roomID = params.get("roomID");
  const isOfferer = roomID === null;

  const rtc = new WRTCHandshake(isOfferer ? "offerer" : "answerer");
  (window as any).rtc = rtc;

  let dc = new Deferred<RTCDataChannel>();
  let dcControlChannel = new Deferred<RTCDataChannel>();

  if (isOfferer) {
    console.log("Creating room...");
    const roomID = await rtc.createRoom();
    console.log("Room created:", roomID);
    document.getElementById("roomID")!.innerHTML = `Room ID: ${roomID}. Link: ${window.location.href}?roomID=${roomID}`;
    await rtc.waitForOpponent(roomID);
    const dc_ = rtc.pc.createDataChannel("main-channel");
    const dcControlChannel_ = rtc.pc.createDataChannel("control-channel");
    dc.resolve(dc_);
    dcControlChannel.resolve(dcControlChannel_);
  } else {
    await rtc.joinRoom(roomID!);
    rtc.pc.ondatachannel = (e) => {
      if (e.channel.label === "main-channel")
        dc.resolve(e.channel);
      if (e.channel.label === "control-channel")
        dcControlChannel.resolve(e.channel);
    };
    await Promise.all([dc.async(), dcControlChannel.async()]);
    console.log("Opponent found.");
  }

  const dc_ = await dc.async();
  const dcControlChannel_ = await dcControlChannel.async();

  const is_remote = !isOfferer;
  let wasm: wasm_ty;
  let world: number;
  let bullets = 0;

  const move = (x: number, y: number, rect: DOMRect) => {
    if (!wasm) return;

    const topInfoBarHeight = wasm.get_top_info_bar_height();
    const worldX = (x - rect.left) / cellSize;
    const worldY = (y - rect.top) / cellSize - topInfoBarHeight;

    if (is_remote) {
      const message = JSON.stringify({ type: "move", x: worldX, y: worldY });
      dcControlChannel_.send(message);
      return;
    }

    wasm.move_p1(world, worldX, worldY);
  };

  const shoot = (x: number, y: number, rect: DOMRect) => {
    if (!wasm) return;

    const topInfoBarHeight = wasm.get_top_info_bar_height();
    const worldX = (x - rect.left) / cellSize;
    const worldY = (y - rect.top) / cellSize - topInfoBarHeight;

    if (is_remote) {
      const message = JSON.stringify({ type: "shoot", x: worldX, y: worldY });
      dcControlChannel_.send(message);
      return;
    }

    wasm.shoot_p1(world, worldX, worldY);
  };

  const listen_to_control_channel_master = (data_channel: RTCDataChannel) => {
    data_channel.onmessage = (event) => {
      const message = event.data;
      const json = JSON.parse(message);
      const type = json.type;

      if (!wasm) return;

      switch (type) {
        case "move":
          wasm.move_p2(world, json.x, json.y);
          break;
        case "shoot":
          wasm.shoot_p2(world, json.x, json.y);
          break;
      }
    };
  };

  const initialize = async () => {
    wasm = await InitWasm();
    world = wasm.new_world();

    const worldSize = wasm.get_world_size();
    const topInfoBarHeight = wasm.get_top_info_bar_height();

    // paint the entire canvas black
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, width, height);

    // Set the fill style to white for the squares
    ctx.fillStyle = "white";

    setInterval(() => {
      document.getElementById("averageFps")!.innerHTML = `Average FPS: ${wasm.get_average_fps(world).toFixed(2)}. Bullets shot: ${bullets}`;
    }, 1000);

    setInterval(() => {
      wasm.tick(world);
      ctx.fillStyle = "black";
      ctx.fillRect(0, 0, width, height);
      ctx.fillStyle = "white";
      let graphics = wasm.get_graphics(world);
      let arr = new Uint8Array(wasm.HEAPU8.buffer, graphics, (worldSize + topInfoBarHeight) * worldSize);
      for (let x = 0; x < width; x += cellSize) {
        for (let y = 0; y < height; y += cellSize) {
          let index = x / cellSize + (y / cellSize) * worldSize;
          if (index >= (worldSize + topInfoBarHeight) * worldSize) {
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
      //@ts-ignore
      dc_.send(compressed_arr);
    }, 33);
  };

  const remote_initialize = async () => {
    console.log("Remote initializing...");
    wasm = await InitWasm();
    world = wasm.new_world();

    const worldSize = wasm.get_world_size();
    const topInfoBarHeight = wasm.get_top_info_bar_height();

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
      document.getElementById("averageFps")!.innerHTML = `Average bandwidth: ${average_bandwidth.toFixed(2)} Kb/s`;
      total_bytes_received = 0;
    }, 1000);

    console.log("Listening to data channel...", dc_.label, dc_.readyState);
    dc_.onmessage = (event) => {
      const compressed_ptr = wasm.get_compressed_graphics(world);
      const compressed_arr = new Uint8Array(event.data);
      wasm.HEAPU8.set(compressed_arr, compressed_ptr);
      wasm.decompress_graphics(world, event.data.byteLength);
      let graphics = wasm.get_graphics(world);
      ctx.fillStyle = "black";
      ctx.fillRect(0, 0, width, height);
      ctx.fillStyle = "white";
      let arr = new Uint8Array(wasm.HEAPU8.buffer, graphics, (worldSize + topInfoBarHeight) * worldSize);
      for (let x = 0; x < width; x += cellSize) {
        for (let y = 0; y < height; y += cellSize) {
          let index = x / cellSize + (y / cellSize) * worldSize;
          if (index >= (worldSize + topInfoBarHeight) * worldSize) {
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
    };
  };

  // Wait for data channels to open
  await new Promise<void>((resolve) => {
    if (dc_.readyState === "open") {
      console.log("Data channel already open");
      resolve();
    } else {
      console.log("Waiting for data channel to open, current state:", dc_.readyState);
      dc_.onopen = () => {
        console.log("Data channel opened");
        resolve();
      };
    }
  });

  await new Promise<void>((resolve) => {
    if (dcControlChannel_.readyState === "open") {
      console.log("Control channel already open");
      resolve();
    } else {
      console.log("Waiting for control channel to open, current state:", dcControlChannel_.readyState);
      dcControlChannel_.onopen = () => {
        console.log("Control channel opened");
        resolve();
      };
    }
  });

  if (isOfferer) {
    await new Promise<void>((resolve) => {
      setTimeout(() => {
        resolve();
      }, 1000);
    });
    listen_to_control_channel_master(dcControlChannel_);
    await initialize();
  } else {
    await new Promise<void>((resolve) => {
      setTimeout(() => {
        resolve();
      }, 1000);
    });
    await remote_initialize();
  }

  canvas.onmousemove = (event) => {
    move(event.clientX, event.clientY, canvas.getBoundingClientRect());
  };

  canvas.onclick = (event) => {
    shoot(event.clientX, event.clientY, canvas.getBoundingClientRect());
  };
}