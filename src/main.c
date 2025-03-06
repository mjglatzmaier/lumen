#include "input/input.h"
#include "renderer/renderer.h"
#include "renderer/vulkan/vulkan_renderer.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    Renderer *renderer = (Renderer *) glfwGetWindowUserPointer(window);
    if (!renderer)
        return;

    VulkanRenderer *vkRenderer = (VulkanRenderer *) renderer;

    if (width == 0 || height == 0)
    {
        printf("🖥 Window minimized, ignoring resize.\n");
        return; // ✅ Ignore minimize events
    }
    renderer->resize(renderer, width, height);
}

int main()
{

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lumen Engine", NULL, NULL);

    if (!window)
    {
        fprintf(stderr, "Failed to create GLFW window!\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    // Init renderer
    Renderer *renderer = createVulkanRenderer(window);
    renderer->init(renderer, window);
    glfwSetWindowUserPointer(window, renderer);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Init input
    input_init(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        input_update();
        renderer->preFrame(renderer);
        renderer->beginFrame(renderer);
        renderer->endFrame(renderer);
    }

    renderer->shutdown(renderer);
    free(renderer);
    glfwDestroyWindow(window);
    glfwTerminate();

    printf("Lumen exited successfully.\n");
    return EXIT_SUCCESS;
}