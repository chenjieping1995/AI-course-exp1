/**
* coding��utf-8
* problem��2n queens
* algorithm��CSP
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
int N;
int* status;

/*���ļ�����*/
void readFile() {
	ifstream fin("input.txt");
	string s;
	getline(fin, s);
	N = atoi(s.c_str());
	fin.close();
}

/*д�ļ�����*/
void writeFile(double totaltime) {
	int* b = new int[N+1];
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

/* ����Ϸ����жϺ���
* ����ֵ�� 1 ������ɽ�㷨���
*			0 ȱʡ���
*			2 ���벻�Ϸ�
*/
int legitimate() {
	// ��N�Ƚ�С��ʱ�򣬿��ܻ���ֳ�ʱ���������������
	// ���ڴ�Ĭ����� N<8 ʱ�Ľ��
	if (N >= 9) {
		return 1;
	}
	else if (N == 8) {
		// ֱ�Ӹ�ֵ
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
		// ֱ�Ӹ�ֵ
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
		// ֱ�Ӹ�ֵ
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
		// ֱ�Ӹ�ֵ
		status[1] = 3;
		status[2] = 5;
		status[3] = 2;
		status[4] = 4;
		status[5] = 1;
		cout << "Found a solution." << endl;
		return 0;
	}
	else if (N == 4) {
		// ֱ�Ӹ�ֵ
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

/*��������*/
void swap(int &x, int &y)
{
	int t;
	t = x;
	x = y;
	y = t;
}

/* ��ȡ��ͻ���������ڳ�ֵ��ϵ��������ͬ�л�ͬ�г�ͻ */ 
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

/* ��ȡ���г�ͻ�� */
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


/* ��ȡ���г�ͻ��С�Ľ����ĳ�ͻ */ 
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

/* CSP�㷨*/
void solution_status()
{
	int i;
	int r1, r2;
	int h, h0;	//��¼��ͻ���� 
	for (i = 1; i <= N; i++){
		// ÿ��ÿ�аڷ�һ������
		status[i] = i;
	}
	for (i = 1; i <= N; i++)	//�����ֵ 
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
	status = new int[N+1]; // ״̬��ʼ��
	for (int i = 1; i <= N; i++){
		status[i] = 0;
	}

	// ʱ����Գ���
	clock_t start, finish;
	double totaltime;
	start = clock();
	// ������
	int num = legitimate(); // �ж������N�ĺϷ���
	if (num == 0) { // �����NΪȱʡ���������ֱ�����
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
	solution_status(); // ��ɽ�㷨���n�ʺ�����
	while ((N % 2 == 1) && detect(status)) {
		// û�����������н� ���� NΪ�������������Ŀ��н���ڹ������ĵ�ԳƵ����
		solution_status();
	}
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;

	cout << "The csp solution run time is: " << totaltime << "s." << endl;
	writeFile(totaltime);

	return 0;
}
