#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <map> 
#include <cmath>






using related_n = pair<node*,int>;  //holding node + dist to it
using position_t = array<int, 2>;
////////////////////---------FUNCTIONS--------////////////////////////



list<node*> A_Star(node start,node goal,vector<node> &nodes ){
    start.f=0;
map<node,node> cameFrom;    
list<node*> closedSet;
list<node*> openSet={&start};

while(!openSet.empty)
{

auto current =openSet.pop_back;
if (current.id == goal.id)
            return reconstruct_path(closedSet);


    for(auto &related : current.related_nodes)
    {
    auto it = find(closedSet.begin(), closedSet.end(), related);
    
        if (it == closedSet.end())
        {
            

            if(related.first.id!=goal.id){
                related.first.g=current.g+related.second;
                related.first.h=get_heur_value(goal,related.first);
                related.first.f=related.first.g+related.first.h;
                if(related.first.f < current.related_nodes[min_element(openSet.begin,openSet.end,[](node a,node b){return a.f<b.f;})].first.f )
                {    
                    if(related.first.f < closedSet[min_element(closedSet.begin,closedSet.end,[](node a,node b){return a.f<b.f;})].f  ){
                        closedSet.push_back(related);
                    }
                    else 
                       continue; 
                }
                else
                    continue;
                


            }else
                break;
        }
        else
            continue;   

    }
   
}













};

list<node*> reconstruct_path(list<node*> &closed){

return closed;
};


// helping functions

int get_heur_value(node &end_point,node &node){    
        return  (int)abs(sqrt(pow(end_point.position[0]-node.position[0],2)
                                +pow(end_point.position[1]-node.position[1],2)));  
    };
int counter (){
    static int count=0;
    return count++;

}
