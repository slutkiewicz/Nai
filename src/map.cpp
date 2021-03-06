
/**
 * This is the simple astar world with SDL2.
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
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "map.h"

#define errcheck(e)                                                            \
  {                                                                            \
    if (e)                                                                     \
      throw std::invalid_argument(SDL_GetError());                             \
  }
  //const int tile_size[] = {32, 32};
const int tile_size[] = {16, 16};


int main(int, char **) {
    using namespace std;
    using namespace std::chrono;
    srand(time(NULL));
    const int FPS = 15;
    const int frameDelay= 1000/FPS;

    Uint32  frameStart;
    int frameTime;

    auto hardware = init_hardware_subsystems(640, 480, false);
    auto level = load_level();    //game state


    for (bool game_active = true; game_active;)
    {
        frameStart=SDL_GetTicks();

        setup_Bots(&level);

        auto intentions = process_input(
                hardware,
                {{event_enum::QUIT, [&]() { game_active = false; }}});

        level = calculate_next_game_state(level, intentions);

        draw_world(hardware->renderer, level);
        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameTime)
            SDL_Delay(frameDelay-frameTime);
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

    game_state_t game_state = game_state_t();

    ////Posible movements
    std::pair<int, int> deltas[] = {{-1, 0},
                                    {0,  -1},
                                    {0,  1},
                                    {1,  0}};

    ////make players
    player_t player;
    player.position={3,3};
    player.intention={};
    player.texture= nullptr;
    game_state.players.push_back(player);
    player.position={2,13};
    player.intention={};
    player.texture= nullptr;
    game_state.players.push_back(player);
    player.position={30,14};
    player.intention={};
    player.texture= nullptr;
    game_state.players.push_back(player);

    ////make pointer

    game_state.pointer.position.first={1, 1};
    game_state.pointer.position.second={NOTHING};
    game_state.pointer.texture= {nullptr} ;

    ////load a map
    game_state.world = std::make_shared<game_map_t>();
    std::ifstream t("data/level1.txt"); // załadujmy plik
    std::istringstream iss(std::string((std::istreambuf_iterator<char>(t)),
    std::istreambuf_iterator<char>()));
    game_state.world->t =
            std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    ////make map nodes
    for (int y = 0; y < game_state.world->t.size(); y++) {
        for (int x = 0; x < game_state.world->t.at(0).size(); x++) {
            node map_node({x, y});

            map_node.map_events.insert({PLAYER,false});
            map_node.map_events.insert({WALL,false});
            map_node.map_events.insert({GRASS,false});
            map_node.map_events.insert({WATER,false});
            map_node.map_events.insert({MARBLE,false});
            map_node.map_events.insert({FIRE,false});
            map_node.map_events.insert({P1_GO,false});
            map_node.map_events.insert({P2_GO,false});
            map_node.map_events.insert({P3_GO,false});

            switch (game_state.world->t[y][x]) {
                case '#':
                    map_node.map_events.at(WALL)=true;
                    game_state.world->nodes_Map.insert(std::pair<position_t, node>({x, y}, map_node));
                    break;
                case ',':
                    map_node.map_events.at(MARBLE)=true;
                    game_state.world->nodes_Map.insert(std::pair<position_t, node>({x, y}, map_node));
                    break;
                case '.':
                    map_node.map_events.at(GRASS)=true;
                    game_state.world->nodes_Map.insert(std::pair<position_t, node>({x, y}, map_node));
                    break;
                case ';':
                    map_node.map_events.at(WATER)=true;
                    game_state.world->nodes_Map.insert(std::pair<position_t, node>({x, y}, map_node));
                    break;
                case ':':
                    map_node.map_events.at(FIRE)=true;
                    game_state.world->nodes_Map.insert(std::pair<position_t, node>({x, y}, map_node));
                    break;
                default:
                    break;
            }
        }
    }
    ////make map relations
    for (auto &node : game_state.world->nodes_Map) {

        for (auto p : deltas) {
            auto it = game_state.world->nodes_Map.find({(node.first.at(0) + p.first),
                                                        (node.first.at(1) + p.second)});

            if (it != game_state.world->nodes_Map.end()) {
                int wage = 0;
                if (it->second.map_events.at(GRASS))
                    wage += 3;
                if (it->second.map_events.at(WATER))
                    wage+=10;
                if (it->second.map_events.at(MARBLE))
                    wage+=1;
                if (it->second.map_events.at(FIRE))
                    wage+=100;


                node.second.related_nodes.emplace_back(&it->second, wage);
            }
        }
    }

    return game_state;
}

/**
 * @brief przetwarzanie wszystkich zdarzeń oraz pobieranie "intencji" ruchu
 * poszczególnych graczy
 */
