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

bool VecGui::Button(std::string_view id, const ImVec2& size)
{
    const static auto mainTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_bg.dds");
    const static auto frameTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_bg.dds");
    const static auto detailTexturePath = ResourceHandler::GetTexture("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Victoria 3\\game\\gfx\\interface\\buttons\\default_button_bg.dds");
    
    


    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;


    const ImGuiID imId = window->GetID(id.data());
    ImVec2 imSize = ImGui::CalcItemSize(size, 16.0f, 16.0f);
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

    VecGui::NineSliceImageFloating(*mainTexturePath.get(), size, { {19.0f,19.0f},{19.0f,19.0f} }, uvMin, { 38.0f / 114.0f + uvMin.x,1 });

    ImGui::SetCursorScreenPos(position);

    return pressed;
}

bool VecGui::ImageButton(std::string_view id, Texture& texture, const ImVec2& size)
{
    if (texture.initialized)
    {
        return ImGui::ImageButton(id.data(), texture.textureID, size);
    }
    else
    {
        return ImGui::Button(id.data(), size);
    }
}

void VecGui::Image(Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax)
{
    if (texture.initialized)
    {
        ImGui::Image(texture.textureID, size, uvMin, uvMax);
    }
    else
    {
        auto draw_list = ImGui::GetWindowDrawList();

        ImVec2 cursor = ImGui::GetCursorScreenPos();


        ImRect bb(cursor, cursor + size);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, 0))
            return;

        

        draw_list->AddRectFilled(cursor, { cursor.x + size.x,cursor.y + size.y }, noImageColor);
    }
}

void VecGui::Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax)
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
        draw_list->AddImage(texture.textureID, Pos, Pos + size, uvMin, uvMax);
    }
    else
    {
        draw_list->AddRectFilled(Pos, Pos + size, noImageColor);
    }

}

//middleSlice is defined as 
void VecGui::NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax)
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

            DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax);
            //draw_list->AddRectFilled(cursor, cursor + size, ImColor(128, 0, 255, 128));

        }

    }
    else
    {
        ImRect bb(cursor, cursor + size);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, 0))
            return;

        draw_list->AddRectFilled(cursor, cursor + size, noImageColor);
    }
}

void VecGui::NineSliceImageFloating(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax)
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

            DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax);
        }

    }
    else
    {
        draw_list->AddRectFilled(cursor, cursor + size, noImageColor);
    }

}
