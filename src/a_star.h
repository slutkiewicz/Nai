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


    node(){
        id=counter();
    }

int id;    
bool AVAILABLE;    
double g,
       h,
       f;
       
std::vector<related_n> related_nodes;  //edge value
position_t position;

};
