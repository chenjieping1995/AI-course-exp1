/**
* coding：utf-8
* problem：2n queens
* algorithm：hill climbing
* author：陈介平
* student number：PB14209115
*/

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstdlib>

using namespace std;
typedef vector<int> CollisionList; // 取一个整型容器的别名

int N;

/*读文件函数*/
void readFile() {
	ifstream fin("input.txt");
	string s;
	getline(fin, s);
	N = atoi(s.c_str());
	fin.close();
}

/*写文件函数*/
void writeFile(int* status, double totaltime) {
	int* b = new int[N];
	if (N % 2 == 0) { // N为偶数，按行取差输出到文件
		for (int i = 1; i <= N; i++) {
			// 为数组b赋初值
			b[i] = N - status[i] + 1;
		}
	}
	else { // N为奇数，按中心点取点对称输出到文件
		for (int i = 1; i <= N; i++) {
			// 为数组b赋初值
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

/* 输入合法性判断函数
* 返回值： 1 调用爬山算法求解
*			0 缺省情况
*			2 输入不合法
*/
int legitimate(int* a) {
	// 当N比较小的时候，可能会出现长时间搜索不到的情况
	// 故在此默认输出 N<8 时的解答
	if (N >= 9) {
		return 1;
	}
	else if (N == 8) {
		// 直接赋值
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
		// 直接赋值
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
		// 直接赋值
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
		// 直接赋值
		a[1] = 3;
		a[2] = 5;
		a[3] = 2;
		a[4] = 4;
		a[5] = 1;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 4) {
		// 直接赋值
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

/*初始化函数：在每行每列放置一个皇后*/
void generate_status(int* status) {
	for (int i = 1; i <= N; i++) {
		status[i] = i;
	}
	/*随机交换*/
	srand((unsigned)time(NULL));
	for (int i = 1; i <= N; i++) {
		int r = rand();
		r = r % N +1;
		swap(status[r], status[N - r + 1]);
	}
}

/*评价函数：返回摆放状态的冲突数*/
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

/* 尝试交换函数：如果交换后冲突不比原来的大，就进行交换
*		当交换成功后，改变c1为新的冲突列表
*		返回值为当前状态的冲突数
*/
int try_exchange(int* status, int row1, int row2, CollisionList& c1) {
	CollisionList new_c1;
	swap(status[row1], status[row2]); // 交换两行皇后的位置
	int new_collision = evaluate(status, new_c1);
	if (new_collision > c1.size()) {
		// 新的状态冲突数增多，取消之前的交换
		swap(status[row1], status[row2]);
	}
	else { // 交换成功，更改冲突列表
		c1 = new_c1;
	}
	return c1.size();
}

/* 寻找下一个更优状态的函数：返回值为下一状态的冲突数
*/
int choose_next_status(int* status, CollisionList& c1) {
	int old_collision = c1.size();
	int new_collision;

	// 优化只有一个冲突的解
	if (old_collision == 1) {
		for (int i = 1; i <= N; i++) {
			if (i != c1[0] && (try_exchange(status, c1[0], i, c1) == 0)) {
				return 0;
			}
		}
	}
	int row1 = -1;
	int row2 = -1;


	// 对当前状态进行优化
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

/*使用随机爬山算法寻找一个N皇后问题的解*/
int nqueens_solution(int* status) {
	long max_tries = N*N;
	long max_steps = N*N;
	int tries = 0;
	cout << "Running..." << endl;
	while (tries < max_tries) {
		tries++; // 尝试次数
		int steps = 0;
		int collision = 0; // 当前冲突数
		CollisionList collision_list; // 冲突列的集合

		srand(time(NULL) + tries * collision);
		generate_status(status); // 随机生成一组皇后摆放
		collision = evaluate(status, collision_list); // 该组摆放的冲突数

													  // 持续寻找更优解
		while ((collision != 0) && (steps < max_steps)) {
			collision = choose_next_status(status, collision_list);
			steps += 1;
		}
		// 找到了冲突为0的解
		if (collision == 0) {
			cout << "Found a solution. Tries: " << tries << ". ";
			cout << "Steps: " << steps << endl;
			return 1;
		}
	}
	return 0;
}

/*
* 判断N奇数时解是否存在两个关于中心点对称的棋子
* true: 存在对称点    false:不存在对称点
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
* 注意到一个特点：
* 当n为偶数时，另外一组皇后，直接将求得的n皇后问题的解按行取对称点即可
* 当n为奇数时，另外一组皇后，如果不存在关于中心点对称的两个点，即可关于中心点取对称点
*				如果存在关于中心点对称的皇后，则可以重新寻找一组解
*/
int main() {
	readFile(); // 读文件
	int* status = new int[N+1]; // 状态初始化

							  // 时间测试程序
	clock_t start, finish;
	double totaltime;
	start = clock();
	// 主程序
	int num = legitimate(status); // 判断输入的N的合法性
	if (num == 0) { // 输入的N为缺省情况，可以直接输出
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
	int result = nqueens_solution(status); // 爬山算法求解n皇后问题
	while ((result == 0) || ((N % 2 == 1) && (detect(status)))) {
		// 没有搜索到可行解 或者 N为奇数且搜索到的可行解存在关于中心点对称的情况
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
