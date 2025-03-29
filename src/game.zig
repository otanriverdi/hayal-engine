const platform = @import("platform/platform.zig");
const render = @import("render/render.zig");
const math = @import("math.zig");
const std = @import("std");

var test_x: f64 = 100;
var test_y: f64 = 100;
var test_velocity: f64 = 500;

pub fn update(render_list: *render.RenderList, input: *platform.Input, _: *platform.Memory) !void {
    const clear = render.Clear{ .color = math.RGBA{ 255, 165, 0, 255 } };
    try render_list.append(clear);

    const up = &input.keys[@intFromEnum(platform.Keys.Up)];
    const down = &input.keys[@intFromEnum(platform.Keys.Down)];
    const left = &input.keys[@intFromEnum(platform.Keys.Left)];
    const right = &input.keys[@intFromEnum(platform.Keys.Right)];

    if (up.is_down) {
        test_y -= test_velocity * input.delta_time;
    }
    if (down.is_down) {
        test_y += test_velocity * input.delta_time;
    }
    if (left.is_down) {
        test_x -= test_velocity * input.delta_time;
    }
    if (right.is_down) {
        test_x += test_velocity * input.delta_time;
    }

    const rect = render.Rectangle{ .position = math.Vec2{
        test_x,
        test_y,
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
