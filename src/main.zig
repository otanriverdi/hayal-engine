const std = @import("std");
const glfw = @import("glfw");

fn errorCallback(error_code: glfw.ErrorCode, description: [:0]const u8) void {
    std.log.err("[GLFW]: {}: {s}\n", .{ error_code, description });
}

fn keyCallback(_: glfw.Window, _: glfw.Key, scancode: i32, action: glfw.Action, _: glfw.Mods) void {
    std.log.info("[GLFW]: input: {any} {d}", .{ action, scancode });
}

pub fn main() !void {
    glfw.setErrorCallback(errorCallback);

    if (!glfw.init(.{})) {
        std.log.err("[GLFW]: failed to initialize GLFW: {?s}", .{glfw.getErrorString()});
        std.process.exit(1);
    }
    defer glfw.terminate();

    const window = glfw.Window.create(640, 480, "My game", null, null, .{}) orelse {
        std.log.err("[GLFW]: failed to create GLFW window: {?s}", .{glfw.getErrorString()});
        std.process.exit(1);
    };
    defer window.destroy();

    while (!window.shouldClose()) {
        window.swapBuffers();
        glfw.Window.setKeyCallback(window, keyCallback);
        glfw.pollEvents();
    }
}
