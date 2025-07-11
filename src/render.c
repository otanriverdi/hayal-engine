#include "render.h"
#include <stdint.h>

void render_commands_clear(render_commands *commands) { commands->len = 0; }

static void push_command(render_commands *commands, render_command cmd) {
  assert(commands->len < MAX_RENDER_COMMANDS);
  commands->data[commands->len++] = cmd;
}

void renderer_render_clear(render_commands *commands, render_command_clear clear) {
  render_command cmd = {.type = RENDER_COMMAND_CLEAR, .clear = clear};
  push_command(commands, cmd);
}

void renderer_render_quad(render_commands *commands, render_command_quad quad) {
  render_command cmd = {.type = RENDER_COMMAND_QUAD, .quad = quad};
  push_command(commands, cmd);
}

void renderer_render_glyph(render_commands *commands, render_command_glyph glyph) {
  render_command cmd = {.type = RENDER_COMMAND_GLYPH, .glyph = glyph};
  push_command(commands, cmd);
}

void renderer_delete_texture(render_commands *commands, render_command_delete_texture delete_texture) {
  render_command cmd = {.type = RENDER_COMMAND_DELETE_TEXTURE, .delete_texture = delete_texture};
  push_command(commands, cmd);
}

void renderer_load_texture(render_commands *commands, render_command_load_texture load_texture) {
  render_command cmd = {.type = RENDER_COMMAND_LOAD_TEXTURE, .load_texture = load_texture};
  push_command(commands, cmd);
}

void renderer_load_glyph(render_commands *commands, render_command_load_glyph load_glyph) {
  render_command cmd = {.type = RENDER_COMMAND_LOAD_GLYPH, .load_glyph = load_glyph};
  push_command(commands, cmd);
}
