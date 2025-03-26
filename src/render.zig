const math = @import("math.zig");
const std = @import("std");

pub const Rectangle = struct {
    position: math.Vector2,
    size: math.Vector2,
    color: math.RGBA,
};

pub const Clear = struct {
    color: math.RGBA,
};

const ElementType = enum {
    clear,
    rectangle,
};

pub const Element = union(ElementType) {
    clear: Clear,
    rectangle: Rectangle,
};

pub const RenderList = struct {
    rectangles: std.ArrayList(Rectangle),
    clear: Clear,
    allocator: std.mem.Allocator,

    pub fn init(allocator: std.mem.Allocator) RenderList {
        return RenderList{ .allocator = allocator, .rectangles = std.ArrayList(Rectangle).init(allocator), .clear = Clear{ .color = math.RGBA{
            .r = 0,
            .g = 0,
            .b = 0,
            .a = 0,
        } } };
    }

    pub fn deinit(self: *RenderList) void {
        self.rectangles.deinit();
    }

    pub fn append(self: *RenderList, element: Element) !void {
        switch (element) {
            .clear => |clear| {
                self.clear = clear;
            },
            .rectangle => |rect| {
                try self.rectangles.append(rect);
            },
        }
    }
};
