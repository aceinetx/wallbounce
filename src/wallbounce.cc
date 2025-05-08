#include "wallbounce.hh"

using namespace wb;

Wallbounce::Wallbounce() {
	InitWindow(screenWidth, screenHeight, "wallbounce");
	SetTargetFPS(60);

	RandSeed(std::time(0));

	Image shield = LoadImage("../assets/shield.png");
	shieldTexture = LoadTextureFromImage(shield);
	UnloadImage(shield);
}

Wallbounce::~Wallbounce() {
	UnloadTexture(shieldTexture);
	CloseWindow();
}

void Wallbounce::ClearObjects(unsigned char wall) {
	if (wall == 0)
		leftWallObjects.clear();
	if (wall == 1)
		rightWallObjects.clear();
	return;
}

void Wallbounce::Run() {
	wallLeft = (Rectangle){.x = 0.0f, .y = 0.0f, .width = 20.0f, .height = (float)screenHeight};
	wallRight = (Rectangle){.x = (float)screenWidth - 20.0f, .y = 0.0f, .width = 20.0f, .height = (float)screenHeight};
	while (!WindowShouldClose()) {
		delta = GetFrameTime();

		accumulator += delta;

		while (accumulator >= fixedTimestep) {
			UpdatePhysics();
			accumulator -= fixedTimestep;
		}

		BeginDrawing();

		switch (gameState) {
		case GameState::STOP:
			PausedStateFrame();
			break;
		case GameState::PLAYING:
			PlayingStateFrame();
			break;
		}

		EndDrawing();
	}
}
