#include "render.h"
#include <stdint.h>
#include <stdlib.h>

render_commands render_commands_init(uint64_t size) {
  uint32_t unit_cap = size / sizeof(render_command);
  return (render_commands){.data = malloc(size), .cap = unit_cap, .len = 0, .camera_pos = {0.0f, 0.0f}};
}

void render_commands_clear(render_commands *commands) { commands->len = 0; }

static void push_command(render_commands *commands, render_command cmd) {
  if (commands->len < commands->cap) {
    commands->data[commands->len++] = cmd;
  }
}

void render_clear(render_commands *commands, render_command_clear clear) {
  render_command cmd = {.type = RENDER_COMMAND_CLEAR, .clear = clear};
  push_command(commands, cmd);
}

void render_rect(render_commands *commands, render_command_rect rect) {
  render_command cmd = {.type = RENDER_COMMAND_RECT, .rect = rect};
  push_command(commands, cmd);
}

void render_sprite(render_commands *commands, render_command_sprite sprite) {
  render_command cmd = {.type = RENDER_COMMAND_SPRITE, .sprite = sprite};
  push_command(commands, cmd);
}

void delete_texture(render_commands *commands, render_command_delete_texture delete_texture) {
  render_command cmd = {.type = RENDER_COMMAND_DELETE_TEXTURE, .delete_texture = delete_texture};
  push_command(commands, cmd);
}

void render_commands_free(render_commands *commands) {
  free(commands->data);
  commands->cap = 0;
  commands->len = 0;
}
