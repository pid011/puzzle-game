#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

using namespace std;

enum class Direction
{
    Up = 72, Down = 80, Left = 75, Right = 77
};

struct Position
{
    short X;
    short Y;
};

const int _width = 4;
const int _height = 4;
const int _blank = -1;
const char* _blankSymbol = "[]";

/// <summary>
/// Puzzle map of the game
/// </summary>
short _puzzleMap[_height][_width];

inline short GetRealPositionX(short x)
{
    return x * 5;
}

inline short GetRealPositionY(short y)
{
    return y * 2;
}

inline short RandPuzzle(short max)
{
    return rand() % max;
}

inline void SetCursorPosition(Position p)
{
    COORD pos = { GetRealPositionX(p.X), GetRealPositionY(p.Y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

/// <summary>
/// Initialize the one-line message output area at the bottom of the game screen.
/// </summary>
void ResetSingleMessage();

/// <summary>
/// Set the puzzle item to a specified location.
/// </summary>
/// <param name="p">Position to set puzzle item</param>
/// <param name="item">Puzzle item to a specified location</param>
void SetPuzzleItem(Position p, int item);

/// <summary>
/// Swap the puzzle item
/// </summary>
void SwapPuzzle(Position a, Position b);

/// <summary>
/// Initialize the map
/// </summary>
void InitializeMap();

/// <summary>
/// Shuffle map item
/// </summary>
void ShuffleMap();

/// <summary>
/// Move the puzzle items
/// </summary>
/// <param name="direction">Arrow keys pressed by the user</param>
void MovePuzzle(Direction direction);
void MovePuzzle(Direction direction, Position puzzleItemPosition);

/// <summary>
/// Check if the puzzle is complete
/// </summary>
/// <returns>If puzzle is completed, return true</returns>
bool IsPuzzleCompleted();

int main()
{
    system("cls");

    InitializeMap();
    for (int i = 3; i > 0; i--)
    {
        ResetSingleMessage();
        cout << i;
        Sleep(1000);
    }
    ShuffleMap();

    cout << "Move the puzzle with the arrow keys" << endl;

    int key = 0;
    while (!IsPuzzleCompleted())
    {
        if (_kbhit())
        {
            key = _getch();
            if (key == 224)
            {
                key = _getch();
                MovePuzzle(static_cast<Direction>(key));
            }
        }
    }
}

void ResetSingleMessage()
{
    Position p = { 0, _height };
    SetCursorPosition(p);
    for (int i = 0; i < 30; i++)
    {
        cout << ' ';
    }
    SetCursorPosition(p);
}

void SetPuzzleItem(Position p, int item)
{
    _puzzleMap[p.Y][p.X] = item;
    SetCursorPosition(p);
    if (item == -1)
    {
        cout << _blankSymbol;
    }
    else
    {
        cout << item << " ";
    }
}

void SwapPuzzle(Position a, Position b)
{
    short numA = _puzzleMap[a.Y][a.X];
    short numB = _puzzleMap[b.Y][b.X];

    SetPuzzleItem(a, numB);
    SetPuzzleItem(b, numA);
}

void InitializeMap()
{
    int num = 1;
    Position p;
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            p.X = j;
            p.Y = i;
            SetPuzzleItem(p, num);
            num++;
        }
    }
    _puzzleMap[_height - 1][_width - 1] = _blank;
    Position p2 = { _height - 1, _width - 1 };
    SetCursorPosition(p2);
    cout << _blankSymbol;
    ResetSingleMessage();
}

void ShuffleMap()
{
    srand((unsigned)time(NULL));

    int shuffleCount = _width * _height * 2;
    Position a, b;

    for (int i = 0; i < shuffleCount; i++)
    {
        a.Y = RandPuzzle(_height), a.X = RandPuzzle(_width);
        do
        {
            b.Y = RandPuzzle(_height), b.X = RandPuzzle(_width);
        } while (a.X == b.X && a.Y == b.Y);

        SwapPuzzle(a, b);
        
        ResetSingleMessage();
        cout << "Shuffle the puzzles..." << shuffleCount - i << endl;
        Sleep(100);
    }
    ResetSingleMessage();
}

void MovePuzzle(Direction direction)
{
    Position blank;
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            if (true)
            {
                // TODO: blank 위치 찾기
            }
        }
    }
    switch (direction)
    {
    case Direction::Up:
        cout << "위" << endl;
        break;
    case Direction::Down:
        cout << "아래" << endl;
        break;
    case Direction::Left:
        cout << "왼쪽" << endl;
        break;
    case Direction::Right:
        cout << "오른쪽" << endl;
        break;
    default:
        break;
    }
}

void MovePuzzle(Direction direction, Position puzzleItemPosition)
{

}

bool IsPuzzleCompleted()
{
    int num = 1;
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            if (i == _height - 1 && j == _width - 1)
            {
                num = _blank;
            }
            if (num != _puzzleMap[i][j])
            {
                return false;
            }
            num++;
        }
    }

    return true;
}
