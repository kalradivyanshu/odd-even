# WebRTC Signaling Server

A Cloudflare Workers-based WebRTC signaling server using D1 database for storing offers and answers.

## Setup

### 1. Create D1 Database

```bash
# Create the database
npx wrangler d1 create webrtc-signaling
```

This will output something like:
```
✅ Successfully created DB 'webrtc-signaling'
database_id = "abc123-def456-..."
```

Copy the `database_id` and update line 24 in `wrangler.jsonc`:
```jsonc
"database_id": "abc123-def456-..."  // Replace YOUR_DATABASE_ID_HERE
```

### 2. Initialize Database Schema

```bash
# For local development
npx wrangler d1 execute webrtc-signaling --local --file=./schema.sql

# For production
npx wrangler d1 execute webrtc-signaling --remote --file=./schema.sql
```

### 3. Generate TypeScript Types

```bash
npx wrangler types
```

### 4. Run Locally

```bash
npm run dev
# or
npx wrangler dev
```

### 5. Deploy

```bash
npm run deploy
# or
npx wrangler deploy
```

## Automatic Cleanup

The server automatically cleans up old and connected rooms whenever a new room is created:
- Rooms created more than 24 hours ago
- Rooms that have been answered (connection already established)

## API Endpoints

### 1. Create Room with Offer
**POST** `/offer`

Request:
```json
{
  "offer": {
    "type": "offer",
    "sdp": "..."
  }
}
```

Response (201):
```json
{
  "roomID": "uuid-here"
}
```

### 2. Get Offer
**GET** `/offer/:roomID`

Response (200):
```json
{
  "offer": {
    "type": "offer",
    "sdp": "..."
  }
}
```

### 3. Submit Answer
**POST** `/answer/:roomID`

Request:
```json
{
  "answer": {
    "type": "answer",
    "sdp": "..."
  }
}
```

Response (200):
```json
{
  "success": true
}
```

### 4. Get Answer
**GET** `/answer/:roomID`

Response (200):
```json
{
  "answer": {
    "type": "answer",
    "sdp": "..."
  }
}
```

Response (200 - no answer yet):
```json
{
  "answer": null
}
```

## WebRTC Handshake Flow

1. **Peer A** creates an offer and sends it to `POST /offer`
2. Server returns a `roomID`
3. **Peer A** shares the `roomID` with **Peer B** (via QR code, link, etc.)
4. **Peer B** fetches the offer using `GET /offer/:roomID`
5. **Peer B** creates an answer and sends it to `POST /answer/:roomID`
6. **Peer A** polls `GET /answer/:roomID` until an answer is available
7. WebRTC connection is established

## CORS

CORS is enabled for all origins (`*`) to allow WebRTC signaling from any client.

## Error Responses

All errors return JSON:
```json
{
  "error": "Error message"
}
```

Status codes:
- 400: Bad request (missing parameters)
- 404: Room not found
- 500: Internal server error
