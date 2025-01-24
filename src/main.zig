const std = @import("std");
const game = @import("game.zig");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    defer {
        const deinit_status = gpa.deinit();
        if (deinit_status == .leak) @panic("Memory leak detected");
    }

    if (c.SDL_Init(c.SDL_INIT_VIDEO | c.SDL_INIT_AUDIO) != 0) {
        c.SDL_LogError(c.SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s", c.SDL_GetError());
        return error.SDLInitializationFailed;
    }
    defer c.SDL_Quit();

    const target_frame_rate = 60;

    const window = c.SDL_CreateWindow("game", c.SDL_WINDOWPOS_UNDEFINED, c.SDL_WINDOWPOS_UNDEFINED, 1920, 1080, c.SDL_WINDOW_OPENGL) orelse {
        c.SDL_LogError(c.SDL_LOG_CATEGORY_ERROR, "Unable to create window: %s", c.SDL_GetError());
        return error.SDLInitializationFailed;
    };
    defer c.SDL_DestroyWindow(window);

    const renderer = c.SDL_CreateRenderer(window, -1, 0) orelse {
        c.SDL_LogError(c.SDL_LOG_CATEGORY_ERROR, "Unable to create renderer: %s", c.SDL_GetError());
        return error.SDLInitializationFailed;
    };
    defer c.SDL_DestroyRenderer(renderer);

    const audio_samples_per_second = 48000;
    const audio_samples_per_frame = audio_samples_per_second / target_frame_rate;
    const audio_channels = 2;
    var audio_spec: c.SDL_AudioSpec = .{
        .channels = audio_channels,
        .format = c.AUDIO_S16LSB,
        .freq = audio_samples_per_second,
        .samples = audio_samples_per_frame * @sizeOf(i16) * audio_channels,
    };
    _ = c.SDL_OpenAudio(&audio_spec, 0);
    c.SDL_PauseAudio(0);
    var sound_buffer = try game.SoundBuffer.init(audio_samples_per_frame, allocator);
    defer sound_buffer.deinit();

    var window_width: c_int = undefined;
    var window_height: c_int = undefined;
    c.SDL_GetWindowSize(window, &window_width, &window_height);
    var offscreen_buffer = try game.OffscreenBuffer.init(@as(usize, @intCast(window_width)), @as(usize, @intCast(window_height)), allocator);
    defer offscreen_buffer.deinit();
    var texture = c.SDL_CreateTexture(renderer, c.SDL_PIXELFORMAT_RGBA8888, c.SDL_TEXTUREACCESS_STREAMING, window_width, window_height) orelse {
        return error.SDLTextureFailed;
    };

    var quit = false;
    while (!quit) {
        var event: c.SDL_Event = undefined;
        while (c.SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                c.SDL_QUIT => {
                    quit = true;
                },
                c.SDL_KEYUP, c.SDL_KEYDOWN => {
                    c.SDL_Log("Key press = %i was_down = %d", event.key.keysym.sym);
                },
                c.SDL_WINDOWEVENT_RESIZED => {
                    c.SDL_GetWindowSize(window, &window_width, &window_height);
                    offscreen_buffer.deinit();
                    offscreen_buffer = try game.OffscreenBuffer.init(@as(usize, @intCast(window_width)), @as(usize, @intCast(window_height)), allocator);
                    c.SDL_DestroyTexture(texture);
                    texture = c.SDL_CreateTexture(renderer, c.SDL_PIXELFORMAT_RGBA8888, c.SDL_TEXTUREACCESS_STREAMING, window_width, window_height) orelse {
                        return error.SDLTextureFailed;
                    };
                },
                else => {},
            }
        }

        sound_buffer.clear();

        try game.UpdateAndRender(&offscreen_buffer, &sound_buffer);

        _ = c.SDL_QueueAudio(1, @ptrCast(sound_buffer.samples), sound_buffer.sample_count);
        _ = c.SDL_UpdateTexture(texture, 0, @ptrCast(offscreen_buffer.pixels), @intCast(offscreen_buffer.pitch));
        _ = c.SDL_RenderClear(renderer);
        _ = c.SDL_RenderCopy(renderer, texture, 0, 0);
        c.SDL_RenderPresent(renderer);
    }

    c.SDL_Quit();
}
