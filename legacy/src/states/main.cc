#include "../wallbounce.hh"

using namespace wb;

void Wallbounce::MainStateFrame() {
	Rectangle startButton;
	ClearBackground(RAYWHITE);

	startButton = Rectangle{.width = 150, .height = 50};
	startButton.x = screenWidth / 2.0f - startButton.width / 2.0f;
	startButton.y = screenHeight - 200;

	DrawRectangleRec(startButton, GRAY);
	DrawText("Start", startButton.x + startButton.width / 8.0f, startButton.y + startButton.height / 8.0f, 40.0f, WHITE);

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(GetMousePosition(), startButton)) {
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
}
