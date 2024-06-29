#include <set>
#include <SOIL.h>
#include <filesystem>

#include "catManager.h"
#include "cat.h"
#include "helper.h"
#include "configs.h"
#include "monitorManager.h"
#include "input.h"

#define MOUSE_PRESS_TIME 0.25f
#define CAT_FILE_LOCATION "Resources\\Cats\\"


CCatManager::CCatManager(CScene* parent)
{
	Parent = parent;

	GenerateCatConfigSettings();
	CalculateHues();
	LoadCats();
}

CCatManager::~CCatManager()
{
	for (CCat* cat : Cats) { delete cat; }
	Cats.clear();

	delete CatConfigSettings;
	CatConfigSettings = nullptr;
}

void CCatManager::Update()
{
	//for (CCat* cat : Cats) { cat->Update(); }
	int count = Cats.size();
	for (CCat* cat : Cats) { cat->Update(); }
	UpdateMouseInput();
}

void CCatManager::Render()
{
	for (CCat* cat : Cats) { cat->Draw(); }
}

void CCatManager::UpdateMouseInput()
{
	switch (CurrentMouseState)
	{
		case MouseState::Nothing:
		{
			if (GINPUT::GetLMButtonState() == InputState::INPUT_DOWN_FIRST)
			{
				if (GetHoveredCat(CurrentlySelectedCat))
				{
					CurrentMouseState = MouseState::LMB;
					(*CurrentlySelectedCat)->OnLMBDown();
				}
			}
			else if (GINPUT::GetRMButtonState() == InputState::INPUT_DOWN_FIRST)
			{
				if (GetHoveredCat(CurrentlySelectedCat))
				{
					CurrentMouseState = MouseState::RMB;
					RClickTimePoint = std::chrono::steady_clock::now();
					(*CurrentlySelectedCat)->OnRMBDown();
					RMBHeld = false;
				}
			}

			break;
		}

		case MouseState::LMB:
		{
			if (GINPUT::GetLMButtonState() == InputState::INPUT_UP_FIRST)
			{
				CurrentMouseState = MouseState::Nothing;
				(*CurrentlySelectedCat)->OnLMBUp();
			}

			break;
		}

		case MouseState::RMB:
		{
			if (GINPUT::GetRMButtonState() == InputState::INPUT_UP_FIRST)
			{
				CurrentMouseState = MouseState::Nothing;
				(*CurrentlySelectedCat)->OnRMBUp();

				if (!RMBHeld) { (*CurrentlySelectedCat)->OnRMBPressed(); }
			}
			else if (!RMBHeld)
			{
				float duration = std::chrono::duration<float>(std::chrono::steady_clock::now() - RClickTimePoint).count();

				if (duration >= MOUSE_PRESS_TIME)
				{
					(*CurrentlySelectedCat)->OnRMBHeld();
					RMBHeld = true;
				}
			}

			break;
		}
	}
}

void CCatManager::LoadCats()
{
	CMonitorManager& mm = CMonitorManager::GetInstance();
	std::vector<HMONITOR>& monitors = mm.GetMonitors();
	MonitorInfo info = mm.GetMonitorDetail((int)monitors[0]);

	std::vector<std::string> filenames;

	for (const auto& entry : std::filesystem::directory_iterator(CAT_FILE_LOCATION))
	{
		filenames.push_back(entry.path().stem().u8string());
	}

	int count = filenames.size();

	if (count == 0)
	{
		CCat* cat = new CCat(this, new CConfigs(*CatConfigSettings), glm::vec2(info.Width * 0.5f, info.Height - info.TaskBarHeight));
		Cats.push_back(cat);
		cat->SetListIterator(--Cats.end());
	}
	else
	{
		for (int i = 0; i < filenames.size(); ++i)
		{
			float percentage = (float)i / (float)count + ((1.0f / (float)count) * 0.5f);

			std::string filename = filenames[i];
			CConfigs* configs = new CConfigs(*CatConfigSettings, filename);
			CCat* cat = new CCat(this, configs, glm::vec2(info.Width * percentage, info.Height - info.TaskBarHeight));
			Cats.push_back(cat);
			cat->SetListIterator(--Cats.end());
		}
	}
}

CCat* CCatManager::AddCat(glm::vec2 location, CConfigs* config)
{
	CCat* cat = new CCat(this, new CConfigs(config), location);
	Cats.push_back(cat);
	cat->SetListIterator(--Cats.end());
	return cat;
}

CCat* CCatManager::AddCat(glm::vec2 location)
{
	CCat* cat = new CCat(this, new CConfigs(*CatConfigSettings), location);
	Cats.push_back(cat);
	cat->SetListIterator(--Cats.end());
	return cat;
}

CCat* CCatManager::GetFirstCat()
{
	return *Cats.begin();
}

void CCatManager::DeleteCat(std::list<CCat*>::iterator it)
{
	(*it)->GetConfigs()->Delete();
	delete (*it);
	Cats.erase(it);

	if (Cats.empty())
	{
		exit(0);
	}
}

