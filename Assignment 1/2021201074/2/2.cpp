#include<bits/stdc++.h>
#include<mpi.h>

using namespace std;


struct pym{
    int x, y, dir;
    pym(int x_, int y_, int dir_): x(x_), y(y_), dir(dir_){}
    pym(const pym& another){
        x = another.x;
        y = another.y;
        dir = another.dir;
    }
};

typedef pair<int, int> pii;

const int DIR_UP = 'U';
const int DIR_LEFT = 'L';
const int DIR_DOWN = 'D';
const int DIR_RIGHT = 'R';

int n, m, k, t;

int psize, prank;

int *input = nullptr;

int total;
// int total_rows;
int rows_per_process;
int rows_this_process;
int row_start, row_end;

vector<pym> particles_current;
vector<pym> particles_out_of_bounds;

bool swapFlag = false;

bool isFirstProcess();
bool isLastProcess();
int* serialization(vector<pym>& vv);
void sharePoints();

class SimulationOperation{
    private:
        bool collisionExists(vector<int>& points);

        pym updatePosition(int point);

        void handleCollision(int point);

        void checkCollision();

        void updatePoints();

    public:
        void startSimulation();

        void displayPoints(int position[]);

};

void shareInput(){
    int tmp[4];
    if(isFirstProcess()){
        tmp[0] = n;
        tmp[1] = m;
        tmp[2] = k;
        tmp[3] = t;
    }
    MPI_Bcast(tmp, 4, MPI_INT, 0, MPI_COMM_WORLD);
    n = tmp[0];
    m = tmp[1];
    k = tmp[2];
    t = tmp[3];

    if(not isFirstProcess()){
        input = new int[3*k];
    }

    MPI_Bcast(input, 3*k, MPI_INT, 0, MPI_COMM_WORLD);

}

void initialize(){
    total = m;
    rows_per_process = ceil((double)m/psize);
    if(prank == psize - 1 and total%rows_per_process)
        rows_this_process = total%rows_per_process;
    else
        rows_this_process = rows_per_process;
    
    row_start = prank*rows_per_process;
    row_end = row_start + rows_this_process - 1;

}

bool valid_range(int x){
    return x >= row_start and x <= row_end;
}

void select_points(){
    for(int i=0; i<3*k; i+=3){
        if(valid_range(input[i+1]))
            particles_current.push_back(pym(input[i], input[i+1], input[i+2]));
    }
}


int tilt_direction(int dir){
    switch(dir){
        case DIR_DOWN:
            return DIR_LEFT;
        case DIR_LEFT:
            return DIR_DOWN;
        case DIR_RIGHT:
            return DIR_UP;
        case DIR_UP:
            return DIR_RIGHT;
    }
    return -1;
}

void swap_values(){
    if(swapFlag or n < m)
        return;
    swapFlag = true;
    swap(n, m);
    for(int i=0; i<3*k; i+=3){
        swap(input[i], input[i+1]);
        input[i+2] = tilt_direction(input[i+2]);
    }
}

