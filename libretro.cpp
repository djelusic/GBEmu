#include "libretro.h"
#include "GB.h"

GB *gb;
retro_environment_t env_cb;
retro_video_refresh_t video_cb;
retro_audio_sample_t audio_sample_cb;
retro_audio_sample_batch_t audio_sample_batch_cb;
retro_input_poll_t input_poll_cb;
retro_input_state_t input_state_cb;

RETRO_API void retro_set_environment(retro_environment_t cb) {
  env_cb = cb;
  bool no_rom = true;
  cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);
}

RETRO_API void retro_set_video_refresh(retro_video_refresh_t cb) {
  video_cb = cb;
}

RETRO_API void retro_set_audio_sample(retro_audio_sample_t cb) {
  audio_sample_cb = cb;
}

RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
  audio_sample_batch_cb = cb;
}

RETRO_API void retro_set_input_poll(retro_input_poll_t cb) {
  input_poll_cb = cb;
}

RETRO_API void retro_set_input_state(retro_input_state_t cb) {
  input_state_cb = cb;
}

RETRO_API void retro_init(void) {
  gb = new GB("zelda.gb");
}

RETRO_API void retro_deinit(void) {
}

RETRO_API unsigned retro_api_version(void) {
  return RETRO_API_VERSION;
}

RETRO_API void retro_get_system_info(retro_system_info * info) {
  info->block_extract = false;
  info->library_name = "GBEmu";
  info->library_version = "0.1";
  info->need_fullpath = false;
  info->valid_extensions = "gb|gbc";
}

RETRO_API void retro_get_system_av_info(retro_system_av_info * info) {
  info->timing.fps = 60.0f;
  info->timing.sample_rate = 44100;
  info->geometry.base_width = 160;
  info->geometry.base_height = 144;
  info->geometry.max_width = 160;
  info->geometry.max_height = 144;
  info->geometry.aspect_ratio = 160.0f / 144.0f;

  int pixel_format = RETRO_PIXEL_FORMAT_XRGB8888;
  env_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format);
}

RETRO_API void retro_set_controller_port_device(unsigned port, unsigned device) {
}

RETRO_API void retro_reset(void) {
}

RETRO_API void retro_run(void) {
  gb->AdvanceFrame();
  video_cb(gb->GetGraphics()->GetDisplayPixels(), 160, 144, 0);
}

RETRO_API size_t retro_serialize_size(void) {
  return 0;
}

RETRO_API bool retro_serialize(void * data, size_t size) {
  return false;
}

RETRO_API bool retro_unserialize(const void * data, size_t size) {
  return false;
}

RETRO_API void retro_cheat_reset(void) {
}

RETRO_API void retro_cheat_set(unsigned index, bool enabled, const char * code) {
}

RETRO_API bool retro_load_game(const retro_game_info * game) {
  return true;
}

RETRO_API bool retro_load_game_special(unsigned game_type, const retro_game_info * info, size_t num_info) {
  return false;
}

RETRO_API void retro_unload_game(void) {
}

RETRO_API unsigned retro_get_region(void) {
  return 0;
}

RETRO_API void * retro_get_memory_data(unsigned id) {
  return nullptr;
}

RETRO_API size_t retro_get_memory_size(unsigned id) {
  return 0;
}
