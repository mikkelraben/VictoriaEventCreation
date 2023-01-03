#pragma once
#include "../Core/imgui/imgui.h"
#include "../Core/Texture.h"


namespace VecGui
{
    const static ImColor noImageColor = { 255,0,255 };

    struct VecRect
    {
        ImVec2 min;
        ImVec2 max;


    };
    bool Button(std::string_view id, const ImVec2& size = { 76, 38 });

    bool ImageButton(std::string_view id, Texture& texture, const ImVec2& size);
    void Image(Texture& texture, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 });
    void Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 });
    void NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 });
    void NineSliceImageFloating(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 });
}

