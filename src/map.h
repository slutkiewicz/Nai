#pragma once 

#include "a_star.h"


struct game_map_t {
    std::map< position_t/*CORDS*/ , node> nodes_Map;
        std::vector<std::string> t; // opis kafelkow
  // # WALL
  // , MARBLE
  // . GRASS
  // ; WATER
  // : FIRE
};

enum event_enum { NONE, QUIT  };

struct player_t {
    int id;
   player_t(){
       id=counter();
   }

    position_t position{};
    std::list<position_t> intention;
    std::shared_ptr<SDL_Texture> texture;
};

struct pointer_t {

    std::pair<position_t,game_enum > position;

    std::shared_ptr<SDL_Texture> texture;
};

struct game_state_t {
  // gracze
    std::vector<player_t> players;
    pointer_t pointer;

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

std::pair<position_t,game_enum >
process_input(std::shared_ptr<hardware_objects_t> hw,
              std::map<event_enum, std::function<void()>> event_handlers);

game_state_t
calculate_next_game_state(const game_state_t &previous_state,
                          std::pair<position_t,game_enum > intentions);

void draw_player(SDL_Renderer *renderer, player_t &player);
void draw_pointer(SDL_Renderer *renderer, pointer_t &pointer);

void draw_world(SDL_Renderer *renderer, game_state_t &state);

void setup_Bots(game_state_t* level);

std::list<position_t> position_mapper(std::list<node*> nodes);

node* node_mapper(position_t cords, game_state_t *gameState);