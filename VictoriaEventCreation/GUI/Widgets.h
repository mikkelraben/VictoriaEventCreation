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
    bool Button(std::string_view id, ImVec2 size = {-1,-1});

    bool ImageButton(std::string_view id, Texture& texture, const ImVec2& size);
    void Image(Texture& texture, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE);
    void Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE);
    void Image(ImVec2 Pos, Texture& texture, Texture& alphaMask, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImVec2 uvAlphaMin = { 0,0 }, ImVec2 uvAlphaMax = { 0,0 }, ImColor color = IM_COL32_WHITE);
    void NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE);
    void NineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImVec2 uvAlphaMin = { 0,0 }, ImVec2 uvAlphaMax = { 0,0 },ImColor color = IM_COL32_WHITE, bool isFloating = false);
    void NoImageBehaviour(ImVec2& cursor, ImVec2& size, ImDrawList* draw_list, bool isFloating = false);
    void NineSliceImageFloating(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE);
}

