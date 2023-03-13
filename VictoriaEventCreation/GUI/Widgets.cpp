#include "pch.h"
#include "../Core/pch.h"
#include "Widgets.h"
#include "CustomDrawing.h"
#include "imgui_internal.h"
#include "../Core/ResourceHandler.h"
#include "ImGui Node Editor/imgui_node_editor.h"
#include "ImGuiHelper.h"
namespace ed = ax::NodeEditor;

bool VecGui::Button(std::string_view id, bool actionButton, ImVec2 size)
{
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    auto& style = ImGui::GetStyle();

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 imSize;
    const ImGuiID imId = window->GetID(id.data());
    const ImVec2 label_size = ImGui::CalcTextSize(id.data(), NULL, true);
    if (size.x < 0 && size.y < 0)
    {
        imSize = ImGui::CalcItemSize(label_size, 16.0f, 16.0f);
        imSize = imSize + ImVec2{ 32, 32 };
    }
    else
    {
        if (size.x < 0)
        {
            size.x = ImGui::CalcItemSize(label_size, 16.0f, 16.0f).x + 16;
        }
        imSize = size;
    }

    if (imSize.x < 75)
    {
        imSize.x = 75;
    }
    if (imSize.y < 35)
    {
        imSize.y = 35;
    }

    const ImRect bb(cursor, cursor + imSize);
    ImGui::ItemSize(imSize);
    if (!ImGui::ItemAdd(bb, imId))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, imId, &hovered, &held, 0);


    ImDrawList* drawlist = ImGui::GetWindowDrawList();

    ImVec2 position = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(cursor);

    drawButton(cursor,imSize,hovered,held,actionButton);

    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, id.data(), NULL, &label_size, style.ButtonTextAlign, &bb);
    ImGui::SetCursorScreenPos(position);

    return pressed;
}

bool VecGui::CheckBox(std::string_view id, bool& value)
{
    const static auto mainTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\check_button.dds");

    if (mainTexturePath.get()->initialized)
    {
        ImVec2 size = { 24,24 };
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;


        ImVec2 cursor = ImGui::GetCursorScreenPos();
        VecGui::Image(*mainTexturePath.get(), size, { value ? 0.5f : 0.0f,0.0f }, { value ? 1.0f : 0.5f,1.0f }, IM_COL32_WHITE,true);


        const ImGuiID imId = window->GetID(id.data());
        if (id == "@@node")
        {
            id = value ? "yes" : "no";
        }
        const ImVec2 labelSize = ImGui::CalcTextSize(id.data(), NULL, true);
        const ImVec2 labelOffset = { 4,0 };

        const ImRect bb(cursor, cursor + size);
        const ImRect total(cursor, cursor + size + ImVec2(labelSize.x,0) + labelOffset);

        ImGui::ItemSize(total);
        if (!ImGui::ItemAdd(total, imId))
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total, imId, &hovered, &held, 0);

        ImVec2 labelPos = { bb.Max.x + labelOffset.x,bb.Min.y };

        if (labelSize.x > 0)
        {
            ImGui::RenderText(labelPos, id.data());
        }


        if (pressed)
        {
            value = !value;
        }
        return pressed;
    }
    else
    {
        return ImGui::Checkbox(id.data(), &value);
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

bool VecGui::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    const static auto expandIcon = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\expand_arrow_expanded.dds");

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    bool insideNode = ed::GetCurrentEditor();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    RE_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

    const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : ImGui::CalcItemWidth();
    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + 6.0f + style.FramePadding.y * 2.0f));
    const ImRect total_bb(bb.Min, bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &bb))
        return false;

    // Open on click
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if (pressed && !popup_open)
    {
        if (insideNode)
        {
            ed::Suspend();
        }
        ImGui::OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;

        if (insideNode)
        {
            ed::Resume();
        }
    }

    drawButton(bb.Min, bb.GetSize(), hovered, held);

    const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
    // Render preview and label
    if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
    {
        if (g.LogEnabled)
            ImGui::LogSetNextTextDecoration("{", "}");
        ImGui::RenderTextClipped(bb.Min + style.FramePadding + ImVec2{8,2}, ImVec2(value_x2, bb.Max.y), preview_value, NULL, NULL);
    }
    if (label_size.x > 0)
        ImGui::RenderText(ImVec2(bb.Max.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), label);


    Image({ bb.Max.x - 24, bb.Min.y + 8}, *expandIcon.get(), { 16,16 }, {0,0}, {1,1}, { 255,255,255,(int)(255 * 0.7) }, true);

    if (!popup_open)
    {
        return false;
    }

    if (insideNode)
    {
        ed::Suspend();
        bb.Min = ed::CanvasToScreen(bb.Min);
        bb.Max = ed::CanvasToScreen(bb.Max);
    }

    g.NextWindowData.Flags = backup_next_window_data_flags;
    auto returnValue = ImGui::BeginComboPopup(popup_id, bb, flags);

    if (ed::GetCurrentEditor())
    {
        Image({ bb.Max.x - 24, bb.Min.y + 8 }, *expandIcon.get(), { 16,16 }, { 0,0 }, { 1,1 }, { 255,255,255,(int)(255 * 0.7) }, true);
    }

    return returnValue;
}

