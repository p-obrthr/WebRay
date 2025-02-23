#include "raylib.h"
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <stdio.h>
#include <map>
#include <sstream>

bool running = true;
// Texture2D playerSprite;
Texture2D playerTexture;
// Rectangle playerSrc;
Rectangle playerDest;
float playerSpeed = 3.0f;

std::map<int, Rectangle> otherPlayers;
int playerId = -1;

EMSCRIPTEN_WEBSOCKET_T websocket;

EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
	puts("WebSocket opened");
	return EM_TRUE;
}

EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData) {
	puts("WebSocket error occurred");
	return EM_TRUE;
}

EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData) {
	puts("WebSocket closed");
	return EM_TRUE;
}

EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
	puts("WebSocket message received");
	if (websocketEvent->isText) {
		if (strncmp((const char*)websocketEvent->data, "SET_PLAYER_", 11) == 0) {
			sscanf((const char*)websocketEvent->data, "SET_PLAYER_%d", &playerId);
			return EM_TRUE;
		}

		int receivedId;
		float x, y;
		if (strncmp((const char*)websocketEvent->data, "REMOVE_PLAYER_", 14) == 0) {
			sscanf((const char*)websocketEvent->data, "REMOVE_PLAYER_%d", &receivedId);
			otherPlayers.erase(receivedId);
			return EM_TRUE;
		}
		if (strncmp((const char*)websocketEvent->data, "NEW_PLAYER_", 11) == 0) {
			sscanf((const char*)websocketEvent->data, "NEW_PLAYER_%d %f %f", &receivedId, &x, &y);
			otherPlayers[receivedId] = {x, y, (float)playerTexture.width, (float)playerTexture.height};
			return EM_TRUE;
		}
		if (strncmp((const char*)websocketEvent->data, "PLAYER_LIST", 11) == 0) {
			otherPlayers.clear();
			std::istringstream stream(std::string((const char*)websocketEvent->data + 12));
			int id;
			while (stream >> id >> x >> y) {
				if (id != playerId) {
					otherPlayers[id] = {x, y, (float)playerTexture.width, (float)playerTexture.height};
				}
			}
    
			return EM_TRUE;
		}
		if (strncmp((const char*)websocketEvent->data, "MOVE_PLAYER_", 12) == 0) {
			sscanf((const char*)websocketEvent->data, "MOVE_PLAYER_%d %f %f", &receivedId, &x, &y);
			if (receivedId == playerId) return EM_TRUE;
			otherPlayers[receivedId].x = x;
			otherPlayers[receivedId].y = y;
		}
	}
	return EM_TRUE;
}

void init() 
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 800, "Game");
        //SetExitKey(0);
	SetTargetFPS(60);

	//playerSprite = LoadTexture("./graphics/playerSprite.png");
	playerTexture = LoadTexture("./graphics/playerImage.png");

	// playerSrc = {0, 0, 48, 48};
	// playerDest = {200, 200, 100, 100};
	playerDest = {200, 200, (float)playerTexture.width, (float)playerTexture.height};
	if (!emscripten_websocket_is_supported()) {
		printf("WebSocket not supported\n");
		return;
	}

	EmscriptenWebSocketCreateAttributes ws_attrs = {
		"ws://localhost:8080",
		NULL,
		EM_TRUE,
	};

	websocket = emscripten_websocket_new(&ws_attrs);
	emscripten_websocket_set_onopen_callback(websocket, NULL, onopen);
	emscripten_websocket_set_onerror_callback(websocket, NULL, onerror);
	emscripten_websocket_set_onclose_callback(websocket, NULL, onclose);
	emscripten_websocket_set_onmessage_callback(websocket, NULL, onmessage);
}

void quit()
{
	// UnloadTexture(playerSprite);
	UnloadTexture(playerTexture);
	CloseWindow();
}

void update() 
{
	running = !WindowShouldClose();
}

void drawScene() 
{
	// DrawTexturePro(playerSprite, playerSrc, playerDest, Vector2{playerDest.width / 2, playerDest.height / 2}, 0, WHITE);
	DrawTextureV(playerTexture, Vector2{playerDest.x, playerDest.y}, WHITE);
	for (const auto& [id, rect] : otherPlayers) {
	    DrawTextureV(playerTexture, Vector2{rect.x, rect.y}, WHITE);
	}
}

void render()
{
	BeginDrawing();
	ClearBackground(BLACK);
	drawScene();
	EndDrawing();
}

void input()
{
	if (playerId == -1) return;

	if (IsKeyDown(KEY_J) || IsKeyDown(KEY_DOWN)) {
		playerDest.y += playerSpeed;
	} else if (IsKeyDown(KEY_K) || IsKeyDown(KEY_UP)) {
		playerDest.y -= playerSpeed;
	} else if (IsKeyDown(KEY_H) || IsKeyDown(KEY_LEFT)) {
		playerDest.x -= playerSpeed;
	} else if (IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT)) {
		playerDest.x += playerSpeed;
	} else return;

	char moveMessage[50];
	sprintf(moveMessage, "MOVE_PLAYER_%d %f %f", playerId, playerDest.x, playerDest.y);
	emscripten_websocket_send_utf8_text(websocket, moveMessage);
}

int main ()
{
	init();
	while (running)
	{
		input();
		update();
		render();
	}
	quit();
	return 0;
}

