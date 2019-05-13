#include <iostream>
#include <vector>
#include <cmath>
using namespace std;


class node{
int x, y, heur_value=0,s_n_dist=-1, id;
vector<related_n> related;

public:
    int get_heur_value(node end_point){
        if( heur_value==0)
            return  heur_value=(int)abs(sqrt(pow(end_point.x-x,2)
                                    +pow(end_point.y-y,2)));
        else
            return  heur_value;
            
    };
};

using related_n = pair<vector<node*>,int>;  //holding node + dist to it

vector<node> gen_Graph(int Graph_size);

int main()
{


return 0;
}

vector <node> gen_Graph(int Graph_size){


}