bool VecGui::BeginPopup(const char* str_id, ImGuiWindowFlags flags)
{
        ImGuiContext& g = *GImGui;
        if (g.OpenPopupStack.Size <= g.BeginPopupStack.Size) // Early out for performance
        {
            g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
            return false;
        }
        flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings;
        ImGuiID id = g.CurrentWindow->GetID(str_id);
        return ImGui::BeginPopupEx(id, flags);
}

bool VecGui::SliderScalar(std::string_view label, ImGuiDataType type, void* value, const void* min, const void* max)
{
    const static auto sliderTexture = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\scrollbars\\scrollbar_slider.dds");
    const static auto backgroundTexture = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\dark_area.dds");
    const static auto bgGradienTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\backgrounds\\default_bg_shading.dds");


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

        VecGui::NineSliceImage(*backgroundTexture.get(), frame_bb.GetSize(), { {9.0f,9.0f},{9.0f,9.0f} }, { 0,0 }, { 1,1 }, ImColor{ 1.0f,1.0f,1.0f,0.2f },1,true);

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

        grab_bb.Expand({ 0,-2 });

        // Render grab
        if (grab_bb.Max.x > grab_bb.Min.x)
        {
            //VecGui::Image(grab_bb.Min, *sliderTexture.get(), grab_bb.GetSize(), { 0,0 }, { 0.4999f,1.0f }, IM_COL32_WHITE, true);
            auto cursor = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(grab_bb.Min);
            VecGui::NineSliceImage(*sliderTexture.get(), grab_bb.GetSize(), { {4,5},{4,5} }, { 0,0 }, { 0.5f,1.0f }, IM_COL32_WHITE, 1.0f, true);
            VecGui::NineSliceImage(*bgGradienTexturePath.get(),*sliderTexture.get(), grab_bb.GetSize(), {{4,5},{4,5}}, { 0,0 }, { 1.0f,1.0f } , { 0,0 }, { 0.5f,1.0f }, { 255,255,255,(int)(255 * 0.5) }, 1.0f, true,BlendMode::overlay);
            ImGui::SetCursorScreenPos(cursor);
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

void VecGui::drawButton(ImVec2 pos, ImVec2 size,bool hovered, bool held, bool actionButton)
{
    const static auto mainTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_bg.dds");
    const static auto frameTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_wood_border.dds");
    const static auto detailTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_texture.dds");
    const static auto bgGradienTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_bg_gradient.dds");
    const static auto bevelTexturePath = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_bevel.dds");
    const static auto whiteTexture = ResourceHandler::GetTexture("gfx\\white.dds");

    const static auto mousePress = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_mousepress.dds");
    const static auto mouseOver = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_mouseover.dds");

    ImVec2 uvMin = { 0,0 };
    if (actionButton)
    {
        uvMin = { 38.0f / 114.0f,0 };
    }
    ImVec2 uvMax = { 38.0f / 114.0f + uvMin.x,1 };
    auto cursor = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(pos);
    VecGui::NineSliceImage(*mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, uvMin, uvMax, { 255,255,255,(int)(255 * 0.7) }, 2, true);
    if (hovered && !held)
    {
        VecGui::NineSliceImage(*mouseOver.get(), *mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1,1 }, uvMin, uvMax, { 255,255,255,(int)(255 * 0.5) }, 2, true, BlendMode::color_dodge);
    }
    if (held)
    {
        VecGui::NineSliceImage(*mousePress.get(), *mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1,1 }, uvMin, uvMax, { 255,255,255,(int)(255 * 1) }, 2, true, BlendMode::overlay);
    }
    ImGui::SetCursorScreenPos(pos);
    VecGui::NineSliceImage(*frameTexturePath.get(), *mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1, 1 }, uvMin, uvMax, { 255,255,255,(int)(255 * 0.7) }, 2, true, BlendMode::overlay);
    VecGui::NineSliceImage(*bgGradienTexturePath.get(), *mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1, 1 }, uvMin, uvMax, { 255,255,255,(int)(255 * 0.5) }, 2, true, BlendMode::overlay);
    VecGui::NineSliceImage(*detailTexturePath.get(), *mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1,1 }, uvMin, uvMax, { 255,255,255,(int)(255 * 0.7) }, 2, true, BlendMode::overlay);

    ImGui::SetCursorScreenPos(pos + ImVec2{ 2,2 });
    VecGui::NineSliceImage(*bevelTexturePath.get(), size - ImVec2{ 4,4 }, { {75.0f,35.0f},{75.0f,35.0f} }, { 0,0 }, { 1,1 }, { 255,255,255,(int)(255 * 0.7) }, 2);

    ImGui::SetCursorScreenPos(cursor);

}


