#ifndef NODE_H
#define NODE_H
#include <vector>

class Node
{
public:
		Node(int passed_value, int x, int y);
		int value;

		int x_position;
		int y_position;

		//number of bridges in each direction
		int up_bridges;
		int down_bridges;
		int left_bridges;
		int right_bridges;
		int total_bridges;
		int bridges_in_this_direction(int direction);

		//nodes this one MAY connect to
		Node* get_neighbor(int direction);
		Node* up_neighbor;
		Node* down_neighbor;
		Node* left_neighbor;
		Node* right_neighbor;
		int num_neighbors();
		int total_possible_moves();
		int remaining_possible_moves();

		//bools for blocked directions (other bridges it can't cross, at an edge of the board, etc)
		int unblocked_node();
		std::vector<int> unblocked_nodes();

		bool up_blocked;
		bool down_blocked;
		bool left_blocked;
		bool right_blocked;
		int num_blocked;		//useful to see if only one connection is possible

		void node_filled();
		void direction_blocked(int direction);
		void block_check(Node* neighbor_node);

		//used when traversing the nodes to check for potential islands
		bool visited;

private:
		
};

#endif