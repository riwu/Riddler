#include "ImageManager.hpp"

int ImageManager::image_count = 0;

std::uint32_t ColourSwap(std::uint32_t Colour)
{
    return ((Colour & 0xFF000000) | ((Colour & 0xFF0000) >> 16) | (Colour & 0x00FF00) | ((Colour & 0x0000FF) << 16));
}

std::uint32_t ImageManager::CreateImage(int Width, int Height)
{
    imagemap.insert(std::make_pair(++image_count, std::unique_ptr<CG::Image>(new CG::Image(Width, Height))));
    return image_count;
}

std::uint32_t ImageManager::LoadImage(const char* FilePath)
{
    imagemap.insert(std::make_pair(++image_count, std::unique_ptr<CG::Image>(new CG::Image(FilePath))));
    return image_count;
}

std::uint32_t ImageManager::ImageFromPointer(std::uint8_t* Pointer, int Width, int Height)
{
    imagemap.insert(std::make_pair(++image_count, std::unique_ptr<CG::Image>(new CG::Image((CG::BGRA*)Pointer, Width, Height))));
    return image_count;
}

#if defined _WIN32 || defined _WIN64
std::uint32_t ImageManager::ImageFromCursor()
{
    CURSORINFO Info = {0};
    Info.cbSize = sizeof(Info);
    GetCursorInfo(&Info);
    imagemap.insert(std::make_pair(++image_count, std::unique_ptr<CG::Image>(new CG::Image(Info))));
    return image_count;
}

std::uint32_t ImageManager::ImageFromWindow(HWND Window, int X, int Y, int Width, int Height)
{
    imagemap.insert(std::make_pair(++image_count, std::unique_ptr<CG::Image>(new CG::Image(Window, X, Y, Width, Height))));
    return image_count;
}

std::uint32_t ImageManager::ImageFromDC(HDC DC, int X, int Y, int Width, int Height)
{
    imagemap.insert(std::make_pair(++image_count, std::unique_ptr<CG::Image>(new CG::Image(DC, X, Y, Width, Height))));
    return image_count;
}

void ImageManager::DrawToWindow(std::uint32_t ImageID, HWND Hwnd, int X, int Y, int Width, int Height, int SourceX, int SourceY, DWORD RasterType)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {
        it->second->Draw(Hwnd, X, Y, Width, Height, SourceX, SourceY, RasterType);
    }
}
#endif

void ImageManager::SaveImage(std::uint32_t ImageID, const char* FilePath)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {
        it->second->Save(FilePath);
    }
}

void ImageManager::DeleteImage(std::uint32_t ImageID)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {
        imagemap.erase(it);
    }
}

void ImageManager::Clear()
{
    imagemap.clear();
    decltype(imagemap)().swap(imagemap);
    image_count = 0;
}

std::uint32_t ImageManager::GetPixel(std::uint32_t ImageID, int X, int Y)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {
        return it->second->GetPixel(X, Y).Colour;
    }
    return 0;
}

CG::BGRA* ImageManager::GetPixels(std::uint32_t ImageID, int &Width, int &Height)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {
        Width = it->second->Width();
        Height = it->second->Height();
        return it->second->GetPixels();
    }
    return nullptr;
}

void ImageManager::SetPixel(std::uint32_t ImageID, std::uint32_t Colour, int X, int Y)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {

        it->second->SetPixel(X, Y, Colour);
    }
}

void ImageManager::GetDimensions(std::uint32_t ImageID, int &Width, int &Height)
{
    auto it = imagemap.find(ImageID);
    if (it != imagemap.end())
    {
        Width = it->second->Width();
        Height = it->second->Height();
    }
}
