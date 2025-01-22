const std = @import("std");
const game = @import("game.zig");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    if (c.SDL_Init(c.SDL_INIT_VIDEO) != 0) {
        c.SDL_LogError(c.SDL_LOG_CATEGORY_ERROR, "Unable to initialize SDL: %s", c.SDL_GetError());
        return error.SDLInitializationFailed;
    }
    defer c.SDL_Quit();

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

    var quit = false;
    while (!quit) {
        var event: c.SDL_Event = undefined;
        while (c.SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                c.SDL_QUIT => {
                    quit = true;
                },
                c.SDL_KEYUP, c.SDL_KEYDOWN => {
                    var was_down = false;
                    if (event.key.state == c.SDL_RELEASED) {
                        was_down = true;
                    } else if (event.key.repeat != 0) {
                        was_down = true;
                    }
                    c.SDL_Log("Key press = %i was_down = %d", event.key.keysym.sym, was_down);
                },
                else => {},
            }
        }
        var window_width: c_int = undefined;
        var window_height: c_int = undefined;
        c.SDL_GetWindowSize(window, &window_width, &window_height);

        var offscreen_buffer = try game.OffscreenBuffer.init(@as(usize, @intCast(window_width)), @as(usize, @intCast(window_height)), 0, allocator);
        defer offscreen_buffer.deinit();
        try game.UpdateAndRender(&offscreen_buffer);

        const texture = c.SDL_CreateTexture(renderer, c.SDL_PIXELFORMAT_ARGB8888, c.SDL_TEXTUREACCESS_STREAMING, window_width, window_height) orelse {
            return error.SDLTextureFailed;
        };
        defer c.SDL_DestroyTexture(texture);
        _ = c.SDL_UpdateTexture(texture, 0, @ptrCast(&offscreen_buffer.pixels), 0);

        _ = c.SDL_RenderClear(renderer);
        _ = c.SDL_RenderCopy(renderer, texture, 0, 0);
        c.SDL_RenderPresent(renderer);
    }
}
