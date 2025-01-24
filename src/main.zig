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
        return error.SDLInitializationFailed;
    }
    defer c.SDL_Quit();

    const target_frame_rate = 60;

    const window = c.SDL_CreateWindow("game", c.SDL_WINDOWPOS_UNDEFINED, c.SDL_WINDOWPOS_UNDEFINED, 1920, 1080, c.SDL_WINDOW_OPENGL) orelse {
        return error.SDLInitializationFailed;
    };
    defer c.SDL_DestroyWindow(window);

    const renderer = c.SDL_CreateRenderer(window, -1, 0) orelse {
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
        const perf_counter_frequency: u64 = c.SDL_GetPerformanceFrequency();
        const last_counter: u64 = c.SDL_GetPerformanceCounter();
        defer {
            const end_counter: u64 = c.SDL_GetPerformanceCounter();
            const counter_elapsed: u64 = end_counter - last_counter;
            const ms_per_frame: f64 = (1000.0 * @as(f64, @floatFromInt(counter_elapsed))) / @as(f64, @floatFromInt(perf_counter_frequency));
            const fps: f64 = @as(f64, @floatFromInt(perf_counter_frequency)) / @as(f64, @floatFromInt(counter_elapsed));
            std.log.debug("{d:.02} ms/f, {d:.02} fps", .{ ms_per_frame, fps });
        }

        var event: c.SDL_Event = undefined;
        while (c.SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                c.SDL_QUIT => {
                    quit = true;
                },
                c.SDL_KEYUP, c.SDL_KEYDOWN => {
                    std.log.debug("Key press = {d}", .{event.key.keysym.sym});
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
