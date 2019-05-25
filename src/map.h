#pragma once 

#include "a_star.h"


struct game_map_t {
    std::map< position_t ,node> nodes_Map;
        std::vector<std::string> t; // opis kafelkow
  // # przeszkoda
  // , ziemia
};

enum event_enum { NONE, QUIT };

struct player_intention_t {
    position_t move;
};

struct player_t {
    position_t position;
    player_intention_t intention;

    std::shared_ptr<SDL_Texture> texture;
};

struct game_state_t {
  // gracze
  std::vector<player_t> players;
  // aktualna klatka
  int frame;
    std::shared_ptr<game_map_t> world;

};

struct hardware_objects_t {
  SDL_Renderer *renderer;
  SDL_Window *window;
};

std::shared_ptr<hardware_objects_t> init_hardware_subsystems(int width, int height,
                                                        bool fscreen);

game_state_t load_level();

std::map<int, player_intention_t>
process_input(std::shared_ptr<hardware_objects_t> hw,
              std::map<event_enum, std::function<void()>> event_handlers);

game_state_t
calculate_next_game_state(const game_state_t &previous_state,
                          std::map<int, player_intention_t> intentions,
                          double dt);
void draw_player(SDL_Renderer *renderer, player_t &player);

void draw_world(SDL_Renderer *renderer, game_state_t &state);