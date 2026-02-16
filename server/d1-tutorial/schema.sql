-- WebRTC signaling room table
CREATE TABLE IF NOT EXISTS rooms (
    room_id TEXT PRIMARY KEY,
    offer TEXT NOT NULL,
    answer TEXT,
    created_at INTEGER NOT NULL,
    answered_at INTEGER
);

-- Index for faster lookups
CREATE INDEX IF NOT EXISTS idx_created_at ON rooms(created_at);
