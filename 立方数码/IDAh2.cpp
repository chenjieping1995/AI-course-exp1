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
const int MAXSTEP = 10000000;	// ������������

clock_t start_time, end_time;
int visited = 0;	// ���ʴ���
int MAXVISIT = 100000;	// ����״̬��������
int currentf = 0;	// currentfά��һ����ǰOPEN�б�����С��fֵ

// ö�ٿ��ܵ����ӿ��ܵ���Ϊ
enum movement {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	FORWARD,
	BACK,
	NONE
};

// ��ά����ȷ��һ����
typedef struct Point {
	// ��˵���ĵ����ĵ�������ʽ(�к���ֵ����)
	int x;	// ����
	int y;	// ����
	int z;	// ����
} Point;

// ��ʾ3��������״̬�Ľ��
typedef struct Node {
	int elem[27];	// Ԫ��ֵ�洢����
	//Node* parent;	// ָ�򸸽���ָ��
	movement action;	// �ɸ��������ý�����õ��ж�
	//Node* next;	// ָ������е���һ�����
	int f;	// f(n)=g(n)+h(n), ��ɢ����ֵ
	int g;	// ·����ɢg(n)
	int h;	// ��������h2(n)
	Point blank;	// Ҫ�ƶ��Ŀո��������ڵĵ������
	//�ع�������: a < b ��ʾa���и���ĺ�ɢ����ֵf(n)
	friend bool operator < (const Node a, const Node b) {
		return a.f >= b.f;
	}
} Node, *pNode;

//priority_queue<Node*> OPEN;	// �����б�������ȶ��нṹ��Ĭ�ϵıȽϷ�����Node���Ѿ�������
//Node OPEN[MAXLEN+1];	// �Զ���һ��OPEN����
//map<int*, int> CLOSE;
stack<Node> IDAStack;
movement movelist[100];

inline int position(int x, int y, int z) {
	return x * 9 + y * 3 + z;
}

/*Ѱ�ҵ�ǰ���״̬�еĿո�����λ��*/
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

/*���ļ�����*/
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

/*�����������ļ��е��ϰ���λ���Ƿ���ͬ*/
bool check_barrier(Node* start, Node* target) {
	for (int i = 0; i < 27; i++) {
		if (start->elem[i] == BARRIER && target->elem[i] != BARRIER)
			return false;
	}
	return true;
}

/*��ӡѰ·������ִ�еĶ���*/
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

/*��ӡ����ִ��·���ĺ���*/
void print_path(Node* end) {
	int step = end->g;	// ȡ����ǰ״̬��·����ɢֵ
	for (int i = 1; i <= step; i++) {
		// ��Ҫ�ӵڶ���״̬��ʼ����Ϊ��һ��״̬��actionΪ�գ��������ж����ɵ����һ��״̬
		print_movement(movelist[i]);
	}
	cout << endl;
}

/* ���ݴ�����������������ڵ�λ�� */
inline Point getPosition(int position) {
	Point p;
	p.x = position / 9;
	position %= 9;
	p.y = position / 3;
	position %= 3;
	p.z = position;
	return p;
}

/* �������ӵ���Ŀ��λ�õ��������������پ������Ϊ��������h2(n)��ֵ */
int h2(Node* start, Node* target) {
	int diff = 0;
	for (int i = 0; i < 27; i++) {
		// ��ǰ״̬��ָλ��Ϊ�ա�Ϊ�ϰ���������Ŀ��״̬��λ��������ͬʱ������
		if (start->elem[i] == BLANK || start->elem[i] == BARRIER || start->elem[i] == target->elem[i])
			continue;
		Point chess1 = getPosition(i);
		for (int j = 0; j < 27; j++) {
			if (start->elem[i] == target->elem[j]) {
				Point chess2 = getPosition(j);
				diff += abs(chess1.x - chess2.x) + abs(chess1.y - chess2.y) + abs(chess1.z - chess2.z);
				break;	// ������ǰѭ��
			}
		}
	}
	return diff;
}

