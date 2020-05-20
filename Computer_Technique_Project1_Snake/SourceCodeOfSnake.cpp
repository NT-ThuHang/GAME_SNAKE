#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <time.h>
#include <thread>
#include <conio.h>
#include <stdio.h>
#include <fstream>
#include <string.h>

using namespace std;

#define MAX_SIZE_SNAKE 10
#define MAX_SIZE_FOOD 4
#define MAX_SPEED 4
#define MIN_SIZE_ROCK 4

struct IP
{
	char name[100];
	int score;
};// info player
POINT snake[24];//con ran
POINT food[4];//thuc an
POINT rock[10] = { 0 };//vat can
POINT gate[6] = { 0 };//cong
IP ip[1000];//mang luu tru thong tin nguoi choi
char nameSnake[] = { '1', '8', '1', '2', '0', '0', '2', '7' };//ten cua ran
int CHAR_LOCK;//huong khong duoc phep di chuyen
int MOVING;//huong di chuyen
int SPEED;//toc do
int HEIGH_CONSOLE, WIDTH_CONSOLE;//dai rong cua man hinh
int FOOD_INDEX;//luong thuc an
int SIZE_SNAKE;//do dai ran
int STATE;// tinh trang song hoac chet cua ran
int temp;//luu tru tam thoi do dai ran
int level = 0;//cap do
int Score = 0;//diem so
int len;//do dai ten nguoi choi
IP player;//nguoi choi hien tai

void highScore();

ostream& operator <<(ostream& outDev, IP& p)
{
	outDev << p.name << endl << p.score << endl;
	return outDev;
}
// co dinh man hinh
void FixConsoleWindow()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX)& ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
	system("mode 120,33");
}
//di chuyen den toa do (x,y)
void GotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
// ham kiem tra xem co bij trung voi toa do cua ran hay khong
bool IsValid(int x, int y)
{
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		if (snake[i].x == x && snake[i].y == y)
			return false;
	}
	return true;
}
//ham tao cong
void GenerateGate()
{
	int x, y;
	srand(time(NULL));
	do
	{
		x = rand() % (WIDTH_CONSOLE - 5) + 3;
		y = rand() % (HEIGH_CONSOLE - 5) + 3;
	} while (IsValid(x, y) == false);
	gate[0] = { x - 1, y + 1 };
	gate[1] = { x - 1, y };
	gate[2] = { x, y };
	gate[3] = { x + 1, y };
	gate[4] = { x + 1, y + 1 };
	gate[5] = { x, y + 1 };
}
//ham ve cong
void drawGate()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 12);
	for (int i = 0; i < 5; i++)
	{
		GotoXY(gate[i].x, gate[i].y);
		cout << 'o';
	}
}
// ham xoa cong
void cleanGate()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 12);
	for (int i = 0; i < 5; i++)
	{
		GotoXY(gate[i].x, gate[i].y);
		cout << " ";
		gate[i].x = 0;
		gate[i].y = 0;
	}
}
// kiem tra xem co trung voi toa do cua cong hay cua ran hay khong
bool IsValidSG(int x, int y)
{
	if (IsValid(x, y))
		return true;
	for (int i = 0; i < 6; i++)
	{
		if (gate[i].x == x && gate[i].y == y)
			return false;
	}
	return true;
}
// tao thuc an
void GenerateFood()
{
	int x, y;
	srand(time(NULL));
	for (int i = 0; i < MAX_SIZE_FOOD; i++)
	{
		do
		{
			x = (rand() % (WIDTH_CONSOLE - 5) + 3);
			y = (rand() % (HEIGH_CONSOLE - 5) + 3);
		} while (IsValidSG(x, y) == false);
		food[i] = { x, y };
	}
}
//ve thuc an
void drawFood(char fo)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 13);
	GotoXY(food[FOOD_INDEX].x, food[FOOD_INDEX].y);
	cout << fo;
}
//kiem tra xem co trung toa do voi ran, cong, thuc an hay khong
bool IsValidSGF(int x, int y)
{
	if (IsValidSG(x, y))
		return true;
	for (int i = 0; i < MAX_SIZE_FOOD; i++)
	{
		if (food[i].x == x && food[i].y == y)
			return false;
	}
	return true;
}
//tao vat can
void GenerateRock()
{
	int x, y;
	srand(time(NULL));
	for (int i = 0; i < MIN_SIZE_ROCK + level; i++)
	{
		do
		{
			x = (rand() % (WIDTH_CONSOLE - 5) + 3);
			y = (rand() % (HEIGH_CONSOLE - 5) + 3);
		} while (IsValidSGF(x, y) == false);
		rock[i] = { x, y };
	}
}
//ve vat can
void drawRock(char str)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 9);
	for (int i = 0; i < MIN_SIZE_ROCK + level; i++)
	{
		GotoXY(rock[i].x, rock[i].y);
		cout << str;
	}
}
// khoi tao du lieu ban dau
void ResetData()
{
	CHAR_LOCK = 'A', MOVING = 'D', SPEED = 1;
	FOOD_INDEX = 0, WIDTH_CONSOLE = 70, HEIGH_CONSOLE = 20, SIZE_SNAKE = 8;
	snake[0] = { 10, 5 };
	snake[1] = { 11, 5 };
	snake[2] = { 12, 5 };
	snake[3] = { 13, 5 };
	snake[4] = { 14, 5 };
	snake[5] = { 15, 5 };
	snake[6] = { 16, 5 };
	snake[7] = { 17, 5 };
	GenerateFood();
}

