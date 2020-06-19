#include <iostream>
#include <cstdio>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <thread>
//#define ran(x) (rand()%x)
using namespace std;

bool gameOver = false;

int ran(int x, int y){
	int a;
	if (y<x){
		int t;
		t = x;
		x = y;
		y = t;
	}
	a = rand() % (y - x + 1) + x;
	if(a>0)return a;
	else return -a;
}

void getKeyboardHit(int* n,bool* gameover){
	char c;
	int d;
	for (;;){
		if (*gameover)return;
		if (_kbhit()){
			c = _getch();
			switch (c)
			{
			case 'w':d = 1; break;
			case 's':d = 2; break;
			case 'a':d = 3; break;
			case 'd':d = 4; break;
			default:
				break;
			}
			if ((d == 1 && *n == 2) || (d == 2 && *n == 1));
			else if ((d == 3 && *n == 4) || (d == 4 && *n == 3));
			else *n = d;
		}
	}
	
}

class MAP{
public:
	int x;//地图行数
	int y;//地图列数
	vector<vector<int> > numMap;//存储地图的二维数组
	MAP(int width, int height);//构造函数
	void creatMap(char c);
private:
	int x1, y1, x2, y2, box_num;
	bool getPos(int* x_out, int* y_out, int left_x, int right_x, int up_y, int down_y);//按要求生成一个随机点
	void makeBox();//生成一个方块
	void printMap();//在屏幕上输出MAP的图形
};

//MAP的构造函数
MAP::MAP(int height, int width){
	x = height;
	y = width;
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;
	box_num = 2;
	vector<vector<int> > temp_map(x);//临时地图二位数组
	//初始化数组，填充0
	for (int i = 0; i < x; i++){
		for (int j = 0; j < y; j++){
			temp_map[i].push_back(0);
		}
	}
	for (int j = 0; j < width; j++)//画上下边框
	{
		temp_map[0][j] = 1;
		temp_map[height - 1][j] = 1;
	}
	for (int i = 0; i<height; i++)//画左右边框
	{
		temp_map[i][0] = 1;
		temp_map[i][width - 1] = 1;
	}
	//将临时数组转移到实例数组中
	numMap.swap(temp_map);
}

//打印MAP
void MAP::printMap(){
	system("cls");
	for(int i = 0;i<x;i++){
		for(int j = 0; j < y;j++){
			if(numMap[i][j]<=0)cout<<" ";
			else cout<<"#";
		}
		cout<<endl;
	}
}

//随机生成一个方块
void MAP::makeBox(){
	srand((int)time(0));
	bool TooClose = true;
	//取第二个点失败时重新取第一个点（点1 位置太偏僻）
	while (TooClose){
		TooClose = false;
		do{
			getPos(&x1, &y1, 3, x - 4, 3, y - 4);
		} while (!getPos(&x2, &y2, x1 + 3, x1 + 10, y1 + 3, y1 + 10));
		for (int i = x1 - 3; i < x2 + 3; i++){
			for (int j = y1 - 3; j < y2 + 3; j++){
				if (numMap[i][j] > 0)TooClose = true;
			}
		}
	}
	for (int i = x1; i <= x2; i++){
		for (int j = y1; j <= y2; j++){
			numMap[i][j] = box_num;
		}
	}
	box_num++;
}
//在[left_x , right_x],[up_y , down_y] 的区间内生成坐标点(x_out , y_out)
bool MAP::getPos(int* x_out, int* y_out, int left_x, int right_x, int up_y, int down_y){
	if (right_x > x || down_y > y)return false;
	bool AllZero = false;
	int temp_x, temp_y;
	do{
		AllZero = true;
		temp_x = ran(left_x, right_x);
		temp_y = ran(up_y, down_y);
		for (int i = 0; i < 5; i++){
			for (int j = 0; j < 5; j++){
				if (numMap[temp_x - 2 + i][temp_y - 2 + j] > 0)AllZero = false;
			}
		}
	} while (!AllZero);
	*x_out = temp_x;
	*y_out = temp_y;
	return true;
}
//生成最终随机地图
void MAP::creatMap(char c){
	int NumOfBox;
	if (c == 'a')NumOfBox = 1;
	else if (c == 'b')NumOfBox = 3;
	for (int i = 0; i < NumOfBox; i++){
		makeBox();
	}
	system("cls");
	printMap();
}

