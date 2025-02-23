const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });
let nextPlayerId = 1;

let players = new Map();

wss.on('connection', (ws) => {

    const playerId = nextPlayerId++;
    players.set(ws, { id: playerId, x: 200, y: 200 });
    console.log(`Player ${playerId} connected`);
    ws.send(`SET_PLAYER_${playerId}`);
    let playerList = "PLAYER_LIST";

    //let playerList = players.map(player => ({
    //    id: player.id,
    //    x: player.x,
    //    y: player.y
    //}));

    //ws.send(JSON.stringify({ type: "PLAYER_LIST", players: playerList }));

    players.forEach((player) => {
         playerList += ` ${player.id} ${player.x} ${player.y}`;
    });
    ws.send(playerList);

    wss.clients.forEach(client => {
        if (client !== ws && client.readyState === WebSocket.OPEN) {
            client.send(`NEW_PLAYER_${playerId} 200 200`);
        }
    });

    ws.on('message', (message) => {
        const msg = message.toString();
        console.log(`rcvd: ${msg}`);

        const parts = msg.split(" ");
        if (parts.length < 3) return;


        const playerId = Number(parts[0].replace("MOVE_PLAYER_", ""));
        const directionX = Number(parts[1]);
        const directionY = Number(parts[2]);

        let player = getPlayerById(playerId);
        if (!player) return; 

        player.x = directionX;
        player.y = directionY;

        wss.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(`MOVE_PLAYER_${playerId} ${directionX} ${directionY}`);
            }
        });
    });

    ws.on('close', () => {
        console.log(`player ${playerId} disconnected`);
        players.delete(ws);

        wss.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(`REMOVE_PLAYER_${playerId}`);
            }
        });
    });
});

function getPlayerById(id) {
    for (const player of players.values()) {
        if (player.id === id) {
            return player;
        }
    }
    return null;
}
