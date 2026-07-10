#pragma once
#include "pch.hh"

namespace wb {
class Wallbounce {
public:
	typedef enum class GameState { MAIN, STOP, PLAYING } GameState;

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
	void MainStateFrame();
	void PausedStateFrame();
	void PlayingStateFrame();
	void TextInputFrame();

	void UpdatePhysics();

	void FocusInput();

	float delta;
	float accumulator = 0.0f;

	Rectangle playerRect;

	Rectangle wallLeft;
	Rectangle wallRight;

	uint64_t randSeed;

	bool shieldCollided = false;

	Texture2D shieldTexture;

	std::random_device dev;
	std::mt19937 rng;
	std::uniform_int_distribution<std::mt19937::result_type> dist_wall;

	std::string textInput;
	bool textInputFocused;

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
	void Generate(unsigned char wall = 0);

	void RandSeed(uint64_t seed);
	uint64_t Rand();
};
} // namespace wb
