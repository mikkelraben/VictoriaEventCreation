#include "pch.h"
#include "Graphics.h"
#include "ResourceHandler.h"

using namespace std::chrono_literals;

struct InitError
{
    std::string error;
};

Graphics::Graphics()
{
    window = glfwCreateWindow(width, height, "Victoria Event Creation Tool", NULL, NULL);
    glfwGetFramebufferSize(window, &width, &height);
    if (window == NULL)
    {
        RE_LogMessage("Failed to create window!");
        return;
    }
    RE_LogMessage("Window Created! Initializing Graphics");
    try
    {
        initVulkan();
        initImgui();
    }
    catch (InitError e)
    {
        RE_LogError(e.error);
        throw e;
    }
    ResourceHandler::InitResourceHandler(this);

    whiteTexture = ResourceHandler::GetTexture("gfx\\white.dds");

    while (!whiteTexture.get()->initialized)
    {
        ResourceHandler::SubmitWork();
        std::this_thread::sleep_for(1ms);
    }
    ImGui_ImplVulkan_GetWhiteTexture(whiteTexture.get()->textureID);
    

    RE_LogMessage("Finished Initializing Graphics");
}

Graphics::~Graphics()
{

    RE_LogMessage("Destroying Graphics");
    vkDeviceWaitIdle(device);
    ResourceHandler::DestroyResourceHandler();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    ImGui_ImplVulkanH_DestroyWindow(instance, device, &mainWindowData, nullptr);

    destroyVulkan();
    glfwDestroyWindow(window);	
    glfwTerminate();
    
    RE_LogMessage("Destruction of Graphics Complete");
}

void Graphics::initVulkan()
{
#pragma region instance
    vkb::InstanceBuilder builder;
    builder.set_app_name("Victoria Event Creation Tool");
    if (enableValidationLayers)
    {
        builder.request_validation_layers();
    }


#pragma region debugCallBack
#pragma warning(push)
#pragma warning(disable: 4100)
    builder.set_debug_callback(
        [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        -> VkBool32
        {
            std::string severity = vkb::to_string_message_severity(messageSeverity);
            std::string type = vkb::to_string_message_severity(messageSeverity);
            std::string msg = ": " + type + " " + pCallbackData->pMessage;
            Log::LogMessage(severity, msg);
            return VK_FALSE;
        }
    );
#pragma warning(pop)
#pragma endregion

    auto inst_ret = builder.build();
    if (!inst_ret)
    {
        throw InitError("Failed to create Vulkan instance: " + inst_ret.error().message());
    }
    vkbInstance = inst_ret.value();
    instance = vkbInstance.instance;

#pragma endregion

    if (glfwCreateWindowSurface(vkbInstance.instance, window, NULL, &surface) != VK_SUCCESS)
    {
        throw InitError("Could not construct Vulkan Surface");
    }

#pragma region PhysicalDevice
    vkb::PhysicalDeviceSelector selector{ vkbInstance };

    auto physicalDeviceReturn = selector
        .set_surface(surface)
        .set_minimum_version(1, 3)
        .require_dedicated_transfer_queue()
        .select();

    if (!physicalDeviceReturn)
    {
        throw InitError("Failed to find a suitable Device for Vulkan: " + physicalDeviceReturn.error().message());
    }
    physicalDevice = physicalDeviceReturn.value();
#pragma endregion

#pragma region LogicalDevice
    vkb::DeviceBuilder deviceBuilder{ physicalDeviceReturn.value() };

    VkPhysicalDeviceExtendedDynamicState3FeaturesEXT dynamicState3Features{};

    dynamicState3Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
    dynamicState3Features.extendedDynamicState3ColorBlendEquation = VK_TRUE;

    deviceBuilder.add_pNext(&dynamicState3Features);

    auto deviceReturn = deviceBuilder.build();
    if (!deviceReturn)
    {
        throw InitError("Failed to create a Vulkan Device: " + deviceReturn.error().message());
    }

    vkbDevice = deviceReturn.value();

    device = vkbDevice.device;
#pragma endregion

#pragma region GraphicsQueue
    auto vkbGraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics);
    if (!vkbGraphicsQueue)
    {
        throw InitError("Failed to find graphics queue: " + vkbGraphicsQueue.error().message());
    }
    graphicsQueue = vkbGraphicsQueue.value();
    graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    auto vkbTransferQueue = vkbDevice.get_queue(vkb::QueueType::transfer);
    if (!vkbTransferQueue)
    {
        throw InitError("Failed to find graphics queue: " + vkbTransferQueue.error().message());
    }
    transferQueue = vkbTransferQueue.value();
    transferQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();


#pragma endregion

#pragma region DescriptorPool
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw InitError("Could not create constructor pool");
    }