/* �ƶ��ո����ӵĺ��� */
void move_blank(Node current_status, movement action, Node* target, movement last_move) {
	// ����һ��Ҫ���ʵ�״̬���ȡ��
	Node p;
	// ��ֵ��ע�ⲻ����ֱ�Ӹ�����ֵ
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
	// ���ݵ�ǰ�Ķ����ı�״̬���
	switch (action){
	case DOWN:
		// �м��������ִ�У�Խ���ƶ���������һ����������ƶ����ϰ�������λ��
		if (y >= 2 || last_move == UP || p.elem[position(x, y + 1, z)] == BARRIER) {
			return;
		}
		else {
			p.elem[position(x, y, z)] = p.elem[position(x, y + 1, z)];	// ��ǰ�ո�λ��ֵ�����������Ǹ�����
			p.elem[position(x, y + 1, z)] = BLANK;	// ��ǰ�ո�����λ�õ�ֵ��Ϊ�ո�
			p.blank.y = y + 1;	// �ı�ո��λ������
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
	// ���CLOSE�������޵�ǰ��״̬���p����Ϊp����parent��������OPEN�У���ʾ��Ч
	//if (CLOSE.find(p.elem) == CLOSE.end()) {
	//	p->parent = (Node*)current_status;
	//	//OPEN.push(p);
	//	insert(p);
	//}
	return;
}

/* A*�㷨ʵ�� */
void IDA_star(Node* start, Node* target) {
	start->g = 0;	// ��ʼ״̬��·����ɢΪ0
	start->h = h2(start, target);	// ����������ר�ŵĺ�������
	if (start->h == 0) {
		// ��ʼ״̬��Ŀ��״̬��ͬ��ֱ�ӷ���
		cout << "The start status is the same with the target status." << endl;
		cout << "Do not need to take any action." << endl;
		return;
	}
	start->f = start->g + start->h;	// ��ǰ״̬�ĺ�ɢ����ֵ
	start->action = NONE;	// �����ʼ״̬����Ҫ����
	// ����һЩ��������ʼ��
	movement last_move = NONE;	// ��һ���ƶ���������
	int limit = start->h;	// ��ʼ����ֵ
	int step = 0;	// ̽���Ĳ���

	start_time = clock();	// ��ʼ��ʱ

	// 
	while (limit < LIMIT) {
		int next_limit = limit;
		IDAStack.push(*start);	// ����ʼ״̬ѹ��ջ��

		while (!IDAStack.empty()) {
			Node p = IDAStack.top();	// ��ȡջ��Ԫ��
			IDAStack.pop();

			// ����ѭ��������p.hΪ0����ʾ��ʱ��p��ΪĿ��״̬
			if (p.h == 0) {
				movelist[p.g] = p.action;	// 
				end_time = clock();
				cout << double(end_time - start_time) / CLOCKS_PER_SEC << endl;
				print_path(&p);
				return;
			}
			else {
				if (p.f <= limit) {	// ��ǰ״̬����fֵС��limit
					if (p.action != NONE) {	// ����״̬���p�Ķ�����Ϊ��
						last_move = p.action;
					}
					for (int i = 0; i < 6; i++) {	// ������һ״̬��̽��
						move_blank(p, (movement)i, target, last_move);
					}
				}
			}
			movelist[p.g] = last_move;
		}
		limit = next_limit;	// ��һ��ѭ����������������ֵ
	}
	// ���������пɴ�״̬�����ܵ���Ŀ��״̬
	cout << "No solution!" << endl;
	cout << "Please change the input files and try again later." << endl;
	return;
}

int main() {
	Node *start, *target;
	start = new Node();
	target = new Node();
	// ���ļ�
	readFile("input.txt", start);
	readFile("target.txt", target);
	find_blank(start);
	find_blank(target);
	// �Ա�׼������������ض���ʹ�ñ�׼�����Ŀ���ļ������ǿ���̨
	freopen("output_IDAh2.txt", "w", stdout);
	// �����������ļ��е��ϰ���λ���Ƿ���ͬ
	if (!check_barrier(start, target)) { // �ϰ���λ�ò���ͬʱ��ֱ�ӱ�������
		cout << "The barrier location in source and target is not the same��" << endl;
		cout << "Please check it and try again later." << endl;
		return ERROR;
	}
	// ����A*�㷨�������
	IDA_star(start, target);
	// �ر��ض�������
	fclose(stdout);
	delete start;
	delete target;
	return TRUE;
}
