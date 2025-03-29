const std = @import("std");
const platform = @import("platform/platform.zig");

pub fn main() !void {
    try platform.platformMain();
}
