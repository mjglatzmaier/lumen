#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "../renderer.h"

#define MAX_FRAMES_IN_FLIGHT 2  // ✅ Double-buffering to prevent synchronization issues


typedef struct VulkanRenderer {
    Renderer base;  // ✅ Simulate "inheritance" by embedding Renderer
    GLFWwindow* window;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    VkImageView* swapchainImageViews;
    VkImage* swapchainImages;
    uint32_t swapchainImageCount;
    VkFramebuffer* swapchainFramebuffers;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    uint32_t currentFrame;
    bool resizePending;
} VulkanRenderer;

// ✅ Function declarations
bool createVulkanInstance(VulkanRenderer* renderer);
bool createSurface(VulkanRenderer* renderer, GLFWwindow* window);
void selectPhysicalDevice(VulkanRenderer* renderer);
bool createLogicalDevice(VulkanRenderer* renderer);
void createSwapchain(VulkanRenderer* renderer);
void createImageViews(VulkanRenderer* renderer);
void createRenderPass(VulkanRenderer* renderer);
bool createGraphicsPipeline(VulkanRenderer* renderer);
void createFramebuffers(VulkanRenderer* renderer);
void createCommandPool(VulkanRenderer* renderer);
void createCommandBuffers(VulkanRenderer* renderer);
void recordCommandBuffer(VulkanRenderer* renderer, uint32_t imageIndex);
void drawFrame(VulkanRenderer* renderer);
void reateSyncObjects(VulkanRenderer* renderer);
void cleanupSwapchain(VulkanRenderer* renderer);
void cleanupVulkan(VulkanRenderer* renderer);
void createSyncObjects(VulkanRenderer* renderer);
void recreateSwapchain(VulkanRenderer* renderer);

#endif // VULKAN_RENDERER_H
