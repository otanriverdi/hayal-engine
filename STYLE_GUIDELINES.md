# Hayal Game Engine - C Style Guidelines

This document outlines the coding conventions and style standards used throughout the Hayal game engine codebase to ensure consistency and maintainability.

## Naming Conventions

### Variable Names

- **Use all lowercase with optional underscores**: `counter`, `buffer_size`, `temp_ptr`
- **Prefix with contextual nouns when helpful**: `window_width`, `audio_volume`, `mouse_x`
- **Avoid Hungarian notation and overly cryptic names**
- **Use descriptive names**: prefer `rectangle_count` over `rc`

Examples:

```c
int frame_count;
float delta_time;
Vec2 player_position;
uint32_t texture_id;
```

### Function Names

- **Use all lowercase with underscores**: `init_memory()`, `draw_rectangle()`
- **Name functions with verbs for actions**: `render_sprite()`, `update_physics()`
- **Prefix functions with their module/area**: `audio_init()`, `input_update()`, `render_clear()`
- **Use consistent prefixes for related functionality**

Examples:

```c
void game_init(game_memory *memory);
void render_commands_clear(render_commands *commands);
Vec2 vec2_normalize(vec2 v);
void platform_write_file(const char *path, void *data, size_t size);
```

### Type Names

- **Use lowercase with optional underscores for structs**: `player`, `window_dimensions`, `audio_state`
- **Keep names noun-based (they represent things, not actions)**
- **Avoid unnecessary struct* or s* prefixes**
- **Prefer descriptive, short, and context-aware names**

Examples:

```c
struct vec2 {
    float x, y;
};

struct game_memory {
    struct arena perma_memory;
    struct arena temp_memory;
};

struct player {
    struct vec2 position;
    struct vec2 velocity;
    float health;
};
```

### Constants and Enums

- **Use ALL_CAPS with underscores for constants**: `MAX_BUFFER_SIZE`, `DEFAULT_WINDOW_WIDTH`
- **Enum values use ALL_CAPS with descriptive prefixes**: `KEY_W`, `KEY_SPACE`, `SHADER_VERTEX`

Examples:

```c
enum GameKeys {
    KEY_W = 1,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_SPACE,
    KEY_COUNT,
};

#define MAX_RENDER_COMMANDS 1024
```

## File Structure and Organization

### File Names

- **All lowercase with underscores if needed**: `game_logic.c`, `render_backend.c`
- **File names should reflect contained functions**: `audio.c` contains `audio_*()` functions
- **Use descriptive, domain-specific names**: `platform_linux.c`, `render_gl.c`

### File Organization

- **Group code by domain**: `audio.c/.h`, `render.c/.h`, `game.c/.h`
- **Keep flat file hierarchy unless unavoidable**
- **Use platform-specific suffixes when necessary**: `platform_linux.c`, `render_gl.c`

### Header Files

- **Use classic `#ifndef` include guards**:

```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H
// header contents
#endif
```

- **Guard name should be UPPERCASE with underscores**: `GAME_H`, `RENDER_H`, `MATH_H`
- **Include closing comment for clarity**: `#endif // MODULE_NAME_H`

## Function Visibility and Organization

### Static Functions

- **Use `static` for internal-only functions** that should not be exposed outside the module
- **Group static helper functions at the top of .c files** before public functions
- **Static functions follow the same naming conventions as public functions**

Examples:

```c
// Internal helper functions
static void calculate_2d_quad_points(Vec2 pos, Vec2 size, float *vertices);
static void compile_shader(const char *source, GLenum type);

// Public API functions
void renderer_init(int width, int height);
void renderer_process_commands(Renderer *renderer, RenderCommands *commands);
```