void cleanSaveGame();
// ve bang chinh(noi nguoi choi dieu khien con ran cua minh san moi
void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 9);
	GotoXY(x, y);
	cout << '+';
	for (int i = 1; i < width; i++)
		cout << '+';
	cout << '+';
	GotoXY(x, height + y);
	cout << '+';
	for (int i = 1; i < width; i++)
		cout << '+';
	cout << '+';
	for (int i = y + 1; i < height + y; i++)
	{
		GotoXY(x, i);
		cout << '+';
		GotoXY(x + width, i);
		cout << '+';
	}
	GotoXY(curPosX, curPosY);
}
//ve bang phu (noi chi dan nguoi choi su dung cac phim dieu huong)
void drawGuideBoard()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 12);
	GotoXY(WIDTH_CONSOLE + 1, 0);
	for (int i = 1; i < 119 - WIDTH_CONSOLE; i++)
		cout << '+';
	GotoXY(WIDTH_CONSOLE + 1, HEIGH_CONSOLE / 2 + 2);
	for (int i = 1; i < 120 - WIDTH_CONSOLE; i++)
		cout << '+';
	for (int i = 0; i < HEIGH_CONSOLE / 2 + 2; i++)
	{
		GotoXY(WIDTH_CONSOLE + 1, i);
		cout << '+';
		GotoXY(119, i);
		cout << '+';
	}
	SetConsoleTextAttribute(consoleHandle, 3);
	GotoXY(WIDTH_CONSOLE + 20, 1);
	cout << "GUIDE";
	SetConsoleTextAttribute(consoleHandle, 7);
	GotoXY(WIDTH_CONSOLE + 15, 3);
	cout << ">> T     (upload)";
	GotoXY(WIDTH_CONSOLE + 15, 4);
	cout << ">> L     (save)";
	GotoXY(WIDTH_CONSOLE + 15, 5);
	cout << ">> P     (pause)";
	GotoXY(WIDTH_CONSOLE + 15, 6);
	cout << ">> Esc   (exit)";
	GotoXY(WIDTH_CONSOLE + 15, 7);
	cout << ">> A     (left)";
	GotoXY(WIDTH_CONSOLE + 15, 8);
	cout << ">> D     (right)";
	GotoXY(WIDTH_CONSOLE + 15, 9);
	cout << ">> S     (down)";
	GotoXY(WIDTH_CONSOLE + 15, 10);
	cout << ">> W     (up)";
	GotoXY(0, 0);
}
//ve bang phu noi cap nhat thong tin cuar nguoi choi
void drawInfoBoard()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 2);
	GotoXY(WIDTH_CONSOLE + 1, HEIGH_CONSOLE / 2 + 3);
	for (int i = 1; i < 119 - WIDTH_CONSOLE; i++)
		cout << '=';
	GotoXY(WIDTH_CONSOLE + 1, HEIGH_CONSOLE);
	for (int i = 1; i < 120 - WIDTH_CONSOLE; i++)
		cout << '=';
	for (int i = HEIGH_CONSOLE / 2 + 4; i < HEIGH_CONSOLE + 1; i++)
	{
		GotoXY(WIDTH_CONSOLE + 1, i);
		cout << '|';
		GotoXY(119, i);
		cout << '|';
	}
	SetConsoleTextAttribute(consoleHandle, 3);
	GotoXY(WIDTH_CONSOLE + 20, HEIGH_CONSOLE / 2 + 4);
	cout << player.name;
	SetConsoleTextAttribute(consoleHandle, 7);
	GotoXY(WIDTH_CONSOLE + 15, HEIGH_CONSOLE / 2 + 6);
	cout << "LEVEL:  " << level + 1;
	GotoXY(WIDTH_CONSOLE + 15, HEIGH_CONSOLE / 2 + 8);
	cout << "SCORE:   " << Score;
	GotoXY(0, 0);
}
// ham bat dau voi mot game moi
void startGame()
{
	system("cls");
	ResetData();
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	drawGuideBoard();
	GotoXY(27, HEIGH_CONSOLE / 2 - 2);
	cout << "|------------------------------|";
	GotoXY(27, HEIGH_CONSOLE / 2 - 1);
	cout << "|            MY SNAKE          |";
	GotoXY(27, HEIGH_CONSOLE / 2);
	cout << "|             Name:            |";
	GotoXY(27, HEIGH_CONSOLE / 2 + 1);
	cout << "|                              |";
	GotoXY(27, HEIGH_CONSOLE / 2 + 2);
	cout << "|------------------------------|";
	GotoXY(39, HEIGH_CONSOLE / 2 + 1);
	gets_s(player.name);
	drawInfoBoard();
	cleanSaveGame();
	STATE = 1;
}
//ham ket thuc game
void ExitGame(HANDLE t)
{
	system("cls");
	exit(0);
}
//ham dung game
void PauseGame(HANDLE t)
{
	SuspendThread(t);
}
// ham an thuc an
void Eat()
{
	//Sleep(100/ SPEED);
	Score++;
	drawInfoBoard();
	snake[SIZE_SNAKE] = food[FOOD_INDEX];//luu y
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)//truong hop da du thuc an 
	{
		SIZE_SNAKE++;
		temp = SIZE_SNAKE;
		GenerateGate();
		drawGate();
	}
	else
	{
		FOOD_INDEX++;
		SIZE_SNAKE++;
	}
}
// xu li khi ran chet
void ProcessDead()
{
	player.score = Score;
	ofstream FileOut("data.txt", ios::app);
	FileOut << player;
	FileOut.close();
	STATE = 0;
	gate[0].x = 0;
	gate[0].y = 0;
	int k = 0;
	while (k < 20)
	{
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(consoleHandle, rand() % 7 + 1);
		int j = 0;
		for (int i = 0; i < SIZE_SNAKE; i++)
		{
			GotoXY(snake[i].x, snake[i].y);
			cout << nameSnake[j++];
			if (j > 7)
				j = 0;
			Sleep(50);
			k++;
		}
	}
	GotoXY(0, HEIGH_CONSOLE + 2);
	cout << "Dead, type Y to continue";
}
// ve ran
void drawSnake(char* sna)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 14);
	for (int i = 0; i < SIZE_SNAKE; i++)
	{
		GotoXY(snake[i].x, snake[i].y);
		cout << sna[i % 8];
	}
}
//re phai
void MoveRight()
{
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (snake[SIZE_SNAKE - 1].x == gate[i].x && snake[SIZE_SNAKE - 1].y == gate[i].y)
			{
				ProcessDead();
			}
		}
	}
	for (int i = 0; i < MIN_SIZE_ROCK + level; i++)
	{
		if (rock[i].x == snake[SIZE_SNAKE - 1].x + 1 && rock[i].y == snake[SIZE_SNAKE - 1].y)
		{
			ProcessDead();
		}
	}
	for (int i = 0; i < SIZE_SNAKE - 2; i++)
	{
		if (snake[i].x == snake[SIZE_SNAKE - 1].x + 1 && snake[i].y == snake[SIZE_SNAKE - 1].y)
		{
			ProcessDead();
		}
	}
	if (snake[SIZE_SNAKE - 1].x + 1 == WIDTH_CONSOLE)
	{
		ProcessDead();
	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x + 1 == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].x++;

	}
}
//re trai
void MoveLeft()
{
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (snake[SIZE_SNAKE - 1].x == gate[i].x && snake[SIZE_SNAKE - 1].y == gate[i].y)
			{
				ProcessDead();
			}
		}
	}
	for (int i = 0; i < MIN_SIZE_ROCK + level; i++)
	{
		if (rock[i].x == snake[SIZE_SNAKE - 1].x - 1 && rock[i].y == snake[SIZE_SNAKE - 1].y)
		{
			ProcessDead();
		}
	}
	for (int i = 0; i < SIZE_SNAKE - 2; i++)
	{
		if (snake[i].x == snake[SIZE_SNAKE - 1].x - 1 && snake[i].y == snake[SIZE_SNAKE - 1].y)
		{
			ProcessDead();
		}
	}
	if (snake[SIZE_SNAKE - 1].x - 1 == 0)
	{
		ProcessDead();
	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x - 1 == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].x--;
	}
}
//di xuong
void MoveUp()
{
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (snake[SIZE_SNAKE - 1].x == gate[i].x && snake[SIZE_SNAKE - 1].y == gate[i].y)
			{
				ProcessDead();
			}
		}
		if (snake[SIZE_SNAKE - 1].x == gate[5].x && snake[SIZE_SNAKE - 1].y == gate[5].y)
			SIZE_SNAKE--;
		if (SIZE_SNAKE == 0) {
			SPEED++;
			level++;
			if (level > 0)
			{
				drawRock(32);
			}
			drawInfoBoard();
			if (SPEED == MAX_SPEED)
			{
				cleanGate();
				ResetData();
			}
			else {
				FOOD_INDEX = 0;
				SIZE_SNAKE = temp;
				cleanGate();
				GenerateRock();
				drawRock('#');
			}
		}
	}
	for (int i = 0; i < MIN_SIZE_ROCK + level; i++)
	{
		if (rock[i].x == snake[SIZE_SNAKE - 1].x && rock[i].y == snake[SIZE_SNAKE - 1].y - 1)
		{
			ProcessDead();
		}
	}
	for (int i = 0; i < SIZE_SNAKE - 2; i++)
	{
		if (snake[i].x == snake[SIZE_SNAKE - 1].x && snake[i].y == snake[SIZE_SNAKE - 1].y - 1)
		{
			ProcessDead();
			break;
		}
	}
	if (snake[SIZE_SNAKE - 1].y - 1 == 0)
	{
		ProcessDead();
	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y - 1 == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].y--;
	}

}
//dilen
void MoveDown()
{
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (snake[SIZE_SNAKE - 1].x == gate[i].x && snake[SIZE_SNAKE - 1].y == gate[i].y)
			{
				ProcessDead();
			}
		}
	}
	for (int i = 0; i < MIN_SIZE_ROCK + level; i++)
	{
		if (rock[i].x == snake[SIZE_SNAKE - 1].x && rock[i].y == snake[SIZE_SNAKE - 1].y + 1)
		{
			ProcessDead();
		}
	}
	for (int i = 0; i < SIZE_SNAKE - 2; i++)
	{
		if (snake[i].x == snake[SIZE_SNAKE - 1].x && snake[i].y == snake[SIZE_SNAKE - 1].y + 1)
		{
			ProcessDead();
		}
	}
	if (snake[SIZE_SNAKE - 1].y + 1 == HEIGH_CONSOLE)
	{
		ProcessDead();
	}
	else
	{
		if (snake[SIZE_SNAKE - 1].x == food[FOOD_INDEX].x && snake[SIZE_SNAKE - 1].y + 1 == food[FOOD_INDEX].y)
		{
			Eat();
		}
		for (int i = 0; i < SIZE_SNAKE - 1; i++)
		{
			snake[i].x = snake[i + 1].x;
			snake[i].y = snake[i + 1].y;
		}
		snake[SIZE_SNAKE - 1].y++;
	}
}
//luu la game
void saveGame()
{
	char fileName[100];
	GotoXY(27, HEIGH_CONSOLE / 2 - 2);
	cout << "|------------------------------|";
	GotoXY(27, HEIGH_CONSOLE / 2 - 1);
	cout << "|           Save Game          |";
	GotoXY(27, HEIGH_CONSOLE / 2);
	cout << "|             Name:            |";
	GotoXY(27, HEIGH_CONSOLE / 2 + 1);
	cout << "|                              |";
	GotoXY(27, HEIGH_CONSOLE / 2 + 2);
	cout << "|------------------------------|";
	GotoXY(39, HEIGH_CONSOLE / 2 + 1);
	gets_s(fileName);
	FILE * f = fopen(fileName, "wb");
	fwrite(&SIZE_SNAKE, 1, sizeof(int), f);
	fwrite(&snake, SIZE_SNAKE, sizeof(POINT), f);
	fwrite(&FOOD_INDEX, 1, sizeof(int), f);
	fwrite(&food, MAX_SIZE_FOOD, sizeof(POINT), f);
	fwrite(&level, 1, sizeof(int), f);
	fwrite(&rock, MIN_SIZE_ROCK + level, sizeof(POINT), f);
	fwrite(&Score, 1, sizeof(int), f);
	fwrite(&gate, 6, sizeof(POINT), f);
	fwrite(&SPEED, 1, sizeof(int), f);
	fwrite(&CHAR_LOCK, 1, sizeof(char), f);
	fwrite(&MOVING, 1, sizeof(char), f);
	len = strlen(player.name);
	fwrite(&len, 1, sizeof(int), f);
	fwrite(&player.name, len, 100, f);
	fclose(f);
	GotoXY(0, 0);
}
// xoa bang thong bao luu lai game
void cleanSaveGame()
{
	GotoXY(27, HEIGH_CONSOLE / 2 - 2);
	cout << "                                 ";
	GotoXY(27, HEIGH_CONSOLE / 2 - 1);
	cout << "                                 ";
	GotoXY(27, HEIGH_CONSOLE / 2);
	cout << "                                 ";
	GotoXY(27, HEIGH_CONSOLE / 2 + 1);
	cout << "                                 ";
	GotoXY(27, HEIGH_CONSOLE / 2 + 2);
	cout << "                                 ";
}
//load game
void loadGame()
{
	system("cls");
	ResetData();
	DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE);
	drawGuideBoard();
	char fileName[100];
	GotoXY(27, HEIGH_CONSOLE / 2 - 2);
	cout << "|------------------------------|";
	GotoXY(27, HEIGH_CONSOLE / 2 - 1);
	cout << "|           Load Game          |";
	GotoXY(27, HEIGH_CONSOLE / 2);
	cout << "|             Name:            |";
	GotoXY(27, HEIGH_CONSOLE / 2 + 1);
	cout << "|                              |";
	GotoXY(27, HEIGH_CONSOLE / 2 + 2);
	cout << "|------------------------------|";
	GotoXY(39, HEIGH_CONSOLE / 2 + 1);
	gets_s(fileName);
	FILE * f = fopen(fileName, "rb");
	fread(&SIZE_SNAKE, 1, sizeof(int), f);
	fread(&snake, SIZE_SNAKE, sizeof(POINT), f);
	fread(&FOOD_INDEX, 1, sizeof(int), f);
	fread(&food, MAX_SIZE_FOOD, sizeof(POINT), f);
	fread(&level, 1, sizeof(int), f);
	fread(&rock, MIN_SIZE_ROCK + level, sizeof(POINT), f);
	fread(&Score, 1, sizeof(int), f);
	fread(&gate, 6, sizeof(POINT), f);
	fread(&SPEED, 1, sizeof(int), f);
	fread(&CHAR_LOCK, 1, sizeof(char), f);
	fread(&MOVING, 1, sizeof(char), f);
	fread(&len, 1, sizeof(int), f);
	fread(&player.name, len, 100, f);
	fclose(f);
	drawInfoBoard();
	STATE = 1;
	if (level > 0)
	{
		drawRock('#');
	}
	if (FOOD_INDEX == MAX_SIZE_FOOD - 1)
	{
		drawGate();
		temp = SIZE_SNAKE;
	}
}
//ham chajy tieu trinh
void ThreadFunc()
{
	while (1)
	{
		if (STATE == 1)
		{
			char str[19];
			for (int i = 0; i < 19; i++)
				str[i] = 32;
			drawSnake(str);
			if (gate[0].x == 0 && gate[0].y == 0)
			{
				drawFood('O');
			}
			switch (MOVING)
			{
			case 'A':
				MoveLeft();
				break;
			case 'D':
				MoveRight();
				break;
			case 'S':
				MoveDown();
				break;
			case 'W':
				MoveUp();
				break;
			}

			drawSnake(nameSnake);
			Sleep(200 / SPEED);
		}
	}
}
//ham xoa chuot
void NoCursorType()
{
	CONSOLE_CURSOR_INFO info;
	info.bVisible = FALSE;
	info.dwSize = 20;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
//ham ve chu
void DrawingText(char Text, int x, int y)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (Text == 65 || Text == 97)
	{
		SetConsoleTextAttribute(consoleHandle, 8);
		GotoXY(x, y);     cout << "   ## ";      Sleep(20);
		GotoXY(x, y + 1); cout << " ##  ##";     Sleep(20);
		GotoXY(x, y + 2); cout << " ##  ##";     Sleep(20);
		GotoXY(x, y + 3); cout << " ######";     Sleep(20);
		GotoXY(x, y + 4); cout << " ##  ##";
	}

	else if (Text == 69 || Text == 101)
	{
		SetConsoleTextAttribute(consoleHandle, 9);
		GotoXY(x, y);     cout << " ######";     Sleep(20);
		GotoXY(x, y + 1); cout << " ##    ";     Sleep(20);
		GotoXY(x, y + 2); cout << " ######";     Sleep(20);
		GotoXY(x, y + 3); cout << " ##    ";     Sleep(20);
		GotoXY(x, y + 4); cout << " ######";
	}

	else if (Text == 75 || Text == 107)
	{
		SetConsoleTextAttribute(consoleHandle, 10);
		GotoXY(x, y);     cout << "##    ##";   Sleep(20);
		GotoXY(x, y + 1); cout << "##   ## ";   Sleep(20);
		GotoXY(x, y + 2); cout << "## # #  ";   Sleep(20);
		GotoXY(x, y + 3); cout << "##  ##  ";   Sleep(20);
		GotoXY(x, y + 4); cout << "##    ##";
	}

	else if (Text == 78 || Text == 110)
	{
		SetConsoleTextAttribute(consoleHandle, 11);
		GotoXY(x, y);     cout << "##     ##";  Sleep(20);
		GotoXY(x, y + 1); cout << "## #   ##";  Sleep(20);
		GotoXY(x, y + 2); cout << "##  #  ##";  Sleep(20);
		GotoXY(x, y + 3); cout << "##   # ##";  Sleep(20);
		GotoXY(x, y + 4); cout << "##     ##";
	}

	else if (Text == 83 || Text == 115)
	{
		SetConsoleTextAttribute(consoleHandle, 12);
		GotoXY(x, y);     cout << " ######";     Sleep(20);
		GotoXY(x, y + 1); cout << " ##    ";     Sleep(20);
		GotoXY(x, y + 2); cout << " ######";     Sleep(20);
		GotoXY(x, y + 3); cout << "     ##";     Sleep(20);
		GotoXY(x, y + 4); cout << " ######";
	}
	else if (Text == 77 || Text == 109)
	{
		SetConsoleTextAttribute(consoleHandle, 14);
		GotoXY(x, y);     cout << " ###    ###";     Sleep(20);
		GotoXY(x, y + 1); cout << " ## #  # ##";     Sleep(20);
		GotoXY(x, y + 2); cout << " ##  ##  ##";     Sleep(20);
		GotoXY(x, y + 3); cout << " ##      ##";     Sleep(20);
		GotoXY(x, y + 4); cout << " ##      ##";
	}
	else if (Text == 85 || Text == 117)
	{
		SetConsoleTextAttribute(consoleHandle, 13);
		GotoXY(x, y);     cout << " ##    ##";     Sleep(20);
		GotoXY(x, y + 1); cout << " ##    ##";     Sleep(20);
		GotoXY(x, y + 2); cout << " ##    ##";     Sleep(20);
		GotoXY(x, y + 3); cout << " ##    ##";     Sleep(20);
		GotoXY(x, y + 4); cout << " ########";
	}
}
//ham ve MENU
void drawMenu()
{
	DrawingText('S', 35, 2);
	DrawingText('N', 44, 2);
	DrawingText('A', 54, 2);
	DrawingText('K', 63, 2);
	DrawingText('E', 73, 2);
	DrawingText('M', 40, 9);
	DrawingText('E', 52, 9);
	DrawingText('N', 61, 9);
	DrawingText('U', 71, 9);
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 10);
	GotoXY(55, 17); cout << "1: PLAY";
	GotoXY(55, 19); cout << "2: LOAD GAME";
	GotoXY(55, 21); cout << "3: HIGH SCORE";
	GotoXY(55, 23); cout << "4: EXIT";
}
// ham trich va sap xep du lieu
void highScore()
{
	int n = 0;
	FILE* f1 = fopen("data.txt", "r");
	while (!feof(f1))
	{
		fgets(ip[n].name, 100, f1);
		fscanf(f1, "%d\n", &ip[n].score);
		//ip[n].id = n;
		n++;
	}
	fclose(f1);
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			if (ip[i].score < ip[j].score)
			{
				IP t;
				t = ip[i];
				ip[i] = ip[j];
				ip[j] = t;
			}
		}
	}
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, 10);
	GotoXY(55, 3); cout << "HIGH SCORE";
	GotoXY(50, 4); cout << "type P to back the MENU";
	GotoXY(50, 5); cout << "type Esc to exit";
	int y = 8;
	SetConsoleTextAttribute(consoleHandle, 8);
	if (n > 20)
		n = 20;
	for (int i = 0; i < n; i++)
	{
		GotoXY(30, y + i);
		cout << ip[i].name;
		GotoXY(80, y + i);
		cout << ip[i].score;
	}
	int k;
	k = toupper(_getch());
	if (k == 'P')
	{
		system("cls");
		drawMenu();
	}
	else
	{
		if (k == 27)
		{
			system("cls");
			exit(0);
		}
	}
}

