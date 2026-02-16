import Worker from './worker.ts?worker';
import { WRTCHandshake } from './wrtc.ts';

// window.onload = async function () {
//   const canvas = document.getElementById("gridCanvas") as HTMLCanvasElement;
//   const ctx = canvas.transferControlToOffscreen();
//   const width = canvas.width;
//   const height = canvas.height;

//   const worker = new Worker();
//   const messageChannel = new MessageChannel();
//   worker.postMessage({ type: "initialize", ctx: ctx, width, height, port: messageChannel.port2 }, [ctx, messageChannel.port2]);

//   canvas.onmousemove = (event) => {
//     worker.postMessage({ type: "mousemove", x: event.clientX, y: event.clientY, rect: canvas.getBoundingClientRect() });
//   }

//   canvas.onclick = (event) => {
//     worker.postMessage({ type: "click", x: event.clientX, y: event.clientY, rect: canvas.getBoundingClientRect() });
//   }

//   messageChannel.port1.onmessage = (event) => {
//     switch(event.data.type) {
//       case "fps":
//         document.getElementById("averageFps")!.innerHTML = `Average FPS: ${event.data.fps.toFixed(2)}. Bullets shot: ${event.data.bullets}`;
//         break;
//     }
//   };
// }

window.onload = async function () {
  //get parameters from url
  const url = new URL(window.location.href);
  const params = new URLSearchParams(url.search);
  const isOfferer = params.get("offerer");

  const rtc = new WRTCHandshake(isOfferer ? "offerer" : "answerer");
  (window as any).rtc = rtc;

  if (isOfferer) {
    const roomID = await rtc.createRoom();
    console.log(JSON.stringify(roomID));

    await rtc.waitForOpponent(roomID);
    await rtc.waitForChannel();

    console.log("Opponent found.");
    
    rtc.send("Hello, world!");
  } else {
    const roomID = params.get("roomID")!;
    console.log("Got room", roomID);
    rtc.onMessage = (data) => {
      console.log("Received message:", data);
    };
    await rtc.joinRoom(roomID);
    console.log("Joined room.");
  }
  
}