#pragma endregion

#pragma region CommandPool
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.pNext = nullptr;

    commandPoolInfo.queueFamilyIndex = graphicsQueueFamily;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw InitError("Could not create commandpool");
    }
#pragma endregion

#pragma region CommandBuffer
    VkCommandBufferAllocateInfo cmdBufferInfo{};
    cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferInfo.pNext = nullptr;

    cmdBufferInfo.commandPool = commandPool;
    cmdBufferInfo.commandBufferCount = 1;
    cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if ( vkAllocateCommandBuffers(device,&cmdBufferInfo,&commandBuffer) != VK_SUCCESS)
    {
        throw InitError("Could not allocate command buffers");
    }
#pragma endregion
}

void Graphics::destroyVulkan() 
{
    
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    vkb::destroy_device(vkbDevice);
    vkb::destroy_instance(vkbInstance);
}

PFN_vkVoidFunction VulkanLoadFunctions(const char* function_name,void* userData)
{
    auto graphics = reinterpret_cast<Graphics*>(userData);



    auto fun = vkGetInstanceProcAddr(graphics->instance, function_name);

    if (fun == nullptr)
    {
        std::string name = function_name;
        RE_LogError("Could Not Load Vulkan Function: " + name);
    }

    return fun;
}

void Graphics::initImgui()
{
    if (!ImGui_ImplVulkan_LoadFunctions(VulkanLoadFunctions, this))
    {
        RE_LogError("Could not Load Functions");
        throw InitError("Could not load certain Vulkan functions. Updating drivers might help.");
    }

    SetupVulkanWindow(&mainWindowData);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    SetStyle();
    

    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo init_info{};

    init_info.Instance = instance;
    init_info.PhysicalDevice = physicalDevice;
    init_info.Device = device;
    init_info.QueueFamily = graphicsQueueFamily;
    init_info.Queue = graphicsQueue;

    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = minImageCount;
    init_info.ImageCount = mainWindowData.ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = [](VkResult e)
    {
        if (e == 0)
        {
            return;
        }
        else
        {
            RE_LogError("Vulkan Error: " + e);
        }
        
    };

    ImGui_ImplVulkan_Init(&init_info, mainWindowData.RenderPass);

    // Load Fonts
// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
// - Read 'docs/FONTS.md' for more instructions and details.
// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//io.Fonts->AddFontDefault();
//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
//IM_ASSERT(font != NULL);

// Upload Fonts
    {
        // Use any command queue
        VkCommandPool command_pool = mainWindowData.Frames[mainWindowData.FrameIndex].CommandPool;
        VkCommandBuffer command_buffer = mainWindowData.Frames[mainWindowData.FrameIndex].CommandBuffer;

        if (vkResetCommandPool(device, command_pool, 0) != VK_SUCCESS)
        {
            throw InitError("Could not reset CommandPool");
        }

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
        {
            throw InitError("Could not begin Commandbuffer");
        }

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        {
            throw InitError("Could not end Commandbuffer");
        }
        if (vkQueueSubmit(graphicsQueue, 1, &end_info, VK_NULL_HANDLE) != VK_SUCCESS)
        {
            throw InitError("Could not submit to queue");
        }

        if (vkDeviceWaitIdle(device) != VK_SUCCESS)
        {
            throw InitError("Could not wait for my turn apparently");
        }
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }


}

void LoadFont(ImGuiIO& io, std::vector<std::filesystem::path>& fonts, size_t index, float size)
{
    if (std::filesystem::exists(fonts[index]))
    {
        if (!io.Fonts->AddFontFromFileTTF(fonts[index].string().c_str(), size))
        {
            RE_LogError("Error Loading font: " + fonts[index].string());
        }
    }
    else
    {
        RE_LogWarning("Could Nof Find Font: " + fonts[index].string());
    }
}

