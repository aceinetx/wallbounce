#include "wallbounce.hh"

using namespace wb;

void Wallbounce::UpdatePhysics() {
	switch (gameState) {
	case GameState::PLAYING: {
		playerVelY += gravity * fixedTimestep;

		playerPos.y += playerVelY;
		if (playerDirX == 1)
			playerPos.x += horizontalSpeed * fixedTimestep;
		else
			playerPos.x += -horizontalSpeed * fixedTimestep;

		if (playerPos.y < 0) {
			playerPos.y = 0;
			playerVelY = gravity * fixedTimestep;
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
