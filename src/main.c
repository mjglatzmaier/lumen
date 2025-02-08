#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "renderer/vulkan/vulkan_renderer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    VulkanRenderer* renderer = glfwGetWindowUserPointer(window);
    renderer->framebufferResized = true;
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW!\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lumen Engine", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window!\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    VulkanRenderer renderer = {0};
    renderer.window = window;
    createVulkanInstance(&renderer);
    createSurface(&renderer, window);
    selectPhysicalDevice(&renderer);
    createLogicalDevice(&renderer);
    createSwapchain(&renderer);
    createImageViews(&renderer);
    createRenderPass(&renderer);
    createGraphicsPipeline(&renderer);
    createFramebuffers(&renderer);
    createCommandPool(&renderer);
    createCommandBuffers(&renderer);
    createSyncObjects(&renderer);

    // ✅ Store the VulkanRenderer pointer in the GLFW window before setting the callback
    glfwSetWindowUserPointer(window, &renderer);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // ✅ Only render when swapchain is valid
        if (!renderer.framebufferResized) {
            drawFrame(&renderer);
        }
    }

    //cleanupSwapchain(&renderer);
    cleanupVulkan(&renderer);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    printf("Lumen exited successfully.\n");
    return EXIT_SUCCESS;
}
