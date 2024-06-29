#include "catPropertiesWindow.h"
#include "monitorManager.h"
#include "focusManager.h"
#include "program.h"
#include "mesh.h"
#include "texture.h"
#include "helper.h"
#include "input.h"
#include "textBox.h"
#include "font.h"
#include "slider.h"
#include "demoSprite.h"
#include "cat.h"
#include "graphics.h"
#include "colourPicker.h"
#include "button.h"
#include "configs.h"
#include "timer.h"
#include "catManager.h"

#include <fstream>

CCatPropertiesWindow::CCatPropertiesWindow(CCatManager* catManager) :
	CObject(CProgram::GetProgram("Texture"), CMesh::GetMesh("2D Square")),
	FM(CFocusManager::GetInstance()),
	MM(CMonitorManager::GetInstance())
{
	CTexture* mainTex = CTexture::GetTexture("PropertiesWindow");
	UpdateUniformTexture("tex", mainTex->GetID());

	ColourBoxTexture = CTexture::GetTexture("ColourBox");
	NumberOfColours = catManager->NumberOfColours;
	State = WindowState::Waiting;
	WindowScale = 2;
	IsVisible = false;

	Location = glm::vec3(1000.0f, 500.0f, 0.0f);
	Scale *= glm::vec3(mainTex->GetDimensions(), 1.0f) * (float)WindowScale;

	NameBox = new CTextBox(
		CFont::GetFont("AcknowledgeTTSmall"), 
		glm::vec2(39.0f, 21.0f) * (float)WindowScale, 
		111 * WindowScale,
		glm::vec3(89.0f, 89.0f, 89.0f) / glm::vec3(255.0f, 255.0f, 255.0f),
		glm::vec3(239.0f, 239.0f, 239.0f) / glm::vec3(255.0f, 255.0f, 255.0f));
	NameBox->SetText("");
	Children.push_back(NameBox);

	CatScale = 2.0f;
	ScaleSlider = new CSlider(CatScale, 1.0f, 4.0f, glm::vec2(40.0f, 34.0f) * (float)WindowScale, 107.0f * (float)WindowScale);
	Children.push_back(ScaleSlider);

	std::fill(std::begin(Colours), std::end(Colours), glm::vec3());
	for (int i = 0; i <= NumberOfColours; ++i)
	{
		Colours[i] = glm::vec3();
	}

	DemoSprite = new CDemoSprite(catManager->GetFirstCat(), glm::vec2(40.0f, 85.0f) * (float)WindowScale);
	DemoSprite->UpdateUniformfVec3Array("lookup", 100, glm::value_ptr(Colours[0]));
	Children.push_back(DemoSprite);

	ColourPicker = new CColourPicker(glm::vec2(210.0f, 70.0f) * (float)WindowScale, 150.0f);
	ColourPicker->SetIsVisible(false);
	Children.push_back(ColourPicker);

	CButton* applyButton = new CButton(nullptr, "", glm::vec3(), glm::vec2(5, 124) * (float)WindowScale, glm::vec2(), std::vector<CTexture*>{ CTexture::GetTexture("ApplyUp"), CTexture::GetTexture("ApplyDown") });
	applyButton->SetOnEnter([applyButton]() { applyButton->SetTexture(1); });
	applyButton->SetOnLeave([applyButton]() { applyButton->SetTexture(0); });
	applyButton->SetOnPress([this]() { Apply(); });
	applyButton->Scale *= 2.0f;
	applyButton->UpdateBounds();
	Children.push_back(applyButton);

	CButton* discardButton = new CButton(nullptr, "", glm::vec3(), glm::vec2(79, 124) * (float)WindowScale, glm::vec2(), std::vector<CTexture*>{ CTexture::GetTexture("DiscardUp"), CTexture::GetTexture("DiscardDown") });
	discardButton->SetOnEnter([discardButton]() { discardButton->SetTexture(1); });
	discardButton->SetOnLeave([discardButton]() { discardButton->SetTexture(0); });
	discardButton->SetOnPress([this]() { IsVisible = false; });
	discardButton->Scale *= 2.0f;
	discardButton->UpdateBounds();
	Children.push_back(discardButton);

	CButton* duplicateButton = new CButton(nullptr, "", glm::vec3(), glm::vec2(83, 0) * (float)WindowScale, glm::vec2(), std::vector<CTexture*>{ CTexture::GetTexture("DuplicateUp"), CTexture::GetTexture("DuplicateDown") });
	duplicateButton->SetOnEnter([duplicateButton]() { duplicateButton->SetTexture(1); });
	duplicateButton->SetOnLeave([duplicateButton]() { duplicateButton->SetTexture(0); });
	duplicateButton->SetOnHover([duplicateButton, this]() { FM.SetFocused(); });
	duplicateButton->SetOnPress([this]() { ParentCat->Duplicate(); });
	duplicateButton->Scale *= 2.0f;
	duplicateButton->UpdateBounds();
	Children.push_back(duplicateButton);


	CButton* binButton = new CButton(nullptr, "", glm::vec3(), glm::vec2(97, 0) * (float)WindowScale, glm::vec2(), std::vector<CTexture*>{ CTexture::GetTexture("BinUp"), CTexture::GetTexture("BinDown") });
	binButton->SetOnEnter([binButton]() { binButton->SetTexture(1); });
	binButton->SetOnLeave([binButton]() { binButton->SetTexture(0); });
	binButton->SetOnHover([binButton, this]() { FM.SetFocused(); });
	binButton->SetOnPress([this]() { IsVisible = false; ParentCat->Delete(); });
	binButton->Scale *= 2.0f;
	binButton->UpdateBounds();
	Children.push_back(binButton);
}

