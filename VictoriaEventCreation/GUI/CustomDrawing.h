#pragma once
#include "../Core/Texture.h"
#include "../Core/imgui/imgui.h"

void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color, float scale = 1);

void DrawNineSliceImage(Texture& texture, Texture& alphaMask,ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax,ImVec2 uvAlphaMin,ImVec2 uvAlphaMax,ImColor color, float scale = 1, bool overlay = false);

void GetAxisPosition(const size_t& axis, float& axisValue, float startValue, const ImVec2& MiddleValues, float endValue, float scale);

void WriteIndexNineSlice(ImDrawIdx* IndexWrite, const ImDrawIdx& index, ImDrawList* draw_list, const int& numberOfVertices);

void DrawImageAlphaMask(Texture& texture, Texture& alphamask, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, const ImVec2& uv_a_min, const ImVec2& uv_a_max, bool overlay = false);

struct ImGuiWindow;
struct ImRect;

namespace VictoriaWindow
{
    void RenderWindowDecorations(ImGuiWindow* window, const ImRect& title_bar_rect, bool title_bar_is_highlight, bool handle_borders_and_resize_grips, int resize_grip_count, const ImU32 resize_grip_col[4], float resize_grip_draw_size);
    void RenderWindowTitleBarContents(ImGuiWindow* window, const ImRect& title_bar_rect, const char* name, bool* p_open);
}
