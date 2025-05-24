#ifndef HAYAL_RENDERER
#define HAYAL_RENDERER

#include <glm/glm.hpp>

namespace hayal {

// TODO: move
struct SpriteAsset {
  glm::vec2 size;
  unsigned char *data;
  std::optional<uint32_t> texture_handle;
};

struct Rect {
  glm::vec2 pos;
  glm::vec2 size;
  glm::vec4 color;
};

struct Sprite {
  SpriteAsset *asset;
  glm::vec2 pos;
  glm::vec2 size;
};

struct RenderCommands {
  std::vector<Rect> rects;
  std::vector<Sprite> sprites;
  glm::vec4 clear;
};

struct Renderer;
Renderer RendererInit(int framebuffer_width, int framebuffer_height);
void RendererDestroy(Renderer &renderer);
void CommandsRender(Renderer &renderer, RenderCommands &commands);
inline void CommandsClear(RenderCommands &commands) {
  commands.rects.clear();
  commands.sprites.clear();
};

} // namespace hayal

#endif
