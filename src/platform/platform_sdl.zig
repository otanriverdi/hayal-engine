const std = @import("std");
const platform = @import("./platform.zig");
const game = @import("../game.zig");
const render = @import("../render/render.zig");
const renderer_sdl = @import("../render/renderer_sdl.zig");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

pub fn platformMain() !void {
    if (c.SDL_Init(c.SDL_INIT_VIDEO | c.SDL_INIT_AUDIO) != 0) {
        return error.SDLInitFailed;
    }
    defer c.SDL_Quit();

    const window = c.SDL_CreateWindow(null, c.SDL_WINDOWPOS_CENTERED, c.SDL_WINDOWPOS_CENTERED, 1920, 1080, c.SDL_WINDOW_BORDERLESS) orelse {
        return error.SdlCreateWindowFailed;
    };
    defer c.SDL_DestroyWindow(window);

    const renderer = c.SDL_CreateRenderer(window, -1, c.SDL_RENDERER_ACCELERATED | c.SDL_RENDERER_PRESENTVSYNC) orelse {
        return error.SdlCreateRendererFailed;
    };
    defer c.SDL_DestroyRenderer(renderer);

    var audio_spec: c.SDL_AudioSpec = .{};
    if (c.SDL_OpenAudio(&audio_spec, null) != 0) {
        return error.SdlOpenAudioFailed;
    }
    c.SDL_PauseAudio(0);

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const perma_allocator = gpa.allocator();
    var arena = std.heap.ArenaAllocator.init(perma_allocator);
    defer arena.deinit();
    const transient_allocator = arena.allocator();
    var game_memory = platform.Memory.init(perma_allocator, transient_allocator);
    defer game_memory.deinit();

    const perf_frequency: f64 = @floatFromInt(c.SDL_GetPerformanceFrequency());
    var last_counter = c.SDL_GetPerformanceCounter();

    try game.init(&game_memory);

    var input: platform.Input = platform.Input.init();
    var quit = false;
    while (!quit) {
        const start_counter: u64 = c.SDL_GetPerformanceCounter();
        const delta_time: f64 = @as(f64, @floatFromInt(start_counter - last_counter)) / perf_frequency;
        last_counter = start_counter;
        std.log.debug("Delta time: {d}", .{delta_time});

        const arena_reset = arena.reset(.retain_capacity);
        if (!arena_reset) {
            return error.ArenaResetError;
        }

        var render_queue = render.RenderList.init(transient_allocator);
        defer render_queue.deinit();

        var event: c.SDL_Event = undefined;
        input.advanceFrame(delta_time);
        while (c.SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                c.SDL_QUIT => {
                    quit = true;
                },
                c.SDL_KEYUP, c.SDL_KEYDOWN => {
                    var key: ?*platform.KeyState = null;
                    switch (event.key.keysym.scancode) {
                        c.SDL_SCANCODE_W, c.SDL_SCANCODE_UP => {
                            key = &input.keys[@intFromEnum(platform.Keys.Up)];
                        },
                        c.SDL_SCANCODE_A, c.SDL_SCANCODE_LEFT => {
                            key = &input.keys[@intFromEnum(platform.Keys.Left)];
                        },
                        c.SDL_SCANCODE_D, c.SDL_SCANCODE_RIGHT => {
                            key = &input.keys[@intFromEnum(platform.Keys.Right)];
                        },
                        c.SDL_SCANCODE_S, c.SDL_SCANCODE_DOWN => {
                            key = &input.keys[@intFromEnum(platform.Keys.Down)];
                        },
                        else => {},
                    }

                    if (key) |k| {
                        const is_down = event.type == c.SDL_KEYDOWN;
                        if (k.is_down != is_down) {
                            k.half_transition_count += 1;
                        }
                        k.is_down = is_down;
                    }
                },
                c.SDL_MOUSEMOTION => {
                    input.mouse_x = event.motion.x;
                    input.mouse_y = event.motion.y;
                },
                else => {},
            }
        }

        try game.update(&render_queue, &input, &game_memory);

        renderer_sdl.sdlRenderList(renderer, render_queue);

        c.SDL_RenderPresent(renderer);
    }
}
