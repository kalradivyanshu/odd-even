type Mode = "offerer" | "answerer";

type MessageHandler = (data: string | ArrayBuffer) => void;

const RTC_CONFIG: RTCConfiguration = {
  iceServers: [{ urls: "stun:stun.l.google.com:19302" }],
};

// Signaling server URL - update this with your deployed worker URL
const SIGNALING_SERVER = "https://d1-tutorial.kalradivyanshu.workers.dev"; // or your deployed URL

export class WRTCHandshake {
  readonly mode: Mode;
  readonly pc: RTCPeerConnection;
  private iceDone: Promise<void>;
  private dc: RTCDataChannel | null = null;
  private dcReady: Promise<RTCDataChannel>;
  private resolveDc!: (dc: RTCDataChannel) => void;
  private roomID: string | null = null;

  onMessage: MessageHandler = (data) => {
    console.log(`[${this.mode}] received:`, data);
  };

  constructor(mode: Mode) {
    this.mode = mode;
    this.pc = new RTCPeerConnection(RTC_CONFIG);

    this.dcReady = new Promise<RTCDataChannel>((resolve) => {
      this.resolveDc = resolve;
    });

    // Resolve once ICE gathering is complete
    this.iceDone = new Promise<void>((resolve) => {
      this.pc.onicecandidate = (e) => {
        if (e.candidate === null) resolve();
      };
    });

    // Answerer: wait for the remote data channel
    this.pc.ondatachannel = (e) => {
      this.setupChannel(e.channel);
    };
  }

  // ── Offerer side ──────────────────────────────────────────────

  /** Create an offer. Returns the full SDP (with ICE candidates gathered). */
  async createOffer(): Promise<RTCSessionDescriptionInit> {
    this.assertMode("offerer");

    // Create the data channel so the SDP includes the media section
    this.setupChannel(this.pc.createDataChannel("data"));

    const offer = await this.pc.createOffer();
    await this.pc.setLocalDescription(offer);
    await this.iceDone;

    return this.pc.localDescription!;
  }

  /** Accept the remote answer produced by the answerer. */
  async acceptAnswer(answer: RTCSessionDescriptionInit): Promise<void> {
    this.assertMode("offerer");
    await this.pc.setRemoteDescription(answer);
  }

  // ── Answerer side ─────────────────────────────────────────────

  /** Accept an offer and return the full answer SDP (with ICE candidates gathered). */
  async acceptOfferAndCreateAnswer(
    offer: RTCSessionDescriptionInit,
  ): Promise<RTCSessionDescriptionInit> {
    this.assertMode("answerer");

    await this.pc.setRemoteDescription(offer);
    const answer = await this.pc.createAnswer();
    await this.pc.setLocalDescription(answer);
    await this.iceDone;

    return this.pc.localDescription!;
  }

  // ── Signaling Server Integration ──────────────────────────────

  /**
   * Create a room on the signaling server with this peer's offer.
   * Returns the roomID to share with the other peer.
   * (Offerer side only)
   */
  async createRoom(): Promise<string> {
    this.assertMode("offerer");

    const offer = await this.createOffer();

    const response = await fetch(`${SIGNALING_SERVER}/offer`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ offer }),
    });

    if (!response.ok) {
      throw new Error(`Failed to create room: ${response.statusText}`);
    }

    const data = await response.json();
    this.roomID = data.roomID;
    return data.roomID;
  }

  /**
   * Poll the signaling server for an answer from the other peer.
   * Automatically accepts the answer when available.
   * (Offerer side only)
   */
  async waitForOpponent(
    roomID?: string,
    pollInterval = 1000,
    timeout = 3600000,
  ): Promise<void> {
    this.assertMode("offerer");

    const targetRoomID = roomID || this.roomID;
    if (!targetRoomID) {
      throw new Error("No roomID provided and no room created");
    }

    const startTime = Date.now();

    while (true) {
      if (Date.now() - startTime > timeout) {
        throw new Error("Timeout waiting for opponent");
      }

      const response = await fetch(`${SIGNALING_SERVER}/answer/${targetRoomID}`);

      if (!response.ok) {
        throw new Error(`Failed to fetch answer: ${response.statusText}`);
      }

      const data = await response.json();

      if (data.answer) {
        await this.acceptAnswer(data.answer);
        console.log(`[${this.mode}] Answer received and accepted`);
        return;
      }

      // Wait before polling again
      await new Promise((resolve) => setTimeout(resolve, pollInterval));
    }
  }

  /**
   * Join an existing room by fetching the offer and posting an answer.
   * (Answerer side only)
   */
  async joinRoom(roomID: string): Promise<void> {
    this.assertMode("answerer");

    // Fetch the offer
    const offerResponse = await fetch(`${SIGNALING_SERVER}/offer/${roomID}`);

    if (!offerResponse.ok) {
      throw new Error(`Failed to fetch offer: ${offerResponse.statusText}`);
    }

    const offerData = await offerResponse.json();
    const offer = offerData.offer;

    // Create and post the answer
    const answer = await this.acceptOfferAndCreateAnswer(offer);

    const answerResponse = await fetch(`${SIGNALING_SERVER}/answer/${roomID}`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ answer }),
    });

    if (!answerResponse.ok) {
      throw new Error(`Failed to post answer: ${answerResponse.statusText}`);
    }

    console.log(`[${this.mode}] Joined room ${roomID}`);
  }

  // ── Messaging ─────────────────────────────────────────────────

  /** Send a string or ArrayBuffer over the data channel. */
  send(data: string | ArrayBuffer): void {
    if (!this.dc || this.dc.readyState !== "open") {
      throw new Error("Data channel is not open yet");
    }
    if (typeof data === "string") {
      this.dc.send(data);
    } else {
      this.dc.send(data);
    }
  }

  /** Resolves when the data channel is open and ready. */
  async waitForChannel(): Promise<void> {
    await this.dcReady;
  }

  // ── Internal ──────────────────────────────────────────────────

  private setupChannel(channel: RTCDataChannel): void {
    this.dc = channel;
    channel.binaryType = "arraybuffer";

    channel.onopen = () => {
      console.log(`[${this.mode}] data channel open`);
      this.resolveDc(channel);
    };

    channel.onmessage = (e: MessageEvent) => {
      this.onMessage(e.data);
    };

    channel.onclose = () => {
      console.log(`[${this.mode}] data channel closed`);
    };
  }

  private assertMode(expected: Mode): void {
    if (this.mode !== expected) {
      throw new Error(
        `Operation requires "${expected}" mode, but this instance is "${this.mode}"`,
      );
    }
  }
}
