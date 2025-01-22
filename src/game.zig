const std = @import("std");

pub const OffscreenBuffer = struct {
    const Self = @This();

    pixels: []u8,
    width: usize,
    height: usize,
    pitch: usize,
    allocator: std.mem.Allocator,

    pub fn init(width: usize, height: usize, pitch: usize, allocator: std.mem.Allocator) !Self {
        return Self{
            .allocator = allocator,
            .pixels = try allocator.alloc(u8, width * height * 4),
            .pitch = pitch,
            .width = width,
            .height = height,
        };
    }

    pub fn deinit(self: *Self) void {
        self.allocator.free(self.pixels);
    }
};

pub fn UpdateAndRender(_: *OffscreenBuffer) !void {}
