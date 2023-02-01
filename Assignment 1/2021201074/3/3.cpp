#include<bits/stdc++.h>
#include<mpi.h>

using namespace std;

typedef pair<int, int> pii;
long long sumc[501] = {};
long long cost[501][501] = {};
int root[501][501] = {};
int node_parent[501] = {};
pii trees[501];
int* input_buffer = nullptr;


int n;

int prank, psize;

int rows_per_process, rows_this_process;
int row_st, row_ed;

unordered_map<int, unordered_map<int, vector<long long>>> values_here;

void system_input(){
    freopen("input.txt","r",stdin);
	// freopen("output.txt","w",stdout);
}

void solve();
void optimalBST();

void sorting();

int main(int argc, char** argv){
    system_input();
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &psize);
    MPI_Comm_rank(MPI_COMM_WORLD, &prank);
    solve();
    MPI_Finalize();
    return 0;
}

bool isFirstProcess(){
    return prank == 0;
}

bool isLastProcess(){
    return prank == psize - 1;
}

void fillParent(int i, int j, int rt = 0){
    if(i>j)
        return;
    if(i == j){
        node_parent[i] = rt;
        return;
    }
    int k = root[i][j];
    int val = trees[k].first;
    fillParent(i, k-1, val);
    node_parent[k] = rt;
    fillParent(k+1, j, val);
}

// unordered_map<int, pii> ump;

void shareInput(){
    if(isFirstProcess()){
        cin >> n;
        for(int i=0; i<n; i++){
            int u, v;
            cin >> u >> v;
            trees[i].first = u;
            trees[i].second = v;
        }
        sorting();
        input_buffer = new int[2*n];
        for(int i=0; i<n; i++){
            input_buffer[2*i] = trees[i].first; 
            input_buffer[2*i+1] = trees[i].second;
        }
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(not isFirstProcess())
        input_buffer = new int[2*n];
    
    MPI_Bcast(input_buffer, 2*n, MPI_INT, 0, MPI_COMM_WORLD);

}

void initialize(){
    rows_per_process = ceil((double)n/psize);
    if(isLastProcess() and n%rows_per_process)
        rows_this_process = n%rows_per_process;
    else
        rows_this_process = rows_per_process;
    
    row_st = prank*rows_per_process;
    row_ed = row_st + rows_this_process;
}

void solve(){

    shareInput();

    initialize();

    // cout << "Rank: " << prank << ", rows_per_process: " << rows_per_process << ", rows_this_process: " << rows_this_process << ", row_st: " << row_st << ", row_ed: " << row_ed << endl;
    optimalBST();

    MPI_Barrier(MPI_COMM_WORLD);

    // cout << "Rank: " << prank << ", done calculation" << endl;
    
    if(isFirstProcess()){
        cout << cost[0][n-1] << endl;
        // for(int i=0; i<n; i++){
        //     for(int j=0; j<n; j++)
        //         cout << cost[i][j] << " ";
        //     cout << endl;
        // }
        // cout << endl;
        // for(int i=0; i<n; i++){
        //     for(int j=0; j<n; j++)
        //         cout << root[i][j] << " ";
        //     cout << endl;
        // }
        fillParent(0, n-1);

        for(int i=0; i<n; i++)
            cout << node_parent[i] << " ";
    }

    if(input_buffer)
        delete[] input_buffer;
}

void sorting(){

    sort(trees, trees+n);

}

long long sum(int freq[], int i, int j)
{
    return sumc[j] - sumc[i-1];
}

long long* serialization(){
    int sz = values_here.size();
    long long *ptr = new long long[4*sz];
    int i=0;
    for(auto& values: values_here){
        auto x = values.first;
        for(auto& val: values.second){
            auto y = val.first;
            auto& v = val.second;
            ptr[i] = x;
            ptr[i+1] = y;
            ptr[i+2] = v[0];
            ptr[i+3] = v[1];
        }
        i+=4;
    }
    return ptr;
}

void deserialization(long long *ptr, int sz){
    for(int i=0; i<sz; i+=4){
        int x = ptr[i];
        int y = ptr[i+1];
        long long c = ptr[i+2];
        int r = ptr[i+3];
        cost[x][y] = c;
        root[x][y] = r;
        values_here[x][y] = {c, r};
    }
}

bool invalid_comm(int dir){
    return ((dir == 1 and  isLastProcess()) or (dir == -1 and isFirstProcess()));
}

void send_message(int dir){
    if(invalid_comm(dir))
        return;

    // bool flag = not vv.empty();
    int amnt = 4*values_here.size();

    MPI_Send(&amnt, 1, MPI_INT, prank+dir, 0, MPI_COMM_WORLD);

    if(not amnt)
        return;
    // cout << "process " << prank << " - " << "full send " << amnt<< endl;
    auto *buff = serialization();
    
    MPI_Send(buff, amnt, MPI_LONG_LONG, prank + dir, 0, MPI_COMM_WORLD);
    if(buff)
        delete[] buff;
}

void recieve_msg(int dir){
    if(invalid_comm(dir))
        return;
    
    int amnt;
    MPI_Recv(&amnt, 1, MPI_INT, prank+dir, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // cout << "process " << prank << " - " << "half recv " << amnt << endl;

    if(not amnt)
        return;

    // cout << "process " << prank << " iteration " << t << " - " << "full recv " << amnt << endl;

    long long *buff = new long long[amnt];
    
    MPI_Recv(buff, amnt, MPI_LONG_LONG, prank+dir, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    deserialization(buff, amnt);
    if(buff)
        delete[] buff;
}



void share_values(){

    //SEND msg to lower rank process;
    send_message(-1);

    values_here.clear();
    // cout << "Rank: " << prank << ", sent msg " << endl;

    //Recieve msg from higher rank process
    recieve_msg(1);
    // cout << "Rank: " << prank << ", recv msg " << endl;

}


void optimalSearchTree(int keys[], int freq[])
{
    
    for (int i = 0; i < n; i++)
        cost[i][i] = freq[i];
        
    for(int i=1; i<=n; i++)
        sumc[i] = sumc[i-1] + freq[i-1];
 
    for (int L = 2; L <= n; L++)
    {
        for (int i = row_st; i <= n-L+1; i++)
        {
            int j = i+L-1;
            cost[i][j] = LONG_MAX;
            auto off_set_sum = sum(freq, i+1, j+1);
 
            for (int r = i; r <= j; r++)
            {
                long long c = off_set_sum;
                if(r > i)
                    c += cost[i][r-1];
                if(r < j)
                    c += cost[r+1][j];
                    
                if (c < cost[i][j]){
                    cost[i][j] = c;
                    root[i][j] = r;
                    values_here[i][j] = {c, r};
                }
            }
        }
        share_values();
    }
    // return cost[0][n-1];
}

void optimalBST(){
    int keys[n], freq[n];
    for(int i=0; i<n; i++){
        keys[i] = input_buffer[2*i];
        freq[i] = input_buffer[2*i+1];
    }

    optimalSearchTree(keys, freq);
}
