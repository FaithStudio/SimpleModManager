#include <switch.h>

#include <mod_browser.h>
#include <toolbox.h> // keep it for debug

//#include <SDL.h>

mod_browser __mod_browser__;

// MAIN
int main(int argc, char **argv){

//  SDL_Event event;
//  SDL_Window *window;
//  SDL_Renderer *renderer;
//  int done = 0, x = 0, w = 1920, h = 1080;
//
//  // mandatory at least on switch, else gfx is not properly closed
//  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
//    SDL_Log("SDL_Init: %s\n", SDL_GetError());
//    return -1;
//  }
//
//  window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1920, 1080, 0);
//  if (!window) {
//    SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
//    SDL_Quit();
//    return -1;
//  }
//
//  renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//  if (!renderer) {
//    SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
//    SDL_Quit();
//    return -1;
//  }

  consoleInit(nullptr);

  int max_depth = 1; // could be a parameter in the future

  __mod_browser__.set_only_show_folders(true);
  __mod_browser__.set_max_relative_depth(max_depth);
  __mod_browser__.initialize();
  __mod_browser__.print_menu();

  // Main loop
  while(appletMainLoop())
  {
    //Scan all the inputs. This should be done once for each frame
    hidScanInput();

    //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
    u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

    if(kDown & KEY_B and __mod_browser__.get_current_relative_depth() == 0){ // back
      break;
    }

    __mod_browser__.scan_inputs(kDown, kHeld);

  } // while

//  SDL_DestroyRenderer(renderer);
//  SDL_DestroyWindow(window);
//  SDL_Quit();

  consoleExit(nullptr);
  return 0;
}