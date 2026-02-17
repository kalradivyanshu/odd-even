import Worker from './worker.ts?worker';
import { WRTCHandshake } from './wrtc.ts';
import { Deferred } from './deferred.ts';

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

  const worker = new Worker();
  const messageChannel = new MessageChannel();

  const dc_ = await dc.async();
  const dcControlChannel_ = await dcControlChannel.async();

  if (isOfferer) {
    worker.postMessage({ type: "initialize", ctx: ctx, width, height, port: messageChannel.port2, data_channel: dc_, data_channel_control: dcControlChannel_ }, [ctx, messageChannel.port2, dc_, dcControlChannel_]);
  } else {
    worker.postMessage({ type: "remote_initialize", ctx: ctx, width, height, port: messageChannel.port2, data_channel: dc_, data_channel_control: dcControlChannel_ }, [ctx, messageChannel.port2, dc_, dcControlChannel_]);
  }

  canvas.onmousemove = (event) => {
    worker.postMessage({ type: "mousemove", x: event.clientX, y: event.clientY, rect: canvas.getBoundingClientRect() });
  }

  canvas.onclick = (event) => {
    worker.postMessage({ type: "click", x: event.clientX, y: event.clientY, rect: canvas.getBoundingClientRect() });
  }

  messageChannel.port1.onmessage = (event) => {
    switch (event.data.type) {
      case "fps":
        document.getElementById("averageFps")!.innerHTML = `Average FPS: ${event.data.fps.toFixed(2)}. Bullets shot: ${event.data.bullets}`;
        break;

      case "network":
        document.getElementById("averageFps")!.innerHTML = `Average bandwidth: ${event.data.bandwidth.toFixed(2)} Kb/s`;
        break;
    }
  };
}