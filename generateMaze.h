#pragma once
# include <vector>
using namespace std;

void pushNeighbors(int u, int v, int H, int W, vector<vector<int>>& seen, vector<pair<int, int>>& edges);
void Prim(int H, int W, vector<vector<int>>& maze);
int getNewMaze(int H, int W, vector<vector<int>>& maze);