class Snake{
public:
	//二维数组表示蛇 body[0]为头的坐标，(body[i][0],body[i][1])为第i段身体的坐标
	vector<vector<int> > body;
	vector<int> food;
	int direction;
	Snake(MAP* m);
	void printSnake();
	void addSnakeToMap(MAP* m);
	void moveSnake(MAP* m);
	void makeFood(MAP* m);
	
};
//构造函数
Snake::Snake(MAP* m){
	bool AllZero = false;
	int temp_x, temp_y;
	vector<vector<int> > temp_body(3);
	do{
		AllZero = true;
		temp_x = ran(3, m->x - 4);
		temp_y = ran(3, m->y - 4);
		for (int i = 0; i < 5; i++){
			for (int j = 0; j < 5; j++){
				if (m->numMap[temp_x - 2 + i][temp_y - 2 + j] > 0)AllZero = false;
			}
		}
	} while (!AllZero);
	//初始化蛇头和第一节蛇身和蛇尾
	temp_body[0].push_back(temp_x);
	temp_body[0].push_back(temp_y);
	temp_body[1].push_back(temp_x - 1);
	temp_body[1].push_back(temp_y);
	temp_body[2].push_back(temp_x - 2);
	temp_body[2].push_back(temp_y);
	body.swap(temp_body);
	//1上，2下，3左，4右
	direction = 2;
	//随机生成一个食物
	do{
		AllZero = true;
		temp_x = ran(3, m->x - 4);
		temp_y = ran(3, m->y - 4);
		for (int i = 0; i < 5; i++){
			for (int j = 0; j < 5; j++){
				if (m->numMap[temp_x - 2 + i][temp_y - 2 + j] != 0)AllZero = false;
			}
		}
	} while (!AllZero);
	food.push_back(temp_x);
	food.push_back(temp_y);
}
//将蛇的身体信息传递到地图数组中
void Snake::addSnakeToMap(MAP* m){
	//-1为果实，-2为蛇头，-3为蛇身，0为蛇尾
	for (unsigned i = 0; i < body.size() - 1; i++){
		if (i == 0)m->numMap[body[0][0]][body[0][1]] = -2;
		else m->numMap[body[i][0]][body[i][1]] = -3;
	}
	m->numMap[food[0]][food[1]] = -1;
	m->numMap[body[body.size() - 1][0]][body[body.size() - 1][1]] = 0;
}
//蛇的下一个位置
void Snake::moveSnake(MAP* m){
	vector<int> next_pos(2);
	switch (direction){
	case 1:next_pos[0] = body[0][0] - 1; next_pos[1] = body[0][1]; break;//向上
	case 2:next_pos[0] = body[0][0] + 1; next_pos[1] = body[0][1]; break;//向下
	case 3:next_pos[0] = body[0][0]; next_pos[1] = body[0][1] - 1; break;//向左
	case 4:next_pos[0] = body[0][0]; next_pos[1] = body[0][1] + 1; break;//向右
	}
	if (m->numMap[next_pos[0]][next_pos[1]] == 0)//头撞空气，身体坐标传递更新
	{
		for (int i = body.size() - 1; i > 0; i--)body[i] = body[i - 1];
		body[0] = next_pos;
	}
	else if (m->numMap[next_pos[0]][next_pos[1]] == -3 || m->numMap[next_pos[0]][next_pos[1]] > 0)
		gameOver = true;//头撞上身体或墙，游戏结束
	else if (m->numMap[next_pos[0]][next_pos[1]] == -1)//头撞食物，尾部身体+1，移动一格
	{
		vector<int> temp_tail(2);
		temp_tail = body[body.size() - 1];
		for (int i = body.size() - 1; i > 0; i--)body[i] = body[i - 1];
		body[0] = next_pos;
		body.push_back(temp_tail);
		makeFood(m);
	}
}
//生成食物
void Snake::makeFood(MAP* m){
	int temp_x,temp_y;
	do{
		temp_x = ran(3, m->x - 4);
		temp_y = ran(3, m->y - 4);
	} while (m->numMap[temp_x][temp_y] != 0);
	food[0]=(temp_x);
	food[1]=(temp_y);
}