bool VecGui::RoundButton(Id id, std::string_view icon, const ImVec2& size, const ImVec2& uvMin, const ImVec2& uvMax, bool isCloseTab)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();


    const auto Icons = Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\button_icons" / (std::string(icon) + ".dds");
    const auto buttons = Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\";
    const static auto smallTexture = ResourceHandler::GetTexture(buttons / "round_button_small_wood.dds");
    const static auto largeTexture = ResourceHandler::GetTexture(buttons / "round_button_big_wood.dds");
    const static auto mousePress = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_mousepress.dds");
    const static auto mouseOver = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\default_button_mouseover.dds");
    const static auto iconSheen = ResourceHandler::GetTexture(Settings::gameDirectory.getSetting() / "game\\gfx\\interface\\buttons\\button_icons\\icon_button_mouse_enter.dds");


    ImVec2 cursor = ImGui::GetCursorScreenPos();

    const ImRect bb(cursor, cursor + size);
    ImGui::ItemSize(size);
    bool isClipped = false;
    if (!isCloseTab)
    {
        if (!ImGui::ItemAdd(bb, id))
            return false;
    }
    else
    {
        isClipped = !ImGui::ItemAdd(bb, id);
    }



    bool hovered, held;
    bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);
    if (isClipped)
    {
        return clicked;
    }

    auto mainBackgroundTexturePath = largeTexture;
    if (size.x < 78 || size.y < 78)
    {
        mainBackgroundTexturePath = smallTexture;
    }

    Image(cursor, *mainBackgroundTexturePath.get(), size, { 0,0 }, { 1,1 }, IM_COL32_WHITE, true);

    if (hovered && !held)
    {
        Image(cursor, *mouseOver.get(), *mainBackgroundTexturePath.get(), size, { 0,0 }, { 1,1 }, { 0,0 }, { 1,1 },{ 255,255,255,(int)(255 * 0.5) }, true, BlendMode::color_dodge);
    }

    if (held)
    {
        Image(cursor, *mousePress.get(), *mainBackgroundTexturePath.get(), size, { 0,0 }, { 1,1 }, { 0,0 }, { 1,1 }, { 255,255,255,(int)(255 * 1) }, true, BlendMode::overlay);
    }


    if (std::filesystem::exists(Icons))
    {
        const auto mainTexturePath = ResourceHandler::GetTexture(Icons);

        Image(cursor, *mainTexturePath.get(), size, { 0,0 }, { 1,1 }, IM_COL32_WHITE, true);
        if (hovered && !held)
        {
            Image(cursor, *iconSheen.get(), *mainBackgroundTexturePath.get(), size, { 0,0 }, { 1,1 }, { 0,0 }, { 1,1 }, { 255,255,255,(int)(255 * 0.7) }, true, BlendMode::overlay);
        }
    }
    return clicked;
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

