#include <SDL.h>

#include <cstdlib>
#include <memory>
#include <utility>

#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include "foundation/asset/asset.h"
#include "foundation/thread_pool.h"
#include "imgui.h"

namespace {

struct DestroyWindow {
  void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
};
using WindowPtr = std::unique_ptr<SDL_Window, DestroyWindow>;

struct DestroyRenderer {
  void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
};
using RendererPtr = std::unique_ptr<SDL_Renderer, DestroyRenderer>;

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  const char* TITLE = "GAME";
  const int SCREEN_WIDTH = 16 * 80;
  const int SCREEN_HEIGHT = 9 * 80;

  foundation::ThreadPool pool(SDL_GetCPUCount() * 2);

  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }
  atexit(SDL_Quit);

  WindowPtr window{SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE)};
  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }

  RendererPtr renderer{SDL_CreateRenderer(
      window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
  if (!renderer) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window.get(), renderer.get());
  ImGui_ImplSDLRenderer_Init(renderer.get());

  bool loop = true;
  while (loop) {
    {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT) {
          loop = false;
        }
      }
    }

    int w, h;
    SDL_GetRendererOutputSize(renderer.get(), &w, &h);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();

    SDL_SetRenderDrawColor(renderer.get(), 0x12, 0x34, 0x56, 0x00);
    SDL_RenderClear(renderer.get());
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer.get());
  }

  return EXIT_SUCCESS;
}
