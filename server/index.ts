import type { ServerWebSocket } from "bun";

enum Color { WHITE, BLACK }

type WebSocketData = {
  userId: number,
}

type Game = {
  gameId: number,
  users: ServerWebSocket<WebSocketData>[],
}

let lobby: ServerWebSocket<WebSocketData>[] = [];
let games: Game[] = [];

const random = (min: number, max: number) => {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

const opponent_searching = (ws: ServerWebSocket<WebSocketData>) => {
  if (lobby.length) {
    const opponent = lobby.shift()!;
    const gameId = random(0, 10000);

    games.push({
      gameId,
      users: [ws, opponent],
    });
    ws.subscribe(`${gameId}`);
    opponent.subscribe(`${gameId}`);

    ws.send(JSON.stringify({ type: "opponent_found", gameId, side: Color.WHITE }));
    opponent.send(JSON.stringify({ type: "opponent_found", gameId, side: Color.BLACK }));
    console.log(`The game ${gameId} was started`);
  } else {
    lobby.push(ws);
    console.log(`${ws.data.userId} are searching for opponent`);
  }
}

const move = (ws: ServerWebSocket<WebSocketData>, data: string) => {
  if (!ws.subscriptions.length) {
    return;
  }
  const gameId = ws.subscriptions[0]!;
  server.publish(gameId, data);
}

const disconnect = (ws: ServerWebSocket<WebSocketData>) => {
  if (!ws.subscriptions.length) {
    return;
  }
  const gameId = ws.subscriptions[0]!;
  const numberGameId = Number(gameId);
  const game = games.find(game => game.gameId == numberGameId)!;

  game.users.forEach(user => {
    user.send(JSON.stringify({ type: "disconnected" }));
    user.unsubscribe(gameId);
  });
  games = games.filter(game => game.gameId != numberGameId);
  console.log(`The game ${gameId} was finished`);
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
          opponent_searching(ws);
          break;
        case "disconnect":
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
