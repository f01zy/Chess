import type { ServerWebSocket } from "bun";

type WebSocketData = {
  username: string,
}

const lobby: ServerWebSocket<WebSocketData>[] = [];

const server = Bun.serve({
  async fetch(req, server) {
    const username = await req.text() || "Anonymous";
    server.upgrade(req, {
      data: {
        username: username,
      },
    });
    return undefined;
  },
  websocket: {
    data: {} as WebSocketData,
    async message(ws, message) {

    },
    async open(ws) {
      if (lobby.length) {
        const opponent = lobby.shift()!;
        const gameId = Math.floor(Math.random() * 10000);

        ws.subscribe(`game-${gameId}`);
        opponent.subscribe(`game-${gameId}`);
        ws.send(JSON.stringify({ type: "opponent_found", gameId, color: "white" }));
        opponent.send(JSON.stringify({ type: "opponent_found", gameId, color: "black" }));
        console.log(`The game ${gameId} was started`);
      } else {
        lobby.push(ws);
        console.log(`${ws.data.username} are searching for opponent`);
      }
    },
    async close(ws, code, reason) {
      const game = ws.subscriptions[0];
      if (!game) {
        return;
      }
      server.publish(game, JSON.stringify({ type: "disconnected" }))
      ws.unsubscribe(game);
      console.log(`${ws.data.username} was disconnected`);
    }
  },
});

console.log(`Listening on ${server.hostname}:${server.port}`);
