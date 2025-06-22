#pragma once
#ifndef PATHFINDING_H
#define PATHFINDING_H

struct  sNode
{
	bool bObstacle = false;
	bool bVisited = false;
	float fGlobalGoal;
	float fLocalGoal;
	int x;
	int z;

	std::vector<sNode*> vecNeighbours;
	sNode* parent;
};

struct PathFinder
{
	sNode* nodes = nullptr;
	int nMapWidth = 10;
	int nMapHeight = 10;

	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;

	int startpoint = 0;

	int counter = 0;

	float delay_limit = 10000.0f;

	float delay_counter = 0.0f;

	bool pathfinding_active = false;

	bool following_path = false;

	std::vector<vf3d> path_to_follow;

	void Setup()
	{
		nodes = new sNode[nMapWidth * nMapHeight];

		for (int x = 0; x < nMapWidth; x++)
		{
			for (int z = 0; z < nMapHeight; z++)
			{
				nodes[z * nMapWidth + x].x = x; // ...because we give each node its own coordinates
				nodes[z * nMapWidth + x].z = z;
				nodes[z * nMapWidth + x].bObstacle = false;
				nodes[z * nMapWidth + x].parent = nullptr;
				nodes[z * nMapWidth + x].bVisited = false;
			}
		}

		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				if (y > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 0)]);
				if (y < nMapHeight - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 0)]);
				if (x > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x - 1)]);
				if (x < nMapWidth - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x + 1)]);


			}
		}

		nodeStart = &nodes[0 * nMapWidth + 1];
		nodeEnd = &nodes[0 * nMapWidth + nMapWidth - 1];
	}

	void update(cmn::Engine3D* ptr, vf3d billboard_pos)
	{
		int bz = billboard_pos.z;
		int bx = billboard_pos.x;
		nodeStart = &nodes[bz * nMapWidth + bx];

		int cz = ptr->cam_pos.z;
		int cx = ptr->cam_pos.x;

		nodeEnd = &nodes[cz * nMapWidth + cx];
	}

	bool solve_star()
	{
		// Reset Navigation Graph - default all node states
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].bVisited = false;
				nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;
				nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
				nodes[y * nMapWidth + x].parent = nullptr;	// No parents
			}

		auto distance = [](sNode* a, sNode* b) // For convenience
			{
				return sqrtf((a->x - b->x) * (a->x - b->x) + (a->z - b->z) * (a->z - b->z));
			};

		auto heuristic = [distance](sNode* a, sNode* b) // So we can experiment with heuristic
			{
				return distance(a, b);
			};

		// Setup starting conditions
		sNode* nodeCurrent = nodeStart;
		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

		// Add start node to not tested list - this will ensure it gets tested.
		// As the algorithm progresses, newly discovered nodes get added to this
		// list, and will themselves be tested later
		std::list<sNode*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		// if the not tested list contains nodes, there may be better paths
		// which have not yet been explored. However, we will also stop
		// searching when we reach the target - there may well be better
		// paths but this one will do - it wont be the longest.
		while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)// Find absolutely shortest path // && nodeCurrent != nodeEnd)
		{
			// Sort Untested nodes by global goal, so lowest is first
			listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

			// Front of listNotTestedNodes is potentially the lowest distance node. Our
			// list may also contain nodes that have been visited, so ditch these...
			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
				listNotTestedNodes.pop_front();

			// ...or abort because there are no valid nodes left to test
			if (listNotTestedNodes.empty())
				break;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true; // We only explore a node once


			// Check each of this node's neighbours...
			for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
			{
				// ... and only if the neighbour is not visited and is
				// not an obstacle, add it to NotTested List
				if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
					listNotTestedNodes.push_back(nodeNeighbour);

				// Calculate the neighbours potential lowest parent distance
				float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

				// If choosing to path through this node is a lower distance than what
				// the neighbour currently has set, update the neighbour to use this node
				// as the path source, and set its distance scores as necessary
				if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

					// The best path length to the neighbour being tested has changed, so
					// update the neighbour's score. The heuristic is used to globally bias
					// the path algorithm, so it knows if its getting better or worse. At some
					// point the algo will realise this path is worse and abandon it, and then go
					// and search along the next best path.
					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}
			}
		}
		return true;
	}

	void setup_path()
	{
		path_to_follow.clear();
		if (nodeEnd != nullptr)
		{
			sNode* p = nodeEnd;
			while (p->parent != nullptr)
			{

				path_to_follow.push_back({ (float)p->x,0,(float)p->z });
				// Set next node to this node's parent
				p = p->parent;
			}
		}

		counter = path_to_follow.size() - 1;
	}

	void path_following(std::vector<Billboard>& billboards, float dt)
	{

		if (!path_to_follow.empty()) {

			if (counter < 0)
			{
				counter = 0;
			}

			while (delay_counter < delay_limit)
			{
				delay_counter += 1 * dt;
			}

			for (auto& b : billboards)
			{
				b.pos.x = path_to_follow[counter].x;
				b.pos.z = path_to_follow[counter].z;
			}


			counter--;
			delay_counter = 0;
		}
		
	}

	void render_node_map(cmn::Engine3D* ptr)
	{
		int nNodeSize = 9;
		int nNodeBorder = 2;
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{

				ptr->FillRect(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, nNodeSize - nNodeBorder, nNodeSize - nNodeBorder,
					nodes[y * nMapWidth + x].bObstacle ? olc::WHITE : olc::BLUE);

				if (nodes[y * nMapWidth + x].bVisited)
					ptr->FillRect(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, nNodeSize - nNodeBorder, nNodeSize - nNodeBorder, olc::BLUE);

				if (&nodes[y * nMapWidth + x] == nodeStart)
					ptr->FillRect(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, nNodeSize - nNodeBorder, nNodeSize - nNodeBorder, olc::GREEN);

				if (&nodes[y * nMapWidth + x] == nodeEnd)
					ptr->FillRect(x * nNodeSize + nNodeBorder, y * nNodeSize + nNodeBorder, nNodeSize - nNodeBorder, nNodeSize - nNodeBorder, olc::RED);

			}
		}

		if (nodeEnd != nullptr)
		{
			sNode* p = nodeEnd;
			while (p->parent != nullptr)
			{
				ptr->DrawLine(p->x * nNodeSize + nNodeSize / 2, p->z * nNodeSize + nNodeSize / 2,
					p->parent->x * nNodeSize + nNodeSize / 2, p->parent->z * nNodeSize + nNodeSize / 2, olc::YELLOW);
		
				// Set next node to this node's parent
				p = p->parent;
			}
		}

		

	}

	void add_Obstacle(vf3d position)
	{
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int z = 0; z < nMapHeight; z++)
			{
				if (position.z == z && position.x == x)
				{
					nodes[z * nMapWidth + x].x = x; // ...because we give each node its own coordinates
					nodes[z * nMapWidth + x].z = z;
					nodes[z * nMapWidth + x].bObstacle = true;
					nodes[z * nMapWidth + x].parent = nullptr;
					nodes[z * nMapWidth + x].bVisited = false;
				}
			}
		}
	}

};

#endif // !PATHFINDING_H

