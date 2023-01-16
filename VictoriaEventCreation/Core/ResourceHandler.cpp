#include "pch.h"
#include "ResourceHandler.h"
#include "Graphics.h"
#include "imgui/imgui_impl_vulkan.h"
#include "freeImage/FreeImage.h"


using namespace std::chrono_literals;

//#TODO Clean Up
Graphics* ResourceHandler::graphics = nullptr;
std::mutex textureMutex;
std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>> ResourceHandler::textures;

std::mutex submitInfosMutex;
static std::vector<VkSubmitInfo> submitInfos;

std::counting_semaphore waitForSubmitDone{ 0 };
std::counting_semaphore waitForTexturesUpdated{ 0 };

std::mutex ImGuiAddTextureMutex;

void ResourceHandler::LoadTexture(const std::filesystem::path& file)
{
    RE_LogMessage("Loading Texture: " + file.string());
    std::string filename = file.lexically_normal().string().c_str();

    auto format = FreeImage_GetFileType(filename.c_str());
    auto bitmap = FreeImage_Load(format, filename.c_str(), 0);

    //std::this_thread::sleep_for(3s);
    try
    {
        if (!bitmap)
        {
            throw ResourceError("Could Not Load Texture: " + file.string());
        }
        {
            auto bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
            FreeImage_FlipVertical(bitmap32);
            bool BGR = FreeImage_IsLittleEndian();

            Texture* texture = new Texture;
            texture->device = graphics->device;

            texture->width = FreeImage_GetWidth(bitmap32);
            texture->height = FreeImage_GetHeight(bitmap32);

            //size of bitmap in bytes 
            size_t imageSize = texture->width * texture->height * 4;


            //create image
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = BGR ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
            imageInfo.extent.width = texture->width;
            imageInfo.extent.height = texture->height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (vkCreateImage(graphics->device, &imageInfo, nullptr, &texture->image) != VK_SUCCESS)
            {
                throw ResourceError("Could not create Image");
            }

            VkMemoryRequirements memReq{};

            vkGetImageMemoryRequirements(graphics->device, texture->image, &memReq);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memReq.size;
            allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            if (vkAllocateMemory(graphics->device, &allocInfo, nullptr, &texture->imageMemory) != VK_SUCCESS)
            {
                throw ResourceError("Could not allocate memory on device");
            }

            if (vkBindImageMemory(graphics->device, texture->image, texture->imageMemory, 0) != VK_SUCCESS)
            {
                throw ResourceError("Could not bind image memory");
            }

            //create image view
            VkImageViewCreateInfo imageViewInfo{};
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.image = texture->image;
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = BGR ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.levelCount = 1;
            imageViewInfo.subresourceRange.layerCount = 1;
            if (vkCreateImageView(graphics->device, &imageViewInfo, nullptr, &texture->imageView) != VK_SUCCESS)
            {
                throw ResourceError("Could not create Imageview");
            }

            //create sampler
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.minLod = -1000;
            samplerInfo.maxLod = 1000;
            samplerInfo.maxAnisotropy = 1.0f;
            vkCreateSampler(graphics->device, &samplerInfo, nullptr, &texture->sampler);


            //create texture handle for imgui
            {
                std::scoped_lock addTextureLock(ImGuiAddTextureMutex);
                texture->textureID = ImGui_ImplVulkan_AddTexture(texture->sampler, texture->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }

            //create buffer and copy data into it
            VkBuffer uploadBuffer;

            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = imageSize;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if (vkCreateBuffer(graphics->device, &bufferInfo, nullptr, &uploadBuffer) != VK_SUCCESS)
            {
                throw ResourceError("Could not create buffer");
            }

            VkMemoryRequirements bufMemReq;
            vkGetBufferMemoryRequirements(graphics->device, uploadBuffer, &bufMemReq);

            VkMemoryAllocateInfo bufAllocateInfo{};
            bufAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            bufAllocateInfo.allocationSize = bufMemReq.size;
            bufAllocateInfo.memoryTypeIndex = findMemoryType(bufMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            if (vkAllocateMemory(graphics->device, &bufAllocateInfo, nullptr, &texture->uploadBufferMem) != VK_SUCCESS)
            {
                throw ResourceError("Could not allocate memory");
            }
            if (vkBindBufferMemory(graphics->device, uploadBuffer, texture->uploadBufferMem, 0) != VK_SUCCESS)
            {
                throw ResourceError("Could bind memory");
            }

            void* map = NULL;

            if (vkMapMemory(graphics->device, texture->uploadBufferMem, 0, imageSize, 0, &map) != VK_SUCCESS)
            {
                throw ResourceError("Could not map memory");
            }
            void* textureData = FreeImage_GetBits(bitmap32);
            memcpy(map, textureData, imageSize);

            VkMappedMemoryRange range[1] = {};
            range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range[0].memory = texture->uploadBufferMem;
            range[0].size = imageSize;
            if (vkFlushMappedMemoryRanges(graphics->device, 1, range) != VK_SUCCESS)
            {
                throw ResourceError("Could not flush mapped memory");
            }

            vkUnmapMemory(graphics->device, texture->uploadBufferMem);

            FreeImage_Unload(bitmap);

            //create cmdpool and buffer to enable multithreaded texture loading
            //create command pool
            VkCommandPool cmdPool;
            VkCommandPoolCreateInfo commandPoolInfo{};
            commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolInfo.pNext = nullptr;

            //needs to be graphics queue due to graphics processing further down
            commandPoolInfo.queueFamilyIndex = graphics->graphicsQueueFamily;
            commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            if (vkCreateCommandPool(graphics->device, &commandPoolInfo, nullptr, &cmdPool) != VK_SUCCESS)
            {
                throw ResourceError("Could not create commandpool");
            }
            //create commandbuffer
            VkCommandBuffer commandBuffer;
            VkCommandBufferAllocateInfo cmdBufferInfo{};
            cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferInfo.pNext = nullptr;

            cmdBufferInfo.commandPool = cmdPool;
            cmdBufferInfo.commandBufferCount = 1;
            cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            if (vkAllocateCommandBuffers(graphics->device, &cmdBufferInfo, &commandBuffer) != VK_SUCCESS)
            {
                throw ResourceError("Could not allocate command buffers");
            }


            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;


            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            {
                throw ResourceError("Could not begin Commandbuffer");
            }
            VkImageMemoryBarrier copyBarrier[1] = {};

            copyBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copyBarrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copyBarrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copyBarrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copyBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copyBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copyBarrier[0].image = texture->image;
            copyBarrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyBarrier[0].subresourceRange.levelCount = 1;
            copyBarrier[0].subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copyBarrier);

            VkBufferImageCopy region{};
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.layerCount = 1;
            region.imageExtent.width = texture->width;
            region.imageExtent.height = texture->height;
            region.imageExtent.depth = 1;

            vkCmdCopyBufferToImage(commandBuffer, uploadBuffer, texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


            VkImageMemoryBarrier useBarrier[1] = {};
            useBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            useBarrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            useBarrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            useBarrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            useBarrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            useBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            useBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            useBarrier[0].image = texture->image;
            useBarrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            useBarrier[0].subresourceRange.levelCount = 1;
            useBarrier[0].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, useBarrier);

            VkSubmitInfo endInfo{};
            endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            endInfo.commandBufferCount = 1;
            endInfo.pCommandBuffers = &commandBuffer;

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            {
                throw ResourceError("Could not end CommandBuffer");
            }
            
            {
                std::scoped_lock lock(submitInfosMutex);
                submitInfos.push_back(endInfo);
            }

            waitForSubmitDone.acquire();
            std::lock_guard lock(textureMutex);

            vkDestroyBuffer(graphics->device, uploadBuffer, nullptr);

            texture->initialized = true;
            
            Texture* oldTexture = textures.at(file).get();
            *oldTexture = *texture;

            waitForTexturesUpdated.release();
            vkDestroyCommandPool(graphics->device, cmdPool, nullptr);
        }
    }
    catch (const ResourceError& e)
    {
        RE_LogError(e.error);
    }

}

//Throws ResourceError structs
//Loads a texture and assigns an id
std::shared_ptr<Texture> ResourceHandler::GetTexture(const std::filesystem::path& file)
{
    {
        std::scoped_lock lock(textureMutex);
        //if texture exists
        if (textures.contains(file))
        {
            return textures[file];
        }
        //else create new entry in textures and
        textures[file];
    }
    
    
    
    //probably should have a list of threads in resourceHandler to ensure threads are destroyed if program dies
    auto thread = std::thread(LoadTexture,file);
    thread.detach();

    std::scoped_lock lock(textureMutex);
    auto texturePointer = std::make_shared<Texture>();
    textures[file] = texturePointer;
    return texturePointer;
}

void ResourceHandler::SubmitWork()
{
    std::lock_guard lock(submitInfosMutex);
    if (!submitInfos.empty() && submitInfos.size() > 0)
    {
        if (vkQueueSubmit(graphics->graphicsQueue, (uint32_t)submitInfos.size(), submitInfos.data(), VK_NULL_HANDLE) != VK_SUCCESS)
        {
            throw ResourceError("Could not submit queue");
        }
        
        for (size_t i = 0; i < submitInfos.size(); i++)
        {
            if (vkDeviceWaitIdle(graphics->device) != VK_SUCCESS)
            {
                throw ResourceError("Could not wait till device is idle");
            }
            waitForSubmitDone.release();
            waitForTexturesUpdated.acquire();
        }
        submitInfos.clear();
    }
}

void ResourceHandler::DestroyResourceHandler()
{
    for (auto& texture : textures)
    {
        if (auto texturePointer = texture.second)
        {
            texturePointer.get()->~Texture();
        }
    }
}

uint32_t ResourceHandler::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(graphics->physicalDevice, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return 0xFFFFFFFF; // Unable to find memoryType
}

Texture::~Texture()
{
    if (initialized)
    {
        vkDeviceWaitIdle(device);
        vkDestroyImageView(device, imageView, nullptr);
        vkDestroySampler(device, sampler, nullptr);
        vkDestroyImage(device, image, nullptr);
        vkFreeMemory(device, imageMemory, nullptr);
        vkFreeMemory(device, uploadBufferMem, nullptr);
        initialized = false;
    }

}