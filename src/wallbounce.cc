#include "wallbounce.hh"
#include <raylib.h>

using namespace wb;

Wallbounce::Wallbounce() {
	InitWindow(screenWidth, screenHeight, "wallbounce");
	SetTargetFPS(60);
}

Wallbounce::~Wallbounce() {
	CloseWindow();
}

void Wallbounce::ClearObjects(float xFilter) {
	std::vector<size_t> indexes;
	for (size_t i = 0; i < objects.size(); i++) {
		if (xFilter != 0 && objects[i].rect.x == xFilter)
			indexes.insert(indexes.begin(), i);
	}

	for (size_t index : indexes) {
		objects.erase(objects.begin() + index);
	}
	return;
}

void Wallbounce::GenerateLava(unsigned char wall) {
	if (wall == 0)
		ClearObjects(20);
	else if (wall == 1) {
		ClearObjects(screenWidth - 30);
	}

	size_t tiles = screenHeight / 50;

	std::vector<size_t> skip;
	skip.push_back(rand() % tiles);
	skip.push_back(skip.back() + 1);
	skip.push_back(skip.back() - 2);

	for (size_t i = 0; i < tiles; i++) {
		if (std::find(skip.begin(), skip.end(), i) != skip.end())
			continue;

		GameObject obj;
		obj.type = GameObject::LAVA;
		obj.rect.y = 50.0f * i;
		obj.rect.height = 50;

		if (wall == 0) {
			obj.rect.x = 20;
			obj.rect.width = 10;
		} else if (wall == 1) {
			obj.rect.x = screenWidth - 30;
			obj.rect.width = 10;
		}

		objects.push_back(obj);
	}
}

void Wallbounce::PausedStateFrame() {
	char score_text[64];

	ClearBackground(RAYWHITE);

	DrawText("Click to start", screenWidth / 2.8f, screenHeight / 2, 20, LIGHTGRAY);

	snprintf(score_text, sizeof(score_text), "High score: %d", high_score);
	DrawText(score_text, 0, 0, 25, LIGHTGRAY);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) {
		gameState = GameState::PLAYING;

		objects.clear();
		score = 0;

		playerPos.x = screenWidth / 2.0;
		playerPos.y = screenHeight / 2.0;

		playerVel.x = 200.0f;
		playerVel.y = 0.0f;

		GenerateLava(0);
		GenerateLava(1);
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
		if (playerVel.x < 0)
			GenerateLava(0);
		else {
			GenerateLava(1);
		}
	}
	snprintf(score_text, sizeof(score_text), "%d", score);

	ClearBackground(RAYWHITE);

	DrawRectangleRec(wallLeft, GRAY);
	DrawRectangleRec(wallRight, GRAY);

	for (GameObject obj : objects) {
		switch (obj.type) {
		case GameObject::LAVA: {
			DrawRectangleRec(obj.rect, RED);
			if (CheckCollisionRecs(obj.rect, playerRect)) {
				gameState = GameState::PAUSE;
				return;
			}
		} break;
		}
	}

	DrawRectangleRec(playerRect, LIGHTGRAY);

	DrawText(score_text, 25, 0, 50, LIGHTGRAY);

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
