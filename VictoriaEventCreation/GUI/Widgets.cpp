#include "pch.h"
#include "../Core/pch.h"
#include "Widgets.h"
#include "CustomDrawing.h"
#include "../Core/imgui/imgui_internal.h"
#include "../Core/ResourceHandler.h"

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

bool VecGui::Button(std::string_view id, ImVec2 size)
{
    const static auto mainTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_bg.dds");
    const static auto frameTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_wood_border.dds");
    const static auto detailTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_texture.dds");
    const static auto bgGradienTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_bg_gradient.dds");
    const static auto bevelTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_bevel.dds");
    const static auto whiteTexture = ResourceHandler::GetTexture("gfx\\white.dds");

    ImVec2 cursor = ImGui::GetCursorScreenPos();
    auto& style = ImGui::GetStyle();

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 imSize;
    const ImGuiID imId = window->GetID(id.data());
    const ImVec2 label_size = ImGui::CalcTextSize(id.data(), NULL, true);
    if (size.x < 0 || size.y < 0)
    {
        imSize = ImGui::CalcItemSize(label_size, 16.0f, 16.0f);
        imSize = imSize + ImVec2{ 32, 32 };
    }
    else
    {
        imSize = size;
    }




    const ImRect bb(cursor, cursor + imSize);
    ImGui::ItemSize(imSize);
    if (!ImGui::ItemAdd(bb, imId))
        return false;


    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, imId, &hovered, &held, 0);


    ImDrawList* drawlist = ImGui::GetWindowDrawList();

    //ImU32 col = ImGui::GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
    ImVec2 center = { (cursor.x * 2 + imSize.x) / 2,(cursor.y * 2 + imSize.y) / 2 };
    ImVec2 uvMin = { 0,0 };

    if (hovered)
    {
        uvMin = { 38.0f / 114.0f,0 };
    }

    if (pressed)
    {
        uvMin = { 76.0f / 114.0,0 };
    }

    ImVec2 position = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(cursor);

    VecGui::NineSliceImageFloating(*mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, uvMin, { 38.0f / 114.0f + uvMin.x,1 }, { 255,255,255,(int)(255 * 0.7) },2);
    ImGui::SetCursorScreenPos(cursor);
    //VecGui::Image(cursor,*detailTexturePath.get(), *mainTexturePath.get(),size,{0,0}, { size.x/detailTexturePath.get()->width,size.y / detailTexturePath.get()->height }, uvMin, {38.0f / 114.0f + uvMin.x,1});
    VecGui::NineSliceImage(*frameTexturePath.get(), *mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1, 1 }, uvMin, { 38.0f / 114.0f + uvMin.x,1 }, { 255,255,255,(int)(255 * 0.7) },2,true);
    VecGui::NineSliceImage(*bgGradienTexturePath.get(), *mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1, 1 }, uvMin, { 38.0f / 114.0f + uvMin.x,1 }, { 255,255,255,(int)(255 * 0.5) },2,true);
    VecGui::NineSliceImage(*detailTexturePath.get(), *mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, {1,1}, uvMin, {38.0f / 114.0f + uvMin.x,1}, {255,255,255,(int)(255*0.7)}, 2,true);

    ImGui::SetCursorScreenPos(cursor + ImVec2{ 2,2 });
    VecGui::NineSliceImage(*bevelTexturePath.get(), imSize - ImVec2{ 4,4 }, { {75.0f,35.0f},{75.0f,35.0f} }, { 0,0 }, { 1,1 }, { 255,255,255,(int)(255 * 0.7) },2);

    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, id.data(), NULL, &label_size, style.ButtonTextAlign, &bb);
    ImGui::SetCursorScreenPos(position);

    return pressed;
}

void VecGui::CheckBox(std::string_view id, bool& value)
{
    const static auto mainTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\check_button.dds");

    if (mainTexturePath.get()->initialized)
    {
        ImVec2 size = { 24,24 };
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;


        ImVec2 cursor = ImGui::GetCursorScreenPos();
        VecGui::Image(*mainTexturePath.get(), size, { value ? 0.5f : 0.0f,0.0f }, { value ? 1.0f : 0.5f,1.0f });


        const ImGuiID imId = window->GetID(id.data());


        const ImRect bb(cursor, cursor + size);
        ImGui::ItemSize(size);
        if (!ImGui::ItemAdd(bb, imId))
            return;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(bb, imId, &hovered, &held, 0);

        if (pressed)
        {
            value = !value;
        }
    }
    else
    {
        ImGui::Checkbox(id.data(), &value);
    }

}

