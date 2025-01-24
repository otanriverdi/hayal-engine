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

pub const SoundBuffer = struct {
    const Self = @This();

    samples: []u16,
    sample_count: u32,
    allocator: std.mem.Allocator,

    pub fn init(samples_count: u32, allocator: std.mem.Allocator) !Self {
        return Self{
            .sample_count = samples_count,
            .samples = try allocator.alloc(u16, samples_count),
            .allocator = allocator,
        };
    }

    pub fn clear(self: *Self) void {
        @memset(self.samples, 0);
    }

    pub fn deinit(self: *Self) void {
        self.allocator.free(self.samples);
    }
};

pub fn UpdateAndRender(_: *OffscreenBuffer, _: *SoundBuffer) !void {}
