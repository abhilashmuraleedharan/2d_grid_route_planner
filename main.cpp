/********************************
 *
 * 2D_Grid_RoutePlanner Project
 *
 * Author: Abhilash Muraleedharan
 *
 ********************************/

/*
 * STAGE 1: 
 * First aim is to write code that will parse a grid file which 
 * contain the state information in 1s and 0s separated by a comma.
 * For e.g.
 * Grid File might look something like this
 *   0,1,0,0,0,0,
 *   0,1,0,0,0,0,
 *   0,0,1,0,0,0,
 *   0,0,1,0,1,0,
 * where 0 represents empty space and 1 represents obstacle.
 *
 * Initial aim is to write code to
 * a) Read the grid file
 * b) Parse the data from the file and store the grid board in program
 * c) Print the grid board onto Terminal in nice ASCII Format.
 */

/*
 * STAGE 2:
 * Write an A* Search Function that implements the A* algorithm 
 * to find the most optimum path between two cells in the grid.
 * 
 * A* Algorithm finds a path from the start node to the end node
 * by checking for open neighbours of the current node, computing
 * a heuristic for each of the neighbours, and adding those neighbours
 * to the list of open nodes to explore next. The next node to explore
 * is the one with the lowest total cost+heuristic (g+h).
 * This process is repeated until the end is found or as long as there 
 * are still open nodes to explore.
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm> // For std::sort
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::istringstream;
using std::abs;
using std::sort;

// Custom State Type
enum class State {
   kEmpty,     // To represent empty cell
   kObstacle,  // To represent cell containing obstacle
   kClosed,    // To represent processed cell
   kPath,      // To represent path cell
   kStart,     // To represent start cell
   kFinish,    // To represent goal cell
   kChosen,    // To represent user chosen cell
};

const int direction_delta[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

/*
 * Helper Function that parses a string 
 * and returns a vector<State> object.
 */
vector<State> parseLine(string line) {
   istringstream sLine(line);
   vector<State> row{};
   int n; char c;

   if (sLine) {
      while(sLine >> n >> c && c == ',') {
         (n != 0) ? row.push_back(State::kObstacle)
                  : row.push_back(State::kEmpty);
      }
   }
   return row;   
}

/*
 * Reads the file denoted by input path and
 * returns a grid object containing State cells
 */
auto readGridFile(const string path) {
   ifstream bFile(path);
   vector<vector<State>> grid{};

   if (bFile) {
      string line;
      vector<State> row;
      while(getline(bFile, line)) {
         row = parseLine(line);
	      if (row.empty()) {
            cout << "Failed to parse grid file. Invalid content!" << endl;
	         grid.clear(); 
            return grid;
	      }
         grid.push_back(row);
	      row.clear();
      }
   }
   return grid;    
}

/*
 * Takes State as input and returns corresponding
 * ASCII String
 */
string cellString(State state) {
   switch(state) {
      case State::kObstacle: return  "⛰️   ";
      case State::kPath: return "🚗  ";
      case State::kStart: return "🚦  ";
      case State::kFinish: return "🏁   ";
      default: return "0   "; 
   }
}
    
/*
 * Print the grid states in ASCII string format
 */
void printBoard(const vector<vector<State>> & grid) {
   for(auto row : grid) {
      for (auto cell : row) {
         cout << cellString(cell);
      }
      cout << endl;
   }
}

/*
 * To Compute Heuristic value for each node.
 * In A* algorithm, heuristic value is the manhattan
 * distance between given node and goal node
 */
int heuristic(int x1, int y1, int x2, int y2) {
   return abs(x2 - x1) + abs(y2 - y1);
}

/*
 * Adds the given node to given list of open nodes
 * and updates the state of that node in the grid to kClosed.
 */
void addToOpenNodes(vector<int> node, vector<vector<int>> & open,
                    vector<vector<State>> & grid) {
   open.push_back(node);
   int x = node[0]; int y = node[1];
   grid[x][y] = State::kClosed;
}

/*
 * Helper function for sorting the list in 
 * descending order
 */
