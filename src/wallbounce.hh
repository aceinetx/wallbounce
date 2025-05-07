#pragma once
#include "pch.hh"

namespace wb {
class Wallbounce {
public:
	typedef enum class GameState { PAUSE, PLAYING } GameState;

	typedef struct GameObject {
		union {
			Rectangle rect;
		};
		enum {
			LAVA,
		} type;
	} GameObject;

private:
	void PausedStateFrame();
	void PlayingStateFrame();
	void UpdateScoreText();

	float delta;

	Rectangle playerRect;

	Rectangle wallLeft;
	Rectangle wallRight;

public:
	GameState gameState;
	const int screenWidth = 500;
	const int screenHeight = 700;

	uint32_t score = 0;
	uint32_t high_score = 0;

	Vector2 playerPos;
	Vector2 playerVel;

	std::vector<GameObject> objects;

	Wallbounce();
	~Wallbounce();

	void Run();

	void ClearObjects(float xFilter = 0.0f);
	void GenerateLava(unsigned char wall = 0);
};
} // namespace wb
