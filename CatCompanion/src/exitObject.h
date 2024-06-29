#pragma once
#include <object.h>
#include <chrono>

class CFont;

class CExitObject :
    public CObject
{
public:
    CExitObject();

    void Update() override;
    void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;

private:
    // Time required for the application to quit
    const float TotalTimeRequired = 3.0f;
    const float AlphaRate = 2.0f;
    const float FuzzyTime = 0.2f;

    // Dimensions of the bar
    const float Height = 25.0f;
    const float Width = 150.0f;
    const float Padding = 10.0f;;

    // Colour of the bar
    const glm::vec3 ColourLeft = glm::vec3(0.78f, 0.78f, 0.78f);
    const glm::vec3 ColourRight = glm::vec3(0.4f, 0.4f, 0.4f);

    // Used for calculations
    std::chrono::steady_clock::time_point HoldStartTime;
    float HeldTime;
    float Alpha;
    bool IsHeld;

    const std::string Text = "EXITING";
    glm::ivec2 TextBounds;
    CFont* Font;
};

