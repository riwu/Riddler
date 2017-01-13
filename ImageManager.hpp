#ifndef IMAGEMANAGER_HPP_INCLUDED
#define IMAGEMANAGER_HPP_INCLUDED

#include "Images.hpp"
#include <map>
#include <memory>

class ImageManager
{
    private:
        static int image_count;
        std::map<std::uint32_t, std::unique_ptr<CG::Image>> imagemap;
        std::uint32_t ColourSwap(std::uint32_t Colour);
    public:
        std::uint32_t CreateImage(int Width, int Height);
        std::uint32_t LoadImage(const char* FilePath);
        std::uint32_t ImageFromPointer(std::uint8_t* Pointer, int Width, int Height);

        #if defined _WIN32 || defined _WIN64
        std::uint32_t ImageFromCursor();
        std::uint32_t ImageFromWindow(HWND Window, int X, int Y, int Width, int Height);
        std::uint32_t ImageFromDC(HDC DC, int X, int Y, int Width, int Height);
        void DrawToWindow(std::uint32_t ImageID, HWND Hwnd, int X, int Y, int Width, int Height, int SourceX, int SourceY, DWORD RasterType);
        #endif

        void SaveImage(std::uint32_t ImageID, const char* FilePath);
        void DeleteImage(std::uint32_t ImageID);
        void Clear();
        std::uint32_t GetPixel(std::uint32_t ImageID, int X, int Y);
        CG::BGRA* GetPixels(std::uint32_t ImageID, int &Width, int &Height);
        void SetPixel(std::uint32_t ImageID, std::uint32_t Colour, int X, int Y);
        void GetDimensions(std::uint32_t ImageID, int &Width, int &Height);

        inline std::size_t count() {return imagemap.size();}
};

#endif // IMAGEMANAGER_HPP_INCLUDED
