import Worker from './worker.ts?worker';

window.onload = async function () {
  const canvas = document.getElementById("gridCanvas") as HTMLCanvasElement;
  const ctx = canvas.transferControlToOffscreen();
  const width = canvas.width;
  const height = canvas.height;

  const worker = new Worker();
  const messageChannel = new MessageChannel();
  worker.postMessage({ type: "initialize", ctx: ctx, width, height, port: messageChannel.port2 }, [ctx, messageChannel.port2]);

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
    }
  };
}