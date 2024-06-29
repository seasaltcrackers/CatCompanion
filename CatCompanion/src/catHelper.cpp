#include "catHelper.h"
#include <algorithm>

int HELPER::GetHue(int red, int green, int blue) 
{
    float min = std::min(std::min(red, green), blue);
    float max = std::max(std::max(red, green), blue);

    if (min == max) 
    {
        return 0;
    }

    float hue = 0.0f;

    if (max == red) 
    {
        hue = (green - blue) / (max - min);
    }
    else if (max == green) 
    {
        hue = 2.0f + (blue - red) / (max - min);
    }
    else 
    {
        hue = 4.0f + (red - green) / (max - min);
    }

    hue = hue * 60;
    if (hue < 0) hue = hue + 360;

    return std::round(hue);
}

BOOL HELPER::GetTopHues(HICON hIcon, std::vector<int>& hues)
{
    hues.resize(360);

    HDC hMainDC = NULL;
    HDC hMemDC1 = NULL;
    BITMAP bmp;
    HBITMAP hOldBmp1 = NULL;
    ICONINFO csII;
    BOOL bRetValue = FALSE;

    bRetValue = ::GetIconInfo(hIcon, &csII);
    if (bRetValue == FALSE) return NULL;

    hMainDC = ::GetDC(NULL);
    hMemDC1 = ::CreateCompatibleDC(hMainDC);
    if (hMainDC == NULL || hMemDC1 == NULL) return NULL;

    if (::GetObject(csII.hbmColor, sizeof(BITMAP), &bmp))
    {
        DWORD   dwWidth = csII.xHotspot * 2;
        DWORD   dwHeight = csII.yHotspot * 2;

        hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, csII.hbmColor);

        DWORD    dwLoopY = 0, dwLoopX = 0;
        COLORREF crPixel = 0;
        BYTE     byNewPixel = 0;

        for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++)
        {
            for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++)
            {
                crPixel = ::GetPixel(hMemDC1, dwLoopX, dwLoopY);

                float r = GetRValue(crPixel);
                float g = GetGValue(crPixel);
                float b = GetBValue(crPixel);

                int hue = GetHue(r, g, b);
                hues[hue] += 1;
            }
        }

        ::SelectObject(hMemDC1, hOldBmp1);
    }

    ::DeleteObject(csII.hbmColor);
    ::DeleteObject(csII.hbmMask);
    ::DeleteDC(hMemDC1);
    ::ReleaseDC(NULL, hMainDC);

    return TRUE;
}
