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

void renderer_render_rect(render_commands *commands, render_command_rect rect) {
  render_command cmd = {.type = RENDER_COMMAND_RECT, .rect = rect};
  push_command(commands, cmd);
}

void renderer_render_sprite(render_commands *commands, render_command_sprite sprite) {
  render_command cmd = {.type = RENDER_COMMAND_SPRITE, .sprite = sprite};
  push_command(commands, cmd);
}

void delete_texture(render_commands *commands, render_command_delete_texture delete_texture) {
  render_command cmd = {.type = RENDER_COMMAND_DELETE_TEXTURE, .delete_texture = delete_texture};
  push_command(commands, cmd);
}