void solve(){
    if(isFirstProcess()){
        cin >> n >> m >> k >> t;
        input = new int[3*k];
        for(int i=0; i<3*k; i+=3){
            int x, y;
            char c;
            cin >> x >> y >> c;
            input[i] = x;
            input[i+1] = y;
            input[i+2] = c;
        }

        swap_values();
    }
    

    shareInput();

    initialize();
    
    select_points();

    SimulationOperation obj;
    obj.startSimulation();

    MPI_Barrier(MPI_COMM_WORLD);

    int *values = nullptr;

    int particles_count = 3*particles_current.size();

    if(isFirstProcess())
        values = new int[psize];

    MPI_Gather(&particles_count, 1, MPI_INT, values, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int *counts = nullptr, *displacements = nullptr;
    if(isFirstProcess()){
        counts = new int[psize];
        displacements = new int[psize];

        for(int i=0; i<psize; i++){
            counts[i] = values[i];

            displacements[i] = ((i!=0)?(counts[i-1]+displacements[i-1]): 0);
        }
    }
    if(values)
        delete[] values;
    int* buff = serialization(particles_current);

    MPI_Gatherv(buff, particles_count, MPI_INT, input, counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
    if(counts)
        delete[] counts;
    if(displacements)
        delete[] displacements;

    if(buff)
        delete[] buff;

    if(isFirstProcess()){
        if(swapFlag)
            swap_values();
        //displaying output
        obj.displayPoints(input);
    }
    if(input)
        delete[] input;
}

void system(){
    freopen("../../input.txt","r",stdin);
	// freopen("output.txt","w",stdout);
}

int main(int argc, char** argv){
    
    // system();
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);
    solve();
    MPI_Finalize();
    return 0;
}

void SimulationOperation::displayPoints(int input[]){

    multimap<pii, char> val;
    for(int i=0; i<3*k; i+=3){
        val.insert({{input[i], input[i+1]}, (char)input[i+2]});
    }
    for(auto& v: val)
        cout << v.first.first << " " << v.first.second << " " << v.second << endl;
}

void SimulationOperation::startSimulation(){
    
    while(t--){
        //checking for collision
        checkCollision();

        //updating position of all points
        updatePoints();
        
        //sharing the updated points with other process
        sharePoints();
    }

}

int* serialization(vector<pym>& vv){
    int sz = vv.size();
    int *ptr = new int[3*sz];
    for(int i=0; i<sz; i++){
        ptr[3*i] = vv[i].x;
        ptr[3*i+1] = vv[i].y;
        ptr[3*i+2] = vv[i].dir;
    }
    return ptr;
}

vector<pym> deserialization(int *ptr, int sz){
    vector<pym> vv;
    for(int i=0; i<sz; i+=3){
        vv.push_back(pym(ptr[i], ptr[i+1], ptr[i+2]));
    }
    return vv;
}

bool isFirstProcess(){
    return prank == 0;
}

bool isLastProcess(){
    return prank == psize - 1;
}

void segregate_particles(vector<pym>& above, vector<pym>& below){
    for(auto& particle: particles_out_of_bounds){
        if(particle.y < row_start)
            above.push_back(particle);
        else if(particle.y > row_end)
            below.push_back(particle);
        else   //not possible
            cout << "*****ERROR STATEMENT****** " << "\n" << "while segregate particles, found one inlier" << endl;
    }
}

bool invalid_comm(int dir){
    return ((dir == 1 and  isLastProcess()) or (dir == -1 and isFirstProcess()));
}

void send_message(vector<pym>& vv, int dir){
    if(invalid_comm(dir))
        return;

    bool flag = not vv.empty();
    
    MPI_Send(&flag, 1, MPI_CXX_BOOL, prank+dir, 0, MPI_COMM_WORLD);

    if(not flag)
        return;
    // cout << "process " << prank << " iteration " << t << " - " << "full send " << vv.size()*3<< endl;
    int *buff = serialization(vv);
    
    MPI_Send(buff, vv.size()*3, MPI_INT, prank + dir, 0, MPI_COMM_WORLD);
    if(buff)
    delete[] buff;
}

vector<pym> recieve_msg(int dir){
    if(invalid_comm(dir))
        return {};
    
    bool flag;
    MPI_Recv(&flag, 1, MPI_CXX_BOOL, prank+dir, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


    if(not flag)
        return {};

    MPI_Status status;

    MPI_Probe(prank + dir, 0, MPI_COMM_WORLD, &status);

    // When probe returns, the status object has the size and other
    // attributes of the incoming message. Get the message size
    int amnt;
    MPI_Get_count(&status, MPI_INT, &amnt);

    int *buff = new int[amnt];
    
    MPI_Recv(buff, amnt, MPI_INT, prank+dir, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    auto vv = deserialization(buff, amnt);
    if(buff)
    delete[] buff;
    return vv;
}

void sharePoints(){
    vector<pym> above, below;
    segregate_particles(above, below);
    particles_out_of_bounds.clear();
    if((isFirstProcess() and not above.empty()) or (isLastProcess() and not below.empty()))     //not possible
        cout << "*****ERROR STATEMENT****** " << "\n" << "while share points, found points wrongly segregated, in process rank " << prank << endl;

    //sending msg in one direction - right
    send_message(below, 1);

    below.clear();      // clear vector below

    //recieving msg from opposite side - left
    auto rec_left = recieve_msg(-1);

    if(not rec_left.empty())
        particles_current.insert(particles_current.end(), rec_left.begin(), rec_left.end());
    rec_left.clear();

    //sending msg in other direction - now left
    send_message(above, -1);

    above.clear();      //clear vector above

    //recieving msg from opposite direction - right
    auto rec_right = recieve_msg(1);
    
    if(not rec_right.empty())
        particles_current.insert(particles_current.end(), rec_right.begin(), rec_right.end());
    rec_right.clear();

}

void SimulationOperation::updatePoints(){
    //moving all points one unit in their direction
    vector<pym> updatedPoints;
    for(int i=0; i<particles_current.size(); i++){
        pym get = updatePosition(i);
        if(get.x != -1)
            updatedPoints.push_back(get);
    }
    particles_current.clear();
    particles_current = updatedPoints;
    updatedPoints.clear();
}

pym SimulationOperation::updatePosition(int point){
    switch(particles_current[point].dir){
        case DIR_UP:
        {
            if(particles_current[point].y == m-1){
                //Reflection
                particles_current[point].y--;
                particles_current[point].dir = DIR_DOWN;
            }
            else
                particles_current[point].y++;

            if(not valid_range(particles_current[point].y)){
                particles_out_of_bounds.push_back(particles_current[point]);
                return pym(-1,-1,-1);
            }
            break;
        }
        case DIR_DOWN:
        {
            if(particles_current[point].y == 0){
                //Reflection
                particles_current[point].y++;
                particles_current[point].dir = DIR_UP;
            }
            else{
                particles_current[point].y--;
            }
            if(not valid_range(particles_current[point].y)){
                particles_out_of_bounds.push_back(particles_current[point]);
                return pym(-1,-1,-1);
            }
            break;
        }
        case DIR_LEFT:
        {
            if(particles_current[point].x == 0){
                //Reflection
                particles_current[point].x++;
                particles_current[point].dir = DIR_RIGHT;
            }
            else{
                particles_current[point].x--;
            }
            break;
        }
        case DIR_RIGHT:
        {
            if(particles_current[point].x == n-1){
                //Reflection
                particles_current[point].x--;
                particles_current[point].dir = DIR_LEFT;
            }
            else{
                particles_current[point].x++;
            }
            break;
        }
    }
    return particles_current[point];
}


void SimulationOperation::handleCollision(int point){
    switch(particles_current[point].dir){
        case DIR_UP:
        {
            particles_current[point].dir = DIR_RIGHT;
            break;
        }
        case DIR_DOWN:
        {
            particles_current[point].dir = DIR_LEFT;
            break;
        }
        case DIR_LEFT:
        {
            particles_current[point].dir = DIR_UP;
            break;
        }
        case DIR_RIGHT:
        {
            particles_current[point].dir = DIR_DOWN;
            break;
        }
    }
}

void SimulationOperation::checkCollision(){
    map<pii, vector<int>> gridStruc;
    // for(int i=0; i<3*k; i+=3){
    for(int i=0; i<particles_current.size(); i++){
        gridStruc[make_pair(particles_current[i].x, particles_current[i].y)].push_back(i);
    }
    for(auto& cell: gridStruc){
        auto& points = cell.second;
        if(collisionExists(points)){
            for(auto point: points)
                handleCollision(point);
        }
    }
}

bool SimulationOperation::collisionExists(vector<int>& points){
    // No collision if
    if(points.size() != 2  // points are more than two 
    // (or) if both are on border 
    or (particles_current[points[0]].x == 0 and particles_current[points[1]].x == 0)            
    or (particles_current[points[0]].y == 0 and particles_current[points[1]].y == 0)
    or (particles_current[points[0]].x == n-1 and particles_current[points[1]].x == n-1)
    or (particles_current[points[0]].y == m-1 and particles_current[points[1]].y == m-1)
    )
        return false;
    
    int one = points[0];
    int two = points[1];

    // exists if dir of two particles is 
    return (particles_current[one].dir == DIR_DOWN and particles_current[two].dir == DIR_UP) // one is down and two is up
        or (particles_current[one].dir == DIR_UP and particles_current[two].dir == DIR_DOWN) // (or) one is up and two is down
        or (particles_current[one].dir == DIR_LEFT and particles_current[two].dir == DIR_RIGHT) // (or) one is left and two is right
        or (particles_current[one].dir == DIR_RIGHT and particles_current[two].dir == DIR_LEFT) // (or) one is right and two is left
        ;
}


