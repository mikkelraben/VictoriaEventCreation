#pragma once
#include "GLFW/glfw3.h"
#include "Texture.h"

struct ResourceError
{
    std::string error;
};

class Graphics;



class ResourceHandler
{
public:
    static void InitResourceHandler(Graphics* _graphics) { graphics = _graphics; };
    static std::shared_ptr<Texture> GetTexture(const std::filesystem::path& file);
    static void SubmitWork();
    static void DestroyResourceHandler();
private:
    static Graphics* graphics;
    static void LoadTexture(const std::filesystem::path& file);
    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    static std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>> textures;
};