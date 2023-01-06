#pragma once
#include "../Core/Texture.h"
#include "../Core/imgui/imgui.h"

void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color);

void DrawNineSliceImage(Texture& texture, Texture& alphaMask,ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax,ImVec2 uvAlphaMin,ImVec2 uvAlphaMax,ImColor color, bool overlay = false);

void GetAxisPosition(const size_t& axis, float& axisValue, float startValue, const ImVec2& MiddleValues, float endValue);

void WriteIndexNineSlice(ImDrawIdx* IndexWrite, const ImDrawIdx& index, ImDrawList* draw_list, const int& numberOfVertices);

void DrawImageAlphaMask(Texture& texture, Texture& alphamask, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, const ImVec2& uv_a_min, const ImVec2& uv_a_max, bool overlay = false);
