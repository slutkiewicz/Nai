#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <map> 
#include <cmath>
#include "a_star.h"







////////////////////---------FUNCTIONS--------////////////////////////
using namespace std;


list<node*> A_Star(node* start,node* goal )
{

    map<node*,node*> cameFrom;

    map<node*,int> g_Map;
    map<node*,int> h_Map;
    map<node*,int> f_Map;
    list<node*> closedSet;
    list<node*> openSet={start};

    g_Map[start]=0;
    h_Map[start]=0;
    f_Map[start]=0;

    while(!openSet.empty())
    {
        auto current = openSet.back();

        for(auto node : openSet){
            if(f_Map[node] < f_Map[current])
                current = node;
        }
        openSet.remove(current);
        closedSet.push_back(current);


        if (current->id == goal->id){
            return reconstruct_path(start,current,cameFrom);

            //delete maps
        }


        for(auto related : current->related_nodes)
        {

            auto it = find(closedSet.begin(), closedSet.end(), related.first);

            if (it == closedSet.end())  // if not found
            {
                if (related.first->AVAILABLE()) {

                    if (related.first->id != goal->id) {

                        g_Map.insert(pair<node *, int>(related.first, g_Map.at(current) + related.first->CALCULATE_WAGE()));
                        h_Map.insert(pair<node *, int>(related.first, get_heur_value(goal, related.first)));

                        f_Map.insert(
                                pair<node *, int>(related.first, g_Map.at(related.first) + h_Map.at(related.first)));


                        auto it = find(openSet.begin(), openSet.end(), related.first);

                        if (it == openSet.end()) {
                            cameFrom.insert({related.first, current});

                            openSet.push_back(related.first);
                        } else
                            continue;

                    } else {
                        f_Map.insert({related.first, 0});

                        cameFrom.insert({related.first, current});
                        openSet.push_back(related.first);
                        break;
                    }

                }
                else
                {
                    closedSet.push_back(related.first);
                    continue;
                }
            }
            else
                continue;

        }

    }
    return list<node*>();
}

// last in list is start;
list<node*> reconstruct_path(node* start,node* goal,map<node*,node*> cameFrom){
    list<node*> reconstruct;
    auto current=goal;
    while(current!=start)
    {
        reconstruct.push_back(current);
        current=cameFrom[current];

    }
    return reconstruct;
}


// helping functions
// przekazac game sate i stamtad position
int get_heur_value(node *end_point,node *node)
{
    return  (int)abs(sqrt(pow(end_point->position.at(0)-node->position.at(0),2)
                          +pow(end_point->position.at(1)-node->position.at(1),2)));
}

int counter ()
{
    static int count=0;
    return count++;

}
