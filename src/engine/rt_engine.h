#pragma once

#include "engine/engine.h"

class RTEngine : public Engine {

public:

	int initialize(Renderer* renderer) override;
    void clean() override;

	void update(float delta_time) override;
	void render() override;
};
