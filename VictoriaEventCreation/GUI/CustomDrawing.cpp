#include "pch.h"
#include "../Core/pch.h"
#include "CustomDrawing.h"
#include "../Core/imgui/imgui_internal.h"
#include "../Core/imgui/imgui_impl_vulkan.h"

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
        if (axisValue > endValue - MiddleValues.y)
        {
            axisValue = (endValue + startValue) / 2;
        }
    }

    if (axis == 2)
    {
        axisValue = endValue - MiddleValues.y;
        if (axisValue < startValue + MiddleValues.x)
        {
            axisValue = (endValue + startValue) / 2;
        }

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

    WriteIndexNineSlice(IndexWrite, index, draw_list, numberOfVertices);
}


void PrimNineGridUVAlphaMask(const ImVec2& startPos, const ImVec2& endPos, const ImVec4& middleRect,const ImVec2& uv_a_start, const ImVec4& middleUvAlphaRect, const ImVec2& uv_a_end, const ImVec2& uv_start,const ImVec2& uv_end,ImU32 col)
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

            GetAxisPosition(x,position.x,startPos.x,{middleRect.x,middleRect.z},endPos.x);
            GetAxisPosition(y,position.y,startPos.y,{middleRect.y,middleRect.w},endPos.y);

            GetAxisPosition(x, uvAlpha.x, uv_a_start.x, { middleUvAlphaRect.x,middleUvAlphaRect.z }, uv_a_end.x);
            GetAxisPosition(y, uvAlpha.y, uv_a_start.y, { middleUvAlphaRect.y,middleUvAlphaRect.w }, uv_a_end.y);

            ImVec2 size = { endPos.x - startPos.x,endPos.y - startPos.y };
            ImVec2 offset = { position.x - startPos.x,position.y - startPos.y };

            uv.x = offset.x / size.x;
            uv.y = offset.y / size.y;

            VertexWrite[x * 4 + y].pos = position; VertexWrite[x * 4 + y].alphaUV = uvAlpha; VertexWrite[x * 4 + y].col = col; VertexWrite[x * 4 + y].uv = uv;
        }
    }

    WriteIndexNineSlice(IndexWrite, index, draw_list, numberOfVertices);
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

void DrawNineSliceImage(Texture& texture, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImColor color)
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
    PrimNineGridUV(pos, pos + size, middleSlice, uvMin, middleUvSlice, uvMax, color);

    if (push_texture_id)
        draw_list->PopTextureID();

}

void DrawNineSliceImage(Texture& texture, Texture& alphaMask, ImVec2 pos, ImVec2 size, ImVec4 middleSlice, ImVec2 uvMin, ImVec2 uvMax, ImVec2 uvAlphaMin, ImVec2 uvAlphaMax,ImColor color, bool overlay)
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
    PrimNineGridUVAlphaMask(pos, pos + size, middleSlice,uvAlphaMin,middleUvSlice,uvAlphaMax,uvMin,uvMax, color);
    draw_list->CmdBuffer.back().AlphaId = alphaMask.textureID;
    draw_list->CmdBuffer.back().Overlay = overlay;


    if (push_texture_id)
        draw_list->PopTextureID();

}

void DrawImageAlphaMask(Texture& texture, Texture& alphamask,const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, const ImVec2& uv_a_min, const ImVec2& uv_a_max, bool overlay)
{
    auto draw_list = ImGui::GetWindowDrawList();

    const bool push_texture_id = texture.textureID != draw_list->_CmdHeader.TextureId;
    if (push_texture_id)
        draw_list->PushTextureID(texture.textureID);

    draw_list->PrimReserve(6, 4);
    PrimRectUVAlpha(p_min, p_max, uv_min, uv_max, uv_a_min, uv_a_max, IM_COL32_WHITE);
    draw_list->CmdBuffer.back().AlphaId = alphamask.textureID;
    draw_list->CmdBuffer.back().Overlay = true;

    if (push_texture_id)
        draw_list->PopTextureID();
}
