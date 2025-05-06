#include "wallbounce.hh"
#include <raylib.h>

using namespace wb;

Wallbounce::Wallbounce() {
	InitWindow(screenWidth, screenHeight, "wallbounce");
	SetTargetFPS(60);

	playerPos.x = screenWidth / 2.0;
	playerPos.y = screenHeight / 2.0;

	playerVel.x = 200.0f;

	std::memset(wallLeftObjs, 0, sizeof(wallLeftObjs));
	std::memset(wallRightObjs, 0, sizeof(wallRightObjs));

	wallLeftObjs[0] = WallObjType::SPIKE;
	wallLeftObjs[1] = WallObjType::SPIKE;
	wallLeftObjs[2] = WallObjType::SPIKE;
}

Wallbounce::~Wallbounce() {
	CloseWindow();
}

void Wallbounce::PausedStateFrame() {
	char score_text[64];

	ClearBackground(RAYWHITE);

	DrawText("Click to start", screenWidth / 2.8f, screenHeight / 2, 20, LIGHTGRAY);

	snprintf(score_text, sizeof(score_text), "High score: %d", high_score);
	DrawText(score_text, 0, 0, 25, LIGHTGRAY);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		gameState = GameState::PLAYING;
	} else if (IsKeyDown(KEY_SPACE)) {
		gameState = GameState::PLAYING;
	}
}

void Wallbounce::PlayingStateFrame() {
	char score_text[64];

	playerVel.y += 25.0f;

	playerPos.y += playerVel.y * delta;
	playerPos.x += playerVel.x * delta;

	if (playerPos.y < 0) {
		playerPos.y = 0;
		playerVel.y = 25.0f;
	}

	if (playerPos.y > (float)screenHeight) {
		playerPos.y = 0;
	}

	playerRect = {.x = playerPos.x, .y = playerPos.y, .width = 50, .height = 50};

	if (CheckCollisionRecs(playerRect, wallLeft) || CheckCollisionRecs(playerRect, wallRight)) {
		playerVel.x = -playerVel.x;
		score++;
		if (score > high_score)
			high_score = score;
	}
	snprintf(score_text, sizeof(score_text), "%d", high_score);

	ClearBackground(RAYWHITE);
	DrawText(score_text, 25, 0, 50, LIGHTGRAY);

	DrawRectangleRec(wallLeft, GRAY);
	DrawRectangleRec(wallRight, GRAY);

	// draw wall objects
	for (WallObjType obj : wallLeftObjs) {
		switch (obj) {
		case WallObjType::SPIKE: {
		} break;
		default:
			break;
		}
	}

	DrawRectangleRec(playerRect, LIGHTGRAY);

	if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		playerVel.y = -500.0f;
	}
}

void Wallbounce::UpdateScoreText() {
}

void Wallbounce::Run() {
	wallLeft = (Rectangle){.x = 0.0f, .y = 0.0f, .width = 20.0f, .height = (float)screenHeight};
	wallRight = (Rectangle){.x = (float)screenWidth - 20.0f, .y = 0.0f, .width = 20.0f, .height = (float)screenHeight};
	while (!WindowShouldClose()) {
		delta = GetFrameTime();

		BeginDrawing();

		switch (gameState) {
		case GameState::PAUSE:
			PausedStateFrame();
			break;
		case GameState::PLAYING:
			UpdateScoreText();
			PlayingStateFrame();
			break;
		}

		EndDrawing();
	}
}
