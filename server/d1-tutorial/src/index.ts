/**
 * WebRTC Signaling Server using Cloudflare D1
 *
 * Endpoints:
 * POST /offer - Create a room with an offer, returns roomID
 * GET /offer/:roomID - Get the offer for a room
 * POST /answer/:roomID - Submit an answer for a room
 * GET /answer/:roomID - Get the answer for a room
 */

interface Room {
	room_id: string;
	offer: string;
	answer: string | null;
	created_at: number;
	answered_at: number | null;
}

async function cleanupOldRooms(env: Env): Promise<{ deletedCount: number }> {
	const twentyFourHoursAgo = Date.now() - 24 * 60 * 60 * 1000;

	// Delete rooms that are either:
	// 1. Created more than 24 hours ago
	// 2. Have been answered (connection established)
	const result = await env.DB.prepare(
		'DELETE FROM rooms WHERE created_at < ? OR answer IS NOT NULL'
	).bind(twentyFourHoursAgo).run();

	return { deletedCount: result.meta.changes };
}

export default {
	async fetch(request, env, ctx): Promise<Response> {
		const url = new URL(request.url);
		const path = url.pathname;

		// Enable CORS for WebRTC signaling
		const corsHeaders = {
			'Access-Control-Allow-Origin': '*',
			'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
			'Access-Control-Allow-Headers': 'Content-Type',
		};

		// Handle CORS preflight
		if (request.method === 'OPTIONS') {
			return new Response(null, { headers: corsHeaders });
		}

		try {
			// POST /offer - Create room with offer
			if (path === '/offer' && request.method === 'POST') {
				const { offer } = await request.json() as { offer: string };

				if (!offer) {
					return new Response(JSON.stringify({ error: 'Offer is required' }), {
						status: 400,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				// Cleanup old and connected rooms
				await cleanupOldRooms(env);

				// Generate unique room ID
				const roomID = crypto.randomUUID().substring(0, 5).toLowerCase();
				const createdAt = Date.now();

				await env.DB.prepare(
					'INSERT INTO rooms (room_id, offer, created_at) VALUES (?, ?, ?)'
				).bind(roomID, JSON.stringify(offer), createdAt).run();

				return new Response(JSON.stringify({ roomID }), {
					status: 201,
					headers: { 'Content-Type': 'application/json', ...corsHeaders },
				});
			}

			// GET /offer/:roomID - Get offer
			if (path.startsWith('/offer/') && request.method === 'GET') {
				const roomID = path.split('/')[2];

				if (!roomID) {
					return new Response(JSON.stringify({ error: 'Room ID is required' }), {
						status: 400,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				const result = await env.DB.prepare(
					'SELECT offer FROM rooms WHERE room_id = ?'
				).bind(roomID).first<{ offer: string }>();

				if (!result) {
					return new Response(JSON.stringify({ error: 'Room not found' }), {
						status: 404,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				return new Response(JSON.stringify({ offer: JSON.parse(result.offer) }), {
					status: 200,
					headers: { 'Content-Type': 'application/json', ...corsHeaders },
				});
			}

			// POST /answer/:roomID - Submit answer
			if (path.startsWith('/answer/') && request.method === 'POST') {
				const roomID = path.split('/')[2];
				const { answer } = await request.json() as { answer: string };

				if (!roomID || !answer) {
					return new Response(JSON.stringify({ error: 'Room ID and answer are required' }), {
						status: 400,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				// Check if room exists
				const room = await env.DB.prepare(
					'SELECT room_id FROM rooms WHERE room_id = ?'
				).bind(roomID).first();

				if (!room) {
					return new Response(JSON.stringify({ error: 'Room not found' }), {
						status: 404,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				const answeredAt = Date.now();

				await env.DB.prepare(
					'UPDATE rooms SET answer = ?, answered_at = ? WHERE room_id = ?'
				).bind(JSON.stringify(answer), answeredAt, roomID).run();

				return new Response(JSON.stringify({ success: true }), {
					status: 200,
					headers: { 'Content-Type': 'application/json', ...corsHeaders },
				});
			}

			// GET /answer/:roomID - Get answer
			if (path.startsWith('/answer/') && request.method === 'GET') {
				const roomID = path.split('/')[2];

				if (!roomID) {
					return new Response(JSON.stringify({ error: 'Room ID is required' }), {
						status: 400,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				const result = await env.DB.prepare(
					'SELECT answer FROM rooms WHERE room_id = ?'
				).bind(roomID).first<{ answer: string | null }>();

				if (!result) {
					return new Response(JSON.stringify({ error: 'Room not found' }), {
						status: 404,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				if (!result.answer) {
					return new Response(JSON.stringify({ answer: null }), {
						status: 200,
						headers: { 'Content-Type': 'application/json', ...corsHeaders },
					});
				}

				return new Response(JSON.stringify({ answer: JSON.parse(result.answer) }), {
					status: 200,
					headers: { 'Content-Type': 'application/json', ...corsHeaders },
				});
			}

			// Route not found
			return new Response(JSON.stringify({ error: 'Not found' }), {
				status: 404,
				headers: { 'Content-Type': 'application/json', ...corsHeaders },
			});

		} catch (error) {
			console.error('Error:', error);
			return new Response(JSON.stringify({ error: 'Internal server error' }), {
				status: 500,
				headers: { 'Content-Type': 'application/json', ...corsHeaders },
			});
		}
	},
} satisfies ExportedHandler<Env>;