bool VecGui::SliderInt(std::string_view id, int& value, int min, int max)
{
    return SliderScalar(id, ImGuiDataType_S32, &value, &min, &max);
}

bool VecGui::SliderFloat(std::string_view id, float& value, float min, float max)
{
    return SliderScalar(id, ImGuiDataType_Float, &value, &min, &max);
}

bool VecGui::SliderScalar(std::string_view label, ImGuiDataType type, void* value, const void* min, const void* max)
{
    const static auto sliderTexture = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\scrollbars\\scrollbar_slider.dds");
    const static auto backgroundTexture = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\backgrounds\\dark_area.dds");

    if (backgroundTexture.get()->initialized)
    {
        int flags = 0;
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label.data());
        const float w = ImGui::CalcItemWidth();

        const ImVec2 label_size = ImGui::CalcTextSize(label.data(), NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;

        VecGui::NineSliceImageFloating(*backgroundTexture.get(), frame_bb.GetSize(), { {9.0f,9.0f},{9.0f,9.0f} }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.2f });

        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
            return false;
        const char* format = nullptr;
        // Default format string when passing NULL
        if (format == NULL)
            format = ImGui::DataTypeGetInfo(type)->PrintFmt;

        const bool hovered = ImGui::ItemHoverable(frame_bb, id);
        bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
        if (!temp_input_is_active)
        {
            // Tabbing or CTRL-clicking on Slider turns it into an input box
            const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = hovered && ImGui::IsMouseClicked(0, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id);
            if (make_active && clicked)
                ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id)
                    temp_input_is_active = true;

            if (make_active && !temp_input_is_active)
            {
                ImGui::SetActiveID(id, window);
                ImGui::SetFocusID(id, window);
                ImGui::FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        if (temp_input_is_active)
        {
            // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
            const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
            return ImGui::TempInputScalar(frame_bb, id, label.data(), type, value, format, is_clamp_input ? min : NULL, is_clamp_input ? max : NULL);
        }

        // Draw frame
        const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        ImGui::RenderNavHighlight(frame_bb, id);
        //ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);



        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = ImGui::SliderBehavior(frame_bb, id, type, value, min, max, format, flags, &grab_bb);
        if (value_changed)
            ImGui::MarkItemEdited(id);

        // Render grab
        if (grab_bb.Max.x > grab_bb.Min.x)
        {
            VecGui::Image(grab_bb.Min, *sliderTexture.get(), grab_bb.GetSize(), { 0,0 }, { 8.0f / 16.0f,1.0f }, IM_COL32_WHITE, true);

        }

        //window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), type, value, format);
        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("{", "}");
        ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

        if (label_size.x > 0.0f)
            ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label.data());

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
        return value_changed;
    }
    else
    {
        return ImGui::SliderScalar(label.data(), type, value, min, max, nullptr, 0);
    }


}


bool VecGui::RoundButton(std::string_view id, std::string_view icon, const ImVec2& size, const ImVec2& uvMin, const ImVec2& uvMax)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;


    auto Icons = Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\button_icons" / icon;
    auto buttons = Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\";
    const static auto smallTexture = ResourceHandler::GetTexture(buttons / "round_button_small_wood.dds");
    const static auto largeTexture = ResourceHandler::GetTexture(buttons / "round_button_big_wood.dds");

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    const ImGuiID imId = window->GetID(id.data());

    const ImRect bb(cursor, cursor + size);
    ImGui::ItemSize(size);
    if (!ImGui::ItemAdd(bb, imId))
        return false;


    bool hovered, held;
    bool clicked = ImGui::ButtonBehavior(bb, imId, &hovered, &held, 0);


    if (size.x < 160 || size.y < 160)
    {
        Image(cursor, *smallTexture.get(), size, { 0,0 }, { 1,1 }, IM_COL32_WHITE, true);
    }
    else
    {
        Image(cursor, *largeTexture.get(), size, { 0,0 }, { 1,1 }, IM_COL32_WHITE, true);
    }


    if (std::filesystem::exists(Icons))
    {
        const auto mainTexturePath = ResourceHandler::GetTexture(Icons);

        Image(*mainTexturePath.get(), size);
    }
    return clicked;
}

bool VecGui::ImageButton(std::string_view id, Texture& texture, const ImVec2& size, const ImVec2& uvMin, const ImVec2& uvMax)
{
    if (texture.initialized)
    {
        return ImGui::ImageButton(id.data(), texture.textureID, size, uvMin, uvMax, { 0,0,0,0 });
    }
    else
    {
        return ImGui::Button(id.data(), size);
    }
}