std::pair<position_t,game_enum >
process_input(std::shared_ptr<hardware_objects_t> hw,
              std::map<event_enum, std::function<void()>> event_handlers) {

    std::map<SDL_Scancode, std::function<std::pair<int, position_t>()>>
            keyboard_mapping;

    std::pair<position_t,game_enum > intentions;
    intentions.second=NOTHING;
    // pobranie zdarzen sdl
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // pętla sprawdzająca wszystkie zdarzenia
        event_enum evnt = NONE;
        switch (event.type) {
            case SDL_QUIT:
                evnt = QUIT;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        evnt = QUIT;
                        break;
                    case SDLK_LEFT:
                        intentions.first[0] -= 1;
                        break;
                    case SDLK_RIGHT:
                        intentions.first[0] += 1;
                        break;
                    case SDLK_UP:
                        intentions.first[1] -= 1;
                        break;
                    case SDLK_DOWN:
                        intentions.first[1] += 1;
                        break;
                    case SDLK_SPACE:
                        intentions.second = P1_GO;
                        break;
                    case SDLK_w:
                        intentions.second = WALL;
                        break;
                    case SDLK_m:
                        intentions.second = MARBLE;
                        break;
                    case SDLK_g:
                        intentions.second = GRASS;
                        break;
                    case SDLK_f:
                        intentions.second = FIRE;
                        break;
                    case SDLK_e:
                        intentions.second = WATER;
                        break;
                }break;

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
                          std::pair<position_t,game_enum > intentions) {
    game_state_t ret = previous_state;

///move pointer with map borders checking
    intentions.first.at(0) += ret.pointer.position.first.at(0);
    intentions.first.at(1) += ret.pointer.position.first.at(1);

    if (intentions.first.at(0) > (ret.world->t.at(0).size()-1) ||
        intentions.first.at(1) > (ret.world->t.size()-1) ||
        intentions.first.at(0) < 0 ||
        intentions.first.at(1) < 0) {}
    else {
        ret.pointer.position.first = intentions.first;
    }
///make user pointer interaction


    if(intentions.second!=NOTHING)
    {
        if(intentions.second==P1_GO)
        {   ///ASTAR ON PLAYER 1
            if(ret.world->nodes_Map.at(ret.pointer.position.first).AVAILABLE())
            {
                ret.players.front().intention.clear();
                ret.players.front().intention = position_mapper(A_Star(node_mapper(
                        ret.players.at(0).position, &ret
                ), node_mapper(ret.pointer.position.first, &ret)));
            }
        }
        else       ///FIELD ENUM MODIFICATION
            ret.world->nodes_Map.at(intentions.first).map_events.at(intentions.second)
                    = !ret.world->nodes_Map.at(intentions.first).map_events.at(intentions.second);


    }

    //// get intentions from intention lists
    for (auto &player : ret.players)
    {
        if (!player.intention.empty())
        {



            ///check next field availability !!!!!!!!!!!!!!!!
            if (ret.world->nodes_Map.at(player.intention.back()).AVAILABLE())
            {

                ret.world->nodes_Map.at(player.position).map_events.at(PLAYER) = false;
                player.position = player.intention.back();
                ret.world->nodes_Map.at(player.position).map_events.at(PLAYER) = true;
                player.intention.pop_back();
            } else
                {///move somwhere else

                    auto goal = player.intention.front();
                    node* n = nullptr;

                    if(ret.world->nodes_Map.at({player.position.at(0)+1,player.position.at(1)}).AVAILABLE())
                        n=&ret.world->nodes_Map.at({player.position.at(0)+1,player.position.at(1)});
                    else
                        {
                            for (auto &node: ret.world->nodes_Map.at(player.position).related_nodes)
                            {
                                if (node.first->AVAILABLE())
                                {
                                    srand(time(nullptr));
                                    n=node.first;
                                    if((rand() % 1 + 0)==1)
                                        break;
                                    //break;
                                }
                            }
                        }
                    if(n== nullptr)
                        player.intention.push_back(player.position);

                    else
                        {
                        auto start = n;

                        ret.world->nodes_Map.at(player.position).map_events.at(PLAYER) = false;
                        player.position = n->position;
                        ret.world->nodes_Map.at(player.position).map_events.at(PLAYER) = true;

                        auto new_list=position_mapper(A_Star(start, node_mapper(goal, &ret)));
                        if(!new_list.empty())   ///if cant go wait a sec
                        {
                            player.intention.clear();
                            player.intention=new_list;
                        }
                        else
                            player.intention.push_back(n->position);

                        }



                }
        }

    }

    ret.frame++;
    return ret;
}


