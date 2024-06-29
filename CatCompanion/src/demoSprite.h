#pragma once
#include "sprite.h"

class CDemoSprite :
    public CSprite
{
public:
    CDemoSprite(CSprite* parentSprite, glm::vec2 location);

    void Update() override;

private:
    void OnAnimationEnd() override;

    void PlayAnimation(std::string name);
    void IncrementAnimation();

    std::map<std::string, SSpriteAnimation>::iterator CurrentAnimation;
};

