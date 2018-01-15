#include <string>
#include <iostream>
#include <vector>
#include "Node.h"
using namespace std;

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;

int speculative_adds = 0;

//helper function declarations- implementation below main
void connect_nodes(Node* node1, Node* node2, int direction, bool is_speculative);
void bridge_check(Node* curr_node);
bool check_node_string(Node* curr_node);
bool check_for_island(Node* curr_node, int direction, int board_size, int bridge_count);

//declared globally for now to avoid passing a 2D vector by value
vector<vector<Node*>> board;

int main()
{
	//Take a board as input
	int board_size;
	cout << "Input size of board (n x n): ";
	cin >> board_size;

	for (int i = 0; i < board_size; i++)
	{
		vector<Node*> temp;
		for (int j = 0; j < board_size; j++)
		{
			//read each row and build a temporary vector to push later
			char temp_input;
			cin >> temp_input;

			if (temp_input == '.')
			{
				Node* temp_node = new Node(0, j, i);
				temp.push_back(temp_node);
			}
			else
			{
				Node* temp_node = new Node(temp_input - '0', j, i);
				temp.push_back(temp_node);
			}

		}
		board.push_back(temp);
	}

	//Find "neighbors" for nodes, here another node in a cardinal direction it MAY later connect to
	bool up_neighbor_set = false, down_neighbor_set = false, left_neighbor_set = false, right_neighbor_set = false;
	for (int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size; j++)
		{
			if (board[i][j]->value != 0)
			{
				for (int k = 1; k < board_size; k++)
				{
					if (j + k < board_size)
					{
						if (board[i][j + k]->value != 0 && right_neighbor_set == false)
						{
							board[i][j]->right_neighbor = board[i][j + k];
							right_neighbor_set = true;
						}
					}
					if (j - k >= 0)
					{
						if (board[i][j - k]->value != 0 && left_neighbor_set == false)
						{
							board[i][j]->left_neighbor = board[i][j - k];
							left_neighbor_set = true;
						}
					}
					if (i + k < board_size)
					{
						if (board[i + k][j]->value != 0 && down_neighbor_set == false)
						{
							board[i][j]->down_neighbor = board[i + k][j];
							down_neighbor_set = true;
						}
					}
					if (i - k >= 0)
					{
						if (board[i - k][j]->value != 0 && up_neighbor_set == false)
						{
							board[i][j]->up_neighbor = board[i - k][j];
							up_neighbor_set = true;
						}
					}
				}
			}

			up_neighbor_set = false;
			down_neighbor_set = false;
			left_neighbor_set = false;
			right_neighbor_set = false;
		}
	}

	//do one iteration outside of the main loop to set up obvious moves
	for (int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size; j++)
		{
			//Assign obvious blockages- edge nodes and a 1 connecting to a 1 (a common occurance in hashi)
			//Further blockages will be added as bridges are formed

			if (board[i][j]->left_neighbor == NULL || (board[i][j]->value == 1 && board[i][j]->left_neighbor->value == 1))
			{
				board[i][j]->left_blocked = true;
				board[i][j]->num_blocked++;
			}
			if (board[i][j]->right_neighbor == NULL || (board[i][j]->value == 1 && board[i][j]->right_neighbor->value == 1))
			{
				board[i][j]->right_blocked = true;
				board[i][j]->num_blocked++;
			}
			if (board[i][j]->up_neighbor == NULL || (board[i][j]->value == 1 && board[i][j]->up_neighbor->value == 1))
			{
				board[i][j]->up_blocked = true;
				board[i][j]->num_blocked++;
			}
			if (board[i][j]->down_neighbor == NULL || (board[i][j]->value == 1 && board[i][j]->down_neighbor->value == 1))
			{
				board[i][j]->down_blocked = true;
				board[i][j]->num_blocked++;
			}
		}
	}

	//Make as many safe moves as possible for each node, then repeat (much like relaxing edges)
	bool solved = false;
	bool move_found_this_iteration = false;
	while (!solved)
	{
		move_found_this_iteration = false;
		//Look at every node
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{
				//Ignore this node if it's already completely used or is actually a blank space/a bridge piece
				if (board[i][j]->total_bridges == board[i][j]->value || board[i][j]->value <= 0)
				{ }	//do nothing
				else
				{
					//If a node still needs bridges but it is blocked in every direction, or it has more bridges than its value, something has gone wrong
					if (board[i][j]->num_blocked == 4)
					{
						cout << endl << "Logical error- check board arrangement." << endl;

						for (int i = 0; i < board_size; i++)
						{
							for (int j = 0; j < board_size; j++)
							{
								if (board[i][j]->value == 0)
									cout << "  ";
								else if (board[i][j]->value == -1)
									cout << "| ";
								else if (board[i][j]->value == -2)
									cout << "||";
								else if (board[i][j]->value == -3)
									cout << "- ";
								else if (board[i][j]->value == -4)
									cout << "= ";
								else
									cout << board[i][j]->value << ' ';
							}
							cout << endl;
						}
						cout << endl;
					}

					//Check for bridges that would block one edge of the node
					bridge_check(board[i][j]);

					//If 3 directions are blocked you can make an obvious choice to connect the last one
					if (board[i][j]->num_blocked == 3)
					{
						int unblocked_direction = board[i][j]->unblocked_node();
						connect_nodes(board[i][j], board[i][j]->get_neighbor(unblocked_direction), unblocked_direction, false);

						//Make a double bridge if necessary
						if (board[i][j]->value == board[i][j]->total_bridges + 1)
							connect_nodes(board[i][j], board[i][j]->get_neighbor(unblocked_direction), unblocked_direction, false);

						move_found_this_iteration = true;
					}

					//If a node's remaining value equals the number of total possible remaining moves, all bridges must be fully connected
					if (board[i][j]->value - board[i][j]->total_bridges == board[i][j]->total_possible_moves())
					{
						vector<int> temp_vec = board[i][j]->unblocked_nodes();
						for (int k = 0; k < temp_vec.size(); k++)
						{
							//don't add a double bridge to a 1 or a node with a remaining value of 1
							if (board[i][j]->bridges_in_this_direction(temp_vec[k]) == 0
								&& board[i][j]->get_neighbor(temp_vec[k])->value - board[i][j]->get_neighbor(temp_vec[k])->total_bridges > 1)
							{
								connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[k]), temp_vec[k], false);
								connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[k]), temp_vec[k], false);
							}
							//else if (board[i][j]->bridges_in_this_direction(temp_vec[k]) == 1)
							else
								connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[k]), temp_vec[k], false);
						}
						move_found_this_iteration = true;
					}

					//If a node's remaining value equals remaining possible moves - 1
					//	all edges must have at least 1 bridge
					if (board[i][j]->value - board[i][j]->total_bridges == board[i][j]->total_possible_moves() -1)
					{
						vector<int> temp_vec = board[i][j]->unblocked_nodes();
						for (int k = 0; k < temp_vec.size(); k++)
						{
							//check to see if any bridges already exist in that direction
							//  if not, connect one
							if (temp_vec[k] == UP && board[i][j]->up_bridges < 1)
							{
								connect_nodes(board[i][j], board[i][j]->up_neighbor, UP, false);
								move_found_this_iteration = true;
							}
							else if (temp_vec[k] == DOWN && board[i][j]->down_bridges < 1)
							{
								connect_nodes(board[i][j], board[i][j]->down_neighbor, DOWN, false);
								move_found_this_iteration = true;
							}
							else if (temp_vec[k] == LEFT && board[i][j]->left_bridges < 1)
							{
								connect_nodes(board[i][j], board[i][j]->left_neighbor, LEFT, false);
								move_found_this_iteration = true;
							}
							else if (temp_vec[k] == RIGHT && board[i][j]->right_bridges < 1)
							{
								connect_nodes(board[i][j], board[i][j]->right_neighbor, RIGHT, false);
								move_found_this_iteration = true;
							}
						}
					}

					//Check to see if adding a bridge in any of the remaining unblocked directions would create an island
					vector<int> temp_vec = board[i][j]->unblocked_nodes();
					for (int k = 0; k < temp_vec.size(); k++)
					{
						if (temp_vec[k] == UP)
						{
							if (check_for_island(board[i][j], UP, board_size, 1))
								move_found_this_iteration = true;
						}
						else if (temp_vec[k] == DOWN)
						{
							if (check_for_island(board[i][j], DOWN, board_size, 1))
								move_found_this_iteration = true;
						}
						else if (temp_vec[k] == LEFT)
						{
							if (check_for_island(board[i][j], LEFT, board_size, 1))
								move_found_this_iteration = true;
						}
						else if (temp_vec[k] == RIGHT)
						{
							if (check_for_island(board[i][j], RIGHT, board_size, 1))
								move_found_this_iteration = true;
						}
					}

					//if there are two unblocked edges, and a node only has two bridges left to build, will building two bridges in a direction create an island?
					//this will only guarantee one bridge in the other direction, not that a direction is blocked
					if (board[i][j]->num_blocked == 2 && board[i][j]->value - board[i][j]->total_bridges == 2)
					{
						vector<int> temp_vec = board[i][j]->unblocked_nodes();
						for (int k = 0; k < temp_vec.size(); k++)
						{
							if (temp_vec[k] == UP && board[i][j]->get_neighbor(UP)->value >= 2 && board[i][j]->get_neighbor(UP)->total_bridges == 0)
							{
								//at least one bridge must go in the second direction
								if (check_for_island(board[i][j], UP, board_size, 2))
								{
									move_found_this_iteration = true;
									if (k == 0)
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[1]), temp_vec[1], false);
									else
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[0]), temp_vec[0], false);
								}
							}
							else if (temp_vec[k] == DOWN && board[i][j]->get_neighbor(DOWN)->value >= 2 && board[i][j]->get_neighbor(DOWN)->total_bridges == 0)
							{
								if (check_for_island(board[i][j], DOWN, board_size, 2))
								{
									move_found_this_iteration = true;
									if (k == 0)
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[1]), temp_vec[1], false);
									else
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[0]), temp_vec[0], false);
								}
							}
							else if (temp_vec[k] == LEFT && board[i][j]->get_neighbor(LEFT)->value >= 2 && board[i][j]->get_neighbor(LEFT)->total_bridges == 0)
							{
								if (check_for_island(board[i][j], LEFT, board_size, 2))
								{
									move_found_this_iteration = true;
									if (k == 0)
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[1]), temp_vec[1], false);
									else
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[0]), temp_vec[0], false);
								}
							}
							else if (temp_vec[k] == RIGHT && board[i][j]->get_neighbor(RIGHT)->value >= 2 && board[i][j]->get_neighbor(RIGHT)->total_bridges == 0)
							{
								if (check_for_island(board[i][j], RIGHT, board_size, 2))
								{
									move_found_this_iteration = true;
									if (k == 0)
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[1]), temp_vec[1], false);
									else
										connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[0]), temp_vec[0], false);
								}
							}
						}
					}

					//if a node has two unblocked edges and one is not enough to satisfy it, you can connect at least one bridge to the other node
					if (board[i][j]->num_blocked == 2 && board[i][j]->value - board[i][j]->total_bridges >= 2)
					{
						vector<int> temp_vec = board[i][j]->unblocked_nodes();
						for (int k = 0; k < temp_vec.size(); k++)
						{
							if (board[i][j]->get_neighbor(temp_vec[k])->value - board[i][j]->get_neighbor(temp_vec[k])->total_bridges == 1)
							{
								move_found_this_iteration = true;
								if (k == 0)
									connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[1]), temp_vec[1], false);
								else
									connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[0]), temp_vec[0], false);
							}
						}
					}

					//(shamefully hardcoded as I could not discover a general relation)
					//if a certain number of neighbors to a node have only one valid move, then you know that at least one bridge must connect
					//  to a higher valued node

					//e.g. a 6 with 4 neighbors, one of which is a 1.  At least one bridge must connect to the 
					//  3 remaining non-1 nodes, because you can only connect 5 bridges total without touching all of the higher-value nodes
					if (move_found_this_iteration == false)
					{
						int needed_ones = -1;
						if (board[i][j]->value - board[i][j]->total_bridges == 2)
						{
							if (board[i][j]->num_neighbors() == 2)
							{
								needed_ones = 1;
							}
						}
						else if (board[i][j]->value - board[i][j]->total_bridges == 3)
						{
							if (board[i][j]->num_neighbors() == 3)
							{
								needed_ones = 2;
							}
						}
						else if (board[i][j]->value - board[i][j]->total_bridges == 4)
						{
							if (board[i][j]->num_neighbors() == 3)
							{
								needed_ones = 1;
							}
							else if (board[i][j]->num_neighbors() == 4)
							{
								needed_ones = 3;
							}
						}
						else if (board[i][j]->value - board[i][j]->total_bridges == 5)
						{
							if (board[i][j]->num_neighbors() == 4)
							{
								needed_ones = 2;
							}
						}
						else if (board[i][j]->value - board[i][j]->total_bridges == 6)
						{
							if (board[i][j]->num_neighbors() == 4)
							{
								needed_ones = 1;
							}
						}

						vector<int> temp_vec = board[i][j]->unblocked_nodes();
						int num_ones = 0;
						for (int k = 0; k < temp_vec.size(); k++)
						{
							if (board[i][j]->get_neighbor(temp_vec[k])->value - board[i][j]->get_neighbor(temp_vec[k])->total_bridges == 1
								|| board[i][j]->bridges_in_this_direction(temp_vec[k]) == 1)
								num_ones++;
						}
						if (num_ones == needed_ones)
						{
							for (int k = 0; k < temp_vec.size(); k++)
							{
								if (board[i][j]->get_neighbor(temp_vec[k])->value != 1)
									connect_nodes(board[i][j], board[i][j]->get_neighbor(temp_vec[k]), temp_vec[k], false);
							}
						}
					}



				}
			}
		}

		if (move_found_this_iteration == false)
			solved = true;
	}



	//Output board
	for (int i = 0; i < board_size; i++)
	{
		for (int j = 0; j < board_size; j++)
		{
			if (board[i][j]->value == 0)
				cout << "  ";
			else if (board[i][j]->value == -1)
				cout << "| ";
			else if (board[i][j]->value == -2)
				cout << "||";
			else if (board[i][j]->value == -3)
				cout << "- ";
			else if (board[i][j]->value == -4)
				cout << "= ";
			else
				cout << board[i][j]->value << ' ';
		}
		cout << endl;
	}

	system("pause");
	return 0;
}

