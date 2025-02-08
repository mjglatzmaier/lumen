#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "renderer/vulkan/vulkan_renderer.h"
#include "renderer/renderer.h"
#include "input/input.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    Renderer* renderer = (Renderer*)glfwGetWindowUserPointer(window);
    if (renderer && renderer->resize) {
        //printf("⚡ Resize event triggered: %dx%d\n", width, height);
        renderer->resize(renderer, width, height);
    }
}

int main() {

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lumen Engine", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Failed to create GLFW window!\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    // Init renderer
    Renderer* renderer = createVulkanRenderer(window);
    renderer->init(renderer, window);
    glfwSetWindowUserPointer(window, renderer);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    // Init input
    input_init(window);

    while (!glfwWindowShouldClose(window)) {
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