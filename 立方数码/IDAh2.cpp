#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <stack>
#include <queue>
#include <map>

#define ERROR -1
#define TRUE 0

using namespace std;

const int MAXLEN = 200;
const int BLANK = 0;
const int BARRIER = -1;
const int LIMIT = 100;
const int MAXSTEP = 10000000;	// 搜索步数上限

clock_t start_time, end_time;
int visited = 0;	// 访问次数
int MAXVISIT = 100000;	// 访问状态数的上限
int currentf = 0;	// currentf维护一个当前OPEN列表中最小的f值

// 枚举可能的棋子可能的行为
enum movement {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	FORWARD,
	BACK,
	NONE
};

// 三维坐标确定一个点
typedef struct Point {
	// 非说明文档给的点描述格式(行和列值反了)
	int x;	// 层数
	int y;	// 行数
	int z;	// 列数
} Point;

// 表示3立方数码状态的结点
typedef struct Node {
	int elem[27];	// 元素值存储数组
	//Node* parent;	// 指向父结点的指针
	movement action;	// 由父结点产生该结点所用的行动
	//Node* next;	// 指向队列中的下一个结点
	int f;	// f(n)=g(n)+h(n), 耗散评价值
	int g;	// 路径耗散g(n)
	int h;	// 启发函数h2(n)
	Point blank;	// 要移动的空格棋子所在的点的坐标
	//重构操作符: a < b 表示a具有更大的耗散评价值f(n)
	friend bool operator < (const Node a, const Node b) {
		return a.f >= b.f;
	}
} Node, *pNode;

//priority_queue<Node*> OPEN;	// 开启列表采用优先队列结构，默认的比较方法在Node中已经定义了
//Node OPEN[MAXLEN+1];	// 自定义一个OPEN数组
//map<int*, int> CLOSE;
stack<Node> IDAStack;
movement movelist[100];

inline int position(int x, int y, int z) {
	return x * 9 + y * 3 + z;
}

/*寻找当前结点状态中的空格棋子位置*/
void find_blank(Node* node) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				if (node->elem[position(i, j, k)] == BLANK) {
					node->blank.x = i;
					node->blank.y = j;
					node->blank.z = k;
				}
			}
		}
	}
	return;
}

/*读文件函数*/
void readFile(string filename, Node* node) {
	ifstream input(filename);
	string s;
	int a, b, c;
	int x, y;
	x = y = 0;
	while (getline(input, s)) {
		if (s != "" && s != "\n") {
			istringstream ss(s);
			ss >> a >> b >> c;
			node->elem[position(x, y, 0)] = a;
			node->elem[position(x, y, 1)] = b;
			node->elem[position(x, y, 2)] = c;
			y++;
		}
		else {
			y = 0;
			x++;
		}
	}
	return;
}

/*检查输入输出文件中的障碍物位置是否相同*/
bool check_barrier(Node* start, Node* target) {
	for (int i = 0; i < 27; i++) {
		if (start->elem[i] == BARRIER && target->elem[i] != BARRIER)
			return false;
	}
	return true;
}

/*打印寻路过程所执行的动作*/
void print_movement(movement action) {
	switch (action)
	{
	case UP:
		cout << "U";
		break;
	case DOWN:
		cout << "D";
		break;
	case LEFT:
		cout << "L";
		break;
	case RIGHT:
		cout << "R";
		break;
	case FORWARD:
		cout << "F";
		break;
	case BACK:
		cout << "B";
		break;
	case NONE:
		break;
	default:
		break;
	}
}

/*打印动作执行路径的函数*/
void print_path(Node* end) {
	int step = end->g;	// 取出当前状态的路径耗散值
	for (int i = 1; i <= step; i++) {
		// 需要从第二个状态开始，因为第一个状态的action为空，即无须行动即可到达第一个状态
		print_movement(movelist[i]);
	}
	cout << endl;
}

/* 根据传入的棋子序求其所在的位置 */
inline Point getPosition(int position) {
	Point p;
	p.x = position / 9;
	position %= 9;
	p.y = position / 3;
	position %= 3;
	p.z = position;
	return p;
}

/* 所有棋子到其目标位置的三个方向曼哈顿距离和作为启发函数h2(n)的值 */
int h2(Node* start, Node* target) {
	int diff = 0;
	for (int i = 0; i < 27; i++) {
		// 当前状态所指位置为空、为障碍、或者与目标状态该位置棋子相同时，跳过
		if (start->elem[i] == BLANK || start->elem[i] == BARRIER || start->elem[i] == target->elem[i])
			continue;
		Point chess1 = getPosition(i);
		for (int j = 0; j < 27; j++) {
			if (start->elem[i] == target->elem[j]) {
				Point chess2 = getPosition(j);
				diff += abs(chess1.x - chess2.x) + abs(chess1.y - chess2.y) + abs(chess1.z - chess2.z);
				break;	// 跳出当前循环
			}
		}
	}
	return diff;
}

