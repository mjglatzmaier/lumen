#include "vulkan_renderer.h"
#include "../../core/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool createVulkanInstance(VulkanRenderer* renderer) {

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Lumen Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Lumen";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

   // Get required extensions from GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);


        fprintf(stderr, "GLFW extensions: %d\n", glfwExtensionCount);

    const char* extensions[10];  // Array to hold all extensions
    uint32_t extensionCount = 0;

    // Copy GLFW extensions first
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        extensions[extensionCount++] = glfwExtensions[i];
    }

        // Add MoltenVK extensions for macOS
    #ifdef PLATFORM_MACOS
        extensions[extensionCount++] = "VK_MVK_macos_surface";
    #endif

    // Enable portability if required
    #if ENABLE_PORTABILITY_EXTENSIONS
        extensions[extensionCount++] = "VK_KHR_portability_enumeration";
    #endif

    const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = validationLayers;

    #if ENABLE_PORTABILITY_EXTENSIONS
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif

    if (vkCreateInstance(&createInfo, NULL, &renderer->instance) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan instance!\n");
        return false;
    }

    return true;
}

bool createSurface(VulkanRenderer* renderer, GLFWwindow* window) {
    if (glfwCreateWindowSurface(renderer->instance, window, NULL, &renderer->surface) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create window surface!\n");
        return false;
    }
    return true;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties availableExtensions[extensionCount];
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    bool swapchainSupported = false;
    for (uint32_t i = 0; i < extensionCount; i++) {
        if (strcmp(availableExtensions[i].extensionName, "VK_KHR_swapchain") == 0) {
            swapchainSupported = true;
            break;
        }
    }

    return swapchainSupported;
}

void selectPhysicalDevice(VulkanRenderer* renderer) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(renderer->instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stderr, "❌ No Vulkan-supported GPUs found!\n");
        exit(1);
    }

    VkPhysicalDevice* devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(renderer->instance, &deviceCount, devices);

    for (uint32_t i = 0; i < deviceCount; i++) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, NULL);
        VkQueueFamilyProperties* queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);

        int foundGraphics = 0, foundPresent = 0;
        for (uint32_t j = 0; j < queueFamilyCount; j++) {
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, renderer->surface, &presentSupport);

            if (queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                renderer->graphicsQueueFamilyIndex = j;
                foundGraphics = 1;
                fprintf(stdout, "🟢 Selected Graphics Queue Family Index: %d\n", j);
            }
            if (presentSupport) {
                renderer->presentQueueFamilyIndex = j;
                foundPresent = 1;
                fprintf(stdout, "🟢 Selected Present Queue Family Index: %d\n", j);
            }

            if (foundGraphics && foundPresent) {
                renderer->physicalDevice = devices[i];
                free(queueFamilies);
                free(devices);
                return;
            }
        }
        free(queueFamilies);
    }

    free(devices);
    fprintf(stderr, "❌ No suitable GPU found with both graphics and present support!\n");
    exit(1);
}

bool createLogicalDevice(VulkanRenderer* renderer) {
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {0};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = renderer->graphicsQueueFamilyIndex;  // ✅ Ensure correct queue family index
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Enable the required device extensions
    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,          // Required for swapchains
    #ifdef PLATFORM_MACOS
        "VK_KHR_portability_subset",             // ✅ Required for MoltenVK
    #endif
    };

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.enabledExtensionCount = 2;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(renderer->physicalDevice, &createInfo, NULL, &renderer->device) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan device with swapchain support!\n");
        return false;
    }

        // ✅ Retrieve the graphics queue
        vkGetDeviceQueue(renderer->device, renderer->graphicsQueueFamilyIndex, 0, &renderer->graphicsQueue);
        if (renderer->graphicsQueue == VK_NULL_HANDLE) {
            fprintf(stderr, "❌ Error: Failed to get graphics queue!\n");
            exit(1);
        }
    
        // ✅ Retrieve the present queue
        vkGetDeviceQueue(renderer->device, renderer->presentQueueFamilyIndex, 0, &renderer->presentQueue);
        if (renderer->presentQueue == VK_NULL_HANDLE) {
            fprintf(stderr, "❌ Error: Failed to get present queue!\n");
            exit(1);
        }
    
        fprintf(stdout, "✅ Graphics Queue: %p, Present Queue: %p\n", (void*)renderer->graphicsQueue, (void*)renderer->presentQueue);

    return true;
}

