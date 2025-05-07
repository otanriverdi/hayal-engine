#ifndef HAYAL_GAME
#define HAYAL_GAME

#include "mem.hpp"

namespace Game {

struct Memory {
  Mem::Arena temp_memory;
  Mem::FixedBuffer perma_memory;
};

} // namespace Game

#endif
