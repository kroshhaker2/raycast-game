#include "renderer.h"

#include <SDL3/SDL_gpu.h>
#include <cstdint>

bool Renderer::init(SDL_GPUDevice *device, SDL_Window *window, int width,
                    int height) {
    device_ = device;

    buffer_size_ = static_cast<std::size_t>(width) *
                   static_cast<std::size_t>(height) * sizeof(std::uint32_t);

    SDL_GPUTextureCreateInfo texture_info{};
    texture_info.type = SDL_GPU_TEXTURETYPE_2D;
    texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    texture_info.width = width;
    texture_info.height = height;
    texture_info.layer_count_or_depth = 1;
    texture_info.num_levels = 1;
    texture_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

    texture_ = SDL_CreateGPUTexture(device_, &texture_info);

    if (!texture_) {
        SDL_Log("SDL_CreateGPUTexture failed: %s", SDL_GetError());
        return false;
    }

    SDL_GPUTransferBufferCreateInfo transfer_info{};
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size = buffer_size_;

    transfer_buffer_ = SDL_CreateGPUTransferBuffer(device_, &transfer_info);

    if (!transfer_buffer_) {
        SDL_Log("SDL_CreateGPUTransferBuffer failed: %s", SDL_GetError());

        SDL_ReleaseGPUTexture(device_, texture_);
        texture_ = nullptr;

        return false;
    }

    return true;
}

void Renderer::draw(SDL_Window *window, const Framebuffer &framebuffer) {
    void *mapped = SDL_MapGPUTransferBuffer(device_, transfer_buffer_, true);

    if (!mapped) {
        SDL_Log("SDL_MapGPUTransferBuffer failed: %s", SDL_GetError());
        return;
    }

    memcpy(mapped, framebuffer.data(), buffer_size_);

    SDL_UnmapGPUTransferBuffer(device_, transfer_buffer_);

    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device_);

    if (!command_buffer) {
        SDL_Log("SDL_AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return;
    }

    SDL_GPUTexture *swapchain_texture = nullptr;

    Uint32 swapchain_width = 0;
    Uint32 swapchain_height = 0;

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            command_buffer, window, &swapchain_texture, &swapchain_width,
            &swapchain_height)) {

        SDL_Log("SDL_WaitAndAcquireGPUSwapchainTexture failed: %s",
                SDL_GetError());

        return;
    }

    if (!swapchain_texture) {
        SDL_SubmitGPUCommandBuffer(command_buffer);
        return;
    }

    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);

    SDL_GPUTextureTransferInfo source{};
    source.transfer_buffer = transfer_buffer_;
    source.offset = 0;
    source.pixels_per_row = static_cast<Uint32>(framebuffer.width());
    source.rows_per_layer = static_cast<Uint32>(framebuffer.height());

    SDL_GPUTextureRegion destination{};
    destination.texture = texture_;
    destination.mip_level = 0;
    destination.layer = 0;
    destination.x = 0;
    destination.y = 0;
    destination.z = 0;
    destination.w = static_cast<Uint32>(framebuffer.width());
    destination.h = static_cast<Uint32>(framebuffer.height());
    destination.d = 1;

    SDL_UploadToGPUTexture(copy_pass, &source, &destination, true);

    SDL_EndGPUCopyPass(copy_pass);

    SDL_GPUBlitInfo blit{};
    blit.source.texture = texture_;
    blit.source.mip_level = 0;
    blit.source.layer_or_depth_plane = 0;
    blit.source.x = 0;
    blit.source.y = 0;
    blit.source.w = static_cast<Uint32>(framebuffer.width());
    blit.source.h = static_cast<Uint32>(framebuffer.height());

    blit.destination.texture = swapchain_texture;
    blit.destination.mip_level = 0;
    blit.destination.layer_or_depth_plane = 0;
    blit.destination.x = 0;
    blit.destination.y = 0;
    blit.destination.w = swapchain_width;
    blit.destination.h = swapchain_height;

    blit.load_op = SDL_GPU_LOADOP_DONT_CARE;
    blit.filter = SDL_GPU_FILTER_NEAREST;
    blit.cycle = false;

    SDL_BlitGPUTexture(command_buffer, &blit);

    SDL_SubmitGPUCommandBuffer(command_buffer);
}

void Renderer::shutdown() {
    if (transfer_buffer_) {
        SDL_ReleaseGPUTransferBuffer(device_, transfer_buffer_);

        transfer_buffer_ = nullptr;
    }

    if (texture_) {
        SDL_ReleaseGPUTexture(device_, texture_);

        texture_ = nullptr;
    }

    device_ = nullptr;
}