void createSwapchain(VulkanRenderer* renderer) {
    // ✅ Query surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->physicalDevice, renderer->surface, &capabilities);

    // ✅ Query available surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physicalDevice, renderer->surface, &formatCount, NULL);
    VkSurfaceFormatKHR* formats = malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physicalDevice, renderer->surface, &formatCount, formats);

    // ✅ Choose a preferred surface format (default to BGRA8 if available)
    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (uint32_t i = 0; i < formatCount; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB) {
            surfaceFormat = formats[i];
            break;
        }
    }
    free(formats);

    // ✅ Query present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->physicalDevice, renderer->surface, &presentModeCount, NULL);
    VkPresentModeKHR* presentModes = malloc(presentModeCount * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(renderer->physicalDevice, renderer->surface, &presentModeCount, presentModes);

    // ✅ Choose FIFO (vsync) or MAILBOX (low-latency vsync)
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    free(presentModes);

    // ✅ Determine swap extent (resolution)
    VkExtent2D swapExtent = capabilities.currentExtent;
    if (capabilities.currentExtent.width == UINT32_MAX) {
        swapExtent.width = 800;  // Default resolution
        swapExtent.height = 600;
    }

    // ✅ Set image count (ensure valid range)
    renderer->swapchainImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && renderer->swapchainImageCount > capabilities.maxImageCount) {
        renderer->swapchainImageCount = capabilities.maxImageCount;
    }

    // ✅ Fill out swapchain create info
    VkSwapchainCreateInfoKHR swapchainInfo = {0};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = renderer->surface;
    swapchainInfo.minImageCount = renderer->swapchainImageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = swapExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // ✅ Setup queue family indices (single GPU case)
    uint32_t queueFamilyIndices[] = {renderer->graphicsQueueFamilyIndex};
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 1;
    swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;

    swapchainInfo.preTransform = capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    // ✅ Create the swapchain
    if (vkCreateSwapchainKHR(renderer->device, &swapchainInfo, NULL, &renderer->swapchain) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create swapchain!\n");
        exit(1);
    }

    // ✅ Store swapchain properties
    renderer->swapchainImageFormat = surfaceFormat.format;
    renderer->swapchainExtent = swapExtent;

    // ✅ Retrieve swapchain images
    vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchainImageCount, NULL);
    renderer->swapchainImages = malloc(renderer->swapchainImageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchainImageCount, renderer->swapchainImages);
}

void createRenderPass(VulkanRenderer* renderer) {
    VkAttachmentDescription colorAttachment = {0};
    colorAttachment.format = renderer->swapchainImageFormat;  // ✅ Matches swapchain
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // ✅ Clears screen
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {0};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(renderer->device, &renderPassInfo, NULL, &renderer->renderPass) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create render pass!\n");
        exit(1);
    }
}

void createImageViews(VulkanRenderer* renderer) {
    renderer->swapchainImageViews = malloc(renderer->swapchainImageCount * sizeof(VkImageView));

    for (uint32_t i = 0; i < renderer->swapchainImageCount; i++) {
        VkImageViewCreateInfo viewInfo = {0};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = renderer->swapchainImages[i];  // ✅ Corrected this line
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = renderer->swapchainImageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(renderer->device, &viewInfo, NULL, &renderer->swapchainImageViews[i]) != VK_SUCCESS) {
            fprintf(stderr, "❌ Failed to create image view for swapchain image %d!\n", i);
            exit(1);
        }
    }
}

void createFramebuffers(VulkanRenderer* renderer) {
    renderer->swapchainFramebuffers = malloc(renderer->swapchainImageCount * sizeof(VkFramebuffer));

    for (uint32_t i = 0; i < renderer->swapchainImageCount; i++) {
        VkImageView attachments[] = {
            renderer->swapchainImageViews[i]  // ✅ Attach swapchain image view
        };

        VkFramebufferCreateInfo framebufferInfo = {0};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderer->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = renderer->swapchainExtent.width;
        framebufferInfo.height = renderer->swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(renderer->device, &framebufferInfo, NULL, &renderer->swapchainFramebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "❌ Failed to create framebuffer %d!\n", i);
            exit(1);
        }
    }
}

