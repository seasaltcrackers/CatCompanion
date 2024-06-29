#pragma once
#include "object.h"

class CCat;
class CFocusManager;
class CMonitorManager;
class CTexture;
class CColourPicker;
class CDemoSprite;
class CTextBox;
class CSlider;
class CurrentConfig;
class CConfigs;
class CCatManager;

class CCatPropertiesWindow :
    public CObject
{
public:
    CCatPropertiesWindow(CCatManager* catManager);

    void Update() override;
    void Draw(glm::mat4& pv = CCamera::GetProjViewMatrix()) override;

    void SetVisible(CCat* cat);

private:
    enum class WindowState
    {
        Waiting,
        BeingDragged,
    };

    void UpdateFocus();
    void UpdateMoveWindow();
    void UpdateColours();
    void DrawColours();

    void Apply();
    void Save();

    const glm::ivec4 DragArea = glm::ivec4(0, 0, 85, 14);
    const glm::ivec4 WindowArea = glm::ivec4(0, 12, 155, 145);

    CTexture* ColourBoxTexture;

    glm::vec3 Colours[100];
    int NumberOfColours;

    int CurrentColour;
    CColourPicker* ColourPicker;
    CDemoSprite* DemoSprite;
    CTextBox* NameBox;
    CSlider* ScaleSlider;

    std::vector<CObject*> Children;
    CCat* ParentCat;

    CFocusManager& FM;
    CMonitorManager& MM;
    CConfigs* CurrentConfig;

    WindowState State;
    glm::vec3 DragOffset;

    int WindowScale;
    float CatScale = 2.0f;

    bool IsVisible;
};

