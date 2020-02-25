#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

enum {
    kClosed = -1,
    kEmpty,
    kObstacle,
    kPath,
    kStart,
    kFinish
};

enum Parent{
    left = 10,
    right,
    up,
    down
};

// PROTOTYPES FOR FUNCTIONS
vector<vector<int>> ReadBoardFile(string);
vector<int> ParseLine(string);
string CellString(int);
void PrintBoard(vector<vector<int>>&);
vector<vector<int>> Search(vector<vector<int>>&, vector<int>, vector<int>);
void AddToOpen(vector<vector<int>>&, vector<vector<int>>&, int, int, int, int, int);
bool CheckValidCell(vector<vector<int>>& grid, int x, int y);
void CellSort(vector<vector<int>>&);
int Hueristic(int,int,int,int);

bool Compare(vector<int>, vector<int>);
void ExpandNeighbours(vector<vector<int>>&, vector<vector<int>>&, vector<int>, vector<int>);

int main()
{
    vector<vector<int>> board = ReadBoardFile("1.board");
    vector<vector<int>> solution = Search(board, {0,0}, {int(board.size()-1),int(board[0].size()-1)});
    PrintBoard(solution);
    //PrintBoard(board);
    return 0;
}

void PrintBoard(vector<vector<int>>& board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[0].size(); j++)
        {
            cout << CellString(board[i][j]) << "  ";
        }
        cout << std::endl;
    }
}

vector<vector<int>> ReadBoardFile(string fileName)
{
    ifstream input(fileName);
    string line;
    vector<vector<int>> board;
    input >> line;
    while (input)
    {
        board.push_back(ParseLine(line));
        input >> line;  
    }
    return board;
}

vector<int> ParseLine(string line)
{
    istringstream linestream;
    linestream.str(line);
    vector<int> row;
    int n;
    while (linestream >> n)
    {
        row.push_back(n);
        linestream.ignore(1);
    }
    return row;
}

string CellString(int State)
{
    if (State == kObstacle)
        return "⛰️ ";
    if (State == kClosed)
        return "🚗 ";
    if (State == kStart)
        return "🚦 ";
    if (State == kFinish)
        return "🏁 ";
    return "0 ";
}

vector<vector<int>> Search(vector<vector<int>>& grid, vector<int> start, vector<int> end)
{
    vector<vector<int>> open;
    vector<vector<int>> gridLocal = grid;
    int x = 0, y = 0, g = 0, h = Hueristic(start[0], start[1], end[0], end[1]);
    AddToOpen(open, gridLocal, x, y, g, h, -1);

    while (!open.empty())
    {
        CellSort(open);
        vector<int> leastCostNode = open[open.size() - 1];
        open.pop_back();
        //grid[leastCostNode[0]][leastCostNode[1]] = kClosed;
        if (leastCostNode[0] == end[0] && leastCostNode[1] == end[1])
        {
            int i = end[0];
            int j = end[1];
            while (gridLocal[i][j] != -1)
            {
                grid[i][j] = kClosed;
                if (Parent(gridLocal[i][j]) == Parent::left)
                    j--;
                else if (Parent(gridLocal[i][j]) == Parent::right)
                    j++;
                else if (Parent(gridLocal[i][j]) == Parent::up)
                    i--;
                else
                    i++;
            }
            grid[start[0]][start[1]] = int(kStart);
            grid[end[0]][end[1]] = int(kFinish);
            return grid;
        }
        ExpandNeighbours(gridLocal, open, leastCostNode, end);
    }
    cout << "No path found!" << std::endl;
    return vector<vector<int>>{};
}

int Hueristic(int x1, int y1, int x2, int y2)
{
    return abs(x2 - x1) + abs(y2 - y1);
}

void AddToOpen(vector<vector<int>>& nodes, vector<vector<int>>& state, int x, int y, int g, int h, int direction)
{
    nodes.push_back({x, y, g, h});
    state[x][y] = direction;
}

bool Compare(vector<int> a, vector<int> b)
{
    return a[2] + a[3] > b[2] + b[3];
}

void CellSort(vector<vector<int>>& open)
{
    sort(open.begin(), open.end(), Compare);
}

bool CheckValidCell(vector<vector<int>>& grid, int x, int y)
{
    return x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() && grid[x][y] == kEmpty;
}

void ExpandNeighbours(vector<vector<int>>& grid, vector<vector<int>>& open, vector<int> curr, vector<int> goal)
{
    const int deltas[4][2] {{0,1}, {0,-1}, {1,0}, {-1,0}};
    for (int i = 0; i < 4; i++)
    {
        int x = curr[0] + deltas[i][0];
        int y = curr[1] + deltas[i][1];
        if (CheckValidCell(grid, x, y))
        {
            AddToOpen(open, grid, x, y, curr[2]+1, Hueristic(goal[0], goal[1], x, y), i+10);
        }
    }
}