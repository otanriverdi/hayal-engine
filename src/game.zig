const platform = @import("platform.zig");
const std = @import("std");

fn fillBackbufferWithGradient(offscreen_buffer: *platform.OffscreenBuffer) void {
    for (0..offscreen_buffer.height) |y| {
        for (0..offscreen_buffer.width) |x| {
            const r: u8 = @intCast((x * 255) / offscreen_buffer.width);
            const g: u8 = @intCast((y * 255) / offscreen_buffer.height);
            const b: u8 = 128;
            const a: u8 = 255;

            offscreen_buffer.fillPixel(x, y, r, g, b, a);
        }
    }
}

pub fn updateAndRender(offsscreen_buffer: *platform.OffscreenBuffer, _: *platform.SoundBuffer, _: *platform.Input, _: *platform.GameMemory) !void {
    fillBackbufferWithGradient(offsscreen_buffer);
    drawRectangle(offsscreen_buffer, 10, 10, 30, 30);
}

fn drawRectangle(offscreen_buffer: *platform.OffscreenBuffer, minX: u32, minY: u32, maxX: u32, maxY: u32) void {
    std.debug.assert(minX <= maxX);
    std.debug.assert(minY <= maxY);

    const startX = std.math.clamp(minX, 0, offscreen_buffer.width);
    const startY = std.math.clamp(minY, 0, offscreen_buffer.height);
    const endX = std.math.clamp(maxX, 0, offscreen_buffer.width);
    const endY = std.math.clamp(maxY, 0, offscreen_buffer.height);

    for (startY..endY) |y| {
        for (startX..endX) |x| {
            offscreen_buffer.fillPixel(x, y, 255, 255, 255, 255);
        }
    }
}
