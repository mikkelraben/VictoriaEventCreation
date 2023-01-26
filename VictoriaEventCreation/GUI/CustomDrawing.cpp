#include "pch.h"
#include "../Core/pch.h"
#include "CustomDrawing.h"
#include "../Core/imgui/imgui_internal.h"
#include "../Core/imgui/imgui_impl_vulkan.h"
#include "../Core/ResourceHandler.h"
#include "Widgets.h"

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }

static inline bool operator==(const ImVec2& lhs, const ImVec2& rhs)
{
    ImVec2 size = lhs - rhs;
    size.x = size.x * size.x;
    size.y = size.y * size.y;
    float length = sqrt(size.x + size.y);
    return length < 0.001f;
}


void GetAxisPosition(const size_t& axis, float& axisValue, float startValue, const ImVec2& MiddleValues, float endValue, float scale)
{
    if (axis == 0)
    {
        axisValue = startValue;
    }

    if (axis == 1)
    {
        axisValue = startValue + MiddleValues.x * 1 / scale;
    }

    if (axis == 2)
    {
        axisValue = endValue - MiddleValues.y * 1 / scale;
    }

    if (axis == 3)
    {
        axisValue = endValue;
    }
}

void WriteIndexNineSlice(ImDrawIdx* IndexWrite, const ImDrawIdx& index, ImDrawList* draw_list, const int& numberOfVertices)
{
    for (size_t i = 0; i < 9; i++)
    {
        int indexOffset = i + (i / 3);

        IndexWrite[i * 6 + 0] = index + 0 + indexOffset;
        IndexWrite[i * 6 + 1] = index + 1 + indexOffset;
        IndexWrite[i * 6 + 2] = index + 4 + indexOffset;
        IndexWrite[i * 6 + 3] = index + 1 + indexOffset;
        IndexWrite[i * 6 + 4] = index + 5 + indexOffset;
        IndexWrite[i * 6 + 5] = index + 4 + indexOffset;
    }

    draw_list->_VtxWritePtr += numberOfVertices;
    draw_list->_VtxCurrentIdx += numberOfVertices;
    draw_list->_IdxWritePtr += 54;
}

void PrimNineGridUV(const ImVec2& startPos, const ImVec2& endPos, const ImVec4& middleRect, const ImVec2& uv_a, const ImVec4& middleUvRect, const ImVec2& uv_c, ImU32 col, float scale)
{
    auto draw_list = ImGui::GetWindowDrawList();
    auto VertexCurrentIndex = draw_list->_VtxCurrentIdx;
    ImDrawIdx index = (ImDrawIdx)VertexCurrentIndex;

    auto IndexWrite = draw_list->_IdxWritePtr;
    auto VertexWrite = draw_list->_VtxWritePtr;

    const int numberOfVertices = 16;

    //define vertices
    for (size_t x = 0; x < 4; x++)
    {
        for (size_t y = 0; y < 4; y++)
        {
            ImVec2 position;
            ImVec2 uv;

            GetAxisPosition(x, position.x, startPos.x, { middleRect.x,middleRect.z }, endPos.x, scale);
            GetAxisPosition(y, position.y, startPos.y, { middleRect.y,middleRect.w }, endPos.y, scale);

            GetAxisPosition(x, uv.x, uv_a.x, { middleUvRect.x,middleUvRect.z }, uv_c.x, 1);
            GetAxisPosition(y, uv.y, uv_a.y, { middleUvRect.y,middleUvRect.w }, uv_c.y, 1);

            VertexWrite[x * 4 + y].pos = position; VertexWrite[x * 4 + y].uv = uv; VertexWrite[x * 4 + y].col = col;
        }
    }

    WriteIndexNineSlice(IndexWrite, index, draw_list, numberOfVertices);
}

