const std = @import("std");

pub const Vec2 = std.meta.Tuple(&[2]type{ f64, f64 });
pub const RGBA = std.meta.Tuple(&[4]type{ u8, u8, u8, u8 });
