#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <ctime>
#include <iomanip>
#include <string>
#include <iostream>

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

struct ScoreInfo
{
    string name;
    int moveCount;
    double duration;
};

const int _width = 4;
const int _height = 4;
const int _blank = -1;
const string _blankSymbol = "[]";

/// <summary>
/// Puzzle map of the game
/// </summary>
short _puzzleMap[_height][_width];

inline short GetRealMapPositionX(short x)
{
    return x * 5;
}

inline short GetRealMapPositionY(short y)
{
    return y * 2;
}

inline short RandPuzzle(short max)
{
    return rand() % max;
}

inline void MoveCursorToMapPosition(Position puzzlePosition)
{
    COORD pos = { GetRealMapPositionX(puzzlePosition.X), GetRealMapPositionY(puzzlePosition.Y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

inline void MoveCursor(Position p)
{
    COORD pos = { p.X, p.Y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

/// <summary>
/// Use this function to prepare for forced termination of a program.
/// </summary>
inline void MoveCursorToGameBottom()
{
    COORD pos = { 0, GetRealMapPositionY(_height) + 1 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

string GenerateTimeIntervalStringToSecond(double second)
{
    int hour = 0, min = 0;
    while ((second / 3600) >= 1)
    {
        hour++;
        second -= 3600;
    }
    while (second / 60 >= 1)
    {
        min++;
        second -= 60;
    }

    string msg = "";
    if (hour > 0)
    {
        msg += to_string(hour) + "h ";
    }
    if (min > 0)
    {
        msg += to_string(min) + "m ";
    }
    msg += to_string((int)second) + "s";

    return msg;
}

/// <summary>
/// Reset the one-line message output area at the bottom of the game screen.
/// </summary>
void ResetSingleMessage()
{
    Position p = { 0, GetRealMapPositionY(_height) };
    MoveCursor(p);
    string s = "";
    s.append(100, ' ');
    cout << s;
    MoveCursor(p);
}

/// <summary>
/// Print the one-line message
/// </summary>
/// <param name="message"></param>
void PrintSingleMessage(string message)
{
    ResetSingleMessage();
    cout << message;
    MoveCursorToGameBottom();
}

/// <summary>
/// Set the puzzle item to a specified location.
/// </summary>
/// <param name="p">Position to set puzzle item</param>
/// <param name="item">Puzzle item to a specified location</param>
void SetPuzzleItem(Position p, int item)
{
    _puzzleMap[p.Y][p.X] = item;
    MoveCursorToMapPosition(p);
    if (item == -1)
    {
        cout << _blankSymbol;
    }
    else
    {
        cout << item << " ";
    }
}

/// <summary>
/// Swap the puzzle item
/// </summary>
void SwapPuzzle(Position a, Position b)
{
    short numA = _puzzleMap[a.Y][a.X];
    short numB = _puzzleMap[b.Y][b.X];

    SetPuzzleItem(a, numB);
    SetPuzzleItem(b, numA);
}

/// <summary>
/// Initialize the map
/// </summary>
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
    MoveCursorToMapPosition(p2);
    cout << _blankSymbol;
    ResetSingleMessage();
}

/// <summary>
/// Shuffle map item
/// </summary>
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
        PrintSingleMessage("Shuffle the puzzles..." + to_string(shuffleCount - i));
        Sleep(100);
    }
    ResetSingleMessage();
}

/// <summary>
/// Check if the puzzle is complete
/// </summary>
/// <returns>If puzzle is completed, return true</returns>
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

Position FindBlankPosition()
{
    Position blankPosition{};
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            if (_puzzleMap[i][j] == _blank)
            {
                blankPosition.X = j;
                blankPosition.Y = i;
            }
        }
    }

    return blankPosition;
}

/// <summary>
/// Move the puzzle items
/// </summary>
/// <param name="direction">Arrow keys pressed by the user</param>
/// <returns>Current puzzle move count</returns>
int MovePuzzle(Direction direction)
{
    static int moveCount = 0;

    Position blank = FindBlankPosition();
    Position target = blank;

    bool canMovePuzzle = false;

    switch (direction)
    {
    case Direction::Up:
        target.Y--;
        if (target.Y > -1)
        {
            canMovePuzzle = true;
        }
        break;

    case Direction::Down:
        target.Y++;
        if (target.Y < _height)
        {
            canMovePuzzle = true;
        }
        break;

    case Direction::Left:
        target.X--;
        if (target.X > -1)
        {
            canMovePuzzle = true;
        }
        break;

    case Direction::Right:
        target.X++;
        if (target.X < _width)
        {
            canMovePuzzle = true;
        }
        break;

    default:
        break;
    }

    if (canMovePuzzle)
    {
        SwapPuzzle(blank, target);
        moveCount++;
    }
    return moveCount;
}

void PlayPuzzleGame()
{
    system("cls");

    InitializeMap();
    for (int i = 3; i > 0; i--)
    {
        PrintSingleMessage(to_string(i));
        Sleep(1000);
    }
    ShuffleMap();

    int count = 0;
    string moveCountMessage = "Move count: ";
    PrintSingleMessage(moveCountMessage + to_string(count));

    int key = 0;
    clock_t t1 = clock();
    while (!IsPuzzleCompleted())
    {
        if (_kbhit())
        {
            key = _getch();
            if (key == 224)
            {
                key = _getch();
                count = MovePuzzle(static_cast<Direction>(key));
                PrintSingleMessage(moveCountMessage + to_string(count));
            }
        }
    }
    clock_t t2 = clock();
    double duration = (t2 - t1) / CLOCKS_PER_SEC;

    ResetSingleMessage();
    cout << "Done!" << endl;
    string line = "";
    line.append(10, '-');
    cout << line << endl;
    cout << "move count: " << count << endl;
    cout << "play time : " << GenerateTimeIntervalStringToSecond(duration) << endl;
    cout << line << endl;

}

int main()
{
    PlayPuzzleGame();
    return 0;
}