void disconnect_nodes(Node* curr_node, Node* neighbor_node, int direction)
{
	if (direction == UP)
	{
		int distance = abs(neighbor_node->y_position - curr_node->y_position);
		for (int i = 1; i < distance; i++)
		{
			//If there is a double vertical bridge, change values to -1, otherwise, 0
			if (curr_node->up_bridges == 2)
				board[curr_node->y_position - i][curr_node->x_position]->value = -1;
			else if (curr_node->up_bridges == 1)
				board[curr_node->y_position - i][curr_node->x_position]->value = 0;
			else
				cout << endl << "Error disconnecting nodes- a bridge was not already present. Check function usage" << endl;
		}

		curr_node->up_bridges--;
		neighbor_node->down_bridges--;
	}
	else if (direction == DOWN)
	{
		int distance = abs(neighbor_node->y_position - curr_node->y_position);
		for (int i = 1; i < distance; i++)
		{
			if (curr_node->down_bridges == 2)
				board[curr_node->y_position + i][curr_node->x_position]->value = -1;
			else if(curr_node->down_bridges == 1)
				board[curr_node->y_position + i][curr_node->x_position]->value = 0;
			else
				cout << endl << "Error disconnecting nodes- a bridge was not already present. Check function usage" << endl;
		}

		curr_node->down_bridges--;
		neighbor_node->up_bridges--;
	}
	else if (direction == LEFT)
	{
		int distance = abs(neighbor_node->x_position - curr_node->x_position);
		for (int i = 1; i < distance; i++)
		{
			//If there is a double horizontal bridge, change values to -3, otherwise, 0
			if (curr_node->left_bridges == 2)
				board[curr_node->y_position][curr_node->x_position - i]->value = -3;
			else if (curr_node->left_bridges == 1)
				board[curr_node->y_position][curr_node->x_position - i]->value = 0;
			else
				cout << endl << "Error disconnecting nodes- a bridge was not already present. Check function usage" << endl;
		}

		curr_node->left_bridges--;
		neighbor_node->right_bridges--;
	}
	else
	{
		int distance = abs(neighbor_node->x_position - curr_node->x_position);
		for (int i = 1; i < distance; i++)
		{
			if (curr_node->right_bridges == 2)
				board[curr_node->y_position][curr_node->x_position + i]->value = -3;
			else if (curr_node->right_bridges == 1)
				board[curr_node->y_position][curr_node->x_position + i]->value = 0;
			else
				cout << endl << "Error disconnecting nodes- a bridge was not already present. Check function usage" << endl;
		}

		curr_node->right_bridges--;
		neighbor_node->left_bridges--;
	}

	//change any other necessary variables for each node
	curr_node->total_bridges--;
	neighbor_node->total_bridges--;
}

