#include<bits/stdc++.h>
// #include<mpi.h>

using namespace std;


// struct pym{
//     int x, y, dir;
//     pym(int x_, int y_, int dir_): x(x_), y(y_), dir(dir_){}
// };

typedef pair<int, int> pii;

const int DIR_UP = 'U';
const int DIR_LEFT = 'L';
const int DIR_DOWN = 'D';
const int DIR_RIGHT = 'R';

int n, m, k, t;

class SimulationOperation{
    private:
        bool collisionExists(int position[], vector<int>& points);

        void updatePosition(int position[], int point);

        void handleCollision(int position[], int point);

        void checkCollision(int position[]);

        void updatePoints(int position[]);
    public:
        void startSimulation(int position[]);

        void displayPoints(int position[]);

};


void solve(){
    cin >> n >> m >> k >> t;
    int input[3*k];
    for(int i=0; i<3*k; i+=3){
        int x, y, c;
        cin >> x >> y >> c;
        input[i] = x;
        input[i+1] = y;
        input[i+2] = c;
    }
    
    SimulationOperation obj;
    obj.startSimulation(input);

    //displaying output
    obj.displayPoints(input);

}

int main(int argc, char** argv){
    solve();
    return 0;
}

void SimulationOperation::displayPoints(int position[]){
    for(int i=0; i<3*k; i+=3){
        for(int j=0; j<3; j++)
            cout << position[j] << " ";
        cout << endl;
    }
}

void SimulationOperation::startSimulation(int position[]){
    
    while(t--){
        //updating position of all points
        updatePoints(position);
        
        //sharing the updated points with other process

        //checking for collision
        checkCollision(position);
    }
}

void SimulationOperation::updatePoints(int position[]){
    //moving all points one unit in their direction
    for(int i=0; i<3*k; i+=3){
        updatePosition(position, i);
    }
}

void SimulationOperation::updatePosition(int position[], int point){
    switch(position[point + 2]){
        case DIR_UP:
        {
            if(position[point+1] == m-1){
                //Reflection
                position[point+1]--;
                position[point+2] = DIR_DOWN;
            }
            else
                position[point+1]++;
            break;
        }
        case DIR_DOWN:
        {
            if(position[point+1] == 0){
                //Reflection
                position[point+1]++;
                position[point+2] = DIR_UP;
            }
            else{
                position[point+1]--;
            }
            break;
        }
        case DIR_LEFT:
        {
            if(position[point] == 0){
                //Reflection
                position[point]++;
                position[point+2] = DIR_RIGHT;
            }
            else{
                position[point]--;
            }
            break;
        }
        case DIR_RIGHT:
        {
            if(position[point] == n-1){
                //Reflection
                position[point]--;
                position[point+2] = DIR_LEFT;
            }
            else{
                position[point]++;
            }
            break;
        }
    }
}


void SimulationOperation::handleCollision(int position[], int point){
    switch(position[point + 2]){
        case DIR_UP:
        {
            position[point+1]++;
            // position[point]++;
            position[point+2] = DIR_RIGHT;
            break;
        }
        case DIR_DOWN:
        {
            position[point+1]--;
            // position[point]--;
            position[point+2] = DIR_LEFT;
            break;
        }
        case DIR_LEFT:
        {
            position[point]--;
            // position[point+1]++;
            position[point+2] = DIR_UP;
            break;
        }
        case DIR_RIGHT:
        {
            position[point]++;
            // position[point+1]--;
            position[point+2] = DIR_DOWN;
            break;
        }
    }
}

void SimulationOperation::checkCollision(int position[]){
    map<pii, vector<int>> gridStruc;
    for(int i=0; i<3*k; i+=3){
        gridStruc[make_pair(position[i], position[i+1])].push_back(i);
    }
    for(auto& cell: gridStruc){
        auto& points = cell.second;
        if(collisionExists(position, points)){
            for(auto point: points)
                handleCollision(position, point);
        }
    }
}

bool SimulationOperation::collisionExists(int position[], vector<int>& points){
    // No collision if
    if(points.size() != 2  // points are more than two 
    // (or) if both are on border 
    or (position[points[0]] == 0 and position[points[1]] == 0)            
    or (position[points[0]+1] == 0 and position[points[1]+1] == 0)
    or (position[points[0]] == n-1 and position[points[1]] == n-1)
    or (position[points[0]+1] == m-1 and position[points[1]+1] == m-1)
    )
        return false;
    
    int one = points[0] + 2;
    int two = points[1] + 2;

    // exists if dir of two particles is 
    return (position[one] == DIR_DOWN and position[two] == DIR_UP) // one is down and two is up
        or (position[one] == DIR_UP and position[two] == DIR_DOWN) // (or) one is up and two is down
        or (position[one] == DIR_LEFT and position[two] == DIR_RIGHT) // (or) one is left and two is right
        or (position[one] == DIR_RIGHT and position[two] == DIR_LEFT) // (or) one is right and two is left
        ;
}


