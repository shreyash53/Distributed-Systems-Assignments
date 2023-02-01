#include<bits/stdc++.h>

using namespace std;

typedef pair<int, int> pii;
int sumc[1001] = {};
int root[1001][1001] = {};
int node_parent[1001] = {};
pii trees[1001];

int n;


void system_input(){
    freopen("../../input.txt","r",stdin);
	freopen("../../output.txt","w",stdout);
}

void solve();
int optimalBST(pii trees[]);

int main(int argc, char** argv){
    system_input();
    
    solve();

    return 0;
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

unordered_map<int, pii> ump;

void solve(){
    cin >> n;
    for(int i=0; i<n; i++){
        int u, v;
        cin >> u >> v;
        trees[i].first = u;
        trees[i].second = v;
    }
    sort(trees, trees+n);

    // for(int i=0; i<n; i++)
    //     ump[i] = {trees[i].}

    cout << optimalBST(trees) << endl;
    
    fillParent(0, n-1);
    // for(int i=0; i<n; i++){
    //     for(int j=0; j<n; j++)
    //         cout << root[i][j] << " ";
    //         // cout << trees[node_parent[i]].first << " ";
    //     cout << endl;
    // }
    for(int i=0; i<n; i++)
        cout << node_parent[i] << " ";
    
}

int sum(int freq[], int i, int j)
{
    return sumc[j] - sumc[i-1];
}


int optimalSearchTree(int keys[], int freq[])
{
    int cost[n][n];
 
    for (int i = 0; i < n; i++)
        cost[i][i] = freq[i];
        
    for(int i=1; i<=n; i++)
        sumc[i] = sumc[i-1] + freq[i-1];
 
    for (int L = 2; L <= n; L++)
    {
        for (int i = 0; i <= n-L+1; i++)
        {
            int j = i+L-1;
            cost[i][j] = INT_MAX;
            int off_set_sum = sum(freq, i+1, j+1);
 
            for (int r = i; r <= j; r++)
            {
                int c = ((r > i)? cost[i][r-1]:0) +
                        ((r < j)? cost[r+1][j]:0) +
                        off_set_sum;
                if (c < cost[i][j]){
                    cost[i][j] = c;
                    root[i][j] = r;
                }
            }
        }
    }
    return cost[0][n-1];
}

int optimalBST(pii trees[]){
    int keys[n], freq[n];
    for(int i=0; i<n; i++){
        keys[i] = trees[i].first;
        freq[i] = trees[i].second;
    }
    return optimalSearchTree(keys, freq);
}