bool compare(const vector<int> node1, const vector<int> node2) {
   int f1 = node1[2] + node1[3]; // f = g + h
   int f2 = node2[2] + node2[3];
   return f1 > f2;
}

/*
 * Sort the given vector in descending order
 * using the custom compare function
 */
void sortNodes(vector<vector<int>> *v) {
   sort(v->begin(), v->end(), compare);
}

/*
 * Helper function that checks whether the given 
 * x and y values are valid positions on given grid
 */
bool validPosOnGrid(int x, int y, const vector<vector<State>> & grid) {
   bool x_on_grid = (x >= 0 && x < grid.size());
   bool y_on_grid = (y >= 0 && y < grid[0].size());
   return x_on_grid && y_on_grid;
}

/*
 * Returns true if the given node position is 
 * a) On the grid
 * b) Is empty 
 * else returns false
 */
bool validOpenNodePos(int x, int y, const vector<vector<State>> & grid) {
   if (validPosOnGrid(x, y, grid)) {
      return grid[x][y] == State::kEmpty;
   } else { return false; }
}

/*
 * Expand from current node to neighbouring nodes.
 * While iterating through neighbouring nodes,
 * if the node is open i.e. (not closed && empty && on grid)
 * then add that node to open list else skip that node
 */
void expandNeighbours(const vector<int> currNode,
                      const int goal[2],
                      vector<vector<int>> & open, 
                      vector<vector<State>> & grid) {
   // Iterate through potential neighbour node positions
   for (int i=0; i<4; i++) {  // limit 4 is taken from direction_delta 2D array
      int x = currNode[0] + direction_delta[i][0];
      int y = currNode[1] + direction_delta[i][1];
      if (validOpenNodePos(x, y, grid)) {
         // Valid open node position. Assign g and h values and add to open list
         int g = currNode[2] + 1;  // Increment the cost value by 1.
         int h = heuristic(x, y, goal[0], goal[1]);
         // Form the node and add it to open list
         vector<int> node{x, y, g, h};
         addToOpenNodes(node, open, grid);
      }
   }
}

/*
 * Finds the optimum path between start and finish
 * points in the given grid using A* search algorithm
 * and returns the solved grid.
 */
vector<vector<State>> searchPath(vector<vector<State>> grid, const int init[2], const int goal[2]) {

   /*
    * For A* Search algorithm,
    * There should be a list of open nodes which it will check 
    * every iteration. Each node should contain following information
    *   x and y position values 
    *   g value (Represents the cost/steps it took to reach that node from starting node)
    *   h value (Represents the Manhattan distance between this node and finishing node)
    * As you get closer to goal, h value decreases and g value increases.
    * 
    * A* search algorithm picks the node that has the least g+h value from the list of open
    * nodes and expand from it to neighbouring nodes.
    * The neighbouring nodes that are not yet closed(already processed), are on grid and are
    * empty will get added to open nodes list. Once a node is added to open nodes list, it's
    * state should be closed.
    * Keep picking the node with lowest f value(g+h) from the open nodes list and process it 
    * as described above until the path to goal node is found or until there are no more nodes
    * present in the open list.
    */

   /*
    * Form the starting node to be added to list of open node
    */
   int x = init[0]; int y = init[1];  // Position information
   int g = 0; // Cost
   int h = heuristic(x, y, goal[0], goal[1]);  // Calculates Manhattan distance to goal node
   vector<int> startNode{x, y, g, h}; 

   /*
    * Add the node to list of open nodes
    */
   vector<vector<int>> openNodes{};  // Empty list of open nodes
   addToOpenNodes(startNode, openNodes, grid);  // Add the starting node to list of open nodes

   /*
    * Keep processing the list of open nodes until the list is empty
    * or until the goal node is reached
    */
   while(!openNodes.empty()) {
      /*
       * To fetch the node with the least (g+h) value from the open nodes list,
       * sort the list in descending order and pick the last node as the current
       * node to expand to neighbours
       */
      sortNodes(&openNodes); // Sorts the list in descending order
      
      // Fetch the node with least g+h value
      vector<int> currNode = openNodes.back();
      openNodes.pop_back();
      
      // Extract position data
      int x = currNode[0]; int y = currNode[1];

      // Check if current node is the goal node
      if (x == goal[0] && y == goal[1]) {
         // Reached goal. Mark start and end State in grid
         int startX = init[0]; int startY = init[1];
         grid[startX][startY] = State::kStart;
         grid[x][y] = State::kFinish;
         return grid;
      }

      // Not goal node. Update grid State for the current node
      grid[x][y] = State::kPath;

      // Expand neighbours and add valid nodes to open nodes list
      expandNeighbours(currNode, goal, openNodes, grid);
   }

   // No path to goal was found. Return an empty grid
   vector<vector<State>> solution{};
   return solution;
}