void createCommandBuffers(VulkanRenderer* renderer) {

    if (renderer->swapchainImageCount == 0) {
        fprintf(stderr, "❌ Error: swapchainImageCount is 0 before allocating command buffers!\n");
        exit(1);
    }
    renderer->commandBuffers = malloc(renderer->swapchainImageCount * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderer->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = renderer->swapchainImageCount;

    if (vkAllocateCommandBuffers(renderer->device, &allocInfo, renderer->commandBuffers) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to allocate command buffers!\n");
        exit(1);
    }

    for (size_t i = 0; i < renderer->swapchainImageCount; i++) {
        VkCommandBufferBeginInfo beginInfo = {0};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if (vkBeginCommandBuffer(renderer->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            fprintf(stderr, "❌ Failed to begin recording command buffer!\n");
            exit(1);
        }

        VkClearValue clearColor = {{{1.0f, 0.0f, 1.0f, 1.0f}}};  // ✅ Debug pink background
        VkRenderPassBeginInfo renderPassInfo = {0};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderer->renderPass;
        renderPassInfo.framebuffer = renderer->swapchainFramebuffers[i];
        renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
        renderPassInfo.renderArea.extent = renderer->swapchainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(renderer->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(renderer->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->graphicsPipeline);

        vkCmdDraw(renderer->commandBuffers[i], 3, 1, 0, 0);  // ✅ Draw triangle

        vkCmdEndRenderPass(renderer->commandBuffers[i]);

        if (vkEndCommandBuffer(renderer->commandBuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "❌ Failed to record command buffer!\n");
            exit(1);
        }
    }
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VulkanRenderer* renderer) {
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to begin recording command buffer!\n");
        exit(1);
    }

    VkClearValue clearColor = {{{0.1f, 0.1f, 0.1f, 1.0f}}};  // ✅ Dark gray background
    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderer->renderPass;
    renderPassInfo.framebuffer = renderer->swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = renderer->swapchainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    // ✅ Begin the render pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // ✅ Bind the pipeline (Ensures commands execute in the correct graphics state)
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->graphicsPipeline);

    // ✅ Issue a draw call for 3 vertices (Triangle)
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // ✅ End the render pass
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to record command buffer!\n");
        exit(1);
    }
}

void createCommandPool(VulkanRenderer* renderer) {
    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = renderer->graphicsQueueFamilyIndex;  // ✅ Must be set before
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(renderer->device, &poolInfo, NULL, &renderer->commandPool) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create command pool!\n");
        exit(1);
    }
}

void drawFrame(VulkanRenderer* renderer) {
    if (renderer->graphicsQueue == VK_NULL_HANDLE) {
        fprintf(stderr, "❌ Error: graphicsQueue is NULL before vkQueueSubmit!\n");
        exit(1);
    }
    if (renderer->presentQueue == VK_NULL_HANDLE) {
        fprintf(stderr, "❌ Error: presentQueue is NULL before vkQueuePresentKHR!\n");
        exit(1);
    }

    if (renderer->framebufferResized) {
        recreateSwapchain(renderer);  // ✅ Recreate swapchain if resized
        renderer->framebufferResized = false;
    }

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(renderer->device, renderer->swapchain, UINT64_MAX,
                                            renderer->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        fprintf(stderr, "❌ Failed to acquire swapchain image!\n");
        exit(1);
    }

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {renderer->imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &renderer->commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderer->renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // ✅ Print queue info before submitting
    //fprintf(stdout, "🟢 Submitting to queue: %p\n", (void*)renderer->graphicsQueue);

    if (vkQueueSubmit(renderer->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to submit draw command buffer!\n");
        exit(1);
    }

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &renderer->swapchain;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(renderer->presentQueue, &presentInfo);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to present swapchain image!\n");
        exit(1);
    }
}

VkShaderModule createShaderModule(VkDevice device, const char* filename) {
    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "src/renderer/shaders/%s", filename);

    FILE* file = fopen(fullPath, "rb");
    if (!file) {
        fprintf(stderr, "❌ Failed to open shader file: %s\n", fullPath);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    if (fileSize == 0) {
        fprintf(stderr, "❌ Shader file is empty: %s\n", fullPath);
        exit(1);
    }

    printf("✅ Loading shader: %s (Size: %zu bytes)\n", fullPath, fileSize);

    // ✅ Allocate a properly aligned buffer
    char* buffer = malloc(fileSize);
    fread(buffer, fileSize, 1, file);
    fclose(file);

    // ✅ Read the first 4 bytes as an aligned uint32_t
    uint32_t magicNumber;
    memcpy(&magicNumber, buffer, sizeof(uint32_t));

    printf("Shader magic number: 0x%08x\n", magicNumber);
    if (magicNumber != 0x07230203) {
        fprintf(stderr, "❌ Shader file is corrupt: %s\n", fullPath);
        exit(1);
    }

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = (uint32_t*)buffer;  // Still cast to uint32_t* but now properly aligned.

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create shader module: %s\n", fullPath);
        exit(1);
    }

    free(buffer);
    return shaderModule;
}