void CCatPropertiesWindow::Update()
{
	IfThenReturn(!IsVisible, );

	UpdateFocus();
	UpdateMoveWindow();
	UpdateColours();
	CObject::Update();

	for (CObject* obj : Children)
	{
		// Updates the model matrix in relation to this window
		obj->Location += Location;
		obj->Update();
		obj->Location -= Location;
	}

	DemoSprite->Scale = glm::one<glm::vec3>() * CatScale;
}

void CCatPropertiesWindow::Draw(glm::mat4& pv)
{
	IfThenReturn(!IsVisible, )

	CObject::Draw(pv);
	DrawColours();

	for (CObject* obj : Children)
	{
		// Draws the model matrix in relation to this window
		obj->Location += Location;
		obj->Draw(pv);
		obj->Location -= Location;
	}
}

void CCatPropertiesWindow::SetVisible(CCat* cat)
{
	Location = cat->Location - glm::vec3(0.0f, 200.0f, 0.0f) - Scale * 0.5f;
	Location = glm::clamp(Location, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(CCamera::GetScreenSize() - glm::vec2(Scale.x, Scale.y), 0.0f));

	ParentCat = cat;
	CurrentConfig = cat->GetConfigs();

	NumberOfColours = cat->GetNumberOfColours();
	std::fill(std::begin(Colours), std::end(Colours), glm::vec3());
	for (int i = 0; i <= NumberOfColours; ++i)
	{
		Colours[i] = cat->GetColour(i);
	}

	ScaleSlider->SetValue(ParentCat->Scale.x);
	NameBox->SetText(ParentCat->GetName());

	IsVisible = true;
}

void CCatPropertiesWindow::UpdateFocus()
{
	glm::ivec2 mouseLoc = GINPUT::GetMouseLocation() - glm::vec2(Location);

	if (HELPER::InBounds(mouseLoc, DragArea * WindowScale) ||
		HELPER::InBounds(mouseLoc, WindowArea * WindowScale))
	{
		FM.SetFocused();
	}
	else if (ColourPicker->GetIsVisible())
	{
		glm::vec2 delta = mouseLoc - glm::ivec2(ColourPicker->Location);
		float length = glm::length(delta);

		if (length <= ColourPicker->Scale.x * 0.55f)
		{
			FM.SetFocused();
		}
	}
}

void CCatPropertiesWindow::UpdateMoveWindow()
{
	InputState mouseState = GINPUT::GetLMButtonState();

	if (State == WindowState::Waiting)
	{
		if (mouseState == InputState::INPUT_DOWN_FIRST)
		{
			glm::ivec2 mouseLoc = GINPUT::GetMouseLocation() - glm::vec2(Location);

			if (HELPER::InBounds(mouseLoc, DragArea * WindowScale))
			{
				State = WindowState::BeingDragged;
				DragOffset = Location - glm::vec3(GINPUT::GetMouseLocation(), 0.0f);
			}
		}
	}
	
	if (State == WindowState::BeingDragged)
	{
		Location = glm::vec3(GINPUT::GetMouseLocation(), 0.0f) + DragOffset;

		if (mouseState == InputState::INPUT_UP ||
			mouseState == InputState::INPUT_UP_FIRST)
		{
			State = WindowState::Waiting;
		}
	}
}