void getUserInput(int init[], int goal[], vector<vector<State>> & grid) {
   int x=0; int y=0; 
   string line; 
   bool bReadInput = false;
   cout << endl;
   cout << "Rules to choose your own starting and finishing cell positions in the grid" << endl; 
   cout << "======================================================================================" << endl;
   cout << "1. Row and column index values start from 0" << endl;
   cout << "   Meaning top left cell position is \"0 0\" and bottom right cell position is \"4 5\"" << endl;
   cout << "2. Chosen cell position must be on the grid" << endl;
   cout << "3. Only an empty cell (represented as 0 in grid) can be chosen" << endl;
   cout << "4. Starting and finishing cell cannot be same" << endl;
   cout << "======================================================================================" << endl << endl;
   while(!bReadInput) {
      cout << "Enter starting cell row and column values in grid separated by a space" << endl; 
      getline(std::cin, line);
      istringstream stream(line);
      if (stream) {
         stream >> x >> y; 
         if (validPosOnGrid(x, y, grid)) {
            if (grid[x][y] == State::kEmpty) { 
               bReadInput = true; 
               init[0] = x;
               init[1] = y;
               grid[x][y] = State::kChosen; 
            }
         }
      }
      if (bReadInput == false) { 
         cout << "Invalid Input!!" << endl;
      } else {
         bReadInput = false;
         cout << "Enter finishing cell row and column values in grid separated by a space" << endl;
         getline(std::cin, line);
         istringstream stream(line);
         if (stream) {
            stream >> x >> y;
            if (validPosOnGrid(x, y, grid)) {
               if (grid[x][y] == State::kEmpty) { 
                  bReadInput = true; 
                  goal[0] = x;
                  goal[1] = y;
               }
            }
         }
         if (bReadInput == false) { 
            cout << "Invalid Input!!" << endl;
            // Reset the state of chosen cell back to kEmpty
            int chosenX = init[0]; int chosenY = init[1];
            grid[chosenX][chosenY] = State::kEmpty;
         }
      }
   }
}

int main() {
   cout << "Using A* search algorithm, this program will find the optimum path" << endl;
   cout << "between any 2 user given points in a 2 Dimensional Grid comprising" << endl;
   cout << "of randomly placed obstacles." << endl << endl;

   // Declare an empty grid
   vector<vector<State>> grid{}; 

   cout << "Choose a grid file from the grid_files folder and enter its name below" << endl;
   string path("grid_files/");
   string filename = "";
   getline(std::cin, filename);

   // Read state data from file and populate grid
   grid = readGridFile(path+filename); 

   if (grid.empty()) { 
      cout << "Invalid file path or grid file. Terminating program!" << endl;  
      return 0;
   }  

   cout << "Valid grid board! Printing the grid" << endl;
   printBoard(grid);

   // Get the starting and finishing position
   int startPosition[2]{};
   int finishPosition[2]{};
   getUserInput(startPosition, finishPosition, grid);
   cout << endl;

   /*
    * Search for the optimum path between start to finish
    * using A* Search Algorithm and return the solution
    * grid board
    */
   auto solutionGrid = searchPath(grid, startPosition, finishPosition);

   if (solutionGrid.empty()) {
      cout << "No path found" << endl;
   } else {
      cout << "Optimum path found. Printing solution grid" << endl << endl;
      // Print the solved grid board
      printBoard(solutionGrid); 
   }

   return 0;
}
