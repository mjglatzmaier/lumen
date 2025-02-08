#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Renderer Renderer;

// ✅ Define a common interface that all backends must implement
struct Renderer {
    void (*init)(Renderer* renderer, void* window);  // Initialize renderer
    void (*shutdown)(Renderer* renderer);  // Cleanup
    void (*resize)(Renderer* renderer, int width, int height);  // Handle window resize
    void (*preFrame)(Renderer* renderer);  // Start a frame
    void (*beginFrame)(Renderer* renderer);  // Start a frame
    void (*endFrame)(Renderer* renderer);  // End a frame
};

// ✅ Function to create a Vulkan renderer
Renderer* createVulkanRenderer(void* window);

#endif
