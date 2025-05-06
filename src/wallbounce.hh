#pragma once
#include "pch.hh"

typedef struct {
	Vector2 v1;
	Vector2 v2;
	Vector2 v3;
} Triangle;

namespace wb {
class Wallbounce {
public:
	typedef enum class GameState { PAUSE, PLAYING } GameState;
	typedef enum class WallObjType { NONE, SPIKE } WallObjType;

private:
	void PausedStateFrame();
	void PlayingStateFrame();
	void UpdateScoreText();

	float delta;

	Rectangle playerRect;

	Rectangle wallLeft;
	WallObjType wallLeftObjs[16];

	Rectangle wallRight;
	WallObjType wallRightObjs[16];

public:
	GameState gameState;
	const int screenWidth = 500;
	const int screenHeight = 700;

	uint32_t score = 0;
	uint32_t high_score = 0;

	Vector2 playerPos;
	Vector2 playerVel;

	Wallbounce();
	~Wallbounce();

	void Run();
};
} // namespace wb
