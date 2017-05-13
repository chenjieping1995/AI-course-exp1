/**
* coding��utf-8
* problem��2n queens
* algorithm��hill climbing
* author���½�ƽ
* student number��PB14209115
*/

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstdlib>

using namespace std;
typedef vector<int> CollisionList; // ȡһ�����������ı���

int N;

/*���ļ�����*/
void readFile() {
	ifstream fin("input.txt");
	string s;
	getline(fin, s);
	N = atoi(s.c_str());
	fin.close();
}

/*д�ļ�����*/
void writeFile(int* status, double totaltime) {
	int* b = new int[N];
	if (N % 2 == 0) { // NΪż��������ȡ��������ļ�
		for (int i = 1; i <= N; i++) {
			// Ϊ����b����ֵ
			b[i] = N - status[i] + 1;
		}
	}
	else { // NΪ�����������ĵ�ȡ��Գ�������ļ�
		for (int i = 1; i <= N; i++) {
			// Ϊ����b����ֵ
			b[i] = N - status[N + 1 - i] + 1;
		}
	}
	ofstream outfile;
	outfile.open("output_hill_climbing.txt");
	for (int i = 1; i <= N; i++) {
		outfile << status[i] << endl;
	}
	for (int i = 1; i <= N; i++) {
		outfile << b[i] << endl;
	}
	outfile << totaltime << endl;
	outfile.close();
}

/* ����Ϸ����жϺ���
* ����ֵ�� 1 ������ɽ�㷨���
*			0 ȱʡ���
*			2 ���벻�Ϸ�
*/
int legitimate(int* a) {
	// ��N�Ƚ�С��ʱ�򣬿��ܻ���ֳ�ʱ���������������
	// ���ڴ�Ĭ����� N<8 ʱ�Ľ��
	if (N >= 9) {
		return 1;
	}
	else if (N == 8) {
		// ֱ�Ӹ�ֵ
		a[1] = 5;
		a[2] = 7;
		a[3] = 2;
		a[4] = 6;
		a[5] = 3;
		a[6] = 1;
		a[7] = 4;
		a[8] = 8;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 7) {
		// ֱ�Ӹ�ֵ
		a[1] = 3;
		a[2] = 6;
		a[3] = 2;
		a[4] = 5;
		a[5] = 1;
		a[6] = 4;
		a[7] = 7;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 6) {
		// ֱ�Ӹ�ֵ
		a[1] = 5;
		a[2] = 3;
		a[3] = 1;
		a[4] = 6;
		a[5] = 4;
		a[6] = 2;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 5) {
		// ֱ�Ӹ�ֵ
		a[1] = 3;
		a[2] = 5;
		a[3] = 2;
		a[4] = 4;
		a[5] = 1;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 4) {
		// ֱ�Ӹ�ֵ
		a[1] = 3;
		a[2] = 1;
		a[3] = 4;
		a[4] = 2;
		cout << "Found a solution." << endl;
		return 0;
	}
	else {
		cout << "Input error! Please retry a postive parameter." << endl;
		return 2;
	}
}

/*��ʼ����������ÿ��ÿ�з���һ���ʺ�*/
void generate_status(int* status) {
	for (int i = 1; i <= N; i++) {
		status[i] = i;
	}
	/*�������*/
	srand((unsigned)time(NULL));
	for (int i = 1; i <= N; i++) {
		int r = rand();
		r = r % N +1;
		swap(status[r], status[N - r + 1]);
	}
}

/*���ۺ��������ذڷ�״̬�ĳ�ͻ��*/
inline int evaluate(int* status, CollisionList& collision_list) {
	collision_list.clear();
	int num = 0;
	for (int i = 1; i <= N; i++) {
		for (int j = i + 1; j <= N; j++) {
			int offset = j - i;
			if (abs(status[i] - status[j]) == offset) {
				collision_list.push_back(j);
				num += 1;
			}
		}
	}
	return num;
}

/* ���Խ�������������������ͻ����ԭ���Ĵ󣬾ͽ��н���
*		�������ɹ��󣬸ı�c1Ϊ�µĳ�ͻ�б�
*		����ֵΪ��ǰ״̬�ĳ�ͻ��
*/
int try_exchange(int* status, int row1, int row2, CollisionList& c1) {
	CollisionList new_c1;
	swap(status[row1], status[row2]); // �������лʺ��λ��
	int new_collision = evaluate(status, new_c1);
	if (new_collision > c1.size()) {
		// �µ�״̬��ͻ�����࣬ȡ��֮ǰ�Ľ���
		swap(status[row1], status[row2]);
	}
	else { // �����ɹ������ĳ�ͻ�б�
		c1 = new_c1;
	}
	return c1.size();
}

