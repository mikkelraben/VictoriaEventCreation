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

    struct Id
    {
        Id(std::string_view _id) { id = ImGui::GetID(_id.data()); };
        Id(ImGuiID _id) { id = _id; };
        ImGuiID id;

        operator ImGuiID() { return id; };
    };


    bool Button(std::string_view id, ImVec2 size = {-1,-1});
    bool RoundButton(Id id, std::string_view icon, const ImVec2& size, const ImVec2& uvMin = {0,0}, const ImVec2& uvMax = {1,1}, bool isCloseTab = false);
    void CheckBox(std::string_view id, bool& value);
    bool SliderInt(std::string_view id, int& value, int min, int max);
    bool SliderFloat(std::string_view id, float& value, float min, float max);

    bool SliderScalar(std::string_view label, ImGuiDataType type, void* value, const void* min, const void* max);
    bool ImageButton(std::string_view id, Texture& texture, const ImVec2& size, const ImVec2 & uvMin = { 0,0 }, const ImVec2& uvMax = { 1,1 });
    void Image(Texture& texture, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE, bool isFloating = false, BlendMode colorBlend = BlendMode::normal);
    void Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE, bool floating = false, BlendMode colorBlend = BlendMode::normal);
    void Image(ImVec2 Pos, Texture& texture, Texture& alphaMask, ImVec2 size, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImVec2 uvAlphaMin = { 0,0 }, ImVec2 uvAlphaMax = { 0,0 }, ImColor color = IM_COL32_WHITE, bool floating = false, BlendMode colorBlend = BlendMode::normal);
    void NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImColor color = IM_COL32_WHITE, float scale = 1, bool isFloating = false, BlendMode colorBlend = BlendMode::normal);
    void NineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 size, VecRect middleSlice, ImVec2 uvMin = { 0,0 }, ImVec2 uvMax = { 1,1 }, ImVec2 uvAlphaMin = { 0,0 }, ImVec2 uvAlphaMax = { 0,0 },ImColor color = IM_COL32_WHITE, float scale = 1,bool isFloating = false, BlendMode colorBlend = BlendMode::normal);
    void NoImageBehaviour(ImVec2& cursor, ImVec2& size, ImDrawList* draw_list, bool isFloating = false);
}

