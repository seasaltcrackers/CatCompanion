#pragma once
#include "helper.h"

#include <vector>

namespace HELPER
{
    int GetHue(int red, int green, int blue);
    BOOL GetTopHues(HICON hIcon, std::vector<int>& hues);
}