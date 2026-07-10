#include "wallbounce.hh"

using namespace wb;

void Wallbounce::UpdatePhysics() {
	switch (gameState) {
	case GameState::PLAYING: {
		playerVelY += gravity * fixedTimestep;

		playerPos.y += playerVelY;

		// Move player horizontally
		if (playerDirX == 1)
			playerPos.x += horizontalSpeed * fixedTimestep;
		else
			playerPos.x += -horizontalSpeed * fixedTimestep;

		// Don't go above the ceiling
		if (playerPos.y < 0) {
			playerPos.y = 0;
			playerVelY = gravity * fixedTimestep;
		}

		// Teleport to the top when reached the bottom
		if (playerPos.y > (float)screenHeight) {
			playerPos.y = 0;
		}

		// Update playerRect
		playerRect = {.x = playerPos.x, .y = playerPos.y, .width = 50, .height = 50};

		if (CheckCollisionRecs(playerRect, wallLeft) || CheckCollisionRecs(playerRect, wallRight)) {
			// Bounce off a wall
			playerDirX = -playerDirX;

			// Update the score
			score++;
			if (score > high_score)
				high_score = score;

			// Generate a wall opposite to the player
			if (playerDirX == -1)
				Generate(0);
			else {
				Generate(1);
			}
		}
	} break;
	default:
		break;
	}
}
