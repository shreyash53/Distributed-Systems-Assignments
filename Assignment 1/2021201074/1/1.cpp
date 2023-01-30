#include<bits/stdc++.h>
#include<mpi/mpi.h>

#define endl "\n"
using namespace std;

using Complex = complex<double>;

int input[3];
int total_processes, process_rank, n=16, m=16, k=1000;

int chunk, offset, total;
double xdiff, ydiff;

double firstReal = -1.5, firstImag = 1;


bool isLastProcess(){
    return process_rank == total_processes-1;
}

bool isLastChunk(){
    return total%chunk;
}

class Mandlebrot{
    public:
        static bool mandle(Complex c, int k){
            Complex next = 0; 
            while(k-- and norm(next) <= 4){
                next = pow(next, 2) + c;
            }
            return norm(next) <= 4;
        }

        static void getSet(bool res[]){
            double x,y;
            int loop = 0;
            while(loop<chunk){
                double d1 = (offset%m)*xdiff;
                double d2 = (offset/m)*ydiff;
                x = firstReal + d1; 
                y = firstImag - d2; 
                res[loop] = Mandlebrot::mandle(Complex(x, y), k);
                offset++;
                loop++;
            }
        }
};

void solve(){
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    
    if(process_rank == 0){
        cin >> m >> n >> k;
        input[0] = m;
        input[1] = n;
        input[2] = k;
    }

    MPI_Bcast(input, 3, MPI_INT, 0, MPI_COMM_WORLD);
    m = input[0];
    n = input[1];
    k = input[2];

    total = n*m;
    chunk = ceil(total/(1.0 * total_processes));
    xdiff = (2.5)/(1.0 * (m-1));
    ydiff = (2)/(1.0 * (n-1));
    bool *grid = nullptr;

    if(process_rank == 0){
        grid = new bool[total];
    }
    int lastchunk = total%chunk;
    int maxoffset = chunk;
    if(isLastProcess() and isLastChunk())
        chunk = lastchunk;

    offset = process_rank*maxoffset;    
    bool res[chunk];
    
    Mandlebrot::getSet(res);

    // cout << "rank=" << process_rank << ", chunk=" << chunk << endl;
    MPI_Gather(res, chunk, MPI_CXX_BOOL, grid, chunk, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
    // cout << "rank=" << process_rank << " sent" << endl;

    if(process_rank == 0){
        for(int i=0; i<total; i++){
            cout << grid[i] << " ";
            if((i+1)%m == 0)
                cout << endl;
        }
        delete[] grid;
    }

}

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    
    solve();

    MPI_Finalize();

    return 0;
}