bool CCatManager::GetHoveredCat(std::list<CCat*>::iterator& it)
{
	glm::vec2 mouseLoc = GINPUT::GetMouseLocation();

	it = Cats.begin();

	while (it != Cats.end())
	{
		if ((*it)->IsInBounds(mouseLoc))
		{
			return true;
		}

		it++;
	}

	return false;
}

void CCatManager::GenerateCatConfigSettings()
{
	CatConfigSettings = new CConfigSettings();
	CatConfigSettings->SetDefaultFilename("");
	CatConfigSettings->SetFileLocation(CAT_FILE_LOCATION);

	// Floats
	CatConfigSettings->AddDefaultFloatVariable("max-jump-height", 400.0f);
	CatConfigSettings->AddDefaultFloatVariable("walk-speed", 35.0f);
	CatConfigSettings->AddDefaultFloatVariable("scale", 2.0f);

	// Strings
	CatConfigSettings->AddDefaultStringVariable("name", "");
	CatConfigSettings->AddDefaultStringVariable("colours[1]", "#da6666");
	CatConfigSettings->AddDefaultStringVariable("colours[2]", "#787878");
	CatConfigSettings->AddDefaultStringVariable("colours[3]", "#c6c6c6");
	CatConfigSettings->AddDefaultStringVariable("colours[4]", "#5a5a5a");
	CatConfigSettings->AddDefaultStringVariable("colours[5]", "#1f1f1f");
	CatConfigSettings->AddDefaultStringVariable("colours[6]", "#f7cede");
}

void CCatManager::CalculateHues()
{
	const std::string ImageFilePath = "Resources\\Images\\";
	const std::vector<std::string> FileNames = {
		"SleepingSpritesheet",
		"IdleSpritesheet",
		"CarriedSpritesheet",
		"CatEatingSpritesheet",
		"CatRolling",
		"CatSliding",
		"CatSpriteSheet_v1",
		"JumpEndSpritesheet",
		"JumpMiddleSpritesheet",
		"JumpStartSpritesheet",
	};

	std::set<int> hues;
	hues.insert(-1); // White colour

	std::map<int, float> maxValue;
	maxValue.insert({ -1, 1.0f });

	struct ImageData
	{
		std::string Filename;
		unsigned char* Bits;
		int Width;
		int Height;
	};

	std::vector<ImageData> images;

	for (std::string filename : FileNames)
	{
		int width;
		int height;

		unsigned char* image = SOIL_load_image((ImageFilePath + filename + ".png").c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

		int byteWidth = width * 4;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < byteWidth; x += 4)
			{
				int offset = y * byteWidth + x;
				int a = image[offset + 3];

				// Only count hue if the pixel has alpha
				if (a > 0)
				{
					int r = image[offset + 0];
					int g = image[offset + 1];
					int b = image[offset + 2];

					// Make sure the pixel is not greyscale
					if (r != g || g != b)
					{
						// Calculate hue and value
						glm::vec3 hsv = HELPER::RGBtoHSV(r, g, b);

						int size = hues.size();

						int hue = std::round(hsv.x);
						std::map<int, float>::iterator it = maxValue.find(hue);

						if (it == maxValue.end())
						{
							maxValue.insert(std::make_pair(hue, hsv.z));
							hues.insert(hue);
						}
						else
						{
							it->second = std::max(it->second, hsv.z);
						}

						if (size != hues.size())
						{
							int i = 0;
						}
					}
				}
			}
		}

		images.push_back(ImageData{ filename, image, width, height });
	}

	NumberOfColours = hues.size() - 1;

	for (ImageData image : images)
	{
		int byteWidth = image.Width * 4;

		for (int y = 0; y < image.Height; ++y)
		{
			for (int x = 0; x < byteWidth; x += 4)
			{
				int offset = y * byteWidth + x;
				int a = image.Bits[offset + 3];

				// Only count hue if the pixel has alpha
				if (a > 0)
				{
					int r = image.Bits[offset + 0];
					int g = image.Bits[offset + 1];
					int b = image.Bits[offset + 2];

					// Make sure the pixel is not greyscale
					if (r != g || g != b)
					{
						// Calculate hue and value
						glm::vec3 hsv = HELPER::RGBtoHSV(r, g, b);

						if (hsv.x == 0)
						{
							int i = 0;
						}

						// Generate a lookup index for the lookup texture
						int hue = std::round(hsv.x);
						int index = std::distance(hues.begin(), hues.find(hue));
						image.Bits[offset + 0] = index;

						// Remap value to the brightest pixel of the given hue
						float value = hsv.z * (1.0f / maxValue[hue]);
						image.Bits[offset + 1] = value * 255.0f;
					}
					else
					{
						// Set lookup to the first slot (always white)
						// float lookupPercentage = (1.0f / 100.0f);
						image.Bits[offset + 0] = 0;//lookupPercentage * 255.0f;
					}
				}
			}
		}

		GLuint id;

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		// Nearest pixel will be chosen (good for pixel art)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width, image.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Bits);

		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image.Bits);

		glBindTexture(GL_TEXTURE_2D, 0);

		CTexture::SetTexture(image.Filename, new CTexture(image.Filename, image.Width, image.Height, id));
	}

}