/**
 * @brief Rysowanie gracza na ekranie
 *
 */
void draw_player(SDL_Renderer *renderer, player_t &player) {
    SDL_Rect rect = {((player.position.at(0)) * tile_size[0]),
                     ((player.position.at(1)) * tile_size[1]),
                     tile_size[0], tile_size[1]};

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    //SDL_RenderCopy(renderer, player.texture.get(), NULL, &rect);
}
void draw_pointer(SDL_Renderer *renderer, pointer_t &pointer) {
    SDL_Rect rect = {((pointer.position.first.at(0)) * tile_size[0]),
                     ((pointer.position.first.at(1)) * tile_size[1]),
                     tile_size[0], tile_size[1]};

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
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

            if (state.world->nodes_Map.at({(int)x,(int)y}).map_events.at(WALL))
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            else if (state.world->nodes_Map.at({(int)x,(int)y}).map_events.at(MARBLE))
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else if (state.world->nodes_Map.at({(int)x,(int)y}).map_events.at(WATER))
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            else if (state.world->nodes_Map.at({(int)x,(int)y}).map_events.at(GRASS))
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else if (state.world->nodes_Map.at({(int)x,(int)y}).map_events.at(FIRE))
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_Rect rect = {(int) ((x) * tile_size[0]), (int) ((y) * tile_size[1]),
                             tile_size[0], tile_size[1]};

            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }

    // następnie gracze
    for (auto p : state.players) {
        draw_player(renderer, p);
    }

    draw_pointer(renderer,state.pointer);
    SDL_RenderPresent(renderer); // wyswietlenie backbufora
}


node* node_mapper(position_t cords, game_state_t* gameState) {
    return &gameState->world->nodes_Map.at(cords);

}

std::list<position_t> position_mapper(std::list<node *> nodes) {
    std::list<position_t> ret;

    for (auto node : nodes) {
        ret.push_back(node->position);
    }
    return ret;
}


void setup_Bots(game_state_t* level){

    position_t start_bot_1={2,13};
    position_t goal_bot_1={30,13};

    position_t start_bot_2={30,14};
    position_t goal_bot_2={2,14};

    if(level->players.at(1).intention.empty())
    {
        level->players.at(1).intention = position_mapper(
                level->players.at(1).position == start_bot_1 ?
                A_Star(node_mapper(level->players.at(1).position, level),node_mapper(goal_bot_1, level))
                                                             : A_Star(node_mapper(level->players.at(1).position, level), node_mapper(start_bot_1, level)));

    }
    if(level->players.at(2).intention.empty())
    {
        level->players.at(2).intention = position_mapper(
                level->players.at(2).position == start_bot_2 ?
                A_Star(node_mapper( level->players.at(2).position, level),node_mapper(goal_bot_2, level))
                                                             : A_Star(node_mapper( level->players.at(2).position, level), node_mapper(start_bot_2, level)));
    }
}





















