const std = @import("std");
const glfw = @import("glfw");
const c = @cImport({
    @cInclude("miniaudio.h");
});

fn glfwErrorCallback(error_code: glfw.ErrorCode, description: [:0]const u8) void {
    std.log.err("[GLFW]: {}: {s}\n", .{ error_code, description });
}

fn glfwKeyCallback(_: glfw.Window, _: glfw.Key, scancode: i32, action: glfw.Action, _: glfw.Mods) void {
    std.log.info("[GLFW]: input: {any} {d}", .{ action, scancode });
}

fn miniaudioDataCallback(
    _: ?*anyopaque,
    _: ?*anyopaque,
    _: ?*const anyopaque,
    _: c.ma_uint32,
) callconv(.C) void {
    return;
}

pub fn main() !void {
    glfw.setErrorCallback(glfwErrorCallback);
    if (!glfw.init(.{})) {
        std.log.err("[GLFW]: failed to initialize GLFW: {?s}", .{glfw.getErrorString()});
        std.process.exit(1);
    }
    defer glfw.terminate();
    const window = glfw.Window.create(640, 480, "My game", null, null, .{}) orelse {
        std.log.err("[GLFW]: failed to create GLFW window: {?s}", .{glfw.getErrorString()});
        std.process.exit(1);
    };
    glfw.Window.setKeyCallback(window, glfwKeyCallback);
    defer window.destroy();

    var miniaudio_config = c.ma_device_config_init(c.ma_device_type_playback);
    miniaudio_config.playback.format = c.ma_format_f32;
    miniaudio_config.playback.channels = 0;
    miniaudio_config.sampleRate = 0;
    miniaudio_config.dataCallback = miniaudioDataCallback;
    var device: c.ma_device = undefined;
    defer c.ma_device_uninit(&device);
    if (c.ma_device_init(null, &miniaudio_config, &device) != c.MA_SUCCESS) {
        std.log.err("[MINIAUDIO]: Failed to initialize audio device.\n", .{});
        std.process.exit(1);
    }
    if (c.ma_device_start(&device) != c.MA_SUCCESS) {
        std.log.err("[MINIAUDIO]: Failed to start audio device.\n", .{});
        std.process.exit(1);
    }

    while (!window.shouldClose()) {
        window.swapBuffers();
        glfw.pollEvents();
    }
}
