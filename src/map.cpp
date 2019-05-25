
/**
 * This is the simple hello world for SDL2.
 * 
 * You need C++14 to compile this.
 */

#include <SDL2/SDL.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include "map.h"

#define errcheck(e)                                                            \
  {                                                                            \
    if (e)                                                                     \
      throw std::invalid_argument(SDL_GetError());                             \
  }


const int tile_size[] = {32, 32};



int main(int, char **) {
  using namespace std;
  using namespace std::chrono;

  auto hardware = init_hardware_subsystems(640, 480, false);
  auto level = load_level();    //game state
    cout<<level.world->t.at(0).size()<<endl;
  // auto dt = 15ms;
  milliseconds dt(15);

  steady_clock::time_point current_time = steady_clock::now();
  for (bool game_active = true; game_active;) {



    auto intentions = process_input(
        hardware,
        {{event_enum::QUIT, [&]() { game_active = false; }}});

    level = calculate_next_game_state(level, intentions,
                                      (double)(dt.count()) / 1000.0);

    draw_world(hardware->renderer, level);

    this_thread::sleep_until(current_time = current_time + dt);
  }

  return 0;
}



/**
 * @brief przygotowanie renderera - na tym bedziemy rysowali
 */
std::shared_ptr<hardware_objects_t> init_hardware_subsystems(int width, int height,
                                                        bool fscreen) {
    // inicjalizacja SDL tylko jesli jeszcze nie byl zainicjowany
    errcheck(SDL_Init(SDL_INIT_EVERYTHING) != 0);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    auto window = SDL_CreateWindow(
            "Textures example", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, width, height,
            SDL_WINDOW_SHOWN | (fscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

    errcheck(window == nullptr);

    auto renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    errcheck(renderer == nullptr);

    hardware_objects_t *objects = new hardware_objects_t;
    objects->renderer = renderer;
    objects->window = window;

    // opakowujemy wszystko w smartpointer dzieki czemu nie musimy pamietac o
    // zwalnianiu zasobow
    return std::shared_ptr<hardware_objects_t>(objects, [](auto o) {
        SDL_DestroyRenderer(o->renderer);
        SDL_DestroyWindow(o->window);
        SDL_Quit();
        delete o;
    });
}
/**
 * @brief Ładowanie planszy oraz przygotowanie obiektu gracza
 *
 */
game_state_t load_level() {
    game_state_t game_state;

    std::pair<int,int>  deltas[] = {{-1,0} ,{0,-1},{0,1},{1,0}};

    game_state.players.push_back({.position = {3, 3},
                                         .intention = {0, 0},
                                         .texture = nullptr});
    // ładowanie mapy
    game_state.world = std::make_shared<game_map_t>();
    std::ifstream t("data/level1.txt"); // załadujmy plik
    std::istringstream iss(std::string((std::istreambuf_iterator<char>(t)),
    std::istreambuf_iterator<char>()));
    game_state.world->t =
            std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    for (int y = 0; y < game_state.world->t.size(); y++) {
        for (int x = 0; x < game_state.world->t.at(0).size(); x++) {
            node map_node({x,y});
            switch (game_state.world->t[y][x]) {
                case '#':
                    map_node.AVAILABLE=false;
                    game_state.world->nodes_Map.insert(std::pair<position_t,node>({x,y},map_node));

                    break;
                case ',':
                    map_node.AVAILABLE=true;
                    game_state.world->nodes_Map.insert(std::pair<position_t,node>({x,y},map_node));
                    break;
                default:
                    break;
            }
        }
    }
    for(auto node : game_state.world->nodes_Map){

        for(auto p : deltas)
        {

            auto it = game_state.world->nodes_Map.find({node.first.at(0)+p.first,
                                                        node.first.at(1)+p.second});

            if (it != game_state.world->nodes_Map.end())
                node.second.related_nodes.
                        //insert(pair<node* ,int> (it->second,1));
                        emplace_back(&it->second,1);

        }

    }

    return game_state;
}

/**
 * @brief przetwarzanie wszystkich zdarzeń oraz pobieranie "intencji" ruchu
 * poszczególnych graczy
 */
std::map<int, player_intention_t>
process_input(std::shared_ptr<hardware_objects_t> hw,
              std::map<event_enum, std::function<void()>> event_handlers) {

    std::map<SDL_Scancode, std::function<std::pair<int, player_intention_t>()>>
            keyboard_mapping;

    std::map<int, player_intention_t> intentions;
    // pobranie zdarzen sdl
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // pętla sprawdzająca wszystkie zdarzenia
        event_enum evnt = NONE;
        switch (event.type) {
            case SDL_QUIT:
                evnt = QUIT;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    evnt = QUIT;
                break;
        }
        if (event_handlers.count(evnt))
            event_handlers[evnt]();
    }

    return intentions;
}


/**
 * @brief Obliczenia fizyczne i obsluga wszelakich zdarzen w swiecie gry
 */
game_state_t
calculate_next_game_state(const game_state_t &previous_state,
                          std::map<int, player_intention_t> intentions,
                          double dt) {
    game_state_t ret = previous_state;
    // przetwarzamy intencje (o ile jakieś są)
    for (auto &player : ret.players) {
        player.intention = {0, 0};
    }
    for (auto &intent : intentions) {
        if (intent.first < ret.players.size())
            ret.players[intent.first].intention = intent.second;
    }


    ret.frame++;
    return ret;
}


/**
 * @brief Rysowanie gracza na ekranie
 *
 */
void draw_player(SDL_Renderer *renderer, player_t &player) {
    SDL_Rect rect = {((player.position.at(0) ) * tile_size[0]),
                     ((player.position.at(1) ) * tile_size[1]),
                     tile_size[0], tile_size[1]};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer,&rect);
    //SDL_RenderCopy(renderer, player.texture.get(), NULL, &rect);
}


/**
 * @brief Wyświetlenie wszystkiego
 */
void draw_world(SDL_Renderer *renderer, game_state_t &state) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // na początek plansza
    for (unsigned y = 0; y < state.world->t.size(); y++) {
        for (unsigned x = 0; x < state.world->t.at(0).size(); x++) {
            switch (state.world->t[y][x]) {
                case '#':
                    SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 255);
                    break;
                case ',':
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                    break;
                default:
                    break;
            }
            SDL_Rect rect = {(int)((x)*tile_size[0]), (int)((y)*tile_size[1]),
                             tile_size[0], tile_size[1]};

    SDL_RenderFillRect(renderer,&rect);
            SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }

    // następnie gracze
    for (auto p : state.players)
        draw_player(renderer, p);

    SDL_RenderPresent(renderer); // wyswietlenie backbufora
}






















