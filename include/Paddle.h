// Copyright (c) Eric Jeker. All Rights Reserved.

#pragma once

#include "SceneNode.h"

class Paddle final : public SceneNode
{
public:
    Paddle();
    
    void init() override;
    void update(const float dt) override;
};
