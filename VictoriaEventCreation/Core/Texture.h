#pragma once
#include <vulkan/vulkan.h>

struct Texture
{
    Texture() = default;
    ~Texture();
    VkDevice device;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkSampler sampler;
    VkImageView imageView;
    VkDescriptorSet textureID;
    VkDeviceMemory uploadBufferMem;
    uint32_t width;
    uint32_t height;
    bool initialized = false;
};