/* Ѱ����һ������״̬�ĺ���������ֵΪ��һ״̬�ĳ�ͻ��
*/
int choose_next_status(int* status, CollisionList& c1) {
	int old_collision = c1.size();
	int new_collision;

	// �Ż�ֻ��һ����ͻ�Ľ�
	if (old_collision == 1) {
		for (int i = 1; i <= N; i++) {
			if (i != c1[0] && (try_exchange(status, c1[0], i, c1) == 0)) {
				return 0;
			}
		}
	}
	int row1 = -1;
	int row2 = -1;


	// �Ե�ǰ״̬�����Ż�
	if (N > 100) {
		do {
			row1 = c1.back();
			do {
				row2 = rand() % N+1;
			} while (row1 == row2);
			new_collision = try_exchange(status, row1, row2, c1);
		} while (new_collision > old_collision);
	}
	else {
		do {
			row1 = rand() % N+1;
			do {
				row2 = rand() % N+1;
			} while (row1 == row2);
			new_collision = try_exchange(status, row1, row2, c1);
		} while (new_collision > old_collision);

	}

	return new_collision;
}

/*ʹ�������ɽ�㷨Ѱ��һ��N�ʺ�����Ľ�*/
int nqueens_solution(int* status) {
	long max_tries = N*N;
	long max_steps = N*N;
	int tries = 0;
	cout << "Running..." << endl;
	while (tries < max_tries) {
		tries++; // ���Դ���
		int steps = 0;
		int collision = 0; // ��ǰ��ͻ��
		CollisionList collision_list; // ��ͻ�еļ���

		srand(time(NULL) + tries * collision);
		generate_status(status); // �������һ��ʺ�ڷ�
		collision = evaluate(status, collision_list); // ����ڷŵĳ�ͻ��

													  // ����Ѱ�Ҹ��Ž�
		while ((collision != 0) && (steps < max_steps)) {
			collision = choose_next_status(status, collision_list);
			steps += 1;
		}
		// �ҵ��˳�ͻΪ0�Ľ�
		if (collision == 0) {
			cout << "Found a solution. Tries: " << tries << ". ";
			cout << "Steps: " << steps << endl;
			return 1;
		}
	}
	return 0;
}

/*
* �ж�N����ʱ���Ƿ���������������ĵ�ԳƵ�����
* true: ���ڶԳƵ�    false:�����ڶԳƵ�
*/
bool detect(int* status) {
	if (status[(N + 1) / 2] == (N + 1) / 2)
		return true;
	for (int i = 1; i < (N + 1) / 2; i++) {
		if (status[i] == (N + 1) - status[N + 1 - i]) {
			return true;
		}
	}
	return false;
}

/**
* ע�⵽һ���ص㣺
* ��nΪż��ʱ������һ��ʺ�ֱ�ӽ���õ�n�ʺ�����Ľⰴ��ȡ�ԳƵ㼴��
* ��nΪ����ʱ������һ��ʺ���������ڹ������ĵ�ԳƵ������㣬���ɹ������ĵ�ȡ�ԳƵ�
*				������ڹ������ĵ�ԳƵĻʺ����������Ѱ��һ���
*/
int main() {
	readFile(); // ���ļ�
	int* status = new int[N+1]; // ״̬��ʼ��

							  // ʱ����Գ���
	clock_t start, finish;
	double totaltime;
	start = clock();
	// ������
	int num = legitimate(status); // �ж������N�ĺϷ���
	if (num == 0) { // �����NΪȱʡ���������ֱ�����
		finish = clock();
		totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
		writeFile(status, totaltime);
		system("pause");
		return 0;
	}
	else if (num == 2) {
		system("pause");
		return 0;
	}
	int result = nqueens_solution(status); // ��ɽ�㷨���n�ʺ�����
	while ((result == 0) || ((N % 2 == 1) && (detect(status)))) {
		// û�����������н� ���� NΪ�������������Ŀ��н���ڹ������ĵ�ԳƵ����
		result = nqueens_solution(status);
	}
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;

	if (result == 1) {
		cout << "The hill_climbing solution run time is: " << totaltime << "s." << endl;
		writeFile(status, totaltime);
	}
	return 0;
}