bool createGraphicsPipeline(VulkanRenderer* renderer) {
    // Vertex Shader Stage
    VkShaderModule vertShader = createShaderModule(renderer->device, "triangle.vert.spv");
    VkShaderModule fragShader = createShaderModule(renderer->device, "triangle.frag.spv");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // Vertex Input State (No vertex buffers)
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = NULL;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = NULL;

    // Input Assembly State (Using a simple triangle list)
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport & Scissor State
    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) renderer->swapchainExtent.width;
    viewport.height = (float) renderer->swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = renderer->swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterization State
    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Multisample State
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Color Blend State
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = NULL;

    if (vkCreatePipelineLayout(renderer->device, &pipelineLayoutInfo, NULL, &renderer->pipelineLayout) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create pipeline layout!\n");
        return false;
    }

    // Graphics Pipeline Creation
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = renderer->pipelineLayout;
    pipelineInfo.renderPass = renderer->renderPass;
    pipelineInfo.subpass = 0;

    VkResult result = vkCreateGraphicsPipelines(renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &renderer->graphicsPipeline);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create Vulkan graphics pipeline! Error Code: %d\n", result);
        return false;
    }

    // Cleanup shaders after pipeline creation
    vkDestroyShaderModule(renderer->device, vertShader, NULL);
    vkDestroyShaderModule(renderer->device, fragShader, NULL);

    return true;
}

void recreateSwapchain(VulkanRenderer* renderer) {
    vkDeviceWaitIdle(renderer->device);  // ✅ Ensure device is idle before recreating

    cleanupSwapchain(renderer);  // ✅ Destroy old swapchain

    createSwapchain(renderer);  // ✅ Create new swapchain
    createImageViews(renderer);
    createFramebuffers(renderer);
}

void createSyncObjects(VulkanRenderer* renderer) {
    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(renderer->device, &semaphoreInfo, NULL, &renderer->imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(renderer->device, &semaphoreInfo, NULL, &renderer->renderFinishedSemaphore) != VK_SUCCESS) {
        fprintf(stderr, "❌ Failed to create semaphores!\n");
        exit(1);
    }
}

void cleanupSwapchain(VulkanRenderer* renderer) {
    for (uint32_t i = 0; i < renderer->swapchainImageCount; i++) {
        vkDestroyFramebuffer(renderer->device, renderer->swapchainFramebuffers[i], NULL);
        vkDestroyImageView(renderer->device, renderer->swapchainImageViews[i], NULL);
    }
    vkDestroySwapchainKHR(renderer->device, renderer->swapchain, NULL);
    vkDestroySurfaceKHR(renderer->instance, renderer->surface, NULL);
}

void cleanupVulkan(VulkanRenderer* renderer) {
    vkDestroyPipeline(renderer->device, renderer->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(renderer->device, renderer->pipelineLayout, NULL);
    vkDestroyRenderPass(renderer->device, renderer->renderPass, NULL);
    vkDestroyDevice(renderer->device, NULL);
    vkDestroyInstance(renderer->instance, NULL);
}