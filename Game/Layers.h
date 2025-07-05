#pragma once
#include "Jimara.h"



namespace Game {
	enum class Layer : uint8_t {
		DEFAULT = 0,
		GROUND = 1,
		WALL = 8,
		FOLIAGE = 16,
		PLAYER = 24
	};

	inline static void Game_SetLayerNames() {
		::Jimara::Layers::Writer layers(::Jimara::Layers::Main());
		layers[static_cast<Jimara::Layer>(Layer::DEFAULT)] = "DEFAULT";
		layers[static_cast<Jimara::Layer>(Layer::GROUND)] = "GROUND";
		layers[static_cast<Jimara::Layer>(Layer::WALL)] = "WALL";
		layers[static_cast<Jimara::Layer>(Layer::FOLIAGE)] = "FOLIAGE";
		layers[static_cast<Jimara::Layer>(Layer::PLAYER)] = "PLAYER";
	}
}
