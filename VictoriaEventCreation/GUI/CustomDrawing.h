#pragma once
#include "../Core/Texture.h"
#include "../Core/imgui/imgui.h"

void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax);

void GetAxisPosition(const size_t& axis, float& axisValue, float startValue, const ImVec2& MiddleValues, float endValue);
