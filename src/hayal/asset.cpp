#include "asset.h"
#include "mem.h"
#include "miniaudio.h"
#include "platform.h"
#include "stb_image.h"

using namespace hayal;

std::expected<Image, AssetError> hayal::LoadPng(std::string path, Arena arena) {
  auto allocator = ArenaAllocator<unsigned char>(arena);
  auto res = platform::ReadEntireFile(path, allocator);
  if (!res.has_value()) {
    return std::unexpected(AssetError::READ_FILE);
  }
  auto file = res.value();

  Image image = {};
  int x, y, n;
  image.data = stbi_load_from_memory(file.data(), static_cast<int>(file.size()),
                                     &x, &y, &n, 4);
  image.size.x = x;
  image.size.y = y;

  return image;
};

std::expected<Sound, AssetError> hayal::LoadWav(std::string path, int channels,
                                                int freq, Arena arena) {
  auto allocator = ArenaAllocator<unsigned char>(arena);
  auto res = platform::ReadEntireFile(path, allocator);
  if (!res.has_value()) {
    return std::unexpected(AssetError::READ_FILE);
  }
  auto file = res.value();

  ma_decoder_config config =
      ma_decoder_config_init(ma_format_f32, channels, freq);

  ma_decoder decoder;
  if (ma_decoder_init_memory(file.data(), file.size(), &config, &decoder) !=
      MA_SUCCESS) {
    return std::unexpected(AssetError::DECODE_AUDIO);
  }

  ma_uint64 frame_count;
  if (ma_decoder_get_length_in_pcm_frames(&decoder, &frame_count) !=
      MA_SUCCESS) {
    ma_decoder_uninit(&decoder);
    return std::unexpected(AssetError::DECODE_AUDIO);
  }

  float *buffer = static_cast<float *>(
      ArenaAlloc(arena, frame_count * channels, alignof(float)));
  if (!buffer) {
    ma_decoder_uninit(&decoder);
    return std::unexpected(AssetError::NO_ALLOC);
  }

  ma_uint64 frames_read;
  ma_decoder_read_pcm_frames(&decoder, buffer, frame_count, &frames_read);

  if (frames_read < frame_count) {
    ma_decoder_uninit(&decoder);
    return std::unexpected(AssetError::DECODE_AUDIO);
  }

  Sound sound = {};
  sound.frame_count = frame_count;
  sound.data = buffer;

  ma_decoder_uninit(&decoder);

  return sound;
};
