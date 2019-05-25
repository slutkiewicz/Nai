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



using position_t = std::array<int, 2>;

struct node {
using related_n = std:: pair<node*,int>;  //holding node + dist to it


    node(position_t position1){
        position=position1;
        id=counter();
    }
    int id;
    bool AVAILABLE;
    position_t position;


std::vector<related_n> related_nodes;  //edge value


};


std::list<node*> reconstruct_path(node* start,node* goal,std::map<node*,node*> cameFrom);
int get_heur_value(node *end_point,node *node);

std::list<node*> A_Star(node* start,node* goal );