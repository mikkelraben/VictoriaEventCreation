#pragma once
#include "../Core/Texture.h"
#include "../Core/imgui/imgui.h"
#include <string_view>

void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color, float scale, BlendMode colorBlend);

void DrawNineSliceImage(Texture& texture, Texture& alphaMask,ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax,ImVec2 uvAlphaMin,ImVec2 uvAlphaMax,ImColor color, float scale, BlendMode colorBlend);

void DrawImageAlphaMask(Texture& texture, Texture& alphamask, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, const ImVec2& uv_a_min, const ImVec2& uv_a_max, BlendMode colorBlend);

void GetAxisPosition(const size_t& axis, float& axisValue, float startValue, const ImVec2& MiddleValues, float endValue, float scale);

void WriteIndexNineSlice(ImDrawIdx* IndexWrite, const ImDrawIdx& index, ImDrawList* draw_list, const int& numberOfVertices);

struct ImGuiWindow;
struct ImRect;
struct ImGuiTabBar;
struct ImGuiDockNode;

namespace VictoriaWindow
{
    void RenderWindowDecorations(ImGuiWindow* window, const ImRect& title_bar_rect, bool title_bar_is_highlight, bool handle_borders_and_resize_grips, int resize_grip_count, const ImU32 resize_grip_col[4], float resize_grip_draw_size);
    void RenderWindowTitleBarContents(ImGuiWindow* window, const ImRect& title_bar_rect, const char* name, bool* p_open);
    void RenderDockTabBar(ImGuiWindow* window, const ImRect& title_bar_rect, ImGuiDockNode* node);
    void TabItemBackground(ImDrawList* draw_list, const ImRect& bb, ImGuiTabItemFlags flags, ImU32 col, bool held, bool hovered, bool tab_contents_visible);
    void TabSeperator(ImGuiTabBar* tab_bar, ImGuiWindow* window);
    bool CloseButton(std::string_view id, const ImVec2& size, const ImVec2& pos);
    bool CloseButton(ImGuiID id, const ImVec2& size, const ImVec2& pos);
}
