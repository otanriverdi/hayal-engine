const std = @import("std");

pub const KeyState = struct {
    half_transition_count: u64,
    has_ended_down: bool,
};

pub const Input = struct {
    up: KeyState,
    down: KeyState,
    left: KeyState,
    right: KeyState,
    main_action: KeyState,
    secondary_action: KeyState,

    delta_time: f64,

    pub fn init(delta_time: f64) Input {
        return Input{
            .delta_time = delta_time,
            .up = KeyState{ .half_transition_count = 0, .has_ended_down = false },
            .down = KeyState{ .half_transition_count = 0, .has_ended_down = false },
            .left = KeyState{ .half_transition_count = 0, .has_ended_down = false },
            .right = KeyState{ .half_transition_count = 0, .has_ended_down = false },
            .main_action = KeyState{ .half_transition_count = 0, .has_ended_down = false },
            .secondary_action = KeyState{ .half_transition_count = 0, .has_ended_down = false },
        };
    }
};

pub const GameMemory = struct {
    const Self = @This();

    perma_allocator: std.mem.Allocator,
    transient_allocator: std.mem.Allocator,

    game_state: ?*anyopaque,
    game_state_size: ?usize,

    pub fn init(perma_allocator: std.mem.Allocator, transient_allocator: std.mem.Allocator) Self {
        return Self{
            .perma_allocator = perma_allocator,
            .transient_allocator = transient_allocator,
            .game_state = null,
            .game_state_size = null,
        };
    }

    pub fn getGameState(self: *Self, comptime T: type) !*T {
        if (self.game_state == null) {
            const game_state = try self.perma_allocator.create(T);
            self.game_state = @ptrCast(game_state);
            self.game_state_size = @sizeOf(T);
        }

        if (@sizeOf(T) != self.game_state_size) {
            return error.GameStateSizeMismatch;
        }

        return @ptrCast(self.game_state);
    }

    pub fn deinit(self: *Self) void {
        if (self.game_state) |game_state| {
            std.debug.assert(self.game_state_size != null);
            self.perma_allocator.free(@as([*]u8, @ptrCast(game_state))[0..self.game_state_size.?]);
            self.game_state = null;
            self.game_state_size = null;
        }
    }
};
