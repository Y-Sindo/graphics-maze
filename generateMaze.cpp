
# include <algorithm>
# include <iostream>
# include <utility>
# include <vector>
# include "generateMaze.h"
# include <time.h>
using namespace std;

const int neigh[4][2] = { { 0,1 },{ 0,-1 },{ 1,0 },{ -1,0 } };

void pushNeighbors(int u, int v, int H, int W, vector<vector<int>>& seen, vector<pair<int, int>>& edges)
{
	//Loop over neighbours
	for (int i = 0; i<4; i++)
	{
		int unext = u + neigh[i][0], vnext = v + neigh[i][1];
		//Check that found vertex is valid and unvisited
		if (unext<0 || unext >= H || vnext<0 || vnext >= W || seen[unext][vnext])
			continue;
		//Add the edge between you and the neighbour to the list
		edges.push_back(make_pair(u*W + v, unext*W + vnext));
	}
}

void Prim(int H, int W, vector<vector<int>>& maze)
{
	vector<vector<int>> seen(H, vector<int>(W, 0));
	vector<pair<int, int>> edges;

	//Randomise the pseudorandom generator with the current time
	srand(unsigned(time(0)));

	int u = rand() % H, v = rand() % W;
	seen[u][v] = 1;
	pushNeighbors(u, v, H, W, seen, edges);

	while (!edges.empty())
	{
		//Pick a random edge;
		int L = edges.size();
		int cur = rand() % L;
		if (cur != L - 1)swap(edges[L - 1], edges[cur]);
		auto edge = edges[L - 1];
		edges.pop_back();

		//Check whether the vertex has been visited through some other route
		u = edge.second / W, v = edge.second%W;
		if (seen[u][v])continue;

		//mark (u,v) as seen and remove the barrier to (uneigh,vneigh)
		seen[u][v] = 1;
		int uneigh = edge.first / W, vneigh = edge.first%W;
		maze[u + uneigh + 1][v + vneigh + 1] = 0;
		//Add the edges of (u,v) to the edge list
		pushNeighbors(u, v, H, W, seen, edges);
	}
}

//Create a maze of given size
int getNewMaze(int H, int W, vector<vector<int>>& maze)
{
	if (H <= 0 || H>100 || W <= 0 || W>100)
	{
		cout << "Invalid size for maze : " << "Maze dimensions should be between 1 and 100" << endl;
		return 1;
	}
	maze = vector<vector<int>>(2 * H + 1, vector<int>(2 * W + 2, 0));
	for (int i = 0; i <= 2 * H; i++)
	{
		maze[i][2 * W + 1] = 0;
		//The two bounding positions for the row
		maze[i][0] = maze[i][2 * W] = 1;
		for (int j = 1; j<2 * W; j++)
		{
			if ((i % 2) == 1){
				//We are in a row corresponding to an actual row in the maze
				if ((j % 2) == 1){
					//We are in an actual cell. So leave it free
					maze[i][j] = 0;
				}else{
					//We are at a vertical barrier between two cells
					maze[i][j] = 1;
				}
			}else{
				//We are at a horizontal barrier between two cells
				maze[i][j] = 1;
			}
		}
	}

	//Remove the barrier from the bottom left and top right
	maze[1][0] = maze[2 * H - 1][2 * W] = 0;
	//Run the algorithm to make the maze
	Prim(H, W, maze);
	return 0;
}