void connect_nodes(Node* curr_node, Node* neighbor_node, int direction, bool is_speculative)
{
	//TESTING
	if (is_speculative)
	{
		speculative_adds++;
		//cout << "Number of speculative adds: " << speculative_adds << endl;
		if (speculative_adds >= 10000)
			cout << "Let's check..." << endl;
	}

	//set all nodes in-between to bridge pieces- -1 is a vertical connector, -2 a double vertical, -3 a horizontal, and -4 a double horizontal
	//Vertical connection
	if (direction == UP)
	{
		if (curr_node->x_position != neighbor_node->x_position)		//ERROR CHECKING
			cout << endl << "Error connecting nodes- possible source is in neighbor creation" << endl;

		int distance = abs(neighbor_node->y_position - curr_node->y_position);
		for (int i = 1; i < distance; i++)
		{
			//If there is already a vertical bridge, change values to -2, otherwise, -1
			if (curr_node->up_bridges == 1)
				board[curr_node->y_position - i][curr_node->x_position]->value = -2;
			else
				board[curr_node->y_position - i][curr_node->x_position]->value = -1;
		}

		curr_node->up_bridges++;
		neighbor_node->down_bridges++;
	}
	else if (direction == DOWN)
	{
		if (curr_node->x_position != neighbor_node->x_position)		//ERROR CHECKING
			cout << endl << "Error connecting nodes- possible source is in neighbor creation" << endl;

		int distance = abs(neighbor_node->y_position - curr_node->y_position);
		for (int i = 1; i < distance; i++)
		{
			if (curr_node->down_bridges == 1)
				board[curr_node->y_position + i][curr_node->x_position]->value = -2;
			else
				board[curr_node->y_position + i][curr_node->x_position]->value = -1;
		}

		curr_node->down_bridges++;
		neighbor_node->up_bridges++;
	}
	//Horizontal connection
	else if (direction == LEFT)
	{
		if (curr_node->y_position != neighbor_node->y_position)		//ERROR CHECKING
			cout << endl << "Error connecting nodes- possible source is in neighbor creation" << endl;

		int distance = abs(neighbor_node->x_position - curr_node->x_position);
		for (int i = 1; i < distance; i++)
		{
			//If there is already a vertical bridge, change values to -4, otherwise, -3
			if (curr_node->left_bridges == 1)
				board[curr_node->y_position][curr_node->x_position - i]->value = -4;
			else
				board[curr_node->y_position][curr_node->x_position - i]->value = -3;
		}

		curr_node->left_bridges++;
		neighbor_node->right_bridges++;
	}
	else
	{
		if (curr_node->y_position != neighbor_node->y_position)		//ERROR CHECKING
			cout << endl << "Error connecting nodes- possible source is in neighbor creation" << endl;

		int distance = abs(neighbor_node->x_position - curr_node->x_position);
		for (int i = 1; i < distance; i++)
		{
			if (curr_node->right_bridges == 1)
				board[curr_node->y_position][curr_node->x_position + i]->value = -4;
			else
				board[curr_node->y_position][curr_node->x_position + i]->value = -3;
		}

		curr_node->right_bridges++;
		neighbor_node->left_bridges++;
	}

	//change any other necessary variables for each node
	curr_node->total_bridges++;
	neighbor_node->total_bridges++;


	//Check for newly formed blockages, but not if the function is called from the island checking function
	if (is_speculative == false)
		curr_node->block_check(neighbor_node);
}

