const std = @import("std");
const builtin = @import("builtin");

pub fn platformMain() !void {
    switch (builtin.target.os.tag) {
        .linux, .macos => try @import("platform_sdl.zig").platformMain(),
        else => @compileError("Unsupported platform"),
    }
}

pub const KeyState = struct {
    half_transition_count: u64,
    is_down: bool,
    was_down: bool,
};

pub const Keys = enum(u8) {
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
    ActionA = 4,
    ActionB = 5,
};

const key_count = @typeInfo(Keys).Enum.fields.len;

pub const Input = struct {
    keys: [key_count]KeyState,
    mouse_x: i32,
    mouse_y: i32,

    delta_time: f64,

    pub fn init() Input {
        var self: Input = undefined;
        for (0..self.keys.len) |i| {
            self.keys[i] = .{
                .is_down = false,
                .was_down = false,
                .half_transition_count = 0,
            };
        }
        self.delta_time = 0;
        self.mouse_x = 0;
        self.mouse_y = 0;
        return self;
    }

    pub fn advanceFrame(
        self: *Input,
        delta_time: f64,
    ) void {
        self.delta_time = delta_time;
        for (0..self.keys.len) |i| {
            self.keys[i].was_down = self.keys[i].is_down;
            self.keys[i].half_transition_count = 0;
        }
    }
};

pub const Memory = struct {
    perma_allocator: std.mem.Allocator,
    transient_allocator: std.mem.Allocator,

    game_state: ?*anyopaque,
    game_state_deinit_fn: ?*const fn (std.mem.Allocator, *anyopaque) void = null,

    pub fn init(perma_allocator: std.mem.Allocator, transient_allocator: std.mem.Allocator) Memory {
        return Memory{
            .perma_allocator = perma_allocator,
            .transient_allocator = transient_allocator,
            .game_state = null,
        };
    }

    pub fn getGameState(self: *Memory, comptime T: type) !*T {
        if (self.game_state == null) {
            return error.GameStateUninitialized;
        }

        return @ptrCast(@alignCast(self.game_state.?));
    }

    pub fn setGameState(self: *Memory, comptime T: type) !void {
        self.deinit();
        const game_state = try self.perma_allocator.create(T);
        self.game_state = @ptrCast(game_state);
        self.game_state_deinit_fn = struct {
            fn deinit(allocator: std.mem.Allocator, ptr: *anyopaque) void {
                const typed_ptr: *T = @ptrCast(@alignCast(ptr));
                allocator.destroy(typed_ptr);
            }
        }.deinit;
    }

    pub fn deinit(self: *Memory) void {
        if (self.game_state == null) {
            return;
        }
        std.debug.assert(self.game_state_deinit_fn != null);
        self.game_state_deinit_fn.?(self.perma_allocator, self.game_state.?);
        self.game_state = null;
        self.game_state_deinit_fn = null;
    }
};
