#include "Node.h"
#include <iostream>
#include <vector>
using namespace std;

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

Node::Node(int passed_value, int x, int y)
{
	value = passed_value;
	x_position = x;
	y_position = y;

	up_bridges = 0;
	down_bridges = 0;
	left_bridges = 0;
	right_bridges = 0;
	total_bridges = 0;

	up_blocked = false;
	down_blocked = false;
	left_blocked = false;
	right_blocked = false;
	num_blocked = 0;

	visited = false;
}

int Node::bridges_in_this_direction(int direction)
{
	if (direction == UP)
		return up_bridges;
	if (direction == DOWN)
		return down_bridges;
	if (direction == LEFT)
		return left_bridges;
	if (direction == RIGHT)
		return right_bridges;
	else
		return -500;
}

Node* Node::get_neighbor(int direction)
{
	if (direction == UP)
		return up_neighbor;
	if (direction == DOWN)
		return down_neighbor;
	if (direction == LEFT)
		return left_neighbor;
	if (direction == RIGHT)
		return right_neighbor;
	else
		return NULL;
}

int Node::num_neighbors()
{
	int neighbors = 0;
	if (up_neighbor != NULL)
		neighbors++;
	if (down_neighbor != NULL)
		neighbors++;
	if (left_neighbor != NULL)
		neighbors++;
	if (right_neighbor != NULL)
		neighbors++;

	return neighbors;
}

int Node::remaining_possible_moves()
{
	int moves = 2 * num_neighbors();
	if (up_neighbor != NULL)
	{
		for (int i = 0; i < up_bridges; i++)
			moves--;
		if (up_neighbor->value - up_neighbor->total_bridges == 1 && up_bridges == 0)
			moves--;
		else if (up_neighbor->value - up_neighbor->total_bridges == 0 && up_bridges == 1)
			moves--;
	}
	if (down_neighbor != NULL)
	{
		for (int i = 0; i < down_bridges; i++)
			moves--;
		if (down_neighbor->value - down_neighbor->total_bridges == 1 && down_bridges == 0)
			moves--;
		else if (down_neighbor->value - down_neighbor->total_bridges == 0 && down_bridges == 1)
			moves--;
	}
	if (left_neighbor != NULL)
	{
		for (int i = 0; i < left_bridges; i++)
			moves--;
		if (left_neighbor->value - left_neighbor->total_bridges == 1 && left_bridges == 0)
			moves--;
		else if (left_neighbor->value - left_neighbor->total_bridges == 0 && left_bridges == 1)
			moves--;
	}
	if (right_neighbor != NULL)
	{
		for (int i = 0; i < right_bridges; i++)
			moves--;
		if (right_neighbor->value - right_neighbor->total_bridges == 1 && right_bridges == 0)
			moves--;
		else if (right_neighbor->value - right_neighbor->total_bridges == 0 && right_bridges == 1)
			moves--;
	}

	return moves;
}

//does not take into account nodes with only one possible connection remaining
int Node::total_possible_moves()
{
	int moves = 2 * num_neighbors();
	if (up_neighbor != NULL)
	{
		for (int i = 0; i < up_bridges; i++)
			moves--;
		//if there are no bridges and the left neighbor can only take one more bridge
		//or if there is one bridge and the left neighbor is already full
		if (up_bridges == 1 && up_neighbor->value == up_neighbor->total_bridges)
		//if (up_neighbor->value - up_neighbor->total_bridges == 1 && up_bridges == 0)
			moves--;
		//else if (up_neighbor->value - up_neighbor->total_bridges == 0 && up_bridges == 1)
		//	moves--;
	}
	if (down_neighbor != NULL)
	{
		for (int i = 0; i < down_bridges; i++)
			moves--;
		if (down_bridges == 1 && down_neighbor->value == down_neighbor->total_bridges)
		//if (down_neighbor->value - down_neighbor->total_bridges == 1 && down_bridges == 0)
			moves--;
		//else if (down_neighbor->value - down_neighbor->total_bridges == 0 && down_bridges == 1)
		//	moves--;
	}
	if (left_neighbor != NULL)
	{
		for (int i = 0; i < left_bridges; i++)
			moves--;
		if (left_bridges == 1 && left_neighbor->value == left_neighbor->total_bridges)
		//if (left_neighbor->value - left_neighbor->total_bridges == 1 && left_bridges == 0)
			moves--;
		//else if (left_neighbor->value - left_neighbor->total_bridges == 0 && left_bridges == 1)
		//	moves--;
	}
	if (right_neighbor != NULL)
	{
		for (int i = 0; i < right_bridges; i++)
			moves--;
		if (right_bridges == 1 && right_neighbor->value == right_neighbor->total_bridges)
		//if (right_neighbor->value - right_neighbor->total_bridges == 1 && right_bridges == 0)
			moves--;
		//else if (right_neighbor->value - right_neighbor->total_bridges == 0 && right_bridges == 1)
		//	moves--;
	}

	return moves;
}

