const platform = @import("platform.zig");
const std = @import("std");

fn fillBackbufferWithGradient(offscreen_buffer: *platform.OffscreenBuffer) void {
    var index: usize = 0;
    for (0..offscreen_buffer.height) |y| {
        for (0..offscreen_buffer.width) |x| {
            const r: u8 = @intCast((x * 255) / offscreen_buffer.width); // Horizontal gradient
            const g: u8 = @intCast((y * 255) / offscreen_buffer.height); // Vertical gradient
            const b: u8 = 128; // Constant blue
            const a: u8 = 255; // Full opacity

            // Store the pixel in RGBA format
            offscreen_buffer.pixels[index] = r;
            offscreen_buffer.pixels[index + 1] = g;
            offscreen_buffer.pixels[index + 2] = b;
            offscreen_buffer.pixels[index + 3] = a;
            index += 4; // Move to the next pixel (RGBA)
        }
    }
}

pub fn UpdateAndRender(offsscreen_buffer: *platform.OffscreenBuffer, _: *platform.SoundBuffer, _: *platform.Input, _: *platform.GameMemory) !void {
    fillBackbufferWithGradient(offsscreen_buffer);
}
