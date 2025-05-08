#pragma once
#include "pch.hh"

namespace wb {
class Wallbounce {
public:
	typedef enum class GameState { STOP, PLAYING } GameState;

	typedef struct GameObject {
		union {
			Rectangle rect;
		};
		enum {
			LAVA,
			SHIELD,
		} type;
		bool decayed = false;
	} GameObject;

private:
	void PausedStateFrame();
	void PlayingStateFrame();
	void UpdateScoreText();

	float delta;

	Rectangle playerRect;

	Rectangle wallLeft;
	Rectangle wallRight;

	uint64_t randSeed;

	bool shieldCollided = false;

public:
	GameState gameState;
	const int screenWidth = 500;
	const int screenHeight = 700;
	const float gravity = 25.0f;
	const float horizontalSpeed = 200.0f;
	const float jumpVelocity = -500.0f;
	static constexpr float fixedTimestep = 1.0f / 60.0f;

	uint32_t score = 0;
	uint32_t high_score = 0;

	Vector2 playerPos;

	float playerVelY;
	char playerDirX;

	unsigned char shields;

	std::vector<GameObject> leftWallObjects;
	std::vector<GameObject> rightWallObjects;

	Wallbounce();
	~Wallbounce();

	void Run();

	void ClearObjects(unsigned char wall = 0);
	void GenerateLava(unsigned char wall = 0);

	void RandSeed(uint64_t seed);
	uint64_t Rand();
};
} // namespace wb
