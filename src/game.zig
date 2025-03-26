const platform = @import("platform.zig");
const render = @import("render.zig");
const math = @import("math.zig");
const std = @import("std");

var test_x: f64 = 15;

pub fn update(render_list: *render.RenderList, input: *platform.Input, _: *platform.GameMemory) !void {
    test_x += 500 * input.delta_time;

    const clear = render.Element{ .clear = render.Clear{ .color = math.RGBA{ .r = 255, .g = 165, .b = 0, .a = 255 } } };
    try render_list.append(clear);

    const rect = render.Element{ .rectangle = render.Rectangle{ .position = math.Vector2{
        .x = test_x,
        .y = 15,
    }, .size = math.Vector2{
        .x = 100,
        .y = 100,
    }, .color = math.RGBA{
        .r = 4,
        .g = 59,
        .b = 92,
        .a = 255,
    } } };
    try render_list.append(rect);
}