//This function assumes that only one direction is unblocked, and returns it
int Node::unblocked_node()
{
	if (up_blocked == false)
		return UP;
	else if (down_blocked == false)
		return DOWN;
	else if (left_blocked == false)
		return LEFT;
	else if (right_blocked == false)
		return RIGHT;
	else
	{
		cout << endl << "Improper usage of function unblocked_node()" << endl;
		return -1;
	}
}

//This function assumes more than one direction is unblocked, and returns the directions in a vector
vector<int> Node::unblocked_nodes()
{
	vector<int> result;
	if (up_blocked == false)
		result.push_back(UP);
	if (down_blocked == false)
		result.push_back(DOWN);
	if (left_blocked == false)
		result.push_back(LEFT);
	if (right_blocked == false)
		result.push_back(RIGHT);

	return result;
}

void Node::node_filled()
{
	up_blocked = true;
	down_blocked = true;
	left_blocked = true;
	right_blocked = true;
	num_blocked = 4;

	//also blocks the corresponding directions of neighbor nodes if they aren't already blocked
	if (up_neighbor != NULL)
	{
		if (up_neighbor->down_blocked == false)
		{
			up_neighbor->down_blocked = true;
			up_neighbor->num_blocked++;
		}
	}
	if (down_neighbor != NULL)
	{
		if (down_neighbor->up_blocked == false)
		{
			down_neighbor->up_blocked = true;
			down_neighbor->num_blocked++;
		}
	}
	if (left_neighbor != NULL)
	{
		if (left_neighbor->right_blocked == false)
		{
			left_neighbor->right_blocked = true;
			left_neighbor->num_blocked++;
		}
	}
	if (right_neighbor != NULL)
	{
		if (right_neighbor->left_blocked == false)
		{
			right_neighbor->left_blocked = true;
			right_neighbor->num_blocked++;
		}
	}
}

//block the connection between this node and the neighbor node in the given direction
void Node::direction_blocked(int direction)
{
	if (direction == UP)
	{
		if (up_blocked == false)
		{
			up_blocked = true;
			num_blocked++;
		}
		if (up_neighbor != NULL)
		{
			if (up_neighbor->down_blocked == false)
			{
				up_neighbor->down_blocked = true;
				up_neighbor->num_blocked++;
			}
		}
	}
	else if (direction == DOWN)
	{
		if (down_blocked == false)
		{
			down_blocked = true;
			num_blocked++;
		}
		if (down_neighbor != NULL)
		{
			if (down_neighbor->up_blocked == false)
			{
				down_neighbor->up_blocked = true;
				down_neighbor->num_blocked++;
			}
		}
	}
	else if (direction == LEFT)
	{
		if (left_blocked == false)
		{
			left_blocked = true;
			num_blocked++;
		}
		if (left_neighbor != NULL)
		{
			if (left_neighbor->right_blocked == false)
			{
				left_neighbor->right_blocked = true;
				left_neighbor->num_blocked++;
			}
		}
	}
	else
	{
		if (right_blocked == false)
		{
			right_blocked = true;
			num_blocked++;
		}
		if (right_neighbor != NULL)
		{
			if (right_neighbor->left_blocked == false)
			{
				right_neighbor->left_blocked = true;
				right_neighbor->num_blocked++;
			}
		}
	}
}

void Node::block_check(Node* neighbor_node)
{
	//if you have just "filled up" the node with the new bridge; it has no more valid moves
	if (value == total_bridges)
	{
		node_filled();
	}

	//2 bridges is the maximum in any direction, so that direction is now blocked
	if (up_bridges == 2)
		direction_blocked(UP);
	if (up_neighbor != NULL)
	{
		//check to see if you have just "filled up" the neighbor with the new bridge
		if (up_neighbor->total_bridges == up_neighbor->value)
		{
			//block all directions from and to the neighbor node
			up_neighbor->node_filled();
		}
	}

	if (down_bridges == 2)
		direction_blocked(DOWN);
	if (down_neighbor != NULL)
	{
		if (down_neighbor->total_bridges == down_neighbor->value)
			down_neighbor->node_filled();
	}

	if (left_bridges == 2)
		direction_blocked(LEFT);
	if (left_neighbor != NULL)
	{
		if (left_neighbor->total_bridges == left_neighbor->value)
			left_neighbor->node_filled();
	}

	if (right_bridges == 2)
		direction_blocked(RIGHT);
	if (right_neighbor != NULL)
	{
		if (right_neighbor->total_bridges == right_neighbor->value)
			right_neighbor->node_filled();
	}
}