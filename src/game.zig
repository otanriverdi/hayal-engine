const platform = @import("platform/platform.zig");
const render = @import("render/render.zig");
const math = @import("math.zig");
const std = @import("std");

const Player = struct {
    position: math.Vec2,
    velocity: f64,
};

pub fn init(memory: *platform.Memory) !void {
    try memory.setGameState(Player);
    const player = try memory.getGameState(Player);
    player.* = .{ .position = .{ 100, 100 }, .velocity = 200 };
}

pub fn update(render_list: *render.RenderList, input: *platform.Input, memory: *platform.Memory) !void {
    const clear = render.Clear{ .color = math.RGBA{ 255, 165, 0, 255 } };
    try render_list.append(clear);

    const player = try memory.getGameState(Player);

    const up = &input.keys[@intFromEnum(platform.Keys.Up)];
    const down = &input.keys[@intFromEnum(platform.Keys.Down)];
    const left = &input.keys[@intFromEnum(platform.Keys.Left)];
    const right = &input.keys[@intFromEnum(platform.Keys.Right)];

    if (up.is_down) {
        player.position[1] -= player.velocity * input.delta_time;
    }
    if (down.is_down) {
        player.position[1] += player.velocity * input.delta_time;
    }
    if (left.is_down) {
        player.position[0] -= player.velocity * input.delta_time;
    }
    if (right.is_down) {
        player.position[0] += player.velocity * input.delta_time;
    }

    const rect = render.Rectangle{ .position = math.Vec2{
        player.position[0],
        player.position[1],
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