void VecGui::DrawPinShape(PinShapes pinShape, bool input,ImU32 color)
{
    auto cursor = ImGui::GetCursorScreenPos();

    const float size = ImGui::GetFontSize();
    const float arrowSize = size / 4;
    ImVec2 maxTotal = { cursor.x + size+arrowSize, cursor.y + size };
    ImVec2 maxIcon = { cursor.x + size, cursor.y + size };
    auto drawlist = ImGui::GetWindowDrawList();
    ImRect bb(cursor, maxTotal);
    ImGui::ItemSize(bb.GetSize());
    ImGui::ItemAdd(bb, 0);
    ImVec2 arrowPosition = cursor + ImVec2(size+2, 0);
    if (input)
    {
        arrowPosition = cursor;
        cursor.x += arrowSize;
        maxIcon.x += arrowSize;
    }

    drawlist->AddTriangleFilled(arrowPosition + ImVec2(0, 6), arrowPosition + ImVec2(0, -6) + ImVec2(0, size), { arrowPosition.x + arrowSize - 2, (arrowPosition.y * 2 + size) / 2 }, color);

    if (pinShape == PinShapes::square)
    {
        drawlist->AddRect(cursor, maxIcon, color, 2, 0, 2);
    }
    else if (pinShape == PinShapes::circle)
    {
        ImVec2 middle = { cursor.x + size / 2, cursor.y + size / 2 };
        drawlist->AddCircle(middle, size / 2, color, 16, 2);
    }
    else if (pinShape == PinShapes::arrow)
    {

    }
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

void VecGui::Image(Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImColor color, bool isFloating, BlendMode colorBlend)
{
    const static auto whiteTexture = ResourceHandler::GetTexture("gfx\\white.dds");

    auto draw_list = ImGui::GetWindowDrawList();
    ImVec2 cursor = ImGui::GetCursorScreenPos();

    if (!isFloating)
    {
        ImRect bb(cursor, cursor + size);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, 0))
            return;
    }
    
    if (!&texture)
    {
        NoImageBehaviour(cursor, size, draw_list);
        return;
    }

    if (texture.initialized && whiteTexture.get()->initialized)
    {
        DrawImageAlphaMask(texture, *whiteTexture.get(), cursor, cursor + size, uvMin, uvMax, { 0,0 }, { 1,1 }, colorBlend);
    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list);
    }
}

void VecGui::Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImColor color, bool floating, BlendMode colorBlend)
{
    auto draw_list = ImGui::GetWindowDrawList();
    const static auto whiteTexture = ResourceHandler::GetTexture("gfx\\white.dds");

    ImVec2 floatingPos;
    if (floating)
    {
        floatingPos = ImGui::GetCursorScreenPos();
    }

    ImRect bb(Pos, Pos + size);
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
    {
        if (floating)
        {
            ImGui::SetCursorScreenPos(floatingPos);
        }
        return;
    }

    if (floating)
    {
        ImGui::SetCursorScreenPos(floatingPos);
    }



    //uncomment to add bounding box around a texture
    //draw_list->AddRect(bb.Min, bb.Max, ImColor(128, 0, 255));

    if (texture.initialized && whiteTexture.get()->initialized)
    {
        DrawImageAlphaMask(texture, *whiteTexture.get(), Pos, Pos + size, uvMin, uvMax, { 0,0 }, { 1,1 }, colorBlend);
    }
    else
    {
        draw_list->AddRectFilled(Pos, Pos + size, noImageColor);
    }

}

void VecGui::Image(ImVec2 Pos, Texture& texture, Texture& alphaMask, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax, ImColor color, bool floating, BlendMode colorBlend)
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

    if (texture.initialized && alphaMask.initialized)
    {
        DrawImageAlphaMask(texture, alphaMask, Pos, Pos + size, uvMin, uvMax, uvAlphaMin, uvAlphaMax, colorBlend);
    }
    else
    {
        draw_list->AddRectFilled(Pos, Pos + size, noImageColor);
    }

}

//middleSlice is defined as pixels from upper left and bottom right corners
void VecGui::NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color, float scale, bool isFloating, BlendMode colorBlend)
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

        DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax, color, scale, colorBlend);
    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list);
    }
}

void VecGui::NineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax, ImColor color, float scale, bool isFloating, BlendMode colorBlend)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec2 textureSize = { (float)texture.width,(float)texture.height };
    ImVec2 uvSize = uvMax - uvMin;
    ImVec2 SectionSize = uvSize * textureSize;

    if (texture.initialized && alphaMask.initialized)
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

        DrawNineSliceImage(texture, alphaMask, cursor, size, middleRect, uvMin, uvMax, uvAlphaMin, uvAlphaMax, color, scale, colorBlend);
        //draw_list->AddRectFilled(cursor, cursor + size, ImColor(128, 0, 255, 128));


    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list, isFloating);
    }
}