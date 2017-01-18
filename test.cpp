#if WINDOWS
    #include <SDL.h>
#else
    #include <SDL2/SDL.h>
#endif

int main() {
  SDL_Window *window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
  SDL_Event event;
  // SDL_Rect r;

  // r.w = 100;
  // r.h = 50;

  Uint32* pixels = new Uint32[160 * 144];

  while (1) {
    SDL_PumpEvents();
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    int pitch = 0;
    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

    for (int i = 0; i < 160; i++) {
      for (int j = 0; j < 144; j++) {
        for (int k = 0; k < 3; k++) {
          Uint32 r = rand() % 256;
          Uint32 g = rand() % 256;
          Uint32 b = rand() % 256;
          pixels[i * 144 + j] = r<<16 | g<<8 | b;
        }
      }
    }

    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);

    // r.x=rand()%100;
    // r.y=rand()%100;

    // SDL_SetRenderTarget(renderer, texture);
    // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    // SDL_RenderClear(renderer);
    // SDL_RenderDrawRect(renderer,&r);
    // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
    // SDL_RenderFillRect(renderer, &r);
    // SDL_SetRenderTarget(renderer, NULL);
    // SDL_RenderCopy(renderer, texture, NULL, NULL);
    // SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
      }
    }
  }
}