#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <ctime>
#include <iomanip>
#include <string>
#include <iostream>
#include <cmath>

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
    int moveCount{};
    double playtime{};
};

const int _width = 4;
const int _height = 4;
const int _blank = -1;
const string _blankSymbol = "[]";
const int _rankCount = 10;
const int _nameLengthLimit = 15;

/// <summary>
/// Puzzle map of the game
/// </summary>
short _puzzleMap[_height][_width];

ScoreInfo _rank[_rankCount];

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

/// <summary>
/// Return time text
/// </summary>
/// <param name="second"></param>
/// <returns></returns>
string GenerateTimeIntervalStringToMillisecond(double ms)
{
    int hour = 0, min = 0, sec = 0;
    while ((ms / 3600) >= 1)
    {
        hour++;
        ms -= 3600;
    }
    while (ms / 60 >= 1)
    {
        min++;
        ms -= 60;
    }
    while (ms / 1 >= 1)
    {
        sec++;
        ms -= 1;
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
    if (sec > 0)
    {
        msg += to_string(sec) + "s ";
    }

    msg += to_string(ms).substr(2, 3) + "ms";

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
    printf("%s", s.c_str());
    MoveCursor(p);
}

/// <summary>
/// Print the one-line message
/// </summary>
/// <param name="message"></param>
void PrintSingleMessage(string message)
{
    ResetSingleMessage();
    printf(message.c_str());
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
        printf(_blankSymbol.c_str());
    }
    else
    {
        printf("%d ", item);
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
    printf(_blankSymbol.c_str());
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

void ShuffleMapTest()
{
    srand((unsigned)time(NULL));
    SwapPuzzle(Position{ 3, 3 }, Position{ 2, 3 });
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

void AddAndPrintRankDialog(int moveCount, double duration)
{
    ScoreInfo newScoreInfo = { "", moveCount, duration };
    printf("Enter your name: ");
    string name;
    getline(cin, name);
    printf("\n");
    newScoreInfo.name = name;

    system("cls");

    printf(":: RANK ::\n");
    printf("----------------------------------------\n");
    bool added = false;
    int i = 0, j = 0;
    while (i < _rankCount)
    {
        if ((!added) && (_rank[j].name == "" || newScoreInfo.playtime < _rank[j].playtime))
        {
            added = true;
            ScoreInfo p1 = _rank[j];
            ScoreInfo p2;
            _rank[j] = newScoreInfo;
            for (int k = j + 1; k < _rankCount; k++)
            {
                p2 = _rank[k];
                _rank[k] = p1;
                p1 = p2;
            }
        }
        else
        {
            j++;
        }
        string timeText = GenerateTimeIntervalStringToMillisecond(_rank[i].playtime).c_str();
        printf(" #%-3d: %-16s : %s\n",
            i + 1,
            (_rank[i].name == newScoreInfo.name ? ("*" + _rank[i].name).c_str() : _rank[i].name.c_str()),
            timeText == "000ms" ? "---ms" : timeText.c_str());
        i++;
    }
    printf("----------------------------------------\n");
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
    //ShuffleMapTest();
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
    double duration = ((double)t2 - t1) / CLOCKS_PER_SEC;

    ResetSingleMessage();
    printf("Done!\n\n");
    printf("--------------------\n");
    printf("move count: %d\n", count);
    printf("play time : %s\n", GenerateTimeIntervalStringToMillisecond(duration).c_str());
    printf("--------------------\n");

    AddAndPrintRankDialog(count, duration);
}

int main()
{
    while (true)
    {
        PlayPuzzleGame();

        string answer;
        while (answer != "y" && answer != "n")
        {
            printf("Do you want to restart the game? (y/n)\n> ");
            getline(cin, answer);
        }
        if (answer == "y")
        {
            continue;
        }
        else
        {
            printf("Bye!\n");
            break;
        }
    }
    return 0;
}
