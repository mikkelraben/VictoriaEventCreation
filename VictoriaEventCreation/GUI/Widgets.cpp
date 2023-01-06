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

bool VecGui::Button(std::string_view id,ImVec2 size)
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

    VecGui::NineSliceImageFloating(*mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, uvMin, { 38.0f / 114.0f + uvMin.x,1 }, { 255,255,255,(int)(255 * 0.7) });
    ImGui::SetCursorScreenPos(cursor);
    //VecGui::Image(cursor,*detailTexturePath.get(), *mainTexturePath.get(),size,{0,0}, { size.x/detailTexturePath.get()->width,size.y / detailTexturePath.get()->height }, uvMin, {38.0f / 114.0f + uvMin.x,1});
    VecGui::NineSliceImage(*frameTexturePath.get(), *mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1, 1 }, uvMin, { 38.0f / 114.0f + uvMin.x,1 }, { 255,255,255,(int)(255 * 0.7) },true);
    VecGui::NineSliceImage(*bgGradienTexturePath.get(), *mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, { 1, 1 }, uvMin, { 38.0f / 114.0f + uvMin.x,1 }, { 255,255,255,(int)(255 * 0.5) },true);
    VecGui::NineSliceImage(*detailTexturePath.get(), *mainTexturePath.get(), imSize, { {19.0f,19.0f},{19.0f,19.0f} }, { 0,0 }, {1,1}, uvMin, {38.0f / 114.0f + uvMin.x,1}, {255,255,255,(int)(255*0.7)}, true);

    ImGui::SetCursorScreenPos(cursor+ ImVec2{2,2});
    VecGui::NineSliceImage(*bevelTexturePath.get(), imSize-ImVec2{4,4}, {{75.0f,35.0f},{75.0f,35.0f}}, {0,0}, {1,1}, {255,255,255,(int)(255 * 0.7)});

    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, id.data(), NULL, &label_size, style.ButtonTextAlign, &bb);
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

void VecGui::Image(Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImColor color)
{
    auto draw_list = ImGui::GetWindowDrawList();
    ImVec2 cursor = ImGui::GetCursorScreenPos();

    if (texture.initialized)
    {
        ImGui::Image(texture.textureID, size, uvMin, uvMax,color);
    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list);
    }
}

void VecGui::Image(ImVec2 Pos, Texture& texture, ImVec2 size, ImVec2 uvMin, ImVec2 uvMax, ImColor color)
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
        draw_list->AddImage(texture.textureID, Pos, Pos + size, uvMin, uvMax,color);
        
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
        DrawImageAlphaMask(texture, alphaMask, Pos, Pos + size, uvMin, uvMax, uvAlphaMin, uvAlphaMax,true);
    }
    else
    {
        draw_list->AddRectFilled(Pos, Pos + size, noImageColor);
    }

}

//middleSlice is defined as 
void VecGui::NineSliceImage(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color)
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

            DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax, color);
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

void VecGui::NineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax,ImColor color, bool isFloating)
{
    auto draw_list = ImGui::GetWindowDrawList();

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec2 textureSize = { (float)texture.width,(float)texture.height };
    ImVec2 uvSize = uvMax - uvMin;
    ImVec2 SectionSize = uvSize * textureSize;

    if (texture.initialized)
    {
        if(!isFloating)
        {
            ImRect bb(cursor, cursor + size);
            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, 0))
                return;
        }

        ImVec2 UvCornerSize = middleSlice.min / textureSize;
        ImVec4 middleRect = { middleSlice.min.x,middleSlice.min.y, middleSlice.max.x,middleSlice.max.y };

        DrawNineSliceImage(texture, alphaMask, cursor, size, middleRect, uvMin, uvMax, uvAlphaMin, uvAlphaMax, color,true);
        //draw_list->AddRectFilled(cursor, cursor + size, ImColor(128, 0, 255, 128));


    }
    else
    {
        NoImageBehaviour(cursor, size, draw_list,isFloating);
    }
}

void VecGui::NineSliceImageFloating(Texture& texture, ImVec2 size, VecRect middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color)
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

            DrawNineSliceImage(texture, cursor, size, middleRect, uvMin, uvMax,color);
        }

    }
    else
    {
        draw_list->AddRectFilled(cursor, cursor + size, noImageColor);
    }

}
