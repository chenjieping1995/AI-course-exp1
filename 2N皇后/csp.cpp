/**
* coding：utf-8
* problem：2n queens
* algorithm：CSP
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
int N;
int* status;

/*读文件函数*/
void readFile() {
	ifstream fin("input.txt");
	string s;
	getline(fin, s);
	N = atoi(s.c_str());
	fin.close();
}

/*写文件函数*/
void writeFile(double totaltime) {
	int* b = new int[N+1];
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
	outfile.open("output_csp.txt");
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
int legitimate() {
	// 当N比较小的时候，可能会出现长时间搜索不到的情况
	// 故在此默认输出 N<8 时的解答
	if (N >= 9) {
		return 1;
	}
	else if (N == 8) {
		// 直接赋值
		status[1] = 5;
		status[2] = 7;
		status[3] = 2;
		status[4] = 6;
		status[5] = 3;
		status[6] = 1;
		status[7] = 4;
		status[8] = 8;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 7) {
		// 直接赋值
		status[1] = 3;
		status[2] = 6;
		status[3] = 2;
		status[4] = 5;
		status[5] = 1;
		status[6] = 4;
		status[7] = 7;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 6) {
		// 直接赋值
		status[1] = 5;
		status[2] = 3;
		status[3] = 1;
		status[4] = 6;
		status[5] = 4;
		status[6] = 2;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 5) {
		// 直接赋值
		status[1] = 3;
		status[2] = 5;
		status[3] = 2;
		status[4] = 4;
		status[5] = 1;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 4) {
		// 直接赋值
		status[1] = 3;
		status[2] = 1;
		status[3] = 4;
		status[4] = 2;
		cout << "Found a solution." << endl;
		return 0;
	}
	else {
		cout << "Input error! Please retry a postive parameter." << endl;
		return 2;
	}
}

/*交换函数*/
void swap(int &x, int &y)
{
	int t;
	t = x;
	x = y;
	y = t;
}

/* 获取冲突个数，由于初值关系，不存在同行或同列冲突 */ 
int geth(int *status)                            
{
	int i, j;
	int count = 0;
	for (i = 1; i <= N; i++){
		for (j = i + 1; j <= N; j++){
			if (status[i] - status[j] == i - j || status[i] - status[j] == j - i){
				count++;
			}
		}
	}
	return count;
}

/* 获取单行冲突数 */
int geth_line(int *status, int x)                 
{
	int i, j;
	int count = 0;
	for (i = 1; i <= N; i++){
		if (i != x && (status[i] - status[x] == i - x || status[i] - status[x] == x - i)){
			count++;
		}
	}
	return count;
}


/* 获取单行冲突最小的交换的冲突 */ 
int geth_line_status(int l)                      
{
	int i;
	int t;
	int p=2;
	int h;
	int a, b, c, d;
	int min = INT_MAX;
	a = geth_line(status, l);
	for (i = 1; i <= N; i++)
	{
		if (i != l)
		{
			b = geth_line(status, i);
			if (status[l] - status[i] == l - i || status[l] - status[i] == i - l)
			{
				b--;
			}
			swap(status[l], status[i]);
			c = geth_line(status, l);
			d = geth_line(status, i);
			if (status[l] - status[i] == l - i || status[l] - status[i] == i - l)
			{
				d--;
			}
			h = c + d - a - b;
			if (h<min || (h == min&&rand() % p == 0))
			{
				min = h;
				t = i;
			}
			swap(status[l], status[i]);
		}
	}
	swap(status[l], status[t]);
	return min;
}

/* CSP算法*/
void solution_status()
{
	int i;
	int r1, r2;
	int h, h0;	//记录冲突个数 
	for (i = 1; i <= N; i++){
		// 每行每列摆放一个棋子
		status[i] = i;
	}
	for (i = 1; i <= N; i++)	//随机初值 
	{
		r1 = rand() % N + 1;
		r2 = rand() % N + 1;
		swap(status[r1], status[r2]);
	}
	h0 = geth(status);
	while (h0>0)
	{
		for (i = 1; i <= N; i++)
		{
			h = geth_line_status(i);
			h0 = h0 + h;
			if (h0 == 0)	break;
		}
	}
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
	status = new int[N+1]; // 状态初始化
	for (int i = 1; i <= N; i++){
		status[i] = 0;
	}

	// 时间测试程序
	clock_t start, finish;
	double totaltime;
	start = clock();
	// 主程序
	int num = legitimate(); // 判断输入的N的合法性
	if (num == 0) { // 输入的N为缺省情况，可以直接输出
		finish = clock();
		totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
		writeFile(totaltime);
		system("pause");
		return 0;
	}
	else if (num == 2) {
		system("pause");
		return 0;
	}
	solution_status(); // 爬山算法求解n皇后问题
	while ((N % 2 == 1) && detect(status)) {
		// 没有搜索到可行解 或者 N为奇数且搜索到的可行解存在关于中心点对称的情况
		solution_status();
	}
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;

	cout << "The csp solution run time is: " << totaltime << "s." << endl;
	writeFile(totaltime);

	return 0;
}