/* 移动空格棋子的函数 */
void move_blank(Node current_status, movement action, Node* target, movement last_move) {
	// 将下一次要访问的状态结点取出
	Node p;
	// 赋值，注意不可以直接赋对象值
	int x = current_status.blank.x;
	int y = current_status.blank.y;
	int z = current_status.blank.z;
	p.blank.x = x;
	p.blank.y = y;
	p.blank.z = z;
	for (int i = 0; i < 27; i++) {
		p.elem[i] = current_status.elem[i];
	}
	p.g = current_status.g + 1;
	p.action = action;
	// 根据当前的动作改变状态结点
	switch (action){
	case DOWN:
		// 有几种情况不执行：越界移动、返回上一步的情况、移动到障碍物所在位置
		if (y >= 2 || last_move == UP || p.elem[position(x, y + 1, z)] == BARRIER) {
			return;
		}
		else {
			p.elem[position(x, y, z)] = p.elem[position(x, y + 1, z)];	// 当前空格位置值变成其下面的那个棋子
			p.elem[position(x, y + 1, z)] = BLANK;	// 当前空格下面位置的值变为空格
			p.blank.y = y + 1;	// 改变空格的位置属性
		}
		break;
	case UP:
		if (y <= 0 || last_move == DOWN || p.elem[position(x, y - 1, z)] == BARRIER)
			return;
		else {
			p.elem[position(x, y, z)] = p.elem[position(x, y - 1, z)];
			p.elem[position(x, y - 1, z)] = BLANK;
			p.blank.y = y - 1;
		}
		break;
	case LEFT:
		if (z <= 0 || last_move == RIGHT || p.elem[position(x, y, z - 1)] == BARRIER)
			return;
		else {
			p.elem[position(x, y, z)] = p.elem[position(x, y, z - 1)];
			p.elem[position(x, y, z - 1)] = BLANK;
			p.blank.z = z - 1;
		}
		break;
	case RIGHT:
		if (z >= 2 || last_move == LEFT || p.elem[position(x, y, z + 1)] == BARRIER)
			return;
		else {
			p.elem[position(x, y, z)] = p.elem[position(x, y, z + 1)];
			p.elem[position(x, y, z + 1)] = BLANK;
			p.blank.z = z + 1;
		}
		break;
	case FORWARD:
		if (x <= 0 || last_move == BACK || p.elem[position(x - 1, y, z)] == BARRIER)
			return;
		else {
			p.elem[position(x, y, z)] = p.elem[position(x - 1, y, z)];
			p.elem[position(x - 1, y, z)] = BLANK;
			p.blank.x = x - 1;
		}
		break;
	case BACK:
		if (x >= 2 || last_move == FORWARD || p.elem[position(x + 1, y, z)] == BARRIER)
			return;
		else {
			p.elem[position(x, y, z)] = p.elem[position(x + 1, y, z)];
			p.elem[position(x + 1, y, z)] = BLANK;
			p.blank.x = x + 1;
		}
		break;
	case NONE:
		break;
	default:
		break;
	}
	p.h = h2(&p, target);
	p.f = p.h + p.g;
	IDAStack.push(p);
	// 如果CLOSE集合中无当前的状态结点p，则为p设置parent，并加入OPEN中，表示有效
	//if (CLOSE.find(p.elem) == CLOSE.end()) {
	//	p->parent = (Node*)current_status;
	//	//OPEN.push(p);
	//	insert(p);
	//}
	return;
}

/* A*算法实现 */
void IDA_star(Node* start, Node* target) {
	start->g = 0;	// 初始状态的路径耗散为0
	start->h = h2(start, target);	// 启发函数由专门的函数计算
	if (start->h == 0) {
		// 初始状态与目标状态相同，直接返回
		cout << "The start status is the same with the target status." << endl;
		cout << "Do not need to take any action." << endl;
		return;
	}
	start->f = start->g + start->h;	// 当前状态的耗散评估值
	start->action = NONE;	// 进入初始状态不需要动作
	// 定义一些变量并初始化
	movement last_move = NONE;	// 上一次移动操作：无
	int limit = start->h;	// 初始上限值
	int step = 0;	// 探索的步数

	start_time = clock();	// 开始计时

	// 
	while (limit < LIMIT) {
		int next_limit = limit;
		IDAStack.push(*start);	// 将初始状态压入栈中

		while (!IDAStack.empty()) {
			Node p = IDAStack.top();	// 获取栈顶元素
			IDAStack.pop();

			// 结束循环条件：p.h为0，表示此时的p即为目标状态
			if (p.h == 0) {
				movelist[p.g] = p.action;	// 
				end_time = clock();
				cout << double(end_time - start_time) / CLOCKS_PER_SEC << endl;
				print_path(&p);
				return;
			}
			else {
				if (p.f <= limit) {	// 当前状态结点的f值小于limit
					if (p.action != NONE) {	// 到达状态结点p的动作不为空
						last_move = p.action;
					}
					for (int i = 0; i < 6; i++) {	// 进行下一状态的探索
						move_blank(p, (movement)i, target, last_move);
					}
				}
			}
			movelist[p.g] = last_move;
		}
		limit = next_limit;	// 下一次循环的启发函数上限值
	}
	// 遍历了所有可达状态而不能到达目标状态
	cout << "No solution!" << endl;
	cout << "Please change the input files and try again later." << endl;
	return;
}

int main() {
	Node *start, *target;
	start = new Node();
	target = new Node();
	// 读文件
	readFile("input.txt", start);
	readFile("target.txt", target);
	find_blank(start);
	find_blank(target);
	// 对标准输出函数进行重定向，使得标准输出到目标文件而不是控制台
	freopen("output_IDAh2.txt", "w", stdout);
	// 检查输入输出文件中的障碍物位置是否相同
	if (!check_barrier(start, target)) { // 障碍物位置不相同时，直接报错并返回
		cout << "The barrier location in source and target is not the same！" << endl;
		cout << "Please check it and try again later." << endl;
		return ERROR;
	}
	// 调用A*算法求解问题
	IDA_star(start, target);
	// 关闭重定向的输出
	fclose(stdout);
	delete start;
	delete target;
	return TRUE;
}
