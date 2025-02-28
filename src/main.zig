const std = @import("std");
const platform = @import("platform.zig");
const game = @import("game.zig");
const c = @cImport({
    @cInclude("SDL2/SDL.h");
});

const AUDIO_SAMPLES_PER_SECOND = 48000;
const TARGET_FRAME_RATE = 60;
const TARGET_WIDTH = 960;
const TARGET_HEIGHT = 520;
const BYTES_PER_PIXEL = 4;

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    defer {
        const deinit_status = gpa.deinit();
        if (deinit_status == .leak) @panic("Memory leak detected");
    }

    if (c.SDL_Init(c.SDL_INIT_VIDEO | c.SDL_INIT_AUDIO) != 0) {
        return error.SDLInitFailed;
    }
    defer c.SDL_Quit();

    const window = createSdlWindow() orelse {
        return error.SDLWindowInitFailed;
    };
    defer c.SDL_DestroyWindow(window);

    const renderer = createSdlRenderer(window) orelse {
        return error.SDLRendererInitFailed;
    };
    defer c.SDL_DestroyRenderer(renderer);

    const audio_device = openSdlAudioDevice(AUDIO_SAMPLES_PER_SECOND, TARGET_FRAME_RATE) orelse {
        return error.SDLAudioDeviceInitFailed;
    };
    c.SDL_PauseAudioDevice(audio_device, 0);
    var sound_buffer = try platform.SoundBuffer.init(AUDIO_SAMPLES_PER_SECOND / TARGET_FRAME_RATE, allocator);
    defer sound_buffer.deinit();

    var offscreen_buffer = try platform.OffscreenBuffer.init(TARGET_WIDTH, TARGET_HEIGHT, BYTES_PER_PIXEL, allocator);
    defer offscreen_buffer.deinit();
    const texture = createSdlTexture(renderer, TARGET_WIDTH, TARGET_HEIGHT) orelse {
        return error.SDLTextureInitFailed;
    };

    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const arena_allocator = arena.allocator();
    var game_memory = platform.GameMemory.init(allocator, arena_allocator);
    defer game_memory.deinit();

    const perf_frequency: u64 = c.SDL_GetPerformanceFrequency();
    const fixed_timestep: f64 = 1.0 / @as(f32, TARGET_FRAME_RATE);
    var last_frame_counter: u64 = c.SDL_GetPerformanceCounter();
    var time_to_advance: f64 = 0.0;

    var quit = false;
    while (!quit) {
        const start_counter: u64 = c.SDL_GetPerformanceCounter();
        const last_frame_time: f64 = @as(f64, @floatFromInt(start_counter - last_frame_counter)) / @as(f64, @floatFromInt(perf_frequency));
        time_to_advance += last_frame_time;
        last_frame_counter = start_counter;

        var input: platform.Input = undefined;
        var event: c.SDL_Event = undefined;
        while (c.SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                c.SDL_QUIT => {
                    quit = true;
                },
                c.SDL_KEYUP, c.SDL_KEYDOWN => {
                    input = parseSdlInput(event);
                },
                else => {},
            }
        }

        sound_buffer.clear();

        while (time_to_advance >= fixed_timestep) {
            try game.updateAndRender(&offscreen_buffer, &sound_buffer, &input, &game_memory);
            time_to_advance -= fixed_timestep;
        }

        _ = c.SDL_QueueAudio(audio_device, @ptrCast(sound_buffer.samples), sound_buffer.sample_count);
        _ = c.SDL_UpdateTexture(texture, 0, @ptrCast(offscreen_buffer.buffer), @intCast(offscreen_buffer.pitch));
        _ = c.SDL_RenderClear(renderer);
        _ = c.SDL_RenderCopy(renderer, texture, 0, 0);
        c.SDL_RenderPresent(renderer);

        const end_counter: u64 = c.SDL_GetPerformanceCounter();
        const elapsed: f64 = @as(f64, @floatFromInt(end_counter - start_counter)) / @as(f64, @floatFromInt(perf_frequency));
        if (elapsed < fixed_timestep) {
            const ms_delay: f64 = (fixed_timestep - elapsed) * 1000.0;
            if (ms_delay > 0) {
                c.SDL_Delay(@as(u32, @intFromFloat(ms_delay)));
            }
        }
    }
}

fn createSdlTexture(renderer: ?*c.SDL_Renderer, width: c_int, height: c_int) ?*c.SDL_Texture {
    return c.SDL_CreateTexture(renderer, c.SDL_PIXELFORMAT_RGBA8888, c.SDL_TEXTUREACCESS_TARGET, width, height);
}

fn createSdlWindow() ?*c.SDL_Window {
    return c.SDL_CreateWindow("game", c.SDL_WINDOWPOS_UNDEFINED, c.SDL_WINDOWPOS_UNDEFINED, 1920, 1080, c.SDL_WINDOW_OPENGL);
}

fn createSdlRenderer(window: ?*c.SDL_Window) ?*c.SDL_Renderer {
    return c.SDL_CreateRenderer(window, -1, c.SDL_RENDERER_ACCELERATED | c.SDL_RENDERER_PRESENTVSYNC);
}

fn openSdlAudioDevice(samples_per_second: u16, target_frame_rate: u16) ?c.SDL_AudioDeviceID {
    const audio_channels = 2;
    const audio_samples_per_frame = samples_per_second / target_frame_rate;
    var audio_spec: c.SDL_AudioSpec = .{
        .channels = 2,
        .format = c.AUDIO_S16LSB,
        .freq = samples_per_second,
        .samples = audio_samples_per_frame * @sizeOf(u16) * audio_channels,
    };

    const device_id = c.SDL_OpenAudioDevice(null, 0, &audio_spec, null, 0);
    return if (device_id != 0) device_id else null;
}

fn parseSdlInput(event: c.SDL_Event) platform.Input {
    std.debug.assert(event.type == c.SDL_KEYDOWN or event.type == c.SDL_KEYUP);

    var input = platform.Input.init();
    var is_down = false;
    if (event.key.state == c.SDL_RELEASED) {
        is_down = true;
    }
    var current_input: ?*platform.KeyState = null;
    switch (event.key.keysym.scancode) {
        c.SDL_SCANCODE_W, c.SDL_SCANCODE_UP => {
            current_input = &input.up;
        },
        c.SDL_SCANCODE_A, c.SDL_SCANCODE_LEFT => {
            current_input = &input.left;
        },
        c.SDL_SCANCODE_D, c.SDL_SCANCODE_RIGHT => {
            current_input = &input.right;
        },
        c.SDL_SCANCODE_S, c.SDL_SCANCODE_DOWN => {
            current_input = &input.down;
        },
        c.SDL_SCANCODE_SPACE => {
            current_input = &input.main_action;
        },
        c.SDL_SCANCODE_INSERT => {
            current_input = &input.secondary_action;
        },
        else => {},
    }
    if (current_input) |i| {
        if (i.has_ended_down != is_down) {
            i.half_transition_count += 1;
        }
        i.has_ended_down = is_down;
    }

    return input;
}
