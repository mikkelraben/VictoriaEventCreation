#include "pch.h"
#include "../Core/pch.h"
#include "CustomDrawing.h"
#include "../Core/imgui/imgui_internal.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

void GetAxisPosition(const size_t& axis, float& axisValue, float startValue, const ImVec2& MiddleValues, float endValue)
{
    if (axis == 0)
    {
        axisValue = startValue;
    }

    if (axis == 1)
    {
        axisValue = startValue + MiddleValues.x;
    }

    if (axis == 2)
    {
        axisValue = endValue - MiddleValues.y;
    }

    if (axis == 3)
    {
        axisValue = endValue;
    }
}

void PrimNineGridUV(const ImVec2& startPos, const ImVec2& endPos, const ImVec4& middleRect,const ImVec2& uv_a, const ImVec4& middleUvRect, const ImVec2& uv_c, ImU32 col)
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

            GetAxisPosition(x,position.x,startPos.x,{middleRect.x,middleRect.z},endPos.x);
            GetAxisPosition(y,position.y,startPos.y,{middleRect.y,middleRect.w},endPos.y);

            GetAxisPosition(x, uv.x, uv_a.x, { middleUvRect.x,middleUvRect.z }, uv_c.x);
            GetAxisPosition(y, uv.y, uv_a.y, { middleUvRect.y,middleUvRect.w }, uv_c.y);

            VertexWrite[x*4+y].pos = position; VertexWrite[x * 4 + y].uv = uv; VertexWrite[x * 4 + y].col = col;
        }
    }

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

void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax)
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

    draw_list->PrimReserve(54, 16);
    PrimNineGridUV(pos, pos + size, middleSlice, uvMin, middleUvSlice, uvMax, IM_COL32_WHITE);

    if (push_texture_id)
        draw_list->PopTextureID();

}
