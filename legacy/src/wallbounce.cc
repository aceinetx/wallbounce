#include "wallbounce.hh"

using namespace wb;

Wallbounce::Wallbounce() : rng(dev()) {
	InitWindow(screenWidth, screenHeight, "wallbounce");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	RandSeed(std::time(0));

	Image shield = LoadImage("../assets/shield.png");
	shieldTexture = LoadTextureFromImage(shield);
	UnloadImage(shield);

	dist_wall = std::uniform_int_distribution<std::mt19937::result_type>(0, screenWidth / 50 - 2);

	textInputFocused = true;
}

Wallbounce::~Wallbounce() {
	UnloadTexture(shieldTexture);
	CloseWindow();
}

void Wallbounce::ClearObjects(unsigned char wall) {
	if (wall == 0)
		leftWallObjects.clear();
	if (wall == 1)
		rightWallObjects.clear();
	return;
}

void Wallbounce::Run() {
	wallLeft = (Rectangle){.x = 0.0f, .y = 0.0f, .width = 20.0f, .height = (float)screenHeight};
	wallRight = (Rectangle){.x = (float)screenWidth - 20.0f, .y = 0.0f, .width = 20.0f, .height = (float)screenHeight};
	while (!WindowShouldClose()) {
		delta = GetFrameTime();

		if (gameState == GameState::PLAYING) {
			accumulator += delta;

			while (accumulator >= fixedTimestep) {
				UpdatePhysics();
				accumulator -= fixedTimestep;
			}
		}

		BeginDrawing();
		rlImGuiBegin();

		if (textInputFocused) {
			ClearBackground(RAYWHITE);
			ImGui::SetNextWindowPos({0, screenHeight / 2.0f});
			ImGui::SetNextWindowSize({(float)screenWidth, (float)screenHeight});
			ImGui::Begin("TextInput", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

			ImGui::TextColored({0, 0, 0, 255}, "%s", textInput.c_str());

			for (int i = 0; i <= 9; i++) {
				if (ImGui::Button(TextFormat("%d", i), {screenWidth / 12.195121951219512f, 30})) {
					textInput += std::to_string(i);
				}
				ImGui::SameLine();
			}
			ImGui::Text("");

			std::vector<std::vector<char>> rows = {{'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'}, {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'}, {'z', 'x', 'c', 'v', 'b', 'n', 'm'}};
			for (auto row : rows) {
				for (char ch : row) {
					if (ImGui::Button(TextFormat("%c", ch), {screenWidth / 12.195121951219512f, 50})) {
						textInput.push_back(ch);
					}
					ImGui::SameLine();
				}
				ImGui::Text("");
			}

			ImGui::End();
		} else {
			switch (gameState) {
			case GameState::MAIN:
				MainStateFrame();
				break;
			case GameState::STOP:
				PausedStateFrame();
				break;
			case GameState::PLAYING:
				PlayingStateFrame();
				break;
			}
		}

		rlImGuiEnd();
		EndDrawing();
	}
}
