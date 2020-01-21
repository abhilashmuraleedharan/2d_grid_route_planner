/********************************
 *
 * OpenStreetMap_RoutePlanner Project
 *
 * Author: Abhilash Muraleedharan
 *
 ********************************/
/*
 * STAGE 1:
 * To start small, write a program that will plan route between 
 * two points in an ASCII board with empty spaces and obstacles.
 *
 * First aim is to write code that will parse a board file which 
 * contain the state information in 1s and 0s separated by a comma.
 * For e.g.
 * Board File might look something like this
 *   0,1,0,0,0,0,
 *   0,1,0,0,0,0,
 *   0,0,1,0,0,0,
 *   0,0,1,0,1,0,
 * where 0 represents empty space and 1 represents obstacle.
 *
 * Initial aim is to write code to
 * a) Read the board file
 * b) Parse the data from the file and store the board in program
 * c) Print the board onto Terminal in nice ASCII Format.
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::istringstream;

// Define State
enum class State {
   kEmpty,
   kObstacle,
};

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
 * returns a board object containing State cells
 */
auto readBoardFile(const string path) {
   ifstream bFile(path);
   vector<vector<State>> board{};

   if (bFile) {
      string line;
      vector<State> row;
      while(getline(bFile, line)) {
         row = parseLine(line);
         board.push_back(row);
      }
   }
   return board;    
}

/*
 * Takes State as input and returns corresponding
 * ASCII String
 */
string cellString(State state) {
   switch(state) {
      case State::kObstacle: return  "⛰️  ";
      default: return "0  ";
   }
}
    
/*
 * Print the board states in ASCII string format
 */
void printBoard(const vector<vector<State>> board) {
   for(auto row: board) {
      for (auto cell: row) {
         cout << cellString(cell);
      }
      cout << endl;
   }
}

int main(int argc, char *argv[]) {
   // Declare an empty board
   vector<vector<State>> board{};
   
   if (argc < 2) {
      cout << "Provide a valid board file path as argument" << endl;
      return 0;
   } 

   // Path string
   string path(argv[1]);
   cout << "Board file path: " << path << endl;

   // Read state data from file and populate board
   board = readBoardFile(path); 

   // Print board data in ASCII format
   printBoard(board); 

   return 0;
}
