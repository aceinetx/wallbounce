#include "wallbounce.hh"

using namespace wb;

Wallbounce::Wallbounce() {
	InitWindow(screenWidth, screenHeight, "wallbounce");
	SetTargetFPS(60);

	randSeed = 1007;
}

Wallbounce::~Wallbounce() {
	CloseWindow();
}

void Wallbounce::ClearObjects(unsigned char wall) {
	if (wall == 0)
		leftWallObjects.clear();
	if (wall == 1)
		rightWallObjects.clear();
	return;
}

void Wallbounce::GenerateLava(unsigned char wall) {
	if (wall == 0)
		ClearObjects(wall);
	else if (wall == 1) {
		ClearObjects(wall);
	}

	size_t tiles = screenHeight / 50;

	std::vector<size_t> skip;
	skip.push_back(Rand() % tiles);
	skip.push_back(skip.back() + 1);
	skip.push_back(skip.back() - 2);

	int shieldPos = -1;
	if ((Rand() % 10) == 5) {
		shieldPos = Rand() % tiles;
		skip.push_back(shieldPos);
		skip.push_back(shieldPos + 1);

		GameObject obj;
		obj.type = GameObject::SHIELD;
		obj.rect.y = 50.0f * shieldPos + 25.0f;
		obj.rect.height = 50;

		if (wall == 0) {
			obj.rect.x = 20;
			obj.rect.width = 10;
			leftWallObjects.push_back(obj);
		} else if (wall == 1) {
			obj.rect.x = screenWidth - 30;
			obj.rect.width = 10;
			rightWallObjects.push_back(obj);
		}
	}

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
			leftWallObjects.push_back(obj);
		} else if (wall == 1) {
			obj.rect.x = screenWidth - 30;
			obj.rect.width = 10;
			rightWallObjects.push_back(obj);
		}
	}
}

void Wallbounce::RandSeed(uint64_t seed) {
	randSeed = seed;
	std::srand(seed);
}

uint64_t Wallbounce::Rand() {
	return std::rand();
}

void Wallbounce::PausedStateFrame() {
	char score_text[64];

	ClearBackground(RAYWHITE);

	DrawText("Click to start", screenWidth / 2.8f, screenHeight / 2, 20, LIGHTGRAY);

	snprintf(score_text, sizeof(score_text), "High score: %d", high_score);
	DrawText(score_text, 0, 0, 25, LIGHTGRAY);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) {
		gameState = GameState::PLAYING;

		leftWallObjects.clear();
		rightWallObjects.clear();

		score = 0;

		playerPos.x = screenWidth / 2.0;
		playerPos.y = screenHeight / 2.0;

		playerDirX = 1;
		playerVelY = 0.0f;

		GenerateLava(0);
		GenerateLava(1);

		shields = 0;
		shieldCollided = false;
	}
}

void Wallbounce::PlayingStateFrame() {
	char score_text[64];

	playerVelY += gravity * delta;

	playerPos.y += playerVelY;
	if (playerDirX == 1)
		playerPos.x += horizontalSpeed * delta;
	else
		playerPos.x += -horizontalSpeed * delta;

	if (playerPos.y < 0) {
		playerPos.y = 0;
		playerVelY = gravity * delta;
	}

	if (playerPos.y > (float)screenHeight) {
		playerPos.y = 0;
	}

	playerRect = {.x = playerPos.x, .y = playerPos.y, .width = 50, .height = 50};

	if (CheckCollisionRecs(playerRect, wallLeft) || CheckCollisionRecs(playerRect, wallRight)) {
		playerDirX = -playerDirX;
		score++;
		if (score > high_score)
			high_score = score;
		if (playerDirX == -1)
			GenerateLava(0);
		else {
			GenerateLava(1);
		}
	}
	snprintf(score_text, sizeof(score_text), "%d", score);

	ClearBackground(RAYWHITE);

	bool didCollide = false;
	{
		auto for_func = [&didCollide, this](GameObject& obj) -> bool {
			if (obj.decayed)
				return true;

			switch (obj.type) {
			case GameObject::LAVA: {
				DrawRectangleRec(obj.rect, RED);
				if (CheckCollisionRecs(obj.rect, playerRect)) {
					didCollide = true;
					if (!shields) {
						gameState = GameState::STOP;
					} else {
						shieldCollided = true;
					}
				}
			} break;
			case GameObject::SHIELD: {
				DrawRectangleRec(obj.rect, BLUE);
				if (CheckCollisionRecs(obj.rect, playerRect)) {
					shields++;
					obj.decayed = true;
				}
			} break;
			}
			return true;
		};

		for (GameObject& obj : rightWallObjects) {
			if (!for_func(obj))
				break;
		}
		for (GameObject& obj : leftWallObjects) {
			if (!for_func(obj))
				break;
		}
	}

	if (!didCollide && shieldCollided) {
		shields--;
		shieldCollided = false;
	}

	DrawRectangleRec(wallLeft, GRAY);
	DrawRectangleRec(wallRight, GRAY);

	DrawRectangleRec(playerRect, LIGHTGRAY);
	if (shields)
		DrawRectangleLinesEx(playerRect, 3.0f, BLUE);

	DrawText(score_text, 35, 0, 50, LIGHTGRAY);

	if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		playerVelY = jumpVelocity * delta;
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
		case GameState::STOP:
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