void CCatPropertiesWindow::UpdateColours()
{
	glm::vec2 topLeft = glm::vec2(Location) + glm::vec2(80.0f, 51.0f) * (float)WindowScale;
	glm::vec2 botRight = topLeft + glm::vec2(69, 69) * (float)WindowScale;

	glm::vec2 mouseLoc = GINPUT::GetMouseLocation();
	bool inBounds = HELPER::InBounds(mouseLoc, topLeft, botRight);
	bool isPressed = GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST;

	if (inBounds && isPressed)
	{
		glm::vec2 texDimensions = ColourBoxTexture->GetDimensions() * WindowScale;
		glm::vec2 localLoc = (mouseLoc - topLeft);
		glm::vec2 boxLoc = (localLoc / (botRight - topLeft)) * glm::vec2(10.0f, 10.0f);
		glm::vec2 localBoxLoc = glm::vec2(std::fmodf(localLoc.x, texDimensions.x + 1.0f * WindowScale), std::fmodf(localLoc.y, texDimensions.y + 1.0f * WindowScale));

		if (localBoxLoc.x > 0.25f * (float)WindowScale &&
			localBoxLoc.y > 0.25f * (float)WindowScale &&
			localBoxLoc.x < texDimensions.x - 0.25f * (float)WindowScale &&
			localBoxLoc.y < texDimensions.y - 0.25f * (float)WindowScale)
		{
			int x = std::floorf(boxLoc.x);
			int y = std::floorf(boxLoc.y);
			int i = y * 11 + x + 1;

			if (i <= NumberOfColours)
			{
				ColourPicker->SetIsVisible(true);

				ColourPicker->Location += Location;
				ColourPicker->SetRGB(glm::abs(Colours[i]), Colours[i].x < 0 || Colours[i].y < 0 || Colours[i].z < 0);
				ColourPicker->Location -= Location;
				
				CurrentColour = i;
			}
		}
		else
		{
			ColourPicker->SetIsVisible(false);
		}
	}
	else if (!inBounds && isPressed && glm::length(glm::vec3(mouseLoc, 0.0f) - (ColourPicker->Location + Location)) > ColourPicker->Scale.x * 0.6f)
	{
		ColourPicker->SetIsVisible(false);
	}
	else if (ColourPicker->GetIsVisible())
	{
		bool isRainbow;
		Colours[CurrentColour] = ColourPicker->GetRGB(isRainbow);
		IfThenStatement(isRainbow, Colours[CurrentColour] *= -1.0f);
	}


	for (int i = 1; i <= NumberOfColours; ++i)
	{
		glm::vec3 colour = Colours[i];
			
		if (colour.r < 0 || colour.g < 0 || colour.b < 0)
		{
			// Increase hue of colour
			colour = glm::abs(colour) * 255.0f;
			glm::vec3 hsv = HELPER::RGBtoHSV(colour.r, colour.g, colour.b);
			hsv.x = std::fmodf(hsv.x + TIMER::GetDeltaSeconds() * 360.0f * 0.5f, 360.0f);
			Colours[i] = HELPER::HSVtoRGB(hsv.x, hsv.y, hsv.z) / -255.0f;
		}
	}
}

void CCatPropertiesWindow::DrawColours()
{
	glm::mat4 pv = CCamera::GetProjViewMatrix();
	glm::vec2 offset = glm::vec2(Location) + glm::vec2(80.0f, 51.0f) * (float)WindowScale;
	glm::vec2 scale = ColourBoxTexture->GetDimensions() * WindowScale;

	for (int i = 1; i <= NumberOfColours; ++i)
	{
		glm::vec2 loc = glm::vec2((i - 1) % 10, i / 10) * (scale + glm::vec2(1.0f, 1.0f) * (float)WindowScale);
		loc += offset;

		glm::mat4 model = GFX::CalculateModelMatrix(loc, scale);
		glm::mat4 pvm = pv * model;

		GFX::DrawRectangle(pv, glm::abs(Colours[i]), loc, scale);
		GFX::DrawImage(ColourBoxTexture->GetID(), pvm);
	}
}

void CCatPropertiesWindow::Apply()
{
	for (int i = 1; i <= NumberOfColours; ++i)
	{
		ParentCat->SetColour(i, Colours[i]);
	}

	ParentCat->SetName(NameBox->GetText());
	ParentCat->Scale = glm::vec3(CatScale, CatScale, 1.0f);
	IsVisible = false;

	Save();
}

void CCatPropertiesWindow::Save()
{
	CurrentConfig->SetFloatVariable("scale", CatScale);

	for (int i = 1; i <= NumberOfColours; ++i)
	{
		glm::vec3 colour = Colours[i];

		if (colour.r < 0 || colour.g < 0 || colour.b < 0)
		{
			colour = glm::abs(colour);
			std::string hex = HELPER::RGBtoHEX(colour.r * 255.0f, colour.g * 255.0f, colour.b * 255.0f);
			CurrentConfig->SetStringVariable("colours[" + std::to_string(i) + "]", hex + "!");
		}
		else
		{
			std::string hex = HELPER::RGBtoHEX(colour.r * 255.0f, colour.g * 255.0f, colour.b * 255.0f);
			CurrentConfig->SetStringVariable("colours[" + std::to_string(i) + "]", hex);
		}
	}

	// Saves file as well
	CurrentConfig->SetStringVariable("name", NameBox->GetText());
	CurrentConfig->SetFileName(NameBox->GetText());
}
