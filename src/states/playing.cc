#include "../wallbounce.hh"

using namespace wb;

void Wallbounce::PlayingStateFrame() {
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
				DrawTextureEx(shieldTexture, Vector2{obj.rect.x, obj.rect.y}, 0.0f, 0.05f, WHITE);
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

	{
		const char* text = TextFormat("%d", score);
		const float fontSize = 50.0f;
		DrawText(text, screenWidth / 2 - MeasureText(text, fontSize) / 2, 0, fontSize, LIGHTGRAY);
	}

	{
		const char* text = TextFormat("%d", shields);
		const float fontSize = 50.0f;
		float textSize = MeasureText(text, fontSize);
		DrawText(text, 35, 0, 50.0f, LIGHTGRAY);
		DrawTextureEx(shieldTexture, Vector2{40 + textSize, 3}, 0.0f, 0.05f, LIGHTGRAY);
	}

	if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		playerVelY = jumpVelocity * fixedTimestep;
	}
}
