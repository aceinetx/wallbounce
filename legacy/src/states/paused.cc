#include "../wallbounce.hh"

using namespace wb;

void Wallbounce::PausedStateFrame() {
	ClearBackground(RAYWHITE);

	DrawText("Click to start", screenWidth / 2.8f, screenHeight / 2, 20, LIGHTGRAY);

	DrawText(TextFormat("High score: %d", high_score), 0, 0, 25, LIGHTGRAY);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) {
		gameState = GameState::PLAYING;

		// Reset state
		leftWallObjects.clear();
		rightWallObjects.clear();

		score = 0;

		playerPos.x = screenWidth / 2.0;
		playerPos.y = screenHeight / 2.0;

		playerDirX = 1;
		playerVelY = 0.0f;

		Generate(0);
		Generate(1);

		shields = 0;
		shieldCollided = false;
	}
}
