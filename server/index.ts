import type { ServerWebSocket } from "bun";

enum Color { WHITE, BLACK }

type WebSocketData = {
  userId: number,
}

const lobby: ServerWebSocket<WebSocketData>[] = [];

const random = (min: number, max: number) => {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

const searching = (ws: ServerWebSocket<WebSocketData>) => {
  if (lobby.length) {
    const opponent = lobby.shift()!;
    const gameId = random(0, 10000);

    ws.subscribe(`game-${gameId}`);
    opponent.subscribe(`game-${gameId}`);
    ws.send(JSON.stringify({ type: "opponent_found", gameId, side: Color.WHITE }));
    opponent.send(JSON.stringify({ type: "opponent_found", gameId, side: Color.BLACK }));
    console.log(`The game ${gameId} was started`);
  } else {
    lobby.push(ws);
    console.log(`${ws.data.userId} are searching for opponent`);
  }
}

const disconnect = (ws: ServerWebSocket<WebSocketData>) => {
  const game = ws.subscriptions[0];
  if (!game) {
    return;
  }
  server.publish(game, JSON.stringify({ type: "disconnected" }))
  ws.unsubscribe(game);
  console.log(`${ws.data.userId} was disconnected`);
}

const move = (ws: ServerWebSocket<WebSocketData>, data: string) => {
  const game = ws.subscriptions[0];
  if (!game) {
    return;
  }
  server.publish(game, data)
}

const server = Bun.serve({
  async fetch(req, server) {
    const userId = random(0, 10000);
    server.upgrade(req, {
      data: {
        userId
      },
    });
    return undefined;
  },
  websocket: {
    data: {} as WebSocketData,
    async message(ws, message: string) {
      let json;

      try {
        json = JSON.parse(message);
      } catch { return };

      switch (json.type) {
        case "searching":
          searching(ws);
          break;
        case "disconnected":
          disconnect(ws);
          break;
        case "move":
          move(ws, message);
          break;
      }
    },
    async open(ws) {
      ws.send(JSON.stringify({ type: "connected" }));
      console.log(`${ws.data.userId} was connected`);
    },
    async close(ws) {
      disconnect(ws);
    }
  },
});

console.log(`Listening on ${server.hostname}:${server.port}`);
