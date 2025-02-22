#include "raylib.h"

bool running = true;
// Texture2D playerSprite;
Texture2D playerTexture;
Rectangle playerSrc;
Rectangle playerDest;
float playerSpeed = 3.0f;

void init() 
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 800, "Game");
        //SetExitKey(0);
	SetTargetFPS(60);

	//playerSprite = LoadTexture("./graphics/playerSprite.png");
	playerTexture = LoadTexture("./graphics/playerImage.png");

	playerSrc = {0, 0, 48, 48};
	// playerDest = {200, 200, 100, 100};
	playerDest = {200, 200, (float)playerTexture.width, (float)playerTexture.height};
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
	 if (IsKeyDown(KEY_J) || IsKeyDown(KEY_DOWN)) {
		playerDest.y += playerSpeed;
	}
	if (IsKeyDown(KEY_K) || IsKeyDown(KEY_UP)) {
		playerDest.y -= playerSpeed;
	}
	if (IsKeyDown(KEY_H) || IsKeyDown(KEY_LEFT)) {
		playerDest.x -= playerSpeed;
	}
	if (IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT)) {
		playerDest.x += playerSpeed;
	}
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