void Graphics::SetStyle()
{
    ImGuiIO& io = ImGui::GetIO();
    RE_LogMessage("Loading Fonts");
    std::vector<std::filesystem::path> fonts = { Settings::gameDirectory.getSetting() / "game\\fonts\\EBGaramond\\EBGaramond-Regular.ttf",Settings::gameDirectory.getSetting() / "game\\fonts\\PlayfairDisplay\\PlayfairDisplay-Regular.ttf"};

    LoadFont(io, fonts, 0, 16);
    LoadFont(io, fonts, 0, 22);
    LoadFont(io, fonts, 1, 26);
    //LoadFont(io, fonts, 1, 48);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    if (io.Fonts->Fonts.Data)
    {
        io.FontDefault = io.Fonts->Fonts[1];
    }


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.0f;
    }
    style.Colors[ImGuiCol_FrameBg] = ImColor{ 0,0,0,51 };
    style.FrameRounding = 4.0f;
    style.WindowRounding = 0.0f;
    style.WindowPadding = { 80,0 };
    style.PopupRounding = 4.0f;
    style.WindowMenuButtonPosition = -1;
    style.WindowTitleAlign = { 0.5f,0.5f };
    style.ScaleAllSizes(1.0f);
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void Graphics::SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd)
{
    wd->Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamily, wd->Surface, &res);
    if (res != VK_TRUE)
    {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(physicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(physicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
    //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT(minImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(instance, physicalDevice, device, wd, graphicsQueueFamily, nullptr, width, height, minImageCount);
}

void Graphics::RenderFrame()
{
    
    ResourceHandler::SubmitWork();

    const bool isMinimized = (ImGui::GetDrawData()->DisplaySize.x <= 0.0f || ImGui::GetDrawData()->DisplaySize.y <= 0.0f);
    
    if (!isMinimized)
    {
        ImGui_ImplVulkanH_Frame* frame = &mainWindowData.Frames[mainWindowData.FrameIndex];

        VkSemaphore imageAcquiredSemaphore = mainWindowData.FrameSemaphores[mainWindowData.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore renderCompleteSemaphore = mainWindowData.FrameSemaphores[mainWindowData.SemaphoreIndex].RenderCompleteSemaphore;

        VkResult rebuildSwapchain = vkAcquireNextImageKHR(device, mainWindowData.Swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &mainWindowData.FrameIndex);
        if (rebuildSwapchain == VK_ERROR_OUT_OF_DATE_KHR || rebuildSwapchain == VK_SUBOPTIMAL_KHR)
        {
            swapchainRebuild = true;
        }
        //else
        {
            vkWaitForFences(device, 1, &frame->Fence, VK_TRUE, 10000);

            vkResetFences(device, 1, &frame->Fence);

            //render frame 
            VkCommandBufferBeginInfo bufferInfo {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            bufferInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;


            vkResetCommandPool(device, frame->CommandPool, 0);
            vkBeginCommandBuffer(frame->CommandBuffer, &bufferInfo);

            VkRenderPassBeginInfo renderInfo{};
            renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderInfo.renderPass = mainWindowData.RenderPass;
            renderInfo.framebuffer = frame->Framebuffer;
            renderInfo.renderArea.extent.width = mainWindowData.Width;
            renderInfo.renderArea.extent.height = mainWindowData.Height;
            renderInfo.clearValueCount = 1;
            renderInfo.pClearValues = &mainWindowData.ClearValue;

            vkCmdBeginRenderPass(frame->CommandBuffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame->CommandBuffer);
    
            vkCmdEndRenderPass(frame->CommandBuffer);

            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &frame->CommandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &renderCompleteSemaphore;

            vkEndCommandBuffer(frame->CommandBuffer);

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame->Fence);
        }

    }

    
    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    if (!isMinimized)
    {
        //present frame
        if (!swapchainRebuild)
        {
            VkSemaphore render_complete_semaphore = mainWindowData.FrameSemaphores[mainWindowData.SemaphoreIndex].RenderCompleteSemaphore;
            VkPresentInfoKHR info = {};
            info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &render_complete_semaphore;
            info.swapchainCount = 1;
            info.pSwapchains = &mainWindowData.Swapchain;
            info.pImageIndices = &mainWindowData.FrameIndex;
            VkResult rebuildSwapchain = vkQueuePresentKHR(graphicsQueue, &info);
            if (rebuildSwapchain == VK_ERROR_OUT_OF_DATE_KHR || rebuildSwapchain == VK_SUBOPTIMAL_KHR)
            {
                swapchainRebuild = true;
            }
            else
            {
                mainWindowData.SemaphoreIndex = (mainWindowData.SemaphoreIndex + 1) % mainWindowData.ImageCount; // Now we can use the next set of semaphores
            }
        }

    }


}

void Graphics::SwapchainRebuild()
{
    if (swapchainRebuild)
    {
        int _width, _height;
        glfwGetFramebufferSize(window, &_width, &_height);
        if (_width > 0 && _height > 0)
        {
            ImGui_ImplVulkan_SetMinImageCount(minImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(instance, physicalDevice, device, &mainWindowData, graphicsQueueFamily, nullptr, _width, _height, minImageCount);
            mainWindowData.FrameIndex = 0;
            swapchainRebuild = false;
        }
    }
}