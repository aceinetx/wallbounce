#include "wallbounce.hh"

using namespace wb;

void Wallbounce::Generate(unsigned char wall) {
	if (wall == 0)
		ClearObjects(wall);
	else if (wall == 1) {
		ClearObjects(wall);
	}

	size_t tiles = screenHeight / 50;

	std::vector<size_t> skip;
	// Generate an empty space
	{
		// size_t y = Rand() % (tiles - 2);
		size_t y = dist_wall(rng);
		skip.push_back(y);
		skip.push_back(y + 1);
		skip.push_back(y - 1);
	}

	// Generate a shield
	int shieldPos = -1;
	if ((Rand() % 10) == 1) {
		// shieldPos = Rand() % (tiles - 2);
		shieldPos = dist_wall(rng);
		for (;;) {
			bool invalid = false;
			for (size_t i : skip) {
				if (i - 1 == shieldPos || i - 2 == shieldPos || i == shieldPos || i + 1 == shieldPos || i + 2 == shieldPos) {
					invalid = true;
					break;
				}
			}
			if (!invalid)
				break;
			shieldPos = dist_wall(rng);
		}
		skip.push_back(shieldPos);
		skip.push_back(shieldPos + 1);

		GameObject obj;
		obj.type = GameObject::SHIELD;
		obj.rect.y = 50.0f * shieldPos + 25.0f;
		obj.rect.height = 50;

		if (wall == 0) {
			obj.rect.x = 20;
			obj.rect.width = 10;
			leftWallObjects.push_back(obj);
		} else if (wall == 1) {
			obj.rect.x = screenWidth - 50;
			obj.rect.width = 10;
			rightWallObjects.push_back(obj);
		}
	}

	// Generate lava
	for (size_t i = 0; i < tiles; i++) {
		if (std::find(skip.begin(), skip.end(), i) != skip.end())
			continue;

		GameObject obj;
		obj.type = GameObject::LAVA;
		obj.rect.y = 50.0f * i;
		obj.rect.height = 50;

		if (wall == 0) {
			obj.rect.x = 20;
			obj.rect.width = 10;
			leftWallObjects.push_back(obj);
		} else if (wall == 1) {
			obj.rect.x = screenWidth - 30;
			obj.rect.width = 10;
			rightWallObjects.push_back(obj);
		}
	}
}
