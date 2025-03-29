const platform = @import("platform.zig");
const render = @import("render.zig");
const math = @import("math.zig");
const std = @import("std");

var test_x: f64 = 15;

pub fn update(render_list: *render.RenderList, input: *platform.Input, _: *platform.GameMemory) !void {
    test_x += 500 * input.delta_time;

    const clear = render.Clear{ .color = math.RGBA{ 255, 165, 0, 255 } };
    try render_list.append(clear);

    const rect = render.Rectangle{ .position = math.Vec2{
        test_x,
        15,
    }, .size = math.Vec2{
        100,
        100,
    }, .color = math.RGBA{
        4,
        59,
        92,
        255,
    } };
    try render_list.append(rect);
}