void PrimNineGridUVAlphaMask(const ImVec2& startPos, const ImVec2& endPos, const ImVec4& middleRect, const ImVec2& uv_a_start, const ImVec4& middleUvAlphaRect, const ImVec2& uv_a_end, const ImVec2& uv_start, const ImVec2& uv_end, ImU32 col, float scale)
{
    auto draw_list = ImGui::GetWindowDrawList();
    auto VertexCurrentIndex = draw_list->_VtxCurrentIdx;
    ImDrawIdx index = (ImDrawIdx)VertexCurrentIndex;

    auto IndexWrite = draw_list->_IdxWritePtr;
    auto VertexWrite = draw_list->_VtxWritePtr;

    const int numberOfVertices = 16;

    //define vertices
    for (size_t x = 0; x < 4; x++)
    {
        for (size_t y = 0; y < 4; y++)
        {
            ImVec2 position;
            ImVec2 uvAlpha;
            ImVec2 uv;

            GetAxisPosition(x, position.x, startPos.x, { middleRect.x,middleRect.z }, endPos.x, scale);
            GetAxisPosition(y, position.y, startPos.y, { middleRect.y,middleRect.w }, endPos.y, scale);

            GetAxisPosition(x, uvAlpha.x, uv_a_start.x, { middleUvAlphaRect.x,middleUvAlphaRect.z }, uv_a_end.x, 1);
            GetAxisPosition(y, uvAlpha.y, uv_a_start.y, { middleUvAlphaRect.y,middleUvAlphaRect.w }, uv_a_end.y, 1);

            ImVec2 size = { endPos.x - startPos.x,endPos.y - startPos.y };
            ImVec2 offset = { position.x - startPos.x,position.y - startPos.y };

            uv.x = offset.x / size.x * uv_end.x;
            uv.y = offset.y / size.y * uv_end.y;

            VertexWrite[x * 4 + y].pos = position; VertexWrite[x * 4 + y].alphaUV = uvAlpha; VertexWrite[x * 4 + y].col = col; VertexWrite[x * 4 + y].uv = uv;
        }
    }

    WriteIndexNineSlice(IndexWrite, index, draw_list, numberOfVertices);
}

void PrimRectUVAlpha(const ImVec2& startPos, const ImVec2& endPos, const ImVec2& uv_min, const ImVec2& uv_max, const ImVec2& uv_a_min, const ImVec2& uv_a_max, ImU32 col)
{
    auto draw_list = ImGui::GetWindowDrawList();
    auto VertexCurrentIndex = draw_list->_VtxCurrentIdx;
    ImDrawIdx index = (ImDrawIdx)VertexCurrentIndex;

    auto IndexWrite = draw_list->_IdxWritePtr;
    auto VertexWrite = draw_list->_VtxWritePtr;

    const int numberOfVertices = 4;


    ImVec2 b(endPos.x, startPos.y), d(startPos.x, endPos.y);
    ImVec2 uv_b(uv_max.x, uv_min.y), uv_d(uv_min.x, uv_max.y);
    ImVec2 uv_a_b(uv_a_max.x, uv_a_min.y), uv_a_d(uv_a_min.x, uv_a_max.y);


    IndexWrite[0] = index; IndexWrite[1] = (ImDrawIdx)(index + 1); IndexWrite[2] = (ImDrawIdx)(index + 2);
    IndexWrite[3] = index; IndexWrite[4] = (ImDrawIdx)(index + 2); IndexWrite[5] = (ImDrawIdx)(index + 3);
    VertexWrite[0].pos = startPos;  VertexWrite[0].uv = uv_min; VertexWrite[0].col = col; VertexWrite[0].alphaUV = uv_a_min;
    VertexWrite[1].pos = b;         VertexWrite[1].uv = uv_b;   VertexWrite[1].col = col; VertexWrite[1].alphaUV = uv_a_b;
    VertexWrite[2].pos = endPos;    VertexWrite[2].uv = uv_max; VertexWrite[2].col = col; VertexWrite[2].alphaUV = uv_a_max;
    VertexWrite[3].pos = d;         VertexWrite[3].uv = uv_d;   VertexWrite[3].col = col; VertexWrite[3].alphaUV = uv_a_d;
    draw_list->_VtxWritePtr += numberOfVertices;
    draw_list->_VtxCurrentIdx += numberOfVertices;
    draw_list->_IdxWritePtr += 6;

}


void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color, float scale, BlendMode colorBlend)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec4 middleUvSlice;

    middleUvSlice.x = middleSlice.x / texture.width;
    middleUvSlice.y = middleSlice.y / texture.height;
    middleUvSlice.z = middleSlice.z / texture.width;
    middleUvSlice.w = middleSlice.w / texture.height;

    const bool push_texture_id = texture.textureID != draw_list->_CmdHeader.TextureId;
    if (push_texture_id)
        draw_list->PushTextureID(texture.textureID);

    //draw_list->AddCallback(drawCallback, texture.textureID);
    draw_list->PrimReserve(54, 16);
    PrimNineGridUV(pos, pos + size, middleSlice, uvMin, middleUvSlice, uvMax, color, scale);

    if (push_texture_id)
        draw_list->PopTextureID();

}

void DrawNineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax, ImColor color, float scale, BlendMode colorBlend)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec4 middleUvSlice;

    middleUvSlice.x = middleSlice.x / alphaMask.width;
    middleUvSlice.y = middleSlice.y / alphaMask.height;
    middleUvSlice.z = middleSlice.z / alphaMask.width;
    middleUvSlice.w = middleSlice.w / alphaMask.height;

    const bool push_texture_id = texture.textureID != draw_list->_CmdHeader.TextureId;
    if (push_texture_id)
        draw_list->PushTextureID(texture.textureID);

    draw_list->PrimReserve(54, 16);
    PrimNineGridUVAlphaMask(pos, pos + size, middleSlice, uvAlphaMin, middleUvSlice, uvAlphaMax, uvMin, uvMax, color, scale);
    draw_list->CmdBuffer.back().AlphaId = alphaMask.textureID;
    draw_list->CmdBuffer.back().Overlay = colorBlend;


    if (push_texture_id)
        draw_list->PopTextureID();

}

