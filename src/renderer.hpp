#ifndef HAYAL_RENDERER
#define HAYAL_RENDERER

#include <glm/glm.hpp>
#include <vector>

namespace Renderer {

struct Rect {
  glm::vec2 pos;
  glm::vec2 size;
  glm::vec4 color;
};

struct Sprite {
  void *sprite;
  glm::vec2 pos;
  glm::vec2 size;
};

struct Commands {
  std::vector<Rect> rects;
  std::vector<Sprite> sprites;
  glm::vec4 clear;
};

struct Renderer;
Renderer RendererInit(int framebuffer_width, int framebuffer_height);
void RendererDestroy(Renderer &renderer);
void CommandsRender(Renderer &renderer, Commands &commands);
inline void CommandsClear(Commands &commands) {
  commands.rects.clear();
  commands.sprites.clear();
};

} // namespace Renderer

#endif