void bridge_check(Node* curr_node)
{
	int y_position = curr_node->y_position;
	int x_position = curr_node->x_position;

	if (curr_node->up_neighbor != NULL)
	{
		int distance = abs(curr_node->up_neighbor->y_position - y_position);
		for (int i = 1; i < distance; i++)
		{
			if (board[y_position - i][x_position]->value == -3 || board[y_position - i][x_position]->value == -4)
			{
				if (curr_node->up_blocked == false)
				{
					curr_node->up_blocked = true;
					curr_node->num_blocked++;
				}

				if (curr_node->up_neighbor->down_blocked == false)
				{
					curr_node->up_neighbor->down_blocked = true;
					curr_node->up_neighbor->num_blocked++;
				}

				curr_node->up_neighbor->down_neighbor = NULL;
				curr_node->up_neighbor = NULL;

				break;
			}
		}
	}

	if (curr_node->down_neighbor != NULL)
	{
		int distance = abs(curr_node->down_neighbor->y_position - y_position);
		for (int i = 1; i < distance; i++)
		{
			if (board[y_position + i][x_position]->value == -3 || board[y_position + i][x_position]->value == -4)
			{
				if (curr_node->down_blocked == false)
				{
					curr_node->down_blocked = true;
					curr_node->num_blocked++;
				}

				if (curr_node->down_neighbor->up_blocked == false)
				{
					curr_node->down_neighbor->up_blocked = true;
					curr_node->down_neighbor->num_blocked++;
				}

				curr_node->down_neighbor->up_neighbor = NULL;
				curr_node->down_neighbor = NULL;

				break;
			}
		}
	}

	if (curr_node->left_neighbor != NULL)
	{
		int distance = abs(curr_node->left_neighbor->x_position - x_position);
		for (int i = 1; i < distance; i++)
		{
			if (board[y_position][x_position - i]->value == -1 || board[y_position][x_position - i]->value == -2)
			{
				if (curr_node->left_blocked == false)
				{
					curr_node->left_blocked = true;
					curr_node->num_blocked++;
				}

				if (curr_node->left_neighbor->right_blocked == false)
				{
					curr_node->left_neighbor->right_blocked = true;
					curr_node->left_neighbor->num_blocked++;
				}

				curr_node->left_neighbor->right_neighbor = NULL;
				curr_node->left_neighbor = NULL;

				break;
			}
		}
	}

	if (curr_node->right_neighbor != NULL)
	{
		int distance = abs(curr_node->right_neighbor->x_position - x_position);
		for (int i = 1; i < distance; i++)
		{
			if (board[y_position][x_position + i]->value == -1 || board[y_position][x_position + i]->value == -2)
			{
				if (curr_node->right_blocked == false)
				{
					curr_node->right_blocked = true;
					curr_node->num_blocked++;
				}

				if (curr_node->right_neighbor->left_blocked == false)
				{
					curr_node->right_neighbor->left_blocked = true;
					curr_node->right_neighbor->num_blocked++;
				}

				curr_node->right_neighbor->left_neighbor = NULL;
				curr_node->right_neighbor = NULL;

				break;
			}
		}
	}
}

