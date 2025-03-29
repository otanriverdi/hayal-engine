const std = @import("std");
const platform = @import("platform.zig");
const game = @import("game.zig");
const render = @import("render.zig");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

pub fn main() !void {
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
    var game_memory = platform.GameMemory.init(perma_allocator, transient_allocator);
    defer game_memory.deinit();

    const perf_frequency: f64 = @floatFromInt(c.SDL_GetPerformanceFrequency());
    var last_counter = c.SDL_GetPerformanceCounter();

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

        var input: platform.Input = platform.Input.init(delta_time);
        var event: c.SDL_Event = undefined;
        while (c.SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                c.SDL_QUIT => {
                    quit = true;
                },
                c.SDL_KEYUP, c.SDL_KEYDOWN => {
                    var is_down = true;
                    if (event.key.state == c.SDL_RELEASED) {
                        is_down = false;
                    }
                    var current_input: ?*platform.KeyState = null;
                    switch (event.key.keysym.scancode) {
                        c.SDL_SCANCODE_W, c.SDL_SCANCODE_UP => {
                            current_input = &input.up;
                        },
                        c.SDL_SCANCODE_A, c.SDL_SCANCODE_LEFT => {
                            current_input = &input.left;
                        },
                        c.SDL_SCANCODE_D, c.SDL_SCANCODE_RIGHT => {
                            current_input = &input.right;
                        },
                        c.SDL_SCANCODE_S, c.SDL_SCANCODE_DOWN => {
                            current_input = &input.down;
                        },
                        c.SDL_SCANCODE_SPACE => {
                            current_input = &input.main_action;
                        },
                        c.SDL_SCANCODE_INSERT => {
                            current_input = &input.secondary_action;
                        },
                        else => {},
                    }
                    if (current_input) |i| {
                        if (i.has_ended_down != is_down) {
                            i.half_transition_count += 1;
                        }
                        i.has_ended_down = is_down;
                    }
                },
                else => {},
            }
        }

        try game.update(&render_queue, &input, &game_memory);

        sdlRenderQueue(renderer, render_queue);

        c.SDL_RenderPresent(renderer);
    }
}

fn sdlRenderQueue(renderer: *c.SDL_Renderer, queue: render.RenderList) void {
    _ = c.SDL_SetRenderDrawColor(renderer, queue.clear.color[0], queue.clear.color[1], queue.clear.color[2], queue.clear.color[3]);
    _ = c.SDL_RenderClear(renderer);

    for (queue.rectangles.items) |rect| {
        const sdl_rect = c.SDL_Rect{ .x = @intFromFloat(rect.position[0]), .y = @intFromFloat(rect.position[1]), .h = @intFromFloat(rect.size[0]), .w = @intFromFloat(rect.size[1]) };
        _ = c.SDL_SetRenderDrawColor(renderer, rect.color[0], rect.color[1], rect.color[2], rect.color[3]);
        _ = c.SDL_RenderDrawRect(renderer, &sdl_rect);
        _ = c.SDL_RenderFillRect(renderer, &sdl_rect);
    }
}
