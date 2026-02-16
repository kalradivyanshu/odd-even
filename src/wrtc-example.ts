import { WRTCHandshake } from "./wrtc";

// ══════════════════════════════════════════════════════════════
// Peer A (Offerer) - Creates the room
// ══════════════════════════════════════════════════════════════

async function peerA_createAndWait() {
  const handshake = new WRTCHandshake("offerer");

  // Set up message handler
  handshake.onMessage = (data) => {
    console.log("Received:", data);
  };

  // 1. Create a room and get the roomID
  const roomID = await handshake.createRoom();
  console.log("Room created! Share this ID:", roomID);

  // 2. Wait for the other peer to join (polls for answer)
  await handshake.waitForOpponent();
  console.log("Opponent joined!");

  // 3. Wait for the data channel to open
  await handshake.waitForChannel();
  console.log("Connection established!");

  // 4. Now you can send messages
  handshake.send("Hello from Peer A!");
}

// ══════════════════════════════════════════════════════════════
// Peer B (Answerer) - Joins the room
// ══════════════════════════════════════════════════════════════

async function peerB_join(roomID: string) {
  const handshake = new WRTCHandshake("answerer");

  // Set up message handler
  handshake.onMessage = (data) => {
    console.log("Received:", data);
  };

  // 1. Join the room with the roomID from Peer A
  await handshake.joinRoom(roomID);
  console.log("Joined room!");

  // 2. Wait for the data channel to open
  await handshake.waitForChannel();
  console.log("Connection established!");

  // 3. Now you can send messages
  handshake.send("Hello from Peer B!");
}

// ══════════════════════════════════════════════════════════════
// Example flow
// ══════════════════════════════════════════════════════════════

// On the first device:
// peerA_createAndWait();

// On the second device (after getting the roomID):
// peerB_join("the-room-id-from-peer-a");