void DrawImageAlphaMask(Texture& texture, Texture& alphamask, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, const ImVec2& uv_a_min, const ImVec2& uv_a_max, BlendMode colorBlend)
{
    auto draw_list = ImGui::GetWindowDrawList();

    const bool push_texture_id = texture.textureID != draw_list->_CmdHeader.TextureId;
    if (push_texture_id)
        draw_list->PushTextureID(texture.textureID);

    draw_list->PrimReserve(6, 4);
    PrimRectUVAlpha(p_min, p_max, uv_min, uv_max, uv_a_min, uv_a_max, IM_COL32_WHITE);
    draw_list->CmdBuffer.back().AlphaId = alphamask.textureID;
    draw_list->CmdBuffer.back().Overlay = colorBlend;

    if (push_texture_id)
        draw_list->PopTextureID();
}

void VictoriaWindow::RenderWindowDecorations(ImGuiWindow* window, const ImRect& title_bar_rect, bool title_bar_is_highlight, bool handle_borders_and_resize_grips, int resize_grip_count, const ImU32 resize_grip_col[4], float resize_grip_draw_size)
{
    const static auto mainTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\popup_bg_frame.dds");
    const static auto backgroundTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\popup_bg.dds");
    const static auto velvetOverlayPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\textures\\velvet_texture.dds");

    const static auto defaultBackground = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\default_bg.dds");
    const static auto smallFramePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\bg_frame_small.dds");
    const static auto smallFrameMaskPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\bg_frame_small_mask.dds");

    const static auto TooltipPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\tooltip\\tooltip_bg.dds");
    const static auto TooltipFramePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\tooltip\\tooltip_frame.dds");
    const static auto TooltipShadingPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\default_bg_shading.dds");
    const static auto ClothOverlayPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\textures\\cloth_texture.dds");

    const static auto headerMask = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\popup_bg_frame_mask.dds");
    const static auto headerColor = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\header_color.dds");
    const static auto headerShading = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\top_header_bg_shading.dds");
    const static auto FancyPattern2 = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\fancy_pattern_2.dds");
    const static auto HeaderDivider = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\popup_header_divider.dds");



    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImGuiWindowFlags flags = window->Flags;

    // Ensure that ScrollBar doesn't read last frame's SkipItems
    IM_ASSERT(window->BeginCount == 0);
    window->SkipItems = false;

    // Draw window + handle manual resize
    // As we highlight the title bar when want_focus is set, multiple reappearing windows will have their title bar highlighted on their reappearing frame.
    const float window_rounding = window->WindowRounding;
    const float window_border_size = window->WindowBorderSize;
    if (window->Collapsed)
    {
        // Title bar only
        const float backup_border_size = style.FrameBorderSize;
        g.Style.FrameBorderSize = window->WindowBorderSize;
        ImU32 title_bar_col = ImGui::GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBgCollapsed);
        if (window->ViewportOwned)
            title_bar_col |= IM_COL32_A_MASK; // No alpha (we don't support is_docking_transparent_payload here because simpler and less meaningful, but could with a bit of code shuffle/reuse)
        ImGui::RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
        g.Style.FrameBorderSize = backup_border_size;
    }
    else
    {
        // Window background
        if (!(flags & ImGuiWindowFlags_NoBackground))
        {
            bool is_docking_transparent_payload = false;
            if (g.DragDropActive && (g.FrameCount - g.DragDropAcceptFrameCount) <= 1 && g.IO.ConfigDockingTransparentPayload)
                if (g.DragDropPayload.IsDataType(IMGUI_PAYLOAD_TYPE_WINDOW) && *(ImGuiWindow**)g.DragDropPayload.Data == window)
                    is_docking_transparent_payload = true;

            ImU32 bg_col = ImGui::GetColorU32(ImGuiCol_WindowBg);
            //if (window->ViewportOwned)
            //{
            //    bg_col |= IM_COL32_A_MASK; // No alpha
            //    if (is_docking_transparent_payload)
            //        window->Viewport->Alpha *= DOCKING_TRANSPARENT_PAYLOAD_ALPHA;
            //}
            //else
            //{
            //    // Adjust alpha. For docking
            //    bool override_alpha = false;
            //    float alpha = 1.0f;
            //    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasBgAlpha)
            //    {
            //        alpha = g.NextWindowData.BgAlphaVal;
            //        override_alpha = true;
            //    }
            //    if (is_docking_transparent_payload)
            //    {
            //        alpha *= DOCKING_TRANSPARENT_PAYLOAD_ALPHA; // FIXME-DOCK: Should that be an override?
            //        override_alpha = true;
            //    }
            //    if (override_alpha)
            //        bg_col = (bg_col & ~IM_COL32_A_MASK) | (IM_F32_TO_INT8_SAT(alpha) << IM_COL32_A_SHIFT);
            //}

            // Render, for docked windows and host windows we ensure bg goes before decorations
            if (window->DockIsActive)
                window->DockNode->LastBgColor = bg_col;
            ImDrawList* bg_draw_list = window->DockIsActive ? window->DockNode->HostWindow->DrawList : window->DrawList;
            if (window->DockIsActive || (flags & ImGuiWindowFlags_DockNodeHost))
                bg_draw_list->ChannelsSetCurrent(DOCKING_HOST_DRAW_CHANNEL_BG);

            if (!(window->DC.ChildWindows.Size == 1 && window->DockOrder == -1 && window->ViewportId == 0x11111111))
            {
                ImGuiWindowFlags IsChild = ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_Tooltip;
                ImGuiWindowFlags isDock = ImGuiWindowFlags_DockNodeHost;

                float windowWidth = window->Rect().GetSize().x;
                float windowHeight = window->Rect().GetSize().y;
                if (flags & ImGuiWindowFlags_NoTitleBar || window->DockIsActive)
                {
                    //if (window->DockIsActive)
                    //{
                    //    ImGui::PushClipRect({ window->TitleBarRect().Min.x, window->TitleBarRect().Min.y + g.FontSize + 9.0f * 2 + 8 }, { window->Rect().Max.x,window->Rect().Max.y }, true);
                    //}
                    //else
                    {
                        ImGui::PushClipRect({ window->TitleBarRect().Min.x, window->TitleBarRect().Max.y }, { window->Rect().Max.x,window->Rect().Max.y }, true);
                    }
                }
                //background
                if (!(flags & IsChild) || flags & VictoriaEventWindow)
                {
                    DrawNineSliceImage(*backgroundTexturePath.get(), window->Pos, window->Size, { 162,162,162,162 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                    DrawNineSliceImage(*velvetOverlayPath.get(), *backgroundTexturePath.get(), window->Pos, window->Size, { 162,162,162,162 }, { 0,0 }, { windowWidth / velvetOverlayPath.get()->width * 2.0f ,windowHeight / velvetOverlayPath.get()->height * 2.0f }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::overlay);
                }
                else if (flags & ImGuiWindowFlags_Tooltip)
                {
                    DrawNineSliceImage(*TooltipPath.get(), window->Pos, window->Size, { 48,48,48,48 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                    DrawNineSliceImage(*TooltipShadingPath.get(), *backgroundTexturePath.get(), window->Pos, window->Size, { 48,48,48,48 }, { 0,0 }, { 1,1 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::overlay);
                    DrawNineSliceImage(*velvetOverlayPath.get(), *backgroundTexturePath.get(), window->Pos, window->Size, { 48,48,48,48 }, { 0,0 }, { windowWidth / velvetOverlayPath.get()->width * 2.0f ,windowHeight / velvetOverlayPath.get()->height * 2.0f }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::overlay);
                    DrawNineSliceImage(*ClothOverlayPath.get(), *backgroundTexturePath.get(), window->Pos, window->Size, { 48,48,48,48 }, { 0,0 }, { 1,1 }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.15f }, 2, BlendMode::overlay);
                }
                else
                {
                    DrawNineSliceImage(*defaultBackground.get(), *smallFrameMaskPath.get(), window->Pos, window->Size, { 90,90,90,90 }, { 0,0 }, { windowWidth / defaultBackground.get()->width * 2.0f ,windowHeight / defaultBackground.get()->height * 2.0f }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                    DrawNineSliceImage(*velvetOverlayPath.get(), *smallFrameMaskPath.get(), window->Pos, window->Size, { 90,90,90,90 }, { 0,0 }, { windowWidth / velvetOverlayPath.get()->width * 2.0f ,windowHeight / velvetOverlayPath.get()->height * 2.0f }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::overlay);
                }

                //Header Background
                if (!(flags & ImGuiWindowFlags_NoTitleBar) && !window->DockIsActive || flags & VictoriaEventWindow)
                {
                    
                    float headerWidth = window->TitleBarRect().GetSize().x;
                    float headerHeight = window->TitleBarRect().GetSize().y;
                    ImVec2 titleBarMin = window->TitleBarRect().Min;
                    ImVec2 titleBarSize = window->TitleBarRect().GetSize();

                    headerHeight = 82;
                    titleBarMin = window->Rect().Min;
                    titleBarSize = { headerWidth, headerHeight };

                    if (flags & VictoriaEventWindow)
                    {
                        headerWidth = 1190;
                    }
                    

                    DrawNineSliceImage(*headerColor.get(), *headerMask.get(), titleBarMin, titleBarSize, { 162,162,162,162 }, { 0,0 }, { headerWidth / headerColor.get()->width,1 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                    DrawNineSliceImage(*velvetOverlayPath.get(), *headerMask.get(), titleBarMin, titleBarSize, { 162,162,162,162 }, { 0,0 }, { headerWidth / velvetOverlayPath.get()->width * 2.0f ,headerHeight / velvetOverlayPath.get()->height * 2.0f }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.1f }, 2, BlendMode::overlay);
                    if (ImGui::IsWindowFocused() || flags & VictoriaEventWindow)
                    {
                        DrawNineSliceImage(*headerShading.get(), *headerMask.get(), titleBarMin, titleBarSize, { 162,162,162,162 }, { 0,0 }, { 1,1 }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.3f }, 2, BlendMode::overlay);
                    }
                    DrawNineSliceImage(*FancyPattern2.get(), *headerMask.get(), titleBarMin, titleBarSize, { 162,162,162,162 }, { 0,0 }, { headerWidth / FancyPattern2.get()->width * 2.0f,headerHeight / FancyPattern2.get()->height * 2.0f }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.2f }, 2, BlendMode::overlay);
                    DrawNineSliceImage(*ClothOverlayPath.get(), *headerMask.get(), titleBarMin, titleBarSize, { 162,162,162,162 }, { 0,0 }, { headerWidth / ClothOverlayPath.get()->width * 2,headerHeight / ClothOverlayPath.get()->height * 2 }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.1f }, 2, BlendMode::overlay);

                    VecGui::Image({ titleBarMin.x,titleBarMin.y + titleBarSize.y - 12 }, *HeaderDivider.get(), { titleBarSize.x - 16,8 }, { 0,0 }, { headerWidth / HeaderDivider.get()->width * 2, 1 }, ImColor{ 1.0f,1.0f,1.0f,1.0f }, true, BlendMode::normal);

                }

                //Foreground
                if (!(flags & IsChild) || flags & VictoriaEventWindow)
                {
                    DrawNineSliceImage(*mainTexturePath.get(), window->Pos, window->Size, { 162,162,162,162 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                }
                else if (flags & ImGuiWindowFlags_Tooltip)
                {
                    DrawNineSliceImage(*TooltipFramePath.get(), window->Pos, window->Size, { 16,16,16,16 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                }
                else
                {
                    DrawNineSliceImage(*smallFramePath.get(), window->Pos, window->Size, { 90,90,90,90 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
                }

                if (flags & ImGuiWindowFlags_NoTitleBar || window->DockIsActive)
                {
                    ImGui::PopClipRect();
                }



            }


            bg_draw_list->AddRectFilled(window->Pos, window->Pos + window->Size, bg_col, window_rounding, (flags & ImGuiWindowFlags_NoTitleBar) ? 0 : ImDrawFlags_RoundCornersBottom);



            if (window->DockIsActive || (flags & ImGuiWindowFlags_DockNodeHost))
                bg_draw_list->ChannelsSetCurrent(DOCKING_HOST_DRAW_CHANNEL_FG);
        }
        if (window->DockIsActive)
            window->DockNode->IsBgDrawnThisFrame = true;

        // Title bar
        // (when docked, DockNode are drawing their own title bar. Individual windows however do NOT set the _NoTitleBar flag,
        // in order for their pos/size to be matching their undocking state.)
        if (!(flags & ImGuiWindowFlags_NoTitleBar) && !window->DockIsActive)
        {
            ImU32 title_bar_col = ImGui::GetColorU32(title_bar_is_highlight ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg);
            //window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, ImColor(0,0,0,0), window_rounding, ImDrawFlags_RoundCornersTop);
        }

        // Menu bar
        if (flags & ImGuiWindowFlags_MenuBar)
        {
            ImRect menu_bar_rect = window->MenuBarRect();
            menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
            window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), ImGui::GetColorU32(ImGuiCol_MenuBarBg), (flags & ImGuiWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawFlags_RoundCornersTop);
            if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
                window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), ImGui::GetColorU32(ImGuiCol_Border), style.FrameBorderSize);
        }

        // Docking: Unhide tab bar (small triangle in the corner), drag from small triangle to quickly undock
        ImGuiDockNode* node = window->DockNode;
        if (window->DockIsActive && node->IsHiddenTabBar() && !node->IsNoTabBar())
        {
            float unhide_sz_draw = ImFloor(g.FontSize * 0.70f);
            float unhide_sz_hit = ImFloor(g.FontSize * 0.55f);
            ImVec2 p = node->Pos;
            ImRect r(p, p + ImVec2(unhide_sz_hit, unhide_sz_hit));
            ImGuiID unhide_id = window->GetID("#UNHIDE");
            ImGui::KeepAliveID(unhide_id);
            bool hovered, held;
            if (ImGui::ButtonBehavior(r, unhide_id, &hovered, &held, ImGuiButtonFlags_FlattenChildren))
                node->WantHiddenTabBarToggle = true;
            else if (held && ImGui::IsMouseDragging(0))
                ImGui::StartMouseMovingWindowOrNode(window, node, true);

            // FIXME-DOCK: Ideally we'd use ImGuiCol_TitleBgActive/ImGuiCol_TitleBg here, but neither is guaranteed to be visible enough at this sort of size..
            ImU32 col = ImGui::GetColorU32(((held && hovered) || (node->IsFocused && !hovered)) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            window->DrawList->AddTriangleFilled(p, p + ImVec2(unhide_sz_draw, 0.0f), p + ImVec2(0.0f, unhide_sz_draw), col);
        }

        // Scrollbars
        if (window->ScrollbarX)
            ImGui::Scrollbar(ImGuiAxis_X);
        if (window->ScrollbarY)
            ImGui::Scrollbar(ImGuiAxis_Y);

        // Borders (for dock node host they will be rendered over after the tab bar)
    }
}

// Render title text, collapse button, close button
// When inside a dock node, this is handled in DockNodeCalcTabBarLayout() instead.
void VictoriaWindow::RenderWindowTitleBarContents(ImGuiWindow* window, const ImRect& title_bar_rect, const char* name, bool* p_open)
{
    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    ImGuiWindowFlags flags = window->Flags;

    const bool has_close_button = (p_open != NULL);
    const bool has_collapse_button = !(flags & ImGuiWindowFlags_NoCollapse) && (style.WindowMenuButtonPosition != ImGuiDir_None);

    // Close & Collapse button are on the Menu NavLayer and don't default focus (unless there's nothing else on that layer)
    // FIXME-NAV: Might want (or not?) to set the equivalent of ImGuiButtonFlags_NoNavFocus so that mouse clicks on standard title bar items don't necessarily set nav/keyboard ref?
    const ImGuiItemFlags item_flags_backup = g.CurrentItemFlags;
    g.CurrentItemFlags |= ImGuiItemFlags_NoNavDefaultFocus;
    window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;






    ImFont* headerFont = ImGui::GetIO().Fonts->Fonts[2];
    ImGui::PushFont(headerFont);


    // Layout buttons
    // FIXME: Would be nice to generalize the subtleties expressed here into reusable code.
    float pad_l = style.FramePadding.x;
    float pad_r = style.FramePadding.x;
    float button_sz = 47;
    ImVec2 close_button_pos;
    ImVec2 collapse_button_pos;
    if (has_close_button)
    {
        pad_r += button_sz;
        close_button_pos = ImVec2(title_bar_rect.Max.x - pad_r - style.FramePadding.x, title_bar_rect.Min.y);
    }
    if (has_collapse_button && style.WindowMenuButtonPosition == ImGuiDir_Right)
    {
        pad_r += button_sz;
        collapse_button_pos = ImVec2(title_bar_rect.Max.x - pad_r - style.FramePadding.x, title_bar_rect.Min.y);
    }
    if (has_collapse_button && style.WindowMenuButtonPosition == ImGuiDir_Left)
    {
        collapse_button_pos = ImVec2(title_bar_rect.Min.x + pad_l - style.FramePadding.x, title_bar_rect.Min.y);
        pad_l += button_sz;
    }

    // Collapse button (submitting first so it gets priority when choosing a navigation init fallback)
    if (has_collapse_button)
        if (ImGui::CollapseButton(window->GetID("#COLLAPSE"), collapse_button_pos, NULL))
            window->WantCollapseToggle = true; // Defer actual collapsing to next frame as we are too far in the Begin() function



    const static auto background = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\round_button_bg.dds");

    // Close button
    if (has_close_button)
    {
        ImVec2 position = ImGui::GetCursorScreenPos();
        close_button_pos = close_button_pos + ImVec2{ 0,14 };
        VecGui::Image(close_button_pos - ImVec2{ 3,3 }, *background.get(), { 53,53 }, { 0,0 }, { 1,1 }, { 255,255,255,255 }, true);
        if (VictoriaWindow::CloseButton("##CLOSE", { 47,47 }, close_button_pos))
        {
            *p_open = false;
        }

    }

    window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
    g.CurrentItemFlags = item_flags_backup;

    // Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
    // FIXME: Refactor text alignment facilities along with RenderText helpers, this is WAY too much messy code..
    const float marker_size_x = (flags & ImGuiWindowFlags_UnsavedDocument) ? button_sz * 0.80f : 0.0f;
    const ImVec2 text_size = ImGui::CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);

    // As a nice touch we try to ensure that centered title text doesn't get affected by visibility of Close/Collapse button,
    // while uncentered title text will still reach edges correctly.
    if (pad_l > style.FramePadding.x)
        pad_l += g.Style.ItemInnerSpacing.x;
    if (pad_r > style.FramePadding.x)
        pad_r += g.Style.ItemInnerSpacing.x;
    if (style.WindowTitleAlign.x > 0.0f && style.WindowTitleAlign.x < 1.0f)
    {
        float centerness = ImSaturate(1.0f - ImFabs(style.WindowTitleAlign.x - 0.5f) * 2.0f); // 0.0f on either edges, 1.0f on center
        float pad_extend = ImMin(ImMax(pad_l, pad_r), title_bar_rect.GetWidth() - pad_l - pad_r - text_size.x);
        pad_l = ImMax(pad_l, pad_extend * centerness);
        pad_r = ImMax(pad_r, pad_extend * centerness);
    }

    ImRect layout_r(title_bar_rect.Min.x + pad_l, title_bar_rect.Min.y, title_bar_rect.Max.x - pad_r, title_bar_rect.Max.y);
    ImRect clip_r(layout_r.Min.x, layout_r.Min.y, ImMin(layout_r.Max.x + g.Style.ItemInnerSpacing.x, title_bar_rect.Max.x), layout_r.Max.y);
    if (flags & ImGuiWindowFlags_UnsavedDocument)
    {
        ImVec2 marker_pos;
        marker_pos.x = ImClamp(layout_r.Min.x + (layout_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x + text_size.x, layout_r.Min.x, layout_r.Max.x);
        marker_pos.y = (layout_r.Min.y + layout_r.Max.y) * 0.5f;
        if (marker_pos.x > layout_r.Min.x)
        {
            ImGui::RenderBullet(window->DrawList, marker_pos, ImGui::GetColorU32(ImGuiCol_Text));
            clip_r.Max.x = ImMin(clip_r.Max.x, marker_pos.x - (int)(marker_size_x * 0.5f));
        }
    }
    //if (g.IO.KeyShift) window->DrawList->AddRect(layout_r.Min, layout_r.Max, IM_COL32(255, 128, 0, 255)); // [DEBUG]
    //if (g.IO.KeyCtrl) window->DrawList->AddRect(clip_r.Min, clip_r.Max, IM_COL32(255, 128, 0, 255)); // [DEBUG]
    ImGui::RenderTextClipped(layout_r.Min, layout_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_r);
    ImGui::PopFont();
}

void VictoriaWindow::RenderDockTabBar(ImGuiWindow* window, const ImRect& title_bar_rect, ImGuiDockNode* node)
{
    const static auto defaultBackground = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\default_bg.dds");

    const static auto smallFramePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\bg_frame_small.dds");
    const static auto smallFrameMaskPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\bg_frame_small_mask.dds");
    const static auto velvetOverlayPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\textures\\velvet_texture.dds");

    float windowWidth = node->Rect().GetSize().x;
    float windowHeight = node->Rect().GetSize().y;
    ImGui::PushClipRect(title_bar_rect.Min + ImVec2{ 0,0 }, title_bar_rect.Max, true);

    DrawNineSliceImage(*defaultBackground.get(), *smallFrameMaskPath.get(), node->Pos, node->Size, { 90,90,90,90 }, { 0,0 }, { windowWidth / defaultBackground.get()->width * 2.0f ,windowHeight / defaultBackground.get()->height * 2.0f }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);
    DrawNineSliceImage(*velvetOverlayPath.get(), *smallFrameMaskPath.get(), node->Pos, node->Size, { 90,90,90,90 }, { 0,0 }, { windowWidth / velvetOverlayPath.get()->width * 2.0f ,windowHeight / velvetOverlayPath.get()->height * 2.0f }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::overlay);

    DrawNineSliceImage(*smallFramePath.get(), node->Pos, node->Size, { 90,90,90,90 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 2, BlendMode::normal);

    ImGui::PopClipRect();
}

// Render A Tab
void VictoriaWindow::TabItemBackground(ImDrawList* draw_list, const ImRect& bb, ImGuiTabItemFlags flags, ImU32 col, bool held, bool hovered, bool tab_contents_visible)
{
    const static auto mainButton = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\tabs\\tab_button_1.dds");
    const static auto tabSelected = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\tabs\\tab_button_1_selected.dds");
    const static auto velvetOverlayPath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\textures\\velvet_texture.dds");

    bool loaded = true;

    if (!loaded)
    {
        // While rendering tabs, we trim 1 pixel off the top of our bounding box so they can fit within a regular frame height while looking "detached" from it.
        ImGuiContext& g = *GImGui;
        const float width = bb.GetWidth();
        IM_UNUSED(flags);
        IM_ASSERT(width > 0.0f);
        const float rounding = ImMax(0.0f, ImMin((flags & ImGuiTabItemFlags_Button) ? g.Style.FrameRounding : g.Style.TabRounding, width * 0.5f - 1.0f));
        const float y1 = bb.Min.y + 1.0f;
        const float y2 = bb.Max.y + ((flags & ImGuiTabItemFlags_Preview) ? 0.0f : -1.0f);
        draw_list->PathLineTo(ImVec2(bb.Min.x, y2));
        draw_list->PathArcToFast(ImVec2(bb.Min.x + rounding, y1 + rounding), rounding, 6, 9);
        draw_list->PathArcToFast(ImVec2(bb.Max.x - rounding, y1 + rounding), rounding, 9, 12);
        draw_list->PathLineTo(ImVec2(bb.Max.x, y2));
        draw_list->PathFillConvex(col);
        if (g.Style.TabBorderSize > 0.0f)
        {
            draw_list->PathLineTo(ImVec2(bb.Min.x + 0.5f, y2));
            draw_list->PathArcToFast(ImVec2(bb.Min.x + rounding + 0.5f, y1 + rounding + 0.5f), rounding, 6, 9);
            draw_list->PathArcToFast(ImVec2(bb.Max.x - rounding - 0.5f, y1 + rounding + 0.5f), rounding, 9, 12);
            draw_list->PathLineTo(ImVec2(bb.Max.x - 0.5f, y2));
            draw_list->PathStroke(ImGui::GetColorU32(ImGuiCol_Border), 0, g.Style.TabBorderSize);
        }
        return;
    }

    if (!tab_contents_visible)
    {
        ImVec2 uvMin = { 0,0 };

        const ImVec2 frameSize = { 80,44 };
        const ImVec2 frameSizeUV = { frameSize / ImVec2{(float)mainButton.get()->width,(float)mainButton.get()->height} };

        ImVec2 uvMax = frameSizeUV;

        if (hovered)
        {
            uvMin = { frameSizeUV.x,0 };
            uvMax = { frameSizeUV.x * 2,1 };
        }
        DrawNineSliceImage(*mainButton.get(), bb.Min, bb.GetSize(), { 10,30,10,9 }, uvMin, uvMax, IM_COL32_WHITE, 1, BlendMode::normal);
    }
    else
    {
        DrawNineSliceImage(*tabSelected.get(), bb.Min, bb.GetSize(), { 10,30,10,9 }, { 0,0 }, { 1,1 }, IM_COL32_WHITE, 1, BlendMode::normal);
    }

}

void VictoriaWindow::TabSeperator(ImGuiTabBar* tab_bar, ImGuiWindow* window)
{
    const static auto seperatorTexture = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\tabs\\tab_divider.dds");
    float separator_min_x = tab_bar->BarRect.Min.x + tab_bar->WidthAllTabs;
    float separator_max_x = tab_bar->BarRect.Max.x + 29;

    if (window->DockNode)
    {
        auto dock_node = window->DockNode;
        separator_min_x = dock_node->Pos.x + window->WindowBorderSize;
        separator_max_x = dock_node->Pos.x + dock_node->Size.x - window->WindowBorderSize;

    }
    VecGui::Image({ separator_min_x,tab_bar->BarRect.Min.y }, *seperatorTexture.get(), { separator_max_x - separator_min_x,tab_bar->BarRect.GetHeight() });
}

bool VictoriaWindow::CloseButton(std::string_view id, const ImVec2& size, const ImVec2& pos)
{
    ImVec2 position = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(pos);

    bool pressed = false;
    ImVec2 actualSize = size;
    if (size == ImVec2(0, 0))
    {
        actualSize = ImVec2(ImGui::GetFontSize() + 6, ImGui::GetFontSize() + 6);
    }
    std::string new_id = id.data();

    if (VecGui::RoundButton({ "#CLOSE" + new_id }, "close", actualSize, { 0,0 }, { 1,1 }, true))
    {
        pressed = true;
    }

    ImGui::SetCursorScreenPos(position);
    return pressed;
}

bool VictoriaWindow::CloseButton(ImGuiID id, const ImVec2& size, const ImVec2& pos)
{
    ImVec2 position = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(pos);

    bool pressed = false;
    ImVec2 actualSize = size;
    if (size == ImVec2(0, 0))
    {
        actualSize = ImVec2(ImGui::GetFontSize() + 6, ImGui::GetFontSize() + 6);
    }

    if (VecGui::RoundButton(id, "close", actualSize, { 0,0 }, { 1,1 }, true))
    {
        pressed = true;
    }

    ImGui::SetCursorScreenPos(position);
    return pressed;

}
