#pragma once 

#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <map> 
#include <cmath>
#include <tuple>


int counter ();

enum game_enum {PLAYER, NOTHING,WALL, GRASS , WATER , MARBLE , FIRE ,P1_GO, P2_GO , P3_GO  };


using position_t = std::array<int, 2>;

struct node {
using related_n = std:: pair<node*,int>;  //holding  reference + dist to it


    node(position_t position1){
        position=position1;
        id=counter();
    }
    int id;

    position_t position;

    std::map<game_enum,bool> map_events;

    std::vector<related_n> related_nodes;  //edge value

    bool AVAILABLE(){
        return !(map_events.at(WALL)
                 || map_events.at(PLAYER));
    }
    int CALCULATE_WAGE(){
        int wage = 0;
        if (map_events.at(GRASS))
            wage += 3;
        if (map_events.at(WATER))
            wage+=10;
        if (map_events.at(MARBLE))
            wage+=1;
        if (map_events.at(FIRE))
            wage+=100;

        return wage;
    }


};


std::list<node*> reconstruct_path(node* start,node* goal,std::map<node*,node*> cameFrom);

int get_heur_value(node *end_point,node *node);

std::list<node*> A_Star(node* start,node* goal );