//returns true if a direction was blocked
bool check_for_island(Node* curr_node, int direction, int board_size, int bridge_count)
{
	//speculatively add a bridge (or two)
	connect_nodes(curr_node, curr_node->get_neighbor(direction), direction, true);
	if (bridge_count == 2)
		connect_nodes(curr_node, curr_node->get_neighbor(direction), direction, true);

	//an island was not created since all the nodes are not used up
	if (check_node_string(curr_node) == true)
	{
		//remove the bridge- there may still be multiple options for bridges, so it is not
		//definitive if this one will be made- basically, you just know that you can't block this direction
		disconnect_nodes(curr_node, curr_node->get_neighbor(direction), direction);
		if (bridge_count == 2)
			disconnect_nodes(curr_node, curr_node->get_neighbor(direction), direction);

		//reset visited variable for all nodes
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{
				board[i][j]->visited = false;
			}
		}

		return false;
	}

	//check for a fully completed puzzle
	else
	{
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{
				//If the series of recursive calls did not reach every node, there is an island
				if (board[i][j]->visited == false && board[i][j]->value >= 1)
				{
					//The previously added bridge needs to be disconnected and the direction considered blocked
					disconnect_nodes(curr_node, curr_node->get_neighbor(direction), direction);
					if (bridge_count == 2)
						disconnect_nodes(curr_node, curr_node->get_neighbor(direction), direction);

					//when speculating with 2 bridges, you can't block a direction, you just know that at least 1 bridge must go in another direction
					if (bridge_count == 1)
						curr_node->direction_blocked(direction);

					//reset the rest of the variables
					for (int k = i; k < board_size; k++)
					{
						for (int l = 0; l < board_size; l++)
						{
							board[k][l]->visited = false;
						}
					}
					
					return true;
				}

				//reset the visited variable as you go
				board[i][j]->visited = false;
			}
		}
	}

	return false;
}