//局部更新，打印蛇身
void Snake::printSnake(){
	//隐藏光标
	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	cursor_info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	COORD pos;
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.Y = food[0];
	pos.X = food[1];
	SetConsoleCursorPosition(hOutput, pos);
	cout << "*";
	pos.Y = body[0][0];
	pos.X = body[0][1];
	SetConsoleCursorPosition(hOutput,pos);
	cout<<"@";
	for(unsigned i = 1 ;i < body.size() - 1;i++){
		pos.Y = body[i][0];
		pos.X = body[i][1];
		SetConsoleCursorPosition(hOutput,pos);
		cout<<"0";
	}
	pos.Y = body[body.size()-1][0];
	pos.X = body[body.size()-1][1];
	SetConsoleCursorPosition(hOutput,pos);
	cout<<" ";
	pos.Y = 20;
	pos.X = 90;
	SetConsoleCursorPosition(hOutput,pos);
	cout << "得分：" << body.size() - 3;
}
//游戏循环
int gameLoop(char lever){
	MAP M(40, 80);
	M.creatMap(lever);
	Snake s(&M);
	s.direction = 2;
	s.printSnake();
	thread th_getInput = thread(getKeyboardHit, &s.direction, &gameOver);
	bool fastest = false;
	do{
		s.moveSnake(&M);
		s.addSnakeToMap(&M);
		s.printSnake();
		if (fastest)Sleep(50);
		else{
			Sleep(300 - 10 * (s.body.size() - 3));
			if ((300 - 10 * (s.body.size() - 3)) < 50)fastest = true;
		}
	} while (!gameOver);
	th_getInput.join();
	return s.body.size() - 3;
}
//游戏开始
char gameStart(){
	char s;
	cout << "欢迎来到贪吃蛇游戏!" << endl;
	cout << "你是新手还是高手呢？\n新手请输入：a\n高手请输入：b" << endl;
	cin >> s;
	while (!(s == 'a' || s == 'b'))
	{
		cout << "乖，别闹，不会用键盘可以复制粘贴^v^" << endl;
		cout << "来，再输入一次：";
		cin >> s;
		cout << s;
	}
	cout<<"好哦，游戏3秒后自动开始，做好准备哈！"<<endl;
	Sleep(1000);
	cout << "3..." << endl;
	Sleep(1000);
	cout << "2..." << endl;
	Sleep(1000);
	cout << "1..." << endl;
	Sleep(1000);
	system("cls");
	return s;
}
//游戏结束
bool gameEnd(int g){
	system("cls");
	cout<<"游戏结束！你蛇没了！"<<endl;
	if (g <= 20)cout << "你的最终得分只有" << g << "分，蔡！" << endl;
	else if (g <= 70)cout << "你的最终得分为" << g << "分，一般一般啦。" << endl;
	else if (g <= 100)cout << "你的最终得分为" << g << "分，哎哟，不错哦。" << endl;
	else if (g <= 150)cout << "你的最终得分居然有" << g << "分，大神大神！" << endl;
	else cout << "你的最终得分居然有" << g << "分，玩这么久，你是有多无聊？" << endl;
	Sleep(3000);
	cout << "是否开始下一局？[Y/N]：";
	char c;
	cin >> c;
	return (c == 'y' || c == 'Y');
}
int main(){
	system("mode con cols=120 lines=50");
	srand((int)time(0));
	char lever;
	int grade;
	while (true) {
		system("cls");
		lever = gameStart();
		grade = gameLoop(lever);
		if (!gameEnd(grade))break;
		else gameOver = false;
	}
	return 0;
}
