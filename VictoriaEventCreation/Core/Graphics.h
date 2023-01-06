#pragma once

#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "vk-bootstrap/VkBootstrap.h"
#include "ResourceHandler.h"

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Graphics
{
public:
    Graphics();
    ~Graphics();

    void RenderFrame();
    void SwapchainRebuild();


    GLFWwindow* window = nullptr;
    std::shared_ptr<Texture> whiteTexture;
    VkDevice device;
    VkInstance instance;

private:
    const int minImageCount = 2;
    bool swapchainRebuild;

    void initVulkan();
    void destroyVulkan();
    void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd);

    void initImgui();
    void SetStyle();

    int width = 1280, height = 720;

    std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef _DEBUG
    bool enableValidationLayers = true;
#else
    bool enableValidationLayers = false;
#endif


    vkb::Device vkbDevice;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    ImGui_ImplVulkanH_Window mainWindowData;

    VkSurfaceKHR surface;


    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> framebuffers;

    vkb::Instance vkbInstance;

    VkDescriptorPool descriptorPool;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkQueue graphicsQueue;
    uint32_t graphicsQueueFamily;

    VkQueue transferQueue;
    uint32_t transferQueueFamily;

    VkQueue presentQueue;
    uint32_t presentQueueFamily;
    friend ResourceHandler;
};