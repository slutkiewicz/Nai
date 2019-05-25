#pragma once 


struct game_map_t {
  std::vector<std::string> t; // opis kafelkow
  // # przeszkoda
  // . puste pole
  // , ziemia
};

struct game_state_t {
  // gracze
  std::vector<player_t> players;
  // aktualna klatka
  int frame;

  shared_ptr<game_map_t> world;
};

struct player_intention_t {
  position_t move;
};

struct player_t {
  position_t acceleration;
  position_t velocity;
  position_t position;
  player_intention_t intention;

  std::shared_ptr<SDL_Texture> texture;
};

struct hardware_objects_t {
  SDL_Renderer *renderer;
  SDL_Window *window;
};