const render = @import("./render.zig");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

pub fn sdlRenderList(renderer: *c.SDL_Renderer, queue: render.RenderList) void {
    _ = c.SDL_SetRenderDrawColor(renderer, queue.clear.color[0], queue.clear.color[1], queue.clear.color[2], queue.clear.color[3]);
    _ = c.SDL_RenderClear(renderer);

    for (queue.rectangles.items) |rect| {
        const sdl_rect = c.SDL_Rect{ .x = @intFromFloat(rect.position[0]), .y = @intFromFloat(rect.position[1]), .h = @intFromFloat(rect.size[0]), .w = @intFromFloat(rect.size[1]) };
        _ = c.SDL_SetRenderDrawColor(renderer, rect.color[0], rect.color[1], rect.color[2], rect.color[3]);
        _ = c.SDL_RenderDrawRect(renderer, &sdl_rect);
        _ = c.SDL_RenderFillRect(renderer, &sdl_rect);
    }
}