void main()
{
	FixConsoleWindow();
	NoCursorType();
	drawMenu();
	int temp;
	temp = toupper(_getch());
	system("cls");
	if (temp == '3')
	{
		system("cls");
		highScore();
		temp = toupper(_getch());
	}
	if (temp == '4')
	{
		exit(0);
	}
	if (temp == '2')
	{
		loadGame();
		Sleep(500);
		cleanSaveGame();
	}
	else
	{
		startGame();
	}

	thread t1(ThreadFunc);
	HANDLE handle_t1 = t1.native_handle();
	while (1)
	{
		temp = toupper(_getch());
		if (STATE == 1)
		{
			if (temp == 'P')
			{
				PauseGame(handle_t1);
				continue;
			}
			else
			{
				if (temp == 27)
				{
					ExitGame(handle_t1);
					return;
				}
				else
				{
					if (temp == 'L')
					{
						PauseGame(handle_t1);
						saveGame();
						Sleep(500);
						cleanSaveGame();
						ResumeThread(handle_t1);
					}
					else
					{
						if (temp == 'T')
						{
							PauseGame(handle_t1);
							loadGame();
							Sleep(500);
							cleanSaveGame();
							ResumeThread(handle_t1);
						}
						else
						{
							ResumeThread(handle_t1);
							if ((temp != CHAR_LOCK) && (temp == 'D' || temp == 'A' || temp == 'W' || temp == 'S'))
							{
								if (temp == 'D')
									CHAR_LOCK = 'A';
								else
								if (temp == 'W')
									CHAR_LOCK = 'S';
								else
								if (temp == 'S')
									CHAR_LOCK = 'W';
								else
									CHAR_LOCK = 'D';
								MOVING = temp;
							}
						}
					}
				}
			}
		}
		else
		{
			if (temp == 'Y')
				startGame();
			else
			{
				system("cls");
				TerminateThread(handle_t1, NULL);
				highScore();
				ExitGame(handle_t1);
				return;
			}
		}

	}
}


