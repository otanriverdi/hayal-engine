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

pub const KeyState = struct {
    const Self = @This();

    half_transition_count: u64,
    has_ended_down: bool,

    pub fn init() Self {
        return Self{
            .half_transition_count = 0,
            .has_ended_down = false,
        };
    }
};

pub const Input = struct {
    const Self = @This();

    up: KeyState,
    down: KeyState,
    left: KeyState,
    right: KeyState,
    main_button: KeyState,
    secondary_button: KeyState,

    pub fn init() Self {
        return Self{
            .up = KeyState.init(),
            .down = KeyState.init(),
            .left = KeyState.init(),
            .right = KeyState.init(),
            .main_button = KeyState.init(),
            .secondary_button = KeyState.init(),
        };
    }
};

pub const GameMemory = struct {
    const Self = @This();

    perma_allocator: std.mem.Allocator,
    game_state: ?*anyopaque,
    game_state_size: ?usize,

    transient_allocator: std.mem.Allocator,

    pub fn init(perma_allocator: std.mem.Allocator, transient_allocator: std.mem.Allocator) Self {
        return Self{ .perma_allocator = perma_allocator, .game_state = null, .game_state_size = null, .transient_allocator = transient_allocator };
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