void VecGui::Image(Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImColor color)
{
    auto draw_list = ImGui::GetWindowDrawList();
    ImVec2 cursor = ImGui::GetCursorScreenPos();

    if (texture.initialized)
    {
        ImGui::Image(texture.textureID, size, uvMin, uvMax, color);
    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list);
    }
}

void VecGui::Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImColor color, bool floating)
{
    auto draw_list = ImGui::GetWindowDrawList();

    if (!floating)
    {
        ImRect bb(Pos, Pos + size);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, 0))
            return;
    }


    //uncomment to add bounding box around a texture
    //draw_list->AddRect(bb.Min, bb.Max, ImColor(128, 0, 255));

    if (texture.initialized)
    {
        draw_list->AddImage(texture.textureID, Pos, Pos + size, uvMin, uvMax, color);

    }
    else
    {
        draw_list->AddRectFilled(Pos, Pos + size, noImageColor);
    }

}

void VecGui::Image(ImVec2 Pos, Texture& texture, Texture& alphaMask, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax, ImColor color)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImRect bb(Pos, Pos + size);
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    //uncomment to add bounding box around a texture
    //draw_list->AddRect(bb.Min, bb.Max, ImColor(128, 0, 255));

    if (texture.initialized)
    {
        DrawImageAlphaMask(texture, alphaMask, Pos, Pos + size, uvMin, uvMax, uvAlphaMin, uvAlphaMax, true);
    }
    else
    {
        draw_list->AddRectFilled(Pos, Pos + size, noImageColor);
    }

}

//middleSlice is defined as 
void VecGui::NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color, float scale)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec2 textureSize = { (float)texture.width,(float)texture.height };
    ImVec2 uvSize = uvMax - uvMin;
    ImVec2 SectionSize = uvSize * textureSize;

    if (texture.initialized)
    {
        if (size == SectionSize)
        {
            Image(cursor, texture, size, uvMin, uvMax);
        }
        else
        {
            ImRect bb(cursor, cursor + size);
            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, 0))
                return;

            ImVec2 UvCornerSize = middleSlice.min / textureSize;
            ImVec4 middleRect = { middleSlice.min.x,middleSlice.min.y, middleSlice.max.x,middleSlice.max.y };

            DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax, color, scale);
        }

    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list);
    }
}

void VecGui::NoImageBehaviour(ImVec2& cursor, ImVec2& size, ImDrawList* draw_list, bool isFloating)
{
    if (!isFloating)
    {
        ImRect bb(cursor, cursor + size);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, 0))
            return;
    }
    draw_list->AddRectFilled(cursor, cursor + size, noImageColor);
}

void VecGui::NineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax, ImColor color, float scale, bool isFloating)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec2 textureSize = { (float)texture.width,(float)texture.height };
    ImVec2 uvSize = uvMax - uvMin;
    ImVec2 SectionSize = uvSize * textureSize;

    if (texture.initialized)
    {
        if (!isFloating)
        {
            ImRect bb(cursor, cursor + size);
            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, 0))
                return;
        }

        ImVec2 UvCornerSize = middleSlice.min / textureSize;
        ImVec4 middleRect = { middleSlice.min.x,middleSlice.min.y, middleSlice.max.x,middleSlice.max.y };

        DrawNineSliceImage(texture, alphaMask, cursor, size, middleRect, uvMin, uvMax, uvAlphaMin, uvAlphaMax, color, scale, true);
        //draw_list->AddRectFilled(cursor, cursor + size, ImColor(128, 0, 255, 128));


    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list, isFloating);
    }
}

void VecGui::NineSliceImageFloating(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color, float scale)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec2 textureSize = { (float)texture.width,(float)texture.height };
    ImVec2 uvSize = uvMax - uvMin;
    ImVec2 SectionSize = uvSize * textureSize;

    if (texture.initialized)
    {
        if (size == SectionSize)
        {
            draw_list->AddImage(texture.textureID, cursor, cursor + size, uvMin, uvMax);
        }
        else
        {
            ImVec2 UvCornerSize = middleSlice.min / textureSize;
            ImVec4 middleRect = { middleSlice.min.x,middleSlice.min.y, middleSlice.max.x,middleSlice.max.y };

            DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax, color, scale);
        }

    }
    else
    {
        draw_list->AddRectFilled(cursor, cursor + size, noImageColor);
    }

}
