import Worker from './worker.ts?worker';
import { WRTCHandshake } from './wrtc.ts';

window.onload = async function () {
  const canvas = document.getElementById("gridCanvas") as HTMLCanvasElement;
  const ctx = canvas.transferControlToOffscreen();
  const width = canvas.width;
  const height = canvas.height;

  //get parameters from url
  const url = new URL(window.location.href);
  const params = new URLSearchParams(url.search);
  const roomID = params.get("roomID");
  const isOfferer = roomID === null;

  const rtc = new WRTCHandshake(isOfferer ? "offerer" : "answerer");
  (window as any).rtc = rtc;

  let dc: null | RTCDataChannel = null;

  if (isOfferer) {
    console.log("Creating room...");
    const roomID = await rtc.createRoom();
    console.log("Room created:", roomID);
    document.getElementById("roomID")!.innerHTML = `Room ID: ${roomID}. Link: ${window.location.href}?roomID=${roomID}`;
    await rtc.waitForOpponent(roomID);
    dc = rtc.pc.createDataChannel("main-channel");
  } else {
    await rtc.joinRoom(roomID!);
    dc = await new Promise<RTCDataChannel>((resolve) => {
      rtc.pc.ondatachannel = (e) => {
        if(e.channel.label === "main-channel")
          resolve(e.channel);
      };
    });
    console.log("Opponent found.");
  }

  const worker = new Worker();
  const messageChannel = new MessageChannel();
  

  if (isOfferer) {
    worker.postMessage({ type: "initialize", ctx: ctx, width, height, port: messageChannel.port2, data_channel: dc }, [ctx, messageChannel.port2, dc]);
  } else {
    worker.postMessage({ type: "remote_initialize", ctx: ctx, width, height, port: messageChannel.port2, data_channel: dc }, [ctx, messageChannel.port2, dc]);
  }

  canvas.onmousemove = (event) => {
    worker.postMessage({ type: "mousemove", x: event.clientX, y: event.clientY, rect: canvas.getBoundingClientRect() });
  }

  canvas.onclick = (event) => {
    worker.postMessage({ type: "click", x: event.clientX, y: event.clientY, rect: canvas.getBoundingClientRect() });
  }

  messageChannel.port1.onmessage = (event) => {
    switch(event.data.type) {
      case "fps":
        document.getElementById("averageFps")!.innerHTML = `Average FPS: ${event.data.fps.toFixed(2)}. Bullets shot: ${event.data.bullets}`;
        break;

      case "network":
        document.getElementById("averageFps")!.innerHTML = `Average bandwidth: ${event.data.bandwidth.toFixed(2)} Kb/s`;
        break;
    }
  };
}