//Attempts to touch all nodes that are connected to each other
bool check_node_string(Node* curr_node)
{
	//bool return_val = true;

	if (curr_node->value != curr_node->total_bridges)
		return true;

	curr_node->visited = true;

	if (curr_node->up_bridges >= 1 && curr_node->up_neighbor->visited == false)
	{
		if (check_node_string(curr_node->up_neighbor))
			return true;
	}
	if (curr_node->down_bridges >= 1 && curr_node->down_neighbor->visited == false)
	{
		if (check_node_string(curr_node->down_neighbor))
			return true;
	}
	if (curr_node->left_bridges >= 1 && curr_node->left_neighbor->visited == false)
	{
		if (check_node_string(curr_node->left_neighbor))
			return true;
	}
	if (curr_node->right_bridges >= 1 && curr_node->right_neighbor->visited == false)
	{
		if (check_node_string(curr_node->right_neighbor))
			return true;
	}

	//if there is no node where the value does not equal the total bridges, only then should the function return false, indicating an island
	return false;
}

/*

5
2 . . . .
. . 1 . .
4 . . . .
. . . . .
3 . 2 . .
^^^ A very simple starting puzzle

6
1 . 3 . . .
. . . . 1 .
1 . 3 . . .
. . . . 2 .
. . . . . .
2 . 4 . 3 .
^^^ Requires checking for islands

6
2 . 5 . 1 .
. . . . . 2
. . . . . .
1 . 4 . . 3
. . . . . .
. . . . . .
^^^ A simple test of 1-bridge minimum testing

6
2 . 3 . 2 .
. . . . . .
3 . . 3 . .
. . . . . .
2 . 1 . . .
. 1 . 3 . .
^^^ Utilizes the fact that a bridge blocks a potential neighbor

8
2 . 4 . 2 . . .
. 1 . 2 . . . .
. . . . 1 . . .
. 2 . 3 . 2 . .
. . . . . . . .
. . 2 . . 3 . .
. . . . . . . .
. 2 . . . 2 . .
^^^ More bridge blocking

25
2 . 2 . 2 . . 2 . 2 . . 2 . . . . 2 . 2 . 2 . 2 .
. 1 . . . . 2 . . . 4 . . 5 . 2 . . 1 . 2 . 2 . 1
2 . . 5 . 4 . . 3 . . . . . 1 . . 4 . 5 . 1 . 1 .
. . . . . . . . . . . 1 . 3 . . 1 . . . . . . . .
2 . . 6 . 6 . . 8 . 5 . 2 . . 3 . 5 . 7 . . 2 . .
. 1 . . . . . . . . . 1 . . 2 . . . . . 1 . . . 3
2 . . . . 5 . . 6 . 4 . . 2 . . . 2 . 5 . 4 . 2 .
. 2 . 2 . . . . . . . . . . . 3 . . 3 . . . 1 . 2
. . . . . . . . . . 4 . 2 . 2 . . 1 . . . 3 . 1 .
2 . 3 . . 6 . . 2 . . . . . . . . . . 3 . . . . .
. . . . 1 . . 2 . . 5 . . 1 . 4 . 3 . . . . 2 . 4
. . 2 . . 1 . . . . . . 5 . 4 . . . . 4 . 3 . . .
2 . . . 3 . 1 . . . . . . . . 3 . . 5 . 5 . . 2 .
. . . . . 2 . 5 . . 7 . 5 . 3 . 1 . . 1 . . 1 . 4
2 . 5 . 3 . . . . 1 . 2 . 1 . . . . 2 . 4 . . 2 .
. . . . . 1 . . . . . . . . . . 2 . . 2 . 1 . . 3
2 . 6 . 6 . . 2 . . 2 . 2 . 5 . . . . . 2 . . . .
. . . . . 1 . . . 3 . . . . . 1 . . 1 . . 4 . 3 .
. . 4 . 5 . . 2 . . . 2 . . 6 . 6 . . 3 . . . . 3
2 . . . . . . . . . 2 . . 1 . . . . . . 1 . . 1 .
. . 3 . . 3 . 5 . 5 . . 4 . 6 . 7 . . 4 . 6 . . 4
2 . . . 3 . 5 . 2 . 1 . . . . . . . . . . . . . .
. . . . . . . . . 1 . . . . . . 3 . 2 . . 5 . . 5
2 . 3 . 3 . 5 . 4 . 3 . 3 . 4 . . 2 . 2 . . . 1 .
. 1 . 2 . 2 . . . 2 . 2 . . . 2 . . . . 2 . 2 . 2
^^^ Pretty much every trick in the book.  Personally incapable of solving the final portion in the lower-left corner by hand
			using pure logic...
*/