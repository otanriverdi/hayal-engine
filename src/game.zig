const std = @import("std");

pub const OffscreenBuffer = struct {
    const Self = @This();

    pixels: []u8,
    width: u64,
    height: u64,
    pitch: u64,
    allocator: std.mem.Allocator,

    pub fn init(width: u64, height: u64, allocator: std.mem.Allocator) !Self {
        return Self{
            .allocator = allocator,
            .pixels = try allocator.alloc(u8, width * height * 4),
            .pitch = width * 4,
            .width = width,
            .height = height,
        };
    }

    pub fn deinit(self: *Self) void {
        self.allocator.free(self.pixels);
    }
};

var x_offset: u64 = 0;
var y_offset: u64 = 0;

pub fn UpdateAndRender(buffer: *OffscreenBuffer) !void {
    var row_index: usize = 0;
    for (0..buffer.height) |y| {
        for (0..buffer.width) |x| {
            const red = @as(u8, @intCast((x + x_offset) & 0xFF));
            const green = @as(u8, @intCast((y + y_offset) & 0xFF));
            const blue = @as(u8, @intCast((x ^ y) & 0xFF));
            const alpha = 0xFF;

            const pixel_index = row_index + (x * 4);

            buffer.pixels[pixel_index + 0] = red;
            buffer.pixels[pixel_index + 1] = green;
            buffer.pixels[pixel_index + 2] = blue;
            buffer.pixels[pixel_index + 3] = alpha;
        }
        row_index += buffer.pitch;
    }

    // Simple animation by moving the offsets
    x_offset = (x_offset + 1) % buffer.width;
    y_offset = (y_offset + 1) % buffer.height;
}
