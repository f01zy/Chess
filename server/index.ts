const server = Bun.serve({
  fetch(req, server) {
    if (server.upgrade(req)) {
      return;
    }
    return new Response("Upgrade failed", { status: 500 });
  },
  websocket: {
    async message(ws, message) {
      console.log("message from C client:", message);
    },
    async open(ws) {
      console.log("C client connected!");
    }
  },
});

console.log(`Server running at ${server.url}`);
