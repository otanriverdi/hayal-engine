const math = @import("math.zig");
const std = @import("std");

pub const Rectangle = struct {
    position: math.Vec2,
    size: math.Vec2,
    color: math.RGBA,
};

pub const Clear = struct {
    color: math.RGBA,
};

pub const RenderList = struct {
    rectangles: std.ArrayList(Rectangle),
    clear: Clear,
    allocator: std.mem.Allocator,

    pub fn init(allocator: std.mem.Allocator) RenderList {
        return RenderList{ .allocator = allocator, .rectangles = std.ArrayList(Rectangle).init(allocator), .clear = Clear{ .color = math.RGBA{
            0,
            0,
            0,
            0,
        } } };
    }

    pub fn deinit(self: *RenderList) void {
        self.rectangles.deinit();
    }

    pub fn append(self: *RenderList, element: anytype) !void {
        const T = @TypeOf(element);
        switch (T) {
            Clear => {
                self.clear = element;
            },
            Rectangle => {
                try self.rectangles.append(element);
            },
            else => {
                @compileError("Unsupported type for RenderList.append" ++ @typeName(T));
            },
        }
    }
};
