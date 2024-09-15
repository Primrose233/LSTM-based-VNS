#include<iostream>
#include<queue>
#include<stdlib.h>
#include<ctime>
#include<cstdlib>
#include<math.h>
#include<random>
#include<unordered_map>
#include<string>
#include<cstring>
#include<fstream>
#include<algorithm>
#include<vector>
#include <windows.h>
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h>
#define timenum 168//总优化时段为24小时
#define day 7//7天
#define slice 1//1小时划分为1个时段
#define Msli 1
#define totalday 1000//
#define docnum 12
#define minwork 4
#define maxwork 9
#define iteras 50
#define cor1 1.0
#define cor2 60.0
#define cor3 1.0
#define K 5
#define nightlong 8//夜班长度
#define nightstart 16//将夜班结束视为一天的开始（0点），因此夜班开始为16点
#define nowtype 0//1表示看完内队列病人再离开
#define maxnight 2//单个医生最多值夜班数
#define resthour 8//休息班次固定在每天的什么时候
#define shakeadd 8//shake调整班次数目增加数
#define maxcheck 2
#define ifM 0
#define defaultM 6
#define numcase 6
using namespace std;

int Schedule[slice * timenum] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
unordered_map<string, double> Hashmap0;
unordered_map<string, double> Hashmap1;
//const double Arrive_rate[timenum] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
//double Arrive_rate[timenum] = { 4,3,3,2,2,1,1,2,2,3,6,8,8,7,5,6,7,7,6,6,5,7,6,5 };  //test
//double Arrive_rate[timenum] = { 29,14,20,15,11,14,9,11,17,14,11,15,24,29,14,9,2,3,5,4,1,3,20,25 };
//double Arrive_ratio[day] = { 1,0.8,1.1,1.2,0.9,1.2,0.8 };
double Arrive_rate[timenum] = { 13,13,15,13,9,11,12,14,12,8,7,10,11,8,5,4,4,2,2,2,1,1,11,11,13 };
double Arrive_ratio[day] = { 1,1,1,1,1,1,1 };
const double vip_Arrive_rate[timenum] = { 0 };  //test

const double miu1 = 5.683;    //医生服务时长参数1
const double miu11 = 0.8732;//参数2
const double miu2 = 6;    //检查台服务速率
const double ning[2] = { 8.061,0.3597 };
const double sheng[2] = { 8.265,0.3229 };
const double xuechang[2] = { 7.119,0.3083 };
const double xueqi[2] = { 7.309,0.7435 };
const double kuai1[2] = { 8.337,0.2714 };//1为疫情时期
const int nursenum = 100;      //检查台数量
const double probability1 = 0.5804;     //回流率
const double probability2 = 0.2101;
const double threshold = 0.5;    //超时阈值为半小时

const int MaxTotal = 1000;

const int Static_Each_Period = 10;
double Static_Divide[2][Static_Each_Period * timenum] = { 0 };
double Sum_Divide[2][Static_Each_Period * timenum] = { 0 };
int ifcout = 0;

//int workhours1[docnum * day] = { 0,0,nightlong,nightlong,9,10,15,16,17,18 };
int workhours1[docnum * day] = { 0 };
int workhours2[docnum * day] = { 0 };
int M[Msli * timenum] = { 0 };
int tempM[timenum] = { 5,5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 5, 5, 5, 5, 4 };

int change_num = 0;
int change_type = 0;
int tmp1[docnum * day] = { 0 };
int tmp2[docnum * day] = { 0 };
int tmp3[Msli * timenum] = { 0 };

int tmpwk1[docnum * day] = { 0 };
int tmpwk2[docnum * day] = { 0 };
int tmpM[Msli * timenum] = { 0 };

int bestwk1[docnum * day] = { 0 };
int bestwk2[docnum * day] = { 0 };
int bestM[Msli * timenum] = { 0 };

double bestvalue = 0;
double wholebest = 0;
double lastvalue = 0;
double workload[slice * timenum] = { 0 };
double totalload = 0;

int rest[day] = { 0 };
int nightshift[day] = { 0 };
int shifts[day] = { 0 };
int s[4] = { 3,6,8,docnum };
double avewt[slice * timenum] = { 0 };
double prob[12] = { 0.382,0.558,0.721,0.844,0.895,0.931,0.958,0.969,0.979,0.987,0.994 };
vector<double> solutions;
vector<int> searchtimes;
//输出上下班时间和M
void printout(int a1[], int a2[], int a3[]) {
	for (int i = 0; i < docnum * day; i++) {
		//cout << a1[i] << ' ' << a2[i] << endl;
	}
	int tmpsche[slice * timenum] = { 0 };
	for (int i = 0; i < docnum * day; i++) {
		if (a1[i] == 0) {
			tmpsche[0]++;
		}
	}
	for (int i = 1; i < slice * timenum; i++) {
		int tmp = 0;
		for (int j = 0; j < docnum * day; j++) {
			if (a1[j] == i) {
				tmp = tmp + 1;
			}
			if (a2[j] == i) {
				tmp = tmp - 1;
			}
		}
		tmpsche[i] = tmpsche[i - 1] + tmp;
	}
	cout << "上班时间：";
	for (int i = 0; i < docnum * day; i++) {
		cout << a1[i] << ',';
	}
	cout << endl;
	cout << "下班时间：";
	for (int i = 0; i < docnum * day; i++) {
		cout << a2[i] << ',';
	}
	cout << endl;
	cout << "M：";
	for (int i = 0; i < Msli * timenum; i++) {
		cout << a3[i] << ',';
	}
	cout << endl;
	cout << "医生数：";
	for (int i = 0; i < slice * timenum; i++) {
		cout << tmpsche[i] << ',';
	}
	cout << endl;
}

class Patient  //病人类
{
public:
	double arrivetime;   //到达时间
	double lasttime;   //在医生处开始排队的时间，用于统计这个病人的等待时间
	double ons_waittime = 0;   //等待时间加和
	double FCFS_time;   //进入医生、检查台、大厅队列的时间，用于确定排队先来后到的优先级
	int _intari;   //到达时间的整数，用于统计
	int belongdoc;   //从哪个医生出来的，用于回流
	bool first_come = true;    //是否第一次看医生
	int vip; //高等级为1,低等级为2
	int first_check = 0;

	Patient(double ari_t, double lst_t, int int_ari, int vi_p) : arrivetime(ari_t), lasttime(lst_t), _intari(int_ari), vip(vi_p) {}
};

struct patient_compare //先比较优先等级，在比较先来后到
{
	bool operator()(const Patient& a, const Patient& b)//is  a等级比b等级低 && is a比b晚到—>得到 is a比b晚接受服务。 均同 a前
	{
		if (a.vip != b.vip)return a.vip > b.vip;
		else return a.FCFS_time > b.FCFS_time;
	}
};

class Doctor
{
public:
	priority_queue<Patient, vector<Patient>, patient_compare> Doc_que;
	int internal = 0;   //内队列多少人了
	int vip_num = 0;   //vip病人数量
	bool on_duty = 0;  //暂时没用上
	double next_finish_time = DBL_MAX;   //下一个事件发生的时间
	int workhour1;
	int workhour2;
};

class Nurse
{
public:
	priority_queue<Patient, vector<Patient>, patient_compare> Nur_que;
	//队列中人数通过对优先级队列的统计得到。
	int vip_num = 0; //vip病人数量
	double next_finish_time = DBL_MAX;
};

struct Event
{
public:
	double time;
	int what_type;    //-2代表普通病人到达，-1代表vip病人到达,和vip等级相对应，0代表医生处理完，1代表检查台
	int which;   //-1则代表是病人来了，无意义,其他则代表是哪个医生/检查台将要完成服务

	Event(double t, int _what, int _which) : time(t), what_type(_what), which(_which) {}
};

struct event_time_compare  //小顶堆，时间早的优先
{
	bool operator()(const Event& a, const Event& b)//is b在前  同时间的话a在前
	{
		return a.time > b.time;
	}
};

int common_Findmininter(Doctor* Doctors, double modtnow)   //找最小队长医生小函数
{
	int min = 100000;//内队列人数
	int which_inter = -1;
	if (nowtype == 0) {
		for (int i = 0; i < Schedule[(int)floor(modtnow)]; i++)
		{
			if (Doctors[i].internal <= min) { min = Doctors[i].internal; which_inter = i; }
		}
		if (which_inter >= docnum * day) {
			//cout << "error ajkhkjdha " << endl; system("pause");
		}
	}
	else {
		for (int i = 0; i < docnum * day; i++) {
			if (Doctors[i].workhour1 <= (int)floor(modtnow) && Doctors[i].workhour2 > (int)floor(modtnow)) {

				//cout << Doctors[i].workhour1 << endl;
				if (Doctors[i].internal < min) {
					min = Doctors[i].internal;
					which_inter = i;
				}
			}
		}
	}
	//cout << Schedule[(int)floor(modtnow)] << which_inter << endl;;
	if (which_inter == -1) {
		for (int i = 0; i < docnum * day; i++) {
			cout << i << " " << Doctors[i].internal << endl;
		}
		cout << modtnow << " wrong" << endl;
		system("pause");
	}
	return which_inter;
}

int common_Findminnur(Nurse* Nurses, const int nursenum)   //找最小队长检查台小函数
{
	int min = 100000;
	int minnur = -1;
	for (int i = 0; i < nursenum; i++)
	{
		if (Nurses[i].Nur_que.size() < min) { min = (int)Nurses[i].Nur_que.size(); minnur = i; }
	}
	if (minnur == -1) {
		cout << "wrong in nurse" << endl;
		system("pause");
	}
	return minnur;
}

int vip_Find_min_inter(Doctor* Doctors, double modtnow) //找最小vip病人 & 且未满 医生
{
	int min = INT_MAX;
	int which_inter = -1;
	int j = (int)floor(modtnow);
	for (int i = 0; i < Schedule[j]; i++)
	{
		if (Doctors[i].internal < M[j] && Doctors[i].vip_num < min) { min = Doctors[i].internal; which_inter = i; }
	}
	return which_inter;//to do 如果医生数均为满，则返回值不可控。
}

int vip_Find_min_nur(Nurse* Nurses, const int nursenum)//找最小vip病人数量的检查台  //to do 会不是检查台的优先级引入导致的问题？
{
	int min = Nurses[0].vip_num;
	int minnur = 0;
	for (int i = 1; i < nursenum; i++)
	{
		if (Nurses[i].vip_num < min) { min = Nurses[i].vip_num; minnur = i; }
	}
	return minnur;
}
//1为到下班时间看完当前病人即走
double Run_Case_Manager_1(int Schedule[], int M[], int Circle = totalday)
{
	for (int i = 0; i < slice * timenum; i++) {
		workload[i] = 0;
	}
	totalload = 0;
	::std::mt19937 rng(1);
	//::std::mt19937 rng(time(NULL));
	cout << "开始仿真1" << endl;
	//std::exponential_distribution<double> doc_expo(miu1);//?
	uniform_real_distribution<double>check(0, 1);
	lognormal_distribution<double>doc_log(miu1, miu11);
	//std::exponential_distribution<double> nur_expo(miu2);//?
	lognormal_distribution<double>ning_log(ning[0], ning[1]);
	lognormal_distribution<double>sheng_log(sheng[0], sheng[1]);
	lognormal_distribution<double>xueqi_log(xueqi[0], xueqi[1]);

	const gsl_rng_type* T;
	gsl_rng* r;
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	int maxdocnum = 0;//寻找现行排班方案最大医生数量
	for (int i = 0; i < slice * timenum; i++)
	{
		if (Schedule[i] > maxdocnum) maxdocnum = Schedule[i];
	}
	double tnow = 0;
	double modtnow = 0;
	double common_Sumwaittime[slice * timenum] = { 0 };  //普通病人： 等待时间统计
	int common_Sumwaitnum[slice * timenum] = { 0 };      //普通病人： 可统计每小时到达系统的人数
	int common_Overtimenum[slice * timenum] = { 0 };     //普通病人： 第一次看到医生超时的病人数

	double vip_Sumwaittime[slice * timenum] = { 0 };
	int vip_Sumwaitnum[slice * timenum] = { 0 };
	int vip_Overtimenum[slice * timenum] = { 0 };
	double totaltime = 0;
	int totalnum = 0;
	double avgwaittime = 0;

	int countFailIn[2] = { 0 };
	int countcheck = 0;
	Nurse Nurses[nursenum];
	Doctor* Doctors = new Doctor[maxdocnum];

	priority_queue<Patient, vector<Patient>, patient_compare> Pre_queue;   //病人队列
	priority_queue<Event, vector<Event>, event_time_compare> events_que;   //时间队列

	//事件驱动的仿真规则：先生成所有病人到达时间，然后每个医生或检查台开始服务就生成一个这个病人服务完的时间，用一个函数取病人到达、医生服务完病人、检查台服务完病人中最早发生的那个作为下一个事件。以此驱动代码运行

	for (int round = 1; round <= Circle; round++) //仿真正式开始，连续进行round天
	{
		//cout << "test5" << endl;

		for (int i = 0; i < timenum; i++)  //一天的开始，先把当天所有普通病人到达的事件先塞进队
		{
			std::uniform_real_distribution<double> uni(timenum * ((double)round - 1) + i, timenum * ((double)round - 1) + i + 1);
			std::poisson_distribution<int> poi(Arrive_rate[i]);
			int size = poi(rng);// 使用rng引擎获得poi分布
			for (int j = 1; j <= size; j++)  //生成size个均匀分布时刻
			{
				Event p_ari(uni(rng), -2, -1);//  到达时间- 普通病人- 新来
				events_que.push(p_ari);//加入事件列
			}
		}
		/*
		for (int i = 0; i < timenum; i++)  //再塞vip病人---先后顺序对结果有0.5%影响-- 随着仿真次数增加而减少
		{
			std::uniform_real_distribution<double> vip_uni(timenum * (round - 1) + i, timenum * (round - 1) + i + 1);
			std::poisson_distribution<int> poi(vip_Arrive_rate[i]);
			int size = poi(rng);
			for (int j = 1; j <= size; j++)  //生成size个均匀分布时刻
			{
				Event vip_ari(vip_uni(rng), -1, -1);//  到达时间- 高级病人- 新来
				events_que.push(vip_ari);//加入事件列
			}
		}
		*/
		while (1)
		{
			//cout << "test6" << endl;
			//瞬态坍缩
			while (!Pre_queue.empty()) //大厅队列还有人在排着呢（t now 瞬间 外队列向内队列的坍缩）
			{
				//std::cout << count++ << endl;
				//cout << "test10 : " << Pre_queue.size() << endl;
				int min_internal = common_Findmininter(Doctors, slice * modtnow);//定位最小医生

				if (Doctors[min_internal].internal >= M[(int)floor(Msli * modtnow)]) break;  //所有医生内队列都超过阈值，病人进入大厅队列

				if (Pre_queue.top().vip == 1)  //区分，因为普通病人和vip病人的入队选择机制不太一样
				{
					min_internal = vip_Find_min_inter(Doctors, slice * modtnow);
					Doctors[min_internal].vip_num++;
				}

				if (Doctors[min_internal].Doc_que.size() == 0)   //医生处没有人排队，可以直接生成这个病人被服务完的时间节点作为事件
				{
					Patient temp = Pre_queue.top();
					Pre_queue.pop();//病人删除

					temp.FCFS_time = tnow;   //病人是在tnow进入了队列

					if ((tnow - temp.arrivetime) > threshold)
					{
						if (temp.vip == 1) {
							vip_Overtimenum[temp._intari]++;  //初次等待时间超过阈值了
							//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
						}
						else {
							common_Overtimenum[temp._intari]++;
							//Static_Divide[1][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
						}
					}
					temp.first_come = false;
					temp.ons_waittime += tnow - temp.lasttime;  //这次它等了多久

					Doctors[min_internal].Doc_que.push(temp);
					Doctors[min_internal].internal++;
					Doctors[min_internal].next_finish_time = tnow + doc_log(rng) / 3600;  //代表这个医生服务完这个病人的时间

				}
				else if (Doctors[min_internal].Doc_que.size() != 0)   //前面有人排着，简单进队
				{
					Patient temp = Pre_queue.top();
					Pre_queue.pop();

					temp.FCFS_time = tnow;

					if (Doctors[min_internal].Doc_que.top().vip == 2 && temp.vip == 1)   //抢占规则   12.21wxt修改
					{
						Doctors[min_internal].next_finish_time = tnow + doc_log(rng) / 3600; //由于抢占了，更新服务完病人的时间  //这个更新可无
						if ((tnow - temp.arrivetime) > threshold)
						{
							vip_Overtimenum[temp._intari]++;
							//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
						}
						temp.first_come = false;
						temp.ons_waittime += tnow - temp.lasttime;  //这次它等了多久
						//医生队头普病，大厅队头vip病人，抢占
					//被插队病人进入等待状态，其lasttime需要更新？ debug
					}
					Doctors[min_internal].Doc_que.push(temp);
					Doctors[min_internal].internal++;
				}
			}

			double temp_time = DBL_MAX;
			int temp_type = -5;
			int temp_which = -5;
			if (!events_que.empty())
			{
				temp_time = events_que.top().time;
				temp_type = events_que.top().what_type;
				temp_which = events_que.top().which;
			}
			for (int i = 0; i < maxdocnum; i++)
			{
				if (Doctors[i].next_finish_time < temp_time)
				{
					temp_time = Doctors[i].next_finish_time;
					temp_type = 0;
					temp_which = i;//定位最小的下次时间医生。
				}
			}
			for (int i = 0; i < nursenum; i++)
			{
				if (Nurses[i].next_finish_time < temp_time)
				{
					temp_time = Nurses[i].next_finish_time;
					temp_type = 1;
					temp_which = i;//定位最近的检查台
				}
			}
			//由此得到最近的事件节点

			Event temp_thing(temp_time, temp_type, temp_which);  //代表了下一个事件时病人到达还是医生看完病人还是检查台看完病人？
			if (Doctors[0].Doc_que.size() != 0 && Doctors[0].next_finish_time == DBL_MAX) {
				cout << "当前时间：" << temp_time << " 修正：" << modtnow << " 种类：" << temp_type << " 哪里：" << temp_which <<
					" 0的人数：" << Doctors[0].internal << " 0的下一时间：" << Doctors[0].next_finish_time << " 0的队长：" << Doctors[0].Doc_que.size() <<
					" 外队列长：" << Pre_queue.size() << endl;
			}

			if (ifcout == 1) {
				cout << "当前时间：" << temp_time << " 修正：" << modtnow << " 种类：" << temp_type << " 哪里：" << temp_which <<
					" 0的人数：" << Doctors[0].internal << " 0的下一时间：" << Doctors[0].next_finish_time << " 0的队长：" << Doctors[0].Doc_que.size() <<
					" 外队列长：" << Pre_queue.size() << endl;
			}
			if (temp_thing.time == DBL_MAX || temp_thing.time >= (timenum * (double)round))  break;   //没有下一个事件或者到事件到第二天了
			//cout << "test1:" <<tnow<< endl;
			//应该先完成不可能状态到可能状态的转移，再进行时间的更新
			tnow = temp_thing.time; //更新仿真时钟
			modtnow = tnow - timenum * ((double)round - 1);

			if (temp_thing.what_type == -2) //普通病人到达分支开始
			{
				if (Pre_queue.size() >= MaxTotal) {

					common_Sumwaitnum[(int)floor(slice * modtnow)]++;   //这里统计每小时到达人数
					common_Overtimenum[(int)floor(slice * modtnow)]++;
					//Static_Divide[1][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					//Sum_Divide[1][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					events_que.pop();

					countFailIn[1]++;

					continue;
				}

				events_que.pop();

				Patient newpatient(tnow, tnow, (int)floor(slice * modtnow), 2);
				newpatient.FCFS_time = tnow;
				common_Sumwaitnum[(int)floor(slice * modtnow)]++;   //这里统计每小时到达人数

				//Sum_Divide[1][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;

				int min_internal = common_Findmininter(Doctors, slice * modtnow);
				if (Doctors[min_internal].internal >= M[(int)floor(Msli * modtnow)]) Pre_queue.push(newpatient);   //最小内队列无位置则大厅等候
				else   //反之进医生队
				{
					if (Doctors[min_internal].Doc_que.size() == 0)
					{
						Doctors[min_internal].next_finish_time = tnow + doc_log(rng) / 3600;
						newpatient.first_come = false;
					}
					Doctors[min_internal].Doc_que.push(newpatient);
					Doctors[min_internal].internal++;
				}
			}  //病人到达分支结束

			if (temp_thing.what_type == -1)  //vip病人到达
			{
				if (Pre_queue.size() >= MaxTotal) {

					vip_Sumwaitnum[(int)floor(slice * modtnow)]++;   //这里统计每小时到达人数
					vip_Overtimenum[(int)floor(slice * modtnow)]++;
					//Static_Divide[0][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					//Sum_Divide[0][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					events_que.pop();

					countFailIn[0]++;

					continue;
				}

				events_que.pop();

				Patient newpatient(tnow, tnow, (int)floor(slice * modtnow), 1);
				newpatient.FCFS_time = tnow;
				vip_Sumwaitnum[newpatient._intari]++;   //分开统计两类病人每小时达到的数量
				//Sum_Divide[0][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;

				int min_internal = common_Findmininter(Doctors, slice * modtnow);

				if (Doctors[min_internal].internal >= M[(int)floor(Msli * modtnow)]) Pre_queue.push(newpatient);
				else
				{
					min_internal = vip_Find_min_inter(Doctors, slice * modtnow);
					if (Doctors[min_internal].Doc_que.size() == 0 || Doctors[min_internal].Doc_que.top().vip == 2) //没人，或者是普通病人在队头，可以插队一下
					{
						Doctors[min_internal].next_finish_time = tnow + doc_log(rng) / 3600;
						newpatient.first_come = false;
					}
					Doctors[min_internal].Doc_que.push(newpatient);
					Doctors[min_internal].internal++;
					Doctors[min_internal].vip_num++;
					//被插队病人进入等待状态，其lasttime需要更新？ debug
				}
			}

			if (temp_thing.what_type == 0)   //医生分支开始
			{

				int y = temp_thing.which; //y就是哪个医生服务完病人了
				//cout << y << endl;

				double temp1 = Doctors[y].next_finish_time;
				if (Doctors[y].Doc_que.size() == 0) {
					cout << y << endl;
					cout << temp1 << endl;
					cout << modtnow << endl;
					cout << tnow << endl;
					cout << Schedule[(int)floor(slice * modtnow)] << endl;
					system("pause");
				}
				Doctors[y].next_finish_time = DBL_MAX;  //重置这个医生的下一个事件时间

				std::uniform_real_distribution<double> distribution(0, 1);
				double p = distribution(rng);
				int tmp11 = Doctors[y].Doc_que.top().first_check;
				if ((p <= probability1 && tmp11 == 0) || (p < probability2 && tmp11 == 1))    //病人外部检查
				{
					Patient temp = Doctors[y].Doc_que.top();
					temp.FCFS_time = tnow;
					temp.belongdoc = y;

					int minnur;   //代表了这个病人要去的检查台
					if (temp.vip == 1)
					{
						//minnur = vip_Find_min_nur(Nurses, nursenum);
						minnur = common_Findminnur(Nurses, nursenum);
						Nurses[minnur].vip_num++;
					}
					else if (temp.vip == 2)
					{
						minnur = common_Findminnur(Nurses, nursenum);   //找一下病人要进哪一个检查台队列
					}
					double chech = check(rng);
					double checktime = 0;
					if (chech < prob[0]) {
						checktime = max(ning_log(rng), max(sheng_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0])));
					}
					else if (chech < prob[1]) {
						checktime =  max(sheng_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0]));
					}
					else if (chech < prob[2]) {
						checktime = exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0]);
					}
					else if (chech < prob[3]) {
						checktime = sheng_log(rng);
					}
					else if (chech < prob[4]) {
						checktime = max(max(ning_log(rng), xueqi_log(rng)), max(sheng_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0])));
					}
					else if (chech < prob[5]) {
						checktime = exp(gsl_ran_logistic(r, kuai1[1]) + kuai1[0]);
					}
					else if (chech < prob[6]) {
						checktime = max(ning_log(rng), sheng_log(rng));
						
					}
					else if (chech < prob[7]) {
						checktime = max(ning_log(rng), max(xueqi_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0])));
					}
					else if (chech < prob[8]) {
						checktime = max(ning_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0]));;
					}
					else if (chech < prob[9]) {
						checktime = xueqi_log(rng);
					}
					else if (chech < prob[10]) {
						checktime = ning_log(rng);
					}
					else {
						checktime = max(xueqi_log(rng), sheng_log(rng));
					}
					if (Nurses[minnur].Nur_que.empty())
						Nurses[minnur].next_finish_time = tnow + checktime / 3600;
					else if (Nurses[minnur].Nur_que.top().vip == 2 && temp.vip == 1) //抢占
					{
						Nurses[minnur].next_finish_time = tnow + checktime / 3600;
					}
					Nurses[minnur].Nur_que.push(temp);

					if (Nurses[minnur].Nur_que.size() > 1) { cout << "warning hakjhkj " << endl; }

					Doctors[y].Doc_que.pop();

					if (Doctors[y].Doc_que.size() != 0) //前一个病人刚走，还有人在排队，那下一个
					{
						if (y < Schedule[(int)floor(slice * modtnow)]) {
							Patient temp = Doctors[y].Doc_que.top();
							Doctors[y].Doc_que.pop();

							if (temp.first_come == true) //病人是第一次看到医生，记录是否超时并置firstcome属性为false
							{
								temp.ons_waittime = temp.ons_waittime + tnow - temp.arrivetime;
								if ((tnow - temp.arrivetime) > threshold)
								{
									if (temp.vip == 1) {
										vip_Overtimenum[temp._intari]++;  //初次等待时间超过阈值了
										//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
									}
									else {
										common_Overtimenum[temp._intari]++;
										//Static_Divide[1][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
									}
								}
								temp.first_come = false;
							}
							else {
								temp.ons_waittime += tnow - temp.lasttime;
							}

							Doctors[y].Doc_que.push(temp);
							Doctors[y].next_finish_time = tnow + doc_log(rng) / 3600;
						}
						else {
							while (Doctors[y].Doc_que.size() != 0) {
								Patient tmp = Doctors[y].Doc_que.top();
								Doctors[y].Doc_que.pop();
								Doctors[y].internal--;
								tmp.FCFS_time = tnow;
								int tmpnum = common_Findmininter(Doctors, slice * modtnow);
								if (Doctors[tmpnum].Doc_que.size() == 0) {
									Doctors[tmpnum].next_finish_time = tnow + doc_log(rng) / 3600;
									if (tmp.first_come == true) {
										tmp.first_come = false;
										tmp.ons_waittime = tmp.ons_waittime + tnow - tmp.arrivetime;
									}
								}
								Doctors[tmpnum].Doc_que.push(tmp);
								Doctors[tmpnum].internal++;
							}
							Doctors[y].next_finish_time = DBL_MAX;
						}
					}
					else {
						Doctors[y].next_finish_time = DBL_MAX;
					}
				}
				else   //病人离开系统
				{
					//cout << "test1" << endl;
					//离开部分
					if (Doctors[y].Doc_que.top().vip == 1) Doctors[y].vip_num--;

					if (Doctors[y].Doc_que.top().vip == 1)
						vip_Sumwaittime[Doctors[y].Doc_que.top()._intari] += Doctors[y].Doc_que.top().ons_waittime;//离开病人的总等待时间
					else {
						common_Sumwaittime[Doctors[y].Doc_que.top()._intari] += Doctors[y].Doc_que.top().ons_waittime;//离开病人的总等待时间
						if (ifcout == 2) {
							cout << tnow << " " << modtnow << " " << Doctors[y].Doc_que.top().ons_waittime << " " << Doctors[y].Doc_que.top().first_check << " " << Pre_queue.size() << endl;
							//cout << "当前时间：" << temp_time << " 修正：" << modtnow << " 种类：" << temp_type << " 哪里：" << temp_which <<
								//" 0的人数：" << Doctors[0].internal << " 0的下一时间：" << Doctors[0].next_finish_time << " 0的队长：" << Doctors[0].Doc_que.size() <<
								//" 外队列长：" << Pre_queue.size() << endl;
						}

					}

					if (Doctors[y].Doc_que.empty()) {
						cout << "出大事儿了" << endl;
						cout << y << "  " << temp1 << endl;
						cout << "error ahjkh1 " << Doctors[0].next_finish_time << endl;
						cout << "error ahjkh1 " << Doctors[1].next_finish_time << endl;

						cout << "error ahjkh1 " << Doctors[0].internal << endl;
						cout << "error ahjkh1 " << Doctors[1].internal << endl;
						cout << "error ahjkdaaah1 " << Doctors[0].Doc_que.size() << endl;
						cout << "error ahjkdaaah1 " << Doctors[1].Doc_que.size() << endl;
						cout << "error ahjkdaaahss1 " << Pre_queue.size() << endl;

						system("pause");
					}

					Doctors[y].Doc_que.pop();  //医生外队列出队
					Doctors[y].internal--;   //内队列-1
					Doctors[y].next_finish_time = DBL_MAX;
					if (Doctors[y].internal == 0) {

					}
					if (Doctors[y].internal < 0) {//|| Doctors[y].internal>4
						cout << tnow << endl;
						cout << modtnow << endl;
						cout << "error ahjkh2 " << Doctors[0].internal << endl;
						cout << "error ahjkh2 " << Doctors[1].internal << endl;
						cout << "error ahjkdaaah2 " << Doctors[0].Doc_que.size() << endl;
						cout << "error ahjkdaaah2 " << Doctors[1].Doc_que.size() << endl;
						cout << "error ahjkdaaahss2 " << Pre_queue.size() << endl;

					}

					//if (Doctors[y].Doc_que.size() != 0)   //医生为下一个病人看病(瞬间被服务到了）
					if ((Doctors[y].Doc_que.size() != 0 && Doctors[y].Doc_que.top().vip == 1)//debug位置
						|| (Doctors[y].Doc_que.size() != 0 && Pre_queue.empty())
						|| (Doctors[y].Doc_que.size() != 0 && Pre_queue.top().vip != 1)
						)   //医生为内队列下一个病人看病 1 下一病人为高等级 2 下一病人为低等级但外队列没有高等级
					{
						//cout << "test3" << endl;
						if (y < Schedule[(int)floor(slice * modtnow)]) {
							Patient temp = Doctors[y].Doc_que.top();
							Doctors[y].Doc_que.pop();

							if (temp.first_come == true) //病人是第一次看到医生，记录是否超时并置firstcome属性为false
							{
								temp.ons_waittime += tnow - temp.arrivetime;
								if ((tnow - temp.arrivetime) > threshold)
								{
									if (temp.vip == 1) {
										vip_Overtimenum[temp._intari]++;  //初次等待时间超过阈值了
										//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
									}
									else {
										common_Overtimenum[temp._intari]++;
										//Static_Divide[1][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
									}
								}
								temp.first_come = false;
							}
							else {
								temp.ons_waittime += tnow - temp.lasttime;// to do  lasttime需要更新吗
							}

							Doctors[y].Doc_que.push(temp);
							Doctors[y].next_finish_time = tnow + doc_log(rng) / 3600;
						}
						else {
							while (Doctors[y].Doc_que.size() != 0) {
								Patient tmp = Doctors[y].Doc_que.top();
								Doctors[y].Doc_que.pop();
								Doctors[y].internal--;
								tmp.FCFS_time = tnow;
								int tmpnum = common_Findmininter(Doctors, slice * modtnow);
								if (Doctors[tmpnum].Doc_que.size() == 0) {
									Doctors[tmpnum].next_finish_time = tnow + doc_log(rng) / 3600;
									if (tmp.first_come == true) {
										tmp.first_come = false;
										tmp.ons_waittime += tnow - tmp.arrivetime;
									}
								}
								Doctors[tmpnum].Doc_que.push(tmp);
								Doctors[tmpnum].internal++;
							}
							Doctors[y].next_finish_time = DBL_MAX;
						}
					}
					else if (Doctors[y].Doc_que.size() != 0 && !Pre_queue.empty() && Pre_queue.top().vip == 1)// 内队列没有高等级，且外队列有高等级。 （外队列高等级插入进来）
					{


					}
					else if (Doctors[y].Doc_que.size() != 0) {
						cout << "error hjkhakh" << endl;
					}
					else
					{
						Doctors[y].next_finish_time = DBL_MAX;
					}

					//cout << "test2" << endl;

				}   //病人离开系统分支结束
			}   //医生分支结束

			if (temp_thing.what_type == 1)  //检查台分支开始
			{
				countcheck++;
				int y = temp_thing.which;  //是哪一个检查台？
				Nurses[y].next_finish_time = DBL_MAX;
				Patient temp = Nurses[y].Nur_que.top();
				temp.first_check++;
				if (temp.vip == 1) Nurses[y].vip_num--;
				Nurses[y].Nur_que.pop();
				temp.lasttime = tnow; //更新病人的lasttime，因为他马上要回到医生处排队了，需要统计新一轮的等待时间
				temp.FCFS_time = tnow;
				if (temp.belongdoc >= Schedule[(int)floor(slice * modtnow)]) {
					Doctors[temp.belongdoc].internal--;
					temp.belongdoc = common_Findmininter(Doctors, slice * modtnow);
					Doctors[temp.belongdoc].internal++;
				}
				if (Doctors[temp.belongdoc].Doc_que.size() == 0)
					Doctors[temp.belongdoc].next_finish_time = tnow + doc_log(rng) / 3600;  //没人排队，直接生成服务完时间
				else if (Doctors[temp.belongdoc].Doc_que.top().vip == 2 && temp.vip == 1) //抢占
				{
					Doctors[temp.belongdoc].next_finish_time = tnow + doc_log(rng) / 3600;
				}

				Doctors[temp.belongdoc].Doc_que.push(temp);  //病人回到原来的医生处

				if (!Nurses[y].Nur_que.empty()) {
					double chech = check(rng);
					double checktime = 0;
					if (chech < prob[0]) {
						checktime = max(ning_log(rng), max(sheng_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0])));
					}
					else if (chech < prob[1]) {
						checktime = max(sheng_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0]));
					}
					else if (chech < prob[2]) {
						checktime = exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0]);
					}
					else if (chech < prob[3]) {
						checktime = sheng_log(rng);
					}
					else if (chech < prob[4]) {
						checktime = max(max(ning_log(rng), xueqi_log(rng)), max(sheng_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0])));
					}
					else if (chech < prob[5]) {
						checktime = exp(gsl_ran_logistic(r, kuai1[1]) + kuai1[0]);
					}
					else if (chech < prob[6]) {
						checktime = max(ning_log(rng), sheng_log(rng));

					}
					else if (chech < prob[7]) {
						checktime = max(ning_log(rng), max(xueqi_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0])));
					}
					else if (chech < prob[8]) {
						checktime = max(ning_log(rng), exp(gsl_ran_logistic(r, xuechang[1]) + xuechang[0]));;
					}
					else if (chech < prob[9]) {
						checktime = xueqi_log(rng);
					}
					else if (chech < prob[10]) {
						checktime = ning_log(rng);
					}
					else {
						checktime = max(xueqi_log(rng), sheng_log(rng));
					}
					Nurses[y].next_finish_time = tnow + checktime / 3600;
				}
			}  //检查台分支结束
		}
	}

	//清空内存
	while (!Pre_queue.empty()) Pre_queue.pop();
	for (int i = 0; i < maxdocnum; i++) { while (!Doctors[i].Doc_que.empty()) Doctors[i].Doc_que.pop(); }
	for (int i = 0; i < nursenum; i++) { while (!Nurses[i].Nur_que.empty()) Nurses[i].Nur_que.pop(); }
	delete[]Doctors;
	while (!events_que.empty()) events_que.pop();
	int totalover = 0;
	for (int i = 0; i < slice * timenum; i++)
	{
		totalnum += common_Sumwaitnum[i];
		totaltime += common_Sumwaittime[i];
		totalover += common_Overtimenum[i];
		avewt[i] = common_Sumwaittime[i] / 1000;
		//cout << "第" << i << "时段普通到达人数：" << common_Sumwaitnum[i] << "    服务水平:" << 1 - (double)common_Overtimenum[i] / (double)common_Sumwaitnum[i] << "    等待时间:" << common_Sumwaittime[i] / (double)common_Sumwaitnum[i] << endl;
	}

	for (int i = 0; i < timenum; i++)
	{
		//cout << common_Sumwaitnum[i] << " " << Arrive_rate[i] << endl;
		//cout << "第" << i << "时段vip到达人数：" << vip_Sumwaitnum[i] << "    服务水平:" << 1 - (double)vip_Overtimenum[i] / (double)vip_Sumwaitnum[i] << "    等待时间:" << vip_Sumwaittime[i] / (double)vip_Sumwaitnum[i] << endl;
	}

	int count[2] = { 0 };

	for (int i = 0; i < timenum; i++) {
		//cout << "第" << i << "时段普通到达人数：" << endl;
		for (int s = 0; s < Static_Each_Period; s++)
		{
			//cout << "第" << double(i) + double(s) / double(Static_Each_Period) << "小时段普通到达人数:" << Sum_Divide[1][i * Static_Each_Period + s] << "    服务水平:" << 1 - (double)Static_Divide[1][i * Static_Each_Period + s] / (double)Sum_Divide[1][i * Static_Each_Period + s] << endl;
		}
	}

	for (int i = 0; i < timenum; i++) {
		//cout << "第" << i << "时段普通到达人数：" << endl;
		for (int s = 0; s < Static_Each_Period; s++)
		{
			//cout << "第" << double(i) + double(s) / double(Static_Each_Period) << "小时段高等级到达人数:" << Sum_Divide[0][i * Static_Each_Period + s] << "    服务水平:" << 1 - (double)Static_Divide[0][i * Static_Each_Period + s] / (double)Sum_Divide[0][i * Static_Each_Period + s] << endl;
		}
	}

	for (int i = 0; i < timenum; i++) {
		for (int s = 0; s < Static_Each_Period; s++)
		{
			//count[1] += Sum_Divide[1][i * timenum + s];
			//count[0] += Sum_Divide[0][i * timenum + s];
		}
	}

	//cout << totalnum << "  " << count[1] << endl;
	//cout << count[0] << endl;
	//cout << countFailIn[0] << " : " << countFailIn[1] << "  " << count[0] << endl;

	avgwaittime = totaltime / (double)totalnum;
	cout << totaltime << " " << totalnum << " " << avgwaittime << " " << totalover << " " << countcheck << endl;
	return avgwaittime;
}
//2为下班看完内队列病人再走
double Run_Case_Manager_2(int workhours1[], int workhours2[], int Schedule[], int M[], int Circle = totalday)
{
	totalload = 0;
	for (int i = 0; i < slice * timenum; i++) {
		workload[i] = 0;
	}
	::std::mt19937 rng(1);
	cout << "开始仿真2" << endl;
	std::exponential_distribution<double> doc_expo(miu1);//?
	std::exponential_distribution<double> nur_expo(miu2);//?
	int maxdocnum = docnum * day;   //寻找现行排班方案最大医生数量

	double tnow = 0;
	double modtnow = 0;
	double common_Sumwaittime[slice * timenum] = { 0 };  //普通病人： 等待时间统计
	int common_Sumwaitnum[slice * timenum] = { 0 };      //普通病人： 可统计每小时到达系统的人数
	int common_Overtimenum[slice * timenum] = { 0 };     //普通病人： 第一次看到医生超时的病人数

	double vip_Sumwaittime[slice * timenum] = { 0 };     //优先病人： 等待时间统计
	int vip_Sumwaitnum[slice * timenum] = { 0 };         //优先病人： 可统计每小时到达系统的人数
	int vip_Overtimenum[slice * timenum] = { 0 };        //优先病人： 第一次看到医生超时的病人数

	double totaltime = 0;
	int totalnum = 0;
	double avgwaittime = 0;
	int totalover = 0;

	int countFailIn[2] = { 0 };

	Nurse Nurses[nursenum];
	Doctor* Doctors = new Doctor[maxdocnum];
	for (int i = 0; i < maxdocnum; i++) {
		Doctors[i].workhour2 = workhours2[i];
		Doctors[i].workhour1 = workhours1[i];
		//cout <<Doctors[i].workhour1<<" "<< Doctors[i].workhour2 << endl;
	}
	//system("pause");
	priority_queue<Patient, vector<Patient>, patient_compare> Pre_queue;   //病人队列
	priority_queue<Event, vector<Event>, event_time_compare> events_que;   //时间队列

	//事件驱动的仿真规则：先生成所有病人到达时间，然后每个医生或检查台开始服务就生成一个这个病人服务完的时间，用一个函数取病人到达、医生服务完病人、检查台服务完病人中最早发生的那个作为下一个事件。以此驱动代码运行

	for (int round = 1; round <= Circle; round++) //仿真正式开始，连续进行round天
	{
		//cout << "test5" << endl;

		for (int i = 0; i < timenum; i++)  //一天的开始，先把当天所有普通病人到达的事件先塞进队
		{
			std::uniform_real_distribution<double> uni(timenum * ((double)round - 1) + i, timenum * ((double)round - 1) + i + 1);
			std::poisson_distribution<int> poi(Arrive_rate[i]);
			int size = poi(rng);// 使用rng引擎获得poi分布。
			for (int j = 1; j <= size; j++)  //生成size个均匀分布时刻
			{
				Event p_ari(uni(rng), -2, -1);//  到达时间- 普通病人- 新来
				events_que.push(p_ari);//加入事件列
			}
		}
		/*
		for (int i = 0; i < timenum; i++)  //再塞vip病人---先后顺序对结果有0.5%影响-- 随着仿真次数增加而减少
		{
			std::uniform_real_distribution<double> vip_uni(double(timenum) * (round - 1) + i, double(timenum) * (round - 1) + i + 1);
			std::poisson_distribution<int> poi(vip_Arrive_rate[i]);
			int size = poi(rng);
			for (int j = 1; j <= size; j++)  //生成size个均匀分布时刻
			{
				Event vip_ari(vip_uni(rng), -1, -1);//  到达时间- 高级病人- 新来
				events_que.push(vip_ari);//加入事件列
			}
		}
		*/
		while (1)
		{
			//cout << "test6" << endl;
			//瞬态坍缩
			while (!Pre_queue.empty()) //大厅队列还有人在排着呢（t now 瞬间 外队列向内队列的坍缩）
			{
				//std::cout << count++ << endl;
				//cout << "test10 : " << Pre_queue.size() << endl;
				int min_internal = common_Findmininter(Doctors, slice * modtnow);//定位最小医生

				if (Doctors[min_internal].internal >= M[(int)floor(Msli * modtnow)]) break;  //所有医生内队列都超过阈值，病人进入大厅队列

				if (Pre_queue.top().vip == 1)  //区分，因为普通病人和vip病人的入队选择机制不太一样
				{
					min_internal = vip_Find_min_inter(Doctors, slice * modtnow);
					Doctors[min_internal].vip_num++;
				}

				if (Doctors[min_internal].Doc_que.size() == 0)   //医生处没有人排队，可以直接生成这个病人被服务完的时间节点作为事件
				{
					Patient temp = Pre_queue.top();
					Pre_queue.pop();//病人删除

					temp.FCFS_time = tnow;   //病人是在tnow进入了队列

					if ((tnow - temp.arrivetime) > threshold)
					{
						if (temp.vip == 1) {
							vip_Overtimenum[temp._intari]++;  //初次等待时间超过阈值了
							Static_Divide[0][int((temp.arrivetime - double((double)round - 1) * timenum) * double(Static_Each_Period))]++;
						}
						else {
							common_Overtimenum[temp._intari]++;
							//Static_Divide[1][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
						}
					}
					temp.first_come = false;
					temp.ons_waittime += tnow - temp.lasttime;  //这次它等了多久

					Doctors[min_internal].Doc_que.push(temp);
					Doctors[min_internal].internal++;
					Doctors[min_internal].next_finish_time = tnow + doc_expo(rng);  //代表这个医生服务完这个病人的时间

				}
				else if (Doctors[min_internal].Doc_que.size() != 0)   //前面有人排着，简单进队
				{
					Patient temp = Pre_queue.top();
					Pre_queue.pop();

					temp.FCFS_time = tnow;

					if (Doctors[min_internal].Doc_que.top().vip == 2 && temp.vip == 1)   //抢占规则   12.21wxt修改
					{
						Doctors[min_internal].next_finish_time = tnow + doc_expo(rng); //由于抢占了，更新服务完病人的时间  //这个更新可无
						if ((tnow - temp.arrivetime) > threshold)
						{
							vip_Overtimenum[temp._intari]++;
							//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
						}
						temp.first_come = false;
						temp.ons_waittime += tnow - temp.lasttime;  //这次它等了多久
						//医生队头普病，大厅队头vip病人，抢占
					//被插队病人进入等待状态，其lasttime需要更新？ debug


					}
					Doctors[min_internal].Doc_que.push(temp);
					Doctors[min_internal].internal++;
				}
			}

			double temp_time = DBL_MAX;
			int temp_type = -5;
			int temp_which = -5;
			if (!events_que.empty())
			{
				temp_time = events_que.top().time;
				temp_type = events_que.top().what_type;
				temp_which = events_que.top().which;
			}
			for (int i = 0; i < maxdocnum; i++)
			{
				if (Doctors[i].next_finish_time < temp_time)
				{
					temp_time = Doctors[i].next_finish_time;
					temp_type = 0;
					temp_which = i;//定位最小的下次时间医生。
				}
			}

			for (int i = 0; i < nursenum; i++)
			{
				if (Nurses[i].next_finish_time < temp_time)
				{
					temp_time = Nurses[i].next_finish_time;
					temp_type = 1;
					temp_which = i;//定位最近的检查台
				}
			}
			//由此得到最近的事件节点

			Event temp_thing(temp_time, temp_type, temp_which);  //代表了下一个事件时病人到达还是医生看完病人还是检查台看完病人？

			if (temp_thing.time == DBL_MAX || temp_thing.time >= (double(timenum) * round))  break;   //没有下一个事件或者到事件到第二天了

			//应该先完成不可能状态到可能状态的转移，再进行时间的更新
			tnow = temp_thing.time; //更新仿真时钟
			modtnow = tnow - double(timenum) * ((double)round - 1);

			if (temp_thing.what_type == -2) //普通病人到达分支开始
			{
				if (Pre_queue.size() >= MaxTotal) {

					common_Sumwaitnum[(int)floor(slice * modtnow)]++;   //这里统计每小时到达人数
					common_Overtimenum[(int)floor(slice * modtnow)]++;
					//Static_Divide[1][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					//Sum_Divide[1][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					events_que.pop();

					countFailIn[1]++;

					continue;
				}

				events_que.pop();

				Patient newpatient(tnow, tnow, (int)floor(slice * modtnow), 2);
				newpatient.FCFS_time = tnow;
				common_Sumwaitnum[(int)floor(slice * modtnow)]++;   //这里统计每小时到达人数

				//Sum_Divide[1][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;

				int min_internal = common_Findmininter(Doctors, slice * modtnow);
				if (Doctors[min_internal].internal >= M[(int)floor(Msli * modtnow)]) Pre_queue.push(newpatient);   //最小内队列无位置则大厅等候
				else   //反之进医生队
				{
					if (Doctors[min_internal].Doc_que.size() == 0)
					{
						Doctors[min_internal].next_finish_time = tnow + doc_expo(rng);
						newpatient.first_come = false;
					}
					Doctors[min_internal].Doc_que.push(newpatient);
					Doctors[min_internal].internal++;
				}
			}  //病人到达分支结束

			//cout << "test8" << endl;

			if (temp_thing.what_type == -1)  //vip病人到达
			{
				if (Pre_queue.size() >= MaxTotal) {

					vip_Sumwaitnum[(int)floor(slice * modtnow)]++;   //这里统计每小时到达人数
					vip_Overtimenum[(int)floor(slice * modtnow)]++;
					//Static_Divide[0][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					//Sum_Divide[0][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;
					events_que.pop();

					countFailIn[0]++;

					continue;
				}

				events_que.pop();

				Patient newpatient(tnow, tnow, (int)floor(slice * modtnow), 1);
				newpatient.FCFS_time = tnow;
				vip_Sumwaitnum[newpatient._intari]++;   //分开统计两类病人每小时达到的数量
				//Sum_Divide[0][int((tnow - double(round - 1) * timenum) * double(Static_Each_Period))]++;


				int min_internal = common_Findmininter(Doctors, slice * modtnow);

				if (Doctors[min_internal].internal >= M[(int)floor(Msli * modtnow)]) Pre_queue.push(newpatient);
				else
				{
					min_internal = vip_Find_min_inter(Doctors, slice * modtnow);
					if (Doctors[min_internal].Doc_que.size() == 0 || Doctors[min_internal].Doc_que.top().vip == 2) //没人，或者是普通病人在队头，可以插队一下
					{
						Doctors[min_internal].next_finish_time = tnow + doc_expo(rng);
						newpatient.first_come = false;
					}
					Doctors[min_internal].Doc_que.push(newpatient);
					Doctors[min_internal].internal++;
					Doctors[min_internal].vip_num++;
					//被插队病人进入等待状态，其lasttime需要更新？ debug
				}
			}

			//cout << "test9" << endl;

			if (temp_thing.what_type == 0)   //医生分支开始
			{
				int y = temp_thing.which; //y就是哪个医生服务完病人了
				//cout << y << endl;

				double temp1 = Doctors[y].next_finish_time;
				Doctors[y].next_finish_time = DBL_MAX;  //重置这个医生的下一个事件时间

				std::uniform_real_distribution<double> distribution(0, 1);
				double p = distribution(rng);
				int tmp11 = Doctors[y].Doc_que.top().first_check;
				if ((p <= probability1 && tmp11 == 0) || (p < probability2 && tmp11 == 1))//if (p <= probability && Doctors[y].Doc_que.top().first_check <maxcheck)    //病人外部检查
				{
					Patient temp = Doctors[y].Doc_que.top();
					temp.FCFS_time = tnow;
					temp.belongdoc = y;

					int minnur;   //代表了这个病人要去的检查台
					if (temp.vip == 1)
					{
						//minnur = vip_Find_min_nur(Nurses, nursenum);
						minnur = common_Findminnur(Nurses, nursenum);
						Nurses[minnur].vip_num++;
					}
					else if (temp.vip == 2)
					{
						minnur = common_Findminnur(Nurses, nursenum);   //找一下病人要进哪一个检查台队列
					}
					if (Nurses[minnur].Nur_que.empty())
						Nurses[minnur].next_finish_time = tnow + nur_expo(rng);
					else if (Nurses[minnur].Nur_que.top().vip == 2 && temp.vip == 1) //抢占
					{
						Nurses[minnur].next_finish_time = tnow + nur_expo(rng);
					}
					Nurses[minnur].Nur_que.push(temp);

					if (Nurses[minnur].Nur_que.size() > 1) { cout << "warning hakjhkj " << endl; }

					Doctors[y].Doc_que.pop();

					if (Doctors[y].Doc_que.size() != 0) //前一个病人刚走，还有人在排队，那下一个
					{
						Patient temp = Doctors[y].Doc_que.top();
						Doctors[y].Doc_que.pop();

						if (temp.first_come == true) //病人是第一次看到医生，记录是否超时并置firstcome属性为false
						{
							temp.ons_waittime += tnow - temp.arrivetime;
							if ((tnow - temp.arrivetime) > threshold)
							{
								if (temp.vip == 1) {
									vip_Overtimenum[temp._intari]++;  //初次等待时间超过阈值了
									//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
								}
								else {
									common_Overtimenum[temp._intari]++;
									//Static_Divide[1][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
								}
							}
							temp.first_come = false;
						}
						else {
							temp.ons_waittime += tnow - temp.lasttime;
						}
						Doctors[y].Doc_que.push(temp);
						Doctors[y].next_finish_time = tnow + doc_expo(rng);
					}
				}
				else   //病人离开系统
				{
					//cout << "test1" << endl;
					//离开部分
					if (Doctors[y].Doc_que.top().vip == 1) Doctors[y].vip_num--;

					if (Doctors[y].Doc_que.top().vip == 1)
						vip_Sumwaittime[Doctors[y].Doc_que.top()._intari] += Doctors[y].Doc_que.top().ons_waittime;//离开病人的总等待时间
					else
						common_Sumwaittime[Doctors[y].Doc_que.top()._intari] += Doctors[y].Doc_que.top().ons_waittime;//离开病人的总等待时间

					if (Doctors[y].Doc_que.empty()) {
						cout << "出大事儿了" << endl;
						cout << y << "  " << temp1 << endl;
						cout << "error ahjkh3 " << Doctors[0].next_finish_time << endl;
						cout << "error ahjkh3 " << Doctors[1].next_finish_time << endl;

						cout << "error ahjkh3 " << Doctors[0].internal << endl;
						cout << "error ahjkh3 " << Doctors[1].internal << endl;
						cout << "error ahjkdaaah3 " << Doctors[0].Doc_que.size() << endl;
						cout << "error ahjkdaaah3 " << Doctors[1].Doc_que.size() << endl;
						cout << "error ahjkdaaahss3 " << Pre_queue.size() << endl;


						system("pause");
					}

					Doctors[y].Doc_que.pop();  //医生外队列出队
					Doctors[y].internal--;   //内队列-1
					Doctors[y].next_finish_time = DBL_MAX;

					if (Doctors[y].internal == 0) {
						if (Doctors[y].workhour1 > (int)floor(slice * modtnow)) {
							workload[(int)floor(slice * modtnow)] += modtnow + timenum - double(Doctors[y].workhour2) / double(slice);
							//cout << modtnow<<" "<< Doctors[y].workhour1 <<" "<< Doctors[y].workhour2 <<" "<<modtnow + timenum - double(Doctors[y].workhour2) / double(slice) << endl;

							//cout << modtnow + timenum - Doctors[y].workhour2 <<" "<< modtnow<<" "<<y<<" "<<Doctors[y].workhour2<<endl;
							//system("pause");
						}
						else if (Doctors[y].workhour2 <= (int)floor(slice * modtnow)) {
							workload[(int)floor(slice * modtnow)] += modtnow - double(Doctors[y].workhour2) / double(slice);
							//cout << modtnow - Doctors[y].workhour2<<" " <<modtnow << " " << y << " " << Doctors[y].workhour2 << endl;
							//system("pause");
						}
					}

					if (Doctors[y].internal < 0) {// || Doctors[y].internal>4
						cout << "error ahjkh4 " << Doctors[0].internal << endl;
						cout << "error ahjkh4 " << Doctors[1].internal << endl;
						cout << "error ahjkdaaah4 " << Doctors[0].Doc_que.size() << endl;
						cout << "error ahjkdaaah4 " << Doctors[1].Doc_que.size() << endl;
						cout << "error ahjkdaaahss4 " << Pre_queue.size() << endl;

					}

					//if (Doctors[y].Doc_que.size() != 0)   //医生为下一个病人看病(瞬间被服务到了）
					if ((Doctors[y].Doc_que.size() != 0 && Doctors[y].Doc_que.top().vip == 1)//debug位置
						|| (Doctors[y].Doc_que.size() != 0 && Pre_queue.empty())
						|| (Doctors[y].Doc_que.size() != 0 && Pre_queue.top().vip != 1)
						)   //医生为内队列下一个病人看病 1 下一病人为高等级 2 下一病人为低等级但外队列没有高等级
					{
						//cout << "test3" << endl;

						Patient temp = Doctors[y].Doc_que.top();
						Doctors[y].Doc_que.pop();

						if (temp.first_come == true) //病人是第一次看到医生，记录是否超时并置firstcome属性为false
						{
							temp.ons_waittime += tnow - temp.arrivetime;
							if ((tnow - temp.arrivetime) > threshold)
							{
								if (temp.vip == 1) {
									vip_Overtimenum[temp._intari]++;  //初次等待时间超过阈值了
									//Static_Divide[0][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
								}
								else {
									common_Overtimenum[temp._intari]++;
									//Static_Divide[1][int((temp.arrivetime - double(round - 1) * timenum) * double(Static_Each_Period))]++;
								}
							}
							temp.first_come = false;
						}
						else {
							temp.ons_waittime += tnow - temp.lasttime;// to do  lasttime需要更新吗
						}

						Doctors[y].Doc_que.push(temp);
						Doctors[y].next_finish_time = tnow + doc_expo(rng);
					}
					else if (Doctors[y].Doc_que.size() != 0 && !Pre_queue.empty() && Pre_queue.top().vip == 1)// 内队列没有高等级，且外队列有高等级。 （外队列高等级插入进来）
					{


					}
					else if (Doctors[y].Doc_que.size() != 0) {
						cout << "error hjkhakh" << endl;
					}
					else
					{
						Doctors[y].next_finish_time = DBL_MAX;
					}

					//cout << "test2" << endl;

				}   //病人离开系统分支结束
			}   //医生分支结束

			if (temp_thing.what_type == 1)  //检查台分支开始
			{
				int y = temp_thing.which;  //是哪一个检查台？
				Nurses[y].next_finish_time = DBL_MAX;
				Patient temp = Nurses[y].Nur_que.top();
				temp.first_check++;
				if (temp.vip == 1) Nurses[y].vip_num--;
				Nurses[y].Nur_que.pop();
				temp.lasttime = tnow; //更新病人的lasttime，因为他马上要回到医生处排队了，需要统计新一轮的等待时间
				temp.FCFS_time = tnow;
				if (Doctors[temp.belongdoc].Doc_que.size() == 0)
					Doctors[temp.belongdoc].next_finish_time = tnow + doc_expo(rng);  //没人排队，直接生成服务完时间
				else if (Doctors[temp.belongdoc].Doc_que.top().vip == 2 && temp.vip == 1) //抢占
				{
					Doctors[temp.belongdoc].next_finish_time = tnow + doc_expo(rng);
				}
				Doctors[temp.belongdoc].Doc_que.push(temp);  //病人回到原来的医生处
				if (!Nurses[y].Nur_que.empty())   Nurses[y].next_finish_time = tnow + nur_expo(rng);
			}  //检查台分支结束
		}
	}

	//清空内存
	while (!Pre_queue.empty()) Pre_queue.pop();
	for (int i = 0; i < maxdocnum; i++) { while (!Doctors[i].Doc_que.empty()) Doctors[i].Doc_que.pop(); }
	for (int i = 0; i < nursenum; i++) { while (!Nurses[i].Nur_que.empty()) Nurses[i].Nur_que.pop(); }
	delete[]Doctors;
	while (!events_que.empty()) events_que.pop();

	for (int i = 0; i < slice * timenum; i++)
	{
		totalnum += common_Sumwaitnum[i];
		totaltime += common_Sumwaittime[i];
		totalover += common_Overtimenum[i];
		totalload += workload[i];
		//cout << totaltime << endl;
		//cout << "第" << i << "时段普通到达人数：" << common_Sumwaitnum[i] << "    服务水平:" << 1 - (double)common_Overtimenum[i] / (double)common_Sumwaitnum[i] << "    等待时间:" << common_Sumwaittime[i] / (double)common_Sumwaitnum[i] << endl;
	}

	for (int i = 0; i < timenum; i++)
	{
		//cout << "第" << i << "时段vip到达人数：" << vip_Sumwaitnum[i] << "    服务水平:" << 1 - (double)vip_Overtimenum[i] / (double)vip_Sumwaitnum[i] << "    等待时间:" << vip_Sumwaittime[i] / (double)vip_Sumwaitnum[i] << endl;
	}

	int count[2] = { 0 };
	//cout << endl;

	for (int i = 0; i < timenum; i++) {
		//cout << "第" << i << "时段普通到达人数：" << endl;
		for (int s = 0; s < Static_Each_Period; s++)
		{
			//cout << "第" << double(i) + double(s) / double(Static_Each_Period) << "小时段普通到达人数:" << Sum_Divide[1][i * Static_Each_Period + s] << "    服务水平:" << 1 - (double)Static_Divide[1][i * Static_Each_Period + s] / (double)Sum_Divide[1][i * Static_Each_Period + s] << endl;
		}
	}

	for (int i = 0; i < timenum; i++) {
		//cout << "第" << i << "时段普通到达人数：" << endl;
		for (int s = 0; s < Static_Each_Period; s++)
		{
			//cout << "第" << double(i) + double(s) / double(Static_Each_Period) << "小时段高等级到达人数:" << Sum_Divide[0][i * Static_Each_Period + s] << "    服务水平:" << 1 - (double)Static_Divide[0][i * Static_Each_Period + s] / (double)Sum_Divide[0][i * Static_Each_Period + s] << endl;
		}
	}
	for (int i = 0; i < timenum; i++) {
		for (int s = 0; s < Static_Each_Period; s++)
		{
			//count[1] += Sum_Divide[1][i * timenum + s];
			//count[0] += Sum_Divide[0][i * timenum + s];
		}
	}

	//cout << totalnum << "  " << count[1] << "  " << count[0] << endl;
	//cout << countFailIn[0] << " : " << countFailIn[1] << "  " << count[0] << endl;

	avgwaittime = totaltime / (double)totalnum;
	cout << totaltime << " " << totalnum << " " << avgwaittime << " " << totalover << " " << totalload / totalday << endl;
	return  avgwaittime;
}

void convert(int a1[], int a2[], int tmpsche[]) {
	for (int i = 0; i < docnum * day; i++) {
		if (a1[i] == 0) {
			tmpsche[0]++;
		}
	}
	for (int i = 1; i < slice * timenum; i++) {
		int tmp = 0;
		for (int j = 0; j < docnum * day; j++) {
			if (a1[j] == i) {
				tmp = tmp + 1;
			}
			if (a2[j] == i) {
				tmp = tmp - 1;
			}
		}
		tmpsche[i] = tmpsche[i - 1] + tmp;
	}
}

void merge(int arr[], int arr2[], int l1, int mid, int r2, int temp[], int temp2[]) {
	int i = l1, j = mid + 1;
	int p = 0;
	while (i <= mid && j <= r2) {
		if (arr[i] > arr[j]) {
			temp[p] = arr[j];
			temp2[p] = arr2[j];
			p++;
			j++;
		}
		else {
			temp[p] = arr[i];
			temp2[p] = arr2[i];
			p++;
			i++;
		}
	}

	while (i <= mid) {
		temp[p] = arr[i];
		temp2[p] = arr2[i];
		p++;
		i++;
	}
	while (j <= r2) {
		temp[p] = arr[j];
		temp2[p] = arr2[j];
		p++;
		j++;
	}
	//覆盖数组
	int n = r2 - l1 + 1;
	for (int i = 0; i < n; i++) {
		arr[l1 + i] = temp[i];
		arr2[l1 + i] = temp2[i];
		temp[i] = 0;
		temp2[i] = 0;
	}

}

void mergeSort(int arr[], int arr2[], int left, int right, int temp[], int temp2[]) {
	if (left < right) {
		int mid = (left + right) / 2;
		mergeSort(arr, arr2, left, mid, temp, temp2);     //分
		mergeSort(arr, arr2, mid + 1, right, temp, temp2);
		merge(arr, arr2, left, mid, right, temp, temp2); //合
	}

}
//将数组1的值复制给数组2
void copyarray(int a1[], int a2[], int a3 = docnum * day) {
	for (int i = 0; i < a3; i++) {
		a2[i] = a1[i];
	}
}

int checkfeasibility(int a1[], int a2[], int a3[]) {
	//printout(a1, a2, a3);
	int j = 0;
	for (int i = 0; i < day; i++) {
		nightshift[i] = 0;
		rest[i] = 0;
		shifts[i] = 0;
	}
	int tmpsche[slice * timenum] = { 0 };
	convert(a1, a2, tmpsche);
	//基础排班合理性
	int totalrest = 0;
	int totalshifts = 0;
	int totalnight = 0;
	for (int i = 0; i < docnum * day && j == 0; i++) {
		int tmp = a2[i] - a1[i];
		int day1 = a1[i] / (slice * timenum / day);
		int day2 = a2[i] / (slice * timenum / day);
		int hour1 = a1[i] - day1 * slice * (timenum / day);
		int hour2 = a2[i] - day2 * slice * (timenum / day);
		if (tmp > slice * maxwork || (tmp < slice * minwork && tmp != 0) || (day1 < day2 && hour2 != 0) || a1[i] < 0) {
			//cout << "wrong11 " <<i<< endl;
			j = 1;
			//break;
		}
		if (hour1 == slice * nightstart && tmp != slice * nightlong) {
			//cout << "wrong2" << endl;
			j = 1;
			//break;
		}
		if (hour2 == 0 && hour1 != slice * nightstart) {
			j = 1;
		}
		if (hour2 > slice * nightstart && hour2 < slice * timenum / day) {
			//cout << "wrong3" << endl;
			j = 1;
			//break;
		}
		if (hour1 > slice * nightstart && hour1 < slice * timenum / day) {
			//cout << "wrong3" << endl;
			j = 1;
			//break;
		}
		if (tmp == 0) {
			rest[day1]++;
		}
		if (hour1 == slice * nightstart && tmp == slice * nightlong) {
			nightshift[day1]++;
		}
		shifts[day1]++;
	}
	for (int i = 0; i < day; i++) {
		//cout << shifts[i] << " " << rest[i] << " " << nightshift[i] << endl;
	}
	//周排班合理性
	for (int i = 0; i < day && j == 0; i++) {
		totalrest += rest[i];
		totalnight += nightshift[i];
		//一个医生一天最多值一班
		if (shifts[i] > docnum) {
			j = 1;
			//cout << "wrong4" <<shifts[i]<< endl;
		}
		//夜班后要有休息一天
		if (i >= 1) {
			if (rest[i] < nightshift[i - 1]) {
				j = 1;
				//cout << "wrong5" <<rest[i]<<" " <<nightshift[i-1]<<endl;
			}
		}
		if (rest[i] < 1 || nightshift[i] < 1) {
			j = 1;
		}
	}
	if (rest[0] < nightshift[day - 1]) {
		j = 1;
		//cout << "wrong6" << endl;
	}
	//一个医生一周最多maxnight个夜班
	if (totalnight > maxnight * docnum) {
		j = 1;
		//cout << "wrong3" << endl;
	}
	//一个医生一周至少休息一天
	if (totalrest < docnum + totalnight) {
		j = 1;
		//cout << "wrong3" << endl;
	}
	//if (j == 1) { cout << "wrong1" << endl; }
	//每时刻都有医生上班,且M大于2
	for (int i = 0; i < Msli * timenum && j == 0; i++) {
		if (a3[i] < 2) {
			j = 1;
			break;
		}
	}
	for (int i = 0; i < slice * timenum && j == 0; i++) {
		if (tmpsche[i] <= 0) {
			//cout << i << endl;
			//system("pause");
			j = 1;
			break;
		}
	}
	if (j == 1) {
		cout << "infeas" << endl;
	}
	//system("pause"); 
	return j;
}

void primarysolution(int a, int pri_time, double b[], int s[], double mu = 1.0, int m = 6) {

	for (int i = 0; i < Msli * timenum; i++) {
		M[i] = m;
	}
	for (int i = 0; i < docnum * day; i++) {
		workhours1[i] = slice * workhours1[i];
		workhours2[i] = slice * workhours2[i];
	}
	if (day == 1) {
		if (checkfeasibility(workhours1, workhours2, M) == 1) {
			cout << "给定解不可行" << endl;
			//system("pause");
			for (int i = 0; i < docnum * day; i++) {

				if (workhours1[i] == slice * nightstart) {
					workhours2[i] = workhours1[i] + nightlong * slice;
				}
				else {
					workhours2[i] = workhours1[i] + slice * pri_time;
				}
				if (workhours2[i] > slice * timenum) {
					workhours2[i] = slice * timenum;
				}
			}
		}
	}
	else {
		//周排班暂时先给可行初始解
		int tmpj = 0;
		int tmpi = 0;
		for (int i = 0; i < docnum * day; i++) {
			tmpj = i % docnum;
			tmpi = i / docnum;
			if (tmpj < s[0]) {//白班1
				workhours1[i] = timenum / day * slice * tmpi;
				workhours2[i] = workhours1[i] + pri_time * slice;
			}
			else if (tmpj < s[1]) {//白班2
				workhours1[i] = timenum / day * slice * tmpi + pri_time * slice;
				workhours2[i] = workhours1[i] + pri_time * slice;
			}
			else if (tmpj < s[2]) {//夜班
				workhours1[i] = timenum / day * slice * tmpi + nightstart * slice;
				workhours2[i] = workhours1[i] + nightlong * slice;
			}
			else {//休息
				workhours1[i] = timenum / day * slice * tmpi + resthour * slice;
				workhours2[i] = workhours1[i];
			}
		}
	}

	int tmpsche[slice * timenum] = { 0 };
	convert(workhours1, workhours2, tmpsche);
	for (int i = 0; i < slice * timenum; i++) {
		Schedule[i] = tmpsche[i];
	}
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < timenum / day; j++) {
			Arrive_rate[i * timenum / day + j] = Arrive_rate[j] * b[i];
		}
	}
	double test1 = 0;
	for (int i = 0; i < timenum; i++) {
		Arrive_rate[i] = Arrive_rate[i] * mu;
		//cout << Arrive_rate[i] << ",";
		test1 += Arrive_rate[i];
		//if (Arrive_rate[i] > test1) test1 = Arrive_rate[i];
		//cout << test1 << endl;
	}
	//cout << "总到达率为：" << test1 << endl;

	if (checkfeasibility(workhours1, workhours2, M) == 1) {
		system("pause");
	}
	/*
	copyarray(workhours1, bestwh1);
	copyarray(workhours2, bestwh2);
	copyarray(M, bestM, timenum);
	*/
	if (1) {
		//int Schedule1[timenum] = { 2,3,3,3,3,3,3,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,3,2,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,3,3,3,2,3,2,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,3,2,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,3,3,3,4,4,3,3,4,4,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,4,3,2,3,3,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,4,4,3,3,3,2,3,3,3,1,1,1,1,1,1,1,1,1,1,1 };
		//int Schedule1[timenum] = { 2,2,2,3,3,3,3,3,3,2,3,3,2,2,2,1,1,1,1,1,1,1,1,1,2,2,3,3,3,3,3,3,2,3,2,3,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,3,3,3,3,2,3,2,2,3,2,2,2,1,1,1,1,1,1,1,1,1,2,2,3,3,3,3,3,3,3,2,3,2,2,2,2,1,1,1,1,1,1,1,1,1,3,3,3,4,4,3,3,4,4,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,3,3,3,2,2,3,2,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,2,3,3,2,2,3,2,2,1,1,1,1,1,1,1,1,1 };
		//int Schedule1[timenum] = { 2,3,3,3,3,3,2,2,3,2,2,3,3,2,2,1,1,1,1,1,1,1,1,1,2,2,2,3,3,3,3,3,3,2,2,3,2,2,2,1,1,1,1,1,1,1,1,1,2,2,3,3,3,3,2,3,3,2,2,3,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,3,4,4,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,3,3,3,3,4,4,3,4,4,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,2,3,3,3,3,4,3,3,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,2,3,3,3,3,4,3,2,2,3,2,2,2,1,1,1,1,1,1,1,1,1 };
		//int Schedule1[timenum] = { 2,2,2,2,4,4,3,3,3,2,3,3,3,1,1,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,3,3,3,3,3,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,4,4,5,3,3,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,4,2,3,2,3,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1 };
		//int Schedule1[timenum] = { 2,2,2,2,4,3,3,3,3,2,3,3,3,1,1,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,3,3,3,3,2,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,4,4,5,3,3,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,4,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,2,3,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1 };
		//int Schedule1[timenum] = { 2,3,3,3,3,4,3,3,3,3,1,3,3,3,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,4,3,3,3,3,1,3,3,3,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,4,3,3,3,1,3,3,3,2,1,1,1,1,1,1,1,1,1,2,3,3,3,4,3,3,3,4,3,1,3,3,2,2,1,1,1,1,1,1,1,1,1,2,3,4,4,4,4,4,4,3,3,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,3,3,4,3,4,3,4,2,3,3,3,2,2,2,1,1,1,1,1,1,1,1,1,2,3,3,3,3,3,3,3,4,3,1,3,3,3,3,1,1,1,1,1,1,1,1,1 };
		//int Schedule1[timenum] = { 2,3,4,4,3,3,3,3,3,3,2,3,3,3,3,2,1,1,1,1,1,1,1,1,2,3,3,4,4,3,3,3,3,3,3,3,3,2,2,2,1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,1,1,1,1,1,1,1,1,3,3,4,4,4,3,3,3,3,3,3,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,3,3,3,3,4,5,5,3,3,2,2,2,2,1,1,1,1,1,1,1,1,1,3,3,3,3,4,4,4,3,3,3,3,3,3,2,2,2,1,1,1,1,1,1,1,1,2,3,3,3,3,4,4,3,3,3,2,3,3,3,2,2,1,1,1,1,1,1,1,1 };
		//int M2[timenum] = { 8,8,7,7,7,7,9,9,9,9,9,8,8,9,9,9,9,7,7,8,8,8,8,9,9,8,8,8,8,8,8,9,9,9,9,9,9,9,8,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,8,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,9,9,9,9,9,8,8,7,7,7,7,7,7,7,7,9,9,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,7,7,7,7,8,9,9,8,8,8,9,9,8,8,9,9,9,9,9,9,8,9,9,7,7,7,7,7,7,9,9,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,7,7,7,7,7,7,9 };
		//int Schedule1[timenum] = { 2,3,3,4,4,4,4,3,3,4,3,3,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,4,4,5,5,5,4,4,4,3,3,3,2,1,1,1,1,1,1,1,1,2,4,4,4,4,4,5,4,5,5,3,3,3,2,2,2,1,1,1,1,1,1,1,1,2,4,4,4,4,5,5,5,5,4,3,3,3,2,2,2,1,1,1,1,1,1,1,1,3,3,4,4,4,4,4,4,3,3,2,3,3,3,2,2,1,1,1,1,1,1,1,1,2,3,4,4,4,5,5,5,5,4,3,2,3,3,2,2,1,1,1,1,1,1,1,1,2,4,4,4,4,4,5,4,5,4,3,3,3,3,2,2,2,2,2,2,2,2,2,2 };
		//int M2[timenum] = { 5,4,4,4,4,4,4,4,5,4,5,5,4,5,6,4,5,4,5,5,6,4,4,4,6,6,5,4,4,4,4,4,4,5,5,6,4,6,6,4,5,5,4,4,4,5,4,5,5,4,4,4,4,4,4,4,5,5,5,6,6,4,4,4,5,4,5,5,6,4,4,4,6,6,5,4,4,4,4,4,5,5,5,6,4,4,6,4,5,4,5,5,4,5,4,5,6,4,4,4,4,4,4,4,5,5,5,6,4,4,6,4,4,5,4,4,6,4,4,4,6,4,4,4,4,4,4,4,5,5,5,5,4,6,5,4,4,5,5,6,4,4,4,4,6,4,4,4,4,4,4,4,5,5,5,5,4,6,5,6,4,4,5,6,6,4,4,4 };
		int Schedule1[timenum] = { 3,3,3,3,3,3,3,4,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,2,2,2,3,3,3,2,1,1,1,1,1,1,1,1,1,3,3,3,3,4,3,4,3,3,3,3,3,3,2,2,1,2,2,2,2,2,2,2,2,2,2,3,3,4,4,4,5,4,3,3,2,2,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,4,4,3,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,3,3,4,4,4,4,3,1,3,3,3,2,2,2,1,1,1,1,1,1,1,1,1,4,4,4,4,4,4,4,4,3,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1 };
		int M2[timenum] = { 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, };
		//int M2[timenum] = { 3,5,5,5,4,6,3,6,4,6,3,5,3,5,5,5,3,6,3,5,3,4,3,3,3,5,5,5,3,6,3,6,4,6,3,5,6,5,5,5,3,5,3,6,4,4,3,3,3,5,5,5,6,5,5,6,4,6,3,5,3,5,5,5,3,6,3,6,4,4,3,3,3,5,4,5,6,5,5,6,5,6,3,5,3,5,5,5,3,6,3,6,3,4,3,3,3,5,4,6,6,4,4,6,4,4,5,5,3,4,4,6,3,6,6,6,6,4,3,3,3,5,4,5,3,5,4,6,5,6,5,5,3,6,5,5,6,6,3,6,3,4,3,3,3,5,5,5,4,6,5,5,6,6,3,5,3,5,5,6,3,6,3,6,3,4,3,3 };
		for (int i = 0; i < timenum; i++) {
			//M2[i]++;
			Schedule[i] = Schedule1[i];
			M[i] = M2[i];
		}
	}
}

double decode(int a11[], int a12[], int a3[]) {
	int a1[docnum * day] = { 0 };
	int a2[docnum * day] = { 0 };
	double value = 0;
	double outcome = 0;
	for (int i = 0; i < docnum * day; i++) {
		a1[i] = a11[i];
		a2[i] = a12[i];
		//cout << a11[i] << a1[i] << a12[i] << a2[i] << endl;
		value = value + (double)((double)a2[i] - (double)a1[i]) / slice;
	}
	//system("pause");
	srand(0);
	//rng.seed(std::random_device()());
	int tmpsche[slice * timenum] = { 0 };
	convert(a1, a2, tmpsche);
	for (int i = 0; i < slice * timenum; i++) {
		//cout <<i<<" "<< tmpsche[i] << endl;
		Schedule[i] = tmpsche[i];
	}
	//system("pause");
	//printout(a1, a2, a3);
	//cout <<"总工作时长："<< value << endl;
	int temp[docnum * day] = { 0 };
	int temp2[docnum * day] = { 0 };
	mergeSort(a2, a1, 0, docnum * day - 1, temp, temp2);
	mergeSort(a1, a2, 0, docnum * day - 1, temp, temp2);
	for (int i = 0; i < docnum * day; i++) {
		//cout << a11[i] << a1[i] << a12[i] << a2[i] << endl;
	}
	//system("pause");
	string ss = "";
	if (nowtype == 1) {
		for (int i = 0; i < docnum * day; i++) {
			ss += to_string(a1[i]) + ",";
		}
		for (int i = 0; i < docnum * day; i++) {
			ss += to_string(a2[i]) + ",";
		}
		for (int i = 0; i < Msli * timenum; i++) {
			ss += to_string(a3[i]);
		}
		if (Hashmap1.find(ss) != Hashmap1.end()) {
			//cout << "found" << endl;
			outcome = Hashmap1.find(ss)->second;
		}
		else {
			//cout << "no findings" << endl;
			//cout << ss << endl;
			outcome = cor1 * (double)value + cor2 * Run_Case_Manager_2(a1, a2, tmpsche, a3) + cor3 * totalload / totalday;
			Hashmap1.insert(make_pair(ss, outcome));
		}
	}
	else {
		for (int i = 0; i < timenum * slice; i++) {
			ss += to_string(tmpsche[i]);
		}
		for (int i = 0; i < Msli * timenum; i++) {
			ss += to_string(a3[i]);
		}
		if (Hashmap0.find(ss) != Hashmap0.end()) {
			//cout << "found" << endl;
			outcome = Hashmap0.find(ss)->second;
		}
		else {
			//cout << "no findings" << endl;
			//cout << ss << endl;
			outcome = cor1 * (double)value + cor2 * Run_Case_Manager_1(tmpsche, a3) + cor3 * totalload / totalday;
			Hashmap0.insert(make_pair(ss, outcome));
		}
	}
	cout << "结果为：" << outcome << endl;
	return outcome;
}

double decode1(int a11[], int a12[], int a3[]) {
	int a1[docnum * day] = { 0 };
	int a2[docnum * day] = { 0 };
	double value = 0;
	double outcome = 0;
	for (int i = 0; i < docnum * day; i++) {
		a1[i] = a11[i];
		a2[i] = a12[i];
		value = value + (double)((double)a2[i] - (double)a1[i]) / slice;
	}
	srand(0);
	int tmpsche[slice * timenum] = { 0 };
	convert(a1, a2, tmpsche);
	for (int i = 0; i < slice * timenum; i++) {
		Schedule[i] = tmpsche[i];
	}
	int temp[docnum * day] = { 0 };
	int temp2[docnum * day] = { 0 };
	mergeSort(a2, a1, 0, docnum * day - 1, temp, temp2);
	mergeSort(a1, a2, 0, docnum * day - 1, temp, temp2);
	string ss = "";
	if (nowtype == 1) {
		for (int i = 0; i < docnum * day; i++) {
			ss += to_string(a1[i]) + ",";
		}
		for (int i = 0; i < docnum * day; i++) {
			ss += to_string(a2[i]) + ",";
		}
		for (int i = 0; i < Msli * timenum; i++) {
			ss += to_string(a3[i]);
		}
		if (Hashmap1.find(ss) != Hashmap1.end()) {
			outcome = Hashmap1.find(ss)->second;
		}
		else {
			outcome = cor1 * (double)value + cor2 * Run_Case_Manager_2(a1, a2, tmpsche, a3) + cor3 * totalload / totalday;
			Hashmap1.insert(make_pair(ss, outcome));
		}
	}
	else {
		for (int i = 0; i < timenum * slice; i++) {
			ss += to_string(tmpsche[i]);
		}
		for (int i = 0; i < Msli * timenum; i++) {
			ss += to_string(a3[i]);
		}
		if (Hashmap0.find(ss) != Hashmap0.end()) {
			outcome = Hashmap0.find(ss)->second;
		}
		else {

			outcome = cor1 * (double)value + cor2 * Run_Case_Manager_1(tmpsche, a3) + cor3 * totalload / totalday;
			Hashmap0.insert(make_pair(ss, outcome));
		}
	}
	cout << "结果为：" << outcome << endl;
	return outcome;
}

void shake1(int a) {
	Sleep(1.0);
	srand(time(NULL));
	cout << "shake:" << a + shakeadd << endl;
	copyarray(workhours1, tmp1);
	copyarray(workhours2, tmp2);
	copyarray(M, tmp3, Msli * timenum);
	int ta = 0;
	int ran_doc[docnum * day] = { 0 };
	int ran_position, ran_long = 0;
	int doc_flag = 0;
	for (int i = 0; i < a + shakeadd; i++) {
		ta = 0;
		do {
			copyarray(tmp1, tmpwk1);
			copyarray(tmp2, tmpwk2);
			copyarray(tmp3, tmpM, Msli * timenum);
			do {
				doc_flag = 0;
				ran_doc[i] = rand() % (docnum * day);
				if (tmpwk1[ran_doc[i]] == tmpwk2[ran_doc[i]]) {
					//doc_flag = 1;//shake不考虑改变休息的人
				}
				for (int j = 0; j < i; j++) {
					if (ran_doc[i] == ran_doc[j]) {
						doc_flag = 1;
						break;
					}
				}

			} while (doc_flag == 1);
			do {
				ran_position = rand() % (slice * timenum / day);
				ran_long = rand() % (slice * (maxwork - minwork) + 1) + slice * minwork;
			} while (ran_position == tmpwk1[ran_doc[i]]);

			int day1 = tmpwk1[ran_doc[i]] / (slice * timenum / day);
			tmpwk1[ran_doc[i]] = ran_position + day1 * slice * timenum / day;
			if (rand() % 3 < 2) {
				tmpwk2[ran_doc[i]] = tmpwk1[ran_doc[i]] + ran_long;
				if (ran_position > slice * nightstart) {
					tmpwk1[ran_doc[i]] = slice * day1 * (timenum / day) + slice * nightstart;
					tmpwk2[ran_doc[i]] = slice * (day1 + 1) * (timenum / day);
				}
			}
			else {
				tmpwk1[ran_doc[i]] = timenum / day * slice * day1 + resthour * slice;
				tmpwk2[ran_doc[i]] = tmpwk2[ran_doc[i]];
			}

			ta++;
			if (ta > 1000) {
				cout << "nooo" << endl;
				printout(tmp1, tmp2, tmp3);
				cout << ran_doc[i] << " " << tmp1[ran_doc[i]] << " " << tmp2[ran_doc[i]] << endl;
				system("pause");
			}

		} while (checkfeasibility(tmpwk1, tmpwk2, tmpM) == 1);
		copyarray(tmpwk1, tmp1);
		copyarray(tmpwk2, tmp2);
		copyarray(tmpM, tmp3, Msli * timenum);
	}
	int tmmpM = rand() % 3 + 3;
	for (int i = 0; i < Msli * timenum; i++) {
		if (ifM == 1) {
			tmp3[i] = tmmpM;
		}
	}
}

void shake2(int a) {
	cout << "shake:" << a + shakeadd << endl;
	copyarray(workhours1, tmp1);
	copyarray(workhours2, tmp2);
	copyarray(M, tmp3, Msli * timenum);
	int ta = 0;
	int ran_doc[docnum * day] = { 0 };
	int ran_position, ran_long = 0;
	int doc_flag = 0;
	for (int i = 0; i < a + shakeadd; i++) {
		ta = 0;
		do {
			copyarray(tmp1, tmpwk1);
			copyarray(tmp2, tmpwk2);
			copyarray(tmp3, tmpM, Msli * timenum);
			do {
				doc_flag = 0;
				ran_doc[i] = rand() % (docnum * day);
				if (tmpwk1[ran_doc[i]] == tmpwk2[ran_doc[i]]) {
					//doc_flag = 1;//shake不考虑改变休息的人
				}
				for (int j = 0; j < i; j++) {
					if (ran_doc[i] == ran_doc[j]) {
						doc_flag = 1;
						break;
					}
				}

			} while (doc_flag == 1);
			do {
				ran_position = rand() % (slice * timenum / day);
				ran_long = rand() % (slice * (maxwork - minwork) + 1) + slice * minwork;
			} while (ran_position == tmpwk1[ran_doc[i]]);

			int day1 = tmpwk1[ran_doc[i]] / (slice * timenum / day);
			tmpwk1[ran_doc[i]] = ran_position + day1 * slice * timenum / day;
			if (rand() % 3 < 2) {
				tmpwk2[ran_doc[i]] = tmpwk1[ran_doc[i]] + ran_long;
				if (ran_position > slice * nightstart) {
					tmpwk1[ran_doc[i]] = slice * day1 * (timenum / day) + slice * nightstart;
					tmpwk2[ran_doc[i]] = slice * (day1 + 1) * (timenum / day);
				}
			}
			else {
				tmpwk1[ran_doc[i]] = timenum / day * slice * day1 + resthour * slice;
				tmpwk2[ran_doc[i]] = tmpwk2[ran_doc[i]];
			}

			ta++;
			if (ta > 1000) {
				cout << "nooo" << endl;
				printout(tmp1, tmp2, tmp3);
				cout << ran_doc[i] << " " << tmp1[ran_doc[i]] << " " << tmp2[ran_doc[i]] << endl;
				system("pause");
			}

		} while (checkfeasibility(tmpwk1, tmpwk2, tmpM) == 1);
		copyarray(tmpwk1, tmp1);
		copyarray(tmpwk2, tmp2);
		copyarray(tmpM, tmp3, Msli * timenum);
	}
	int tmpp = rand() % (int)(defaultM/2) + defaultM;
	for (int i = 0; i < Msli * timenum; i++) {
		if (rand()%50 == 1) {
			tmp3[i] = tmpp;
		}
	}
	int rand2 = rand() % 10 + 5;
	for (int i = 0; i < rand2; i++) {
		tmp3[rand() % timenum] = rand() % 5 + defaultM-2;
	}
	copyarray(tmp1, workhours1);
	copyarray(tmp2, workhours2);
	copyarray(tmp3, M, Msli * timenum);
}

double neibour1(int a) {
	//int ta = 0;
	double localopt = 1000000;
	double nowopt = 0;
	int tmpt = 0;
	int day1, day2, hour1, hour2 = 0;
	cout << "邻域搜索开始" << endl;
	copyarray(tmp1, tmpwk1);
	copyarray(tmp2, tmpwk2);
	copyarray(tmp3, tmpM, Msli * timenum);
	copyarray(tmp1, bestwk1);
	copyarray(tmp2, bestwk2);
	copyarray(tmp3, bestM, Msli * timenum);
	printout(tmp1, tmp2, tmp3);
	if (a == 1) {//尾时长变化
		for (int i = 0; i < docnum * day; i++) {
			tmp2[i] = tmp2[i] + 1;
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					//change_num = i;
					//change_type = 1;
					localopt = nowopt;
				}
			}
			tmp2[i] = tmp2[i] - 2;
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					//change_num = i;
					//change_type = 0;
					localopt = nowopt;
				}
			}
			tmp2[i] = tmp2[i] + 1;
		}
	}
	if (a == 0) {//时长平移 首时长变化
		for (int i = 0; i < docnum * day; i++) {
			//cout << i << endl;
			tmp1[i] = tmp1[i] + 1;
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					//change_num = i;
					//change_type = 1;
					localopt = nowopt;
				}
			}
			tmp1[i] = tmp1[i] - 2;
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					//change_num = i;
					//change_type = 0;
					localopt = nowopt;
				}
			}
			tmp1[i] = tmp1[i] + 1;
			/*
			tmpt = 0;
			tmp1[i] = tmp1[i] - 1;
			tmp2[i] = tmp2[i] - 1;
			day1 = tmp1[i] / (slice * timenum / day);
			hour1 = tmp1[i] - day1 * slice * (timenum / day);
			if (hour1 == slice * nightlong - 1) {
				//tmpt = tmp1[i] + 1;
				tmp1[i] = slice * nightlong + day1 * slice * (timenum / day);
			}
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					//change_num = i;
					//change_type = 0;
					localopt = nowopt;
				}
			}
			else {
				//printout(tmp1, tmp2, tmp3);
			}
			copyarray(tmpwk1, tmp1);
			copyarray(tmpwk2, tmp2);
			tmp1[i]++;
			tmp2[i]++;
			day2 = tmp2[i] / (slice * timenum / day);
			//hour2 = tmp2[i] - day2 * slice * timenum / day;
			if (day2 > day1) {
				tmp2[i] = day2 * slice * (timenum / day);
			}
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					//change_num = i;
					//change_type = 1;
					localopt = nowopt;
				}
			}
			copyarray(tmpwk1, tmp1);
			copyarray(tmpwk2, tmp2);
			*/
		}
	}
	if (a == 4) {//M变化
		for (int i = 0; i < Msli * timenum; i++) {
			//cout << "M变换时段" << i << endl;
			tmp3[i] = tmp3[i] + 1;
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					//change_num = i;
					//change_type = 1;
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					localopt = nowopt;
				}
			}
			tmp3[i] = tmp3[i] - 2;
			if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
				nowopt = decode(tmp1, tmp2, tmp3);
				if (nowopt < localopt) {
					//change_num = i;
					//change_type = 0;
					copyarray(tmp1, bestwk1);
					copyarray(tmp2, bestwk2);
					copyarray(tmp3, bestM, Msli * timenum);
					localopt = nowopt;
				}
			}
			tmp3[i] = tmp3[i] + 1;
		}
	}
	if (a == 3) {//夜班变化
		int ifMove[day] = { 0 };
		for (int i = 0; i < docnum * day; i++) {
			day1 = tmp1[i] / (slice * timenum / day);
			hour1 = tmp1[i] - day1 * slice * (timenum / day);
			day2 = tmp2[i] / (slice * timenum / day);
			hour2 = tmp2[i] - day2 * slice * timenum / day;
			int temp = tmp1[i] = tmp2[i];
			if (hour1 == nightstart * slice && temp == nightlong * slice) {
				if (ifMove[day1] == 1) {
					continue;
				}
				ifMove[day1] = 1;
				for (int j = 0; j < slice * (timenum / day - nightlong); j++) {
					tmp1[i] = j + day1 * slice * (timenum / day);
					tmp2[i] = tmp1[i] + slice * nightlong;
					if (tmp2[i] > slice * nightstart) {
						tmp2[i] = slice * nightstart + day1 * slice * (timenum / day);
					}
					if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
						//printout(tmp1, tmp2, tmp3);
						nowopt = decode(tmp1, tmp2, tmp3);
						if (nowopt < localopt) {
							//change_num = j;
							//change_type = i;
							copyarray(tmp1, bestwk1);
							copyarray(tmp2, bestwk2);
							copyarray(tmp3, bestM, Msli * timenum);
							localopt = nowopt;
						}
					}
					copyarray(tmpwk1, tmp1);
					copyarray(tmpwk2, tmp2);
				}
			}
			else {
				tmp1[i] = day1 * slice * (timenum / day) + slice * nightstart;
				tmp2[i] = tmp1[i] + slice * nightlong;
				//第二天班次减少


				//
				if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
					//printout(tmp1, tmp2, tmp3);
					nowopt = decode(tmp1, tmp2, tmp3);
					if (nowopt < localopt) {
						//change_num = i;
						//change_type = -1;
						copyarray(tmp1, bestwk1);
						copyarray(tmp2, bestwk2);
						copyarray(tmp3, bestM, Msli * timenum);
						localopt = nowopt;
					}
				}

				copyarray(tmpwk1, tmp1);
				copyarray(tmpwk2, tmp2);
			}
		}
	}
	if (a == 2) {//休息和上班的转换
		int ifchange[day] = { 0 };
		for (int i = 0; i < docnum * day; i++) {
			day1 = tmp1[i] / (slice * timenum / day);
			//hour1 = tmp1[i] - day1 * slice * (timenum / day);
			//day2 = tmp2[i] / (slice * timenum / day);
			//hour2 = tmp2[i] - day2 * slice * timenum / day;
			if (tmp1[i] == tmp2[i]) {
				if (ifchange[day1] == 1) {
					continue;
				}
				ifchange[day1] = 1;
				for (int j = 0; j < slice * (timenum / day - nightstart); j++) {
					tmp1[i] = j + day1 * slice * (timenum / day);
					tmp2[i] = tmp1[i] + slice * minwork;
					if (tmp2[i] > slice * nightstart) {
						tmp2[i] = slice * nightstart + day1 * slice * (timenum / day);
					}
					if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
						//printout(tmp1, tmp2, tmp3);
						nowopt = decode(tmp1, tmp2, tmp3);
						if (nowopt < localopt) {
							//change_num = j;
							//change_type = i;
							copyarray(tmp1, bestwk1);
							copyarray(tmp2, bestwk2);
							copyarray(tmp3, bestM, Msli * timenum);
							localopt = nowopt;
						}
					}
					copyarray(tmpwk1, tmp1);
					copyarray(tmpwk2, tmp2);
				}

			}
			else {
				tmp1[i] = timenum / day * slice * day1 + resthour * slice;
				tmp2[i] = tmp1[i];
				if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
					//printout(tmp1, tmp2, tmp3);
					nowopt = decode(tmp1, tmp2, tmp3);
					if (nowopt < localopt) {
						copyarray(tmp1, bestwk1);
						copyarray(tmp2, bestwk2);
						copyarray(tmp3, bestM, Msli * timenum);
						localopt = nowopt;
					}
				}
				copyarray(tmpwk1, tmp1);
				copyarray(tmpwk2, tmp2);
			}
		}
	}
	if (a == 5) {//测试邻域
		for (int i = 0; i < docnum * day; i++) {
			for (int j = 0; j < docnum * day; j++) {
				if (j != i) {
					tmp2[i]--;
					tmp2[j]++;
					if (checkfeasibility(tmp1, tmp2, tmp3) == 0) {
						nowopt = decode(tmp1, tmp2, tmp3);
						if (nowopt < localopt) {
							//change_num = i;
							//change_type = j;
							copyarray(tmp1, bestwk1);
							copyarray(tmp2, bestwk2);
							copyarray(tmp3, bestM, Msli * timenum);
							localopt = nowopt;
						}
					}
					copyarray(tmpwk1, tmp1);
					copyarray(tmpwk2, tmp2);
				}
			}
		}
	}
	return localopt;
}

void solutiontest(int a1[], int a2[], int a3[], double mu, double b[]) {

	for (int i = 0; i < docnum * day; i++) {
		a1[i] = slice * a1[i];
		a2[i] = slice * a2[i];
	}
	int aa3[Msli * timenum] = { 0 };
	for (int i = 0; i < Msli * timenum; i++) {
		aa3[i] = a3[i / Msli];
	}
	int tmpsche[slice * timenum] = { 0 };
	convert(a1, a2, tmpsche);
	for (int i = 0; i < slice * timenum; i++) {
		Schedule[i] = tmpsche[i];
	}
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < timenum / day; j++) {
			Arrive_rate[i * timenum / day + j] = Arrive_rate[j] * b[i];
		}
	}
	for (int i = 0; i < timenum; i++) {
		Arrive_rate[i] = Arrive_rate[i] * mu;
	}
	printout(a1, a2, aa3);
	decode(a1, a2, aa3);
	system("pause");
}

int solutionmatch(int allocate[day][docnum], int allshift[day][docnum]) {//2休息，1夜班，0正常
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < docnum; j++) {
			allocate[i][j] = 0;
			cout << allshift[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	//system("pause");
	int tmpnight = 0;
	int flagdoc = 0;

	int nightallocate = 0;
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < docnum; j++) {
			if (allshift[i][j] == 1) {//如果此班是夜班
				nightallocate = nightallocate % docnum + 1;
				if (flagdoc == 1) {
					if (i == day - 1) {
						for (int ii = nightallocate - 1; ii < docnum; ii++) {
							int flagfind = 0;
							for (int jj = 0; jj < docnum; jj++) {
								if (allocate[i - 1][jj] == ii + 1) {
									flagfind = 1;
									break;
								}
								if (allocate[0][jj] == ii + 1) {
									flagfind = 1;
									break;
								}
								if (allocate[i][jj] == ii + 1) {
									flagfind = 1;
									break;
								}
							}
							if (flagfind == 0) {
								allocate[i][j] = ii + 1;
								break;
							}
						}
					}
					else {
						allocate[i][j] = nightallocate;
						if (nightallocate == docnum && flagdoc == 0) {
							flagdoc = 1;
						}
					}
				}
				else {
					allocate[i][j] = nightallocate;
					if (nightallocate == docnum && flagdoc == 0) {
						flagdoc = 1;
					}
				}

			}
		}
	}
	int fla = 0;
	while (fla == 0) {
		fla = 1;
		int spi = 0, spj = 0;
		for (int i = 0; i < day; i++) {
			for (int j = 0; j < docnum; j++) {
				if (allshift[i][j] == 1 && allocate[i][j] == 0) {
					fla = 0;
					spi = i;
					spj = j;
				}
			}
		}

	}
	//先分配夜班对应的休息
	for (int i = 0; i < day - 1; i++) {
		for (int j = 0; j < docnum; j++) {
			if (allshift[i][j] == 1) {
				tmpnight++;
				for (int jj = 0; jj < docnum; jj++) {
					if (allshift[i + 1][jj] == 2 && allocate[i + 1][jj] == 0) {
						allocate[i + 1][jj] = allocate[i][j];
						break;
					}
				}
			}
		}
	}
	for (int j = 0; j < docnum; j++) {
		if (allshift[day - 1][j] == 1) {
			tmpnight++;
			for (int jj = 0; jj < docnum; jj++) {
				if (allshift[0][jj] == 2 && allocate[0][jj] == 0) {
					allocate[0][jj] = allocate[day - 1][j];
					break;
				}
			}
		}
	}
	/*
	if (tmpnight >= docnum) {
		flagdoc = 1;
	}
	else {
		nightallocate = tmpnight;
	}
	*/
	flagdoc = 0;
	nightallocate = 0;
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < docnum; j++) {
			if (allshift[i][j] == 2 && allocate[i][j] == 0) {
				if (flagdoc == 0) {
					nightallocate = nightallocate % docnum + 1;
					allocate[i][j] = nightallocate;
					if (nightallocate == docnum) {
						flagdoc = 1;
					}
				}
				else {
					for (int ii = 0; ii < docnum; ii++) {
						int flagfind = 0;
						for (int jj = 0; jj < docnum; jj++) {
							if (allocate[i][jj] == ii + 1) {
								flagfind = 1;
								break;
							}
						}
						if (flagfind == 0) {
							allocate[i][j] = ii + 1;
							break;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < docnum; j++) {
			if (allocate[i][j] == 0 && allshift[i][j] == 0) {
				for (int ii = 0; ii < docnum; ii++) {
					int flagfind = 0;
					for (int jj = 0; jj < docnum; jj++) {
						if (allocate[i][jj] == ii + 1) {
							flagfind = 1;
							break;
						}
					}
					if (flagfind == 0) {
						allocate[i][j] = ii + 1;
						break;
					}
				}
			}
		}
	}


	for (int i = 0; i < day; i++) {
		for (int j = 0; j < docnum; j++) {
			cout << allocate[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	//system("pause");
	return 0;
}

void shiftsort(int a1[], int a2[], int a3[]) {
	for (int i = 0; i < day; i++) {
		nightshift[i] = 0;
		rest[i] = 0;
	}
	int allshift[day][docnum] = { 0 };
	int allocate[day][docnum] = { 0 };
	int day1 = 0, day2 = 0, hour1 = 0, hour2 = 0;
	int nowday = 0;
	int j = 0;
	for (int i = 0; i < docnum * day; i++) {
		day1 = a1[i] / (slice * timenum / day);
		day2 = a2[i] / (slice * timenum / day);
		hour1 = a1[i] - day1 * slice * (timenum / day);
		hour2 = a2[i] - day2 * slice * (timenum / day);
		int temp = a2[i] - a1[i];
		if (a2[i] == a1[i]) {
			allshift[day1][j] = 2;
		}
		else if (hour1 == slice * nightstart && temp == slice * nightlong) {
			allshift[day1][j] = 1;
		}
		else {
			allshift[day1][j] = 0;
		}
		j = (j + 1) % docnum;
	}
	solutionmatch(allocate, allshift);
}

void paracheck() {
	if (nightlong + nightstart != timenum / day) {
		cout << "wrong" << endl;
		system("pause");
	}
}

double generateRandomFloat(double min, double max)
{
	// 使用默认的随机设备创建种子
	std::random_device rd;

	// 使用种子初始化梅森旋转引擎
	std::mt19937 mt(rd());

	// 创建一个均匀分布，范围为[min, max]
	std::uniform_real_distribution<double> dist(min, max);

	// 生成一个位于范围内的随机浮点数
	return dist(mt);
}

int checkrho(double Arr[], int Sche[], int M[], double max = 1.2, double min = 0.6) {
	double totalarr = 0;
	int totalSch = 0;
	for (int i = 0; i < slice * timenum; i++) {
		totalarr += Arr[i];
		totalSch += Sche[i];
	}
	double rho = totalarr * (1 + probability1 + probability1 * probability2) / (totalSch * miu1);

	cout << rho << " " << totalSch <<" "<<totalarr<< endl;
	return (rho < max&& rho>min);
}
//随机医生数（不保证可行）
void generatedata(int number = 1) {
	//输入有 到达率/每小时医生数/内队列人数
	//输出有 平均等待时间
	string pathdoc = "doc" + to_string(number) + ".csv";
	string pathM = "M" + to_string(number) + ".csv";
	string patharr = "arr" + to_string(number) + ".csv";
	string pathwt = "wt" + to_string(number) + ".csv";
	ofstream outFiledoc(pathdoc, ofstream::app);
	ofstream outFileM(pathM, ofstream::app);
	ofstream outFilearr(patharr, ofstream::app);
	ofstream outFilewt(pathwt, ofstream::app);

	srand(time(NULL));
	double Arrive_exp[timenum / day] = { 29,14,20,15,11,14,9,11,17,14,11,15,24,29,14,9,2,3,5,4,1,3,20,25 };
	int randomnum = 5000;

	for (int j = 0; j < randomnum; j++) {
		cout << j << endl;
		//首先随机产生医生数
		for (int i = 0; i < timenum * slice; i++) {
			Schedule[i] = rand() % 7 + 1;
			//cout << i << " " << Schedule[i]<<endl;
		}
		//产生随机内队列人数
		int MM[timenum * Msli] = { 0 };
		for (int i = 0; i < timenum * Msli; i++) {
			MM[i] = rand() % 4 + 3;
			//cout << i << " " << MM[i] << endl;
		}
		//产生随机到达率
		std::random_device rd;
		std::mt19937 mt(rd());
		for (int i = 0; i < day; i++) {
			for (int j = 0; j < timenum / day; j++) {
				Arrive_rate[i * timenum / day + j] = 0.8 * generateRandomFloat(0.7, 1.3) * Arrive_exp[j];
			}
		}
		for (int i = 0; i < timenum * slice; i++) {
			//Arrive_rate[i] = generateRandomFloat(1, 25);
			//cout << i << " " << Arrive_rate[i]<< endl;
		}
		//运行
		//system("pause");
		double value = Run_Case_Manager_1(Schedule, MM);
		int value2 = checkrho(Arrive_rate, Schedule, MM);
		if (value > 5 || value2 == 0) {
			j--;
		}
		else {
			for (int i = 0; i < slice * timenum - 1; i++)
			{
				outFilewt << avewt[i] << ",";
			}
			outFilewt << avewt[slice * timenum - 1] << endl;
			for (int i = 0; i < timenum * slice - 1; i++) {
				outFiledoc << Schedule[i] << ",";
			}
			outFiledoc << Schedule[timenum * slice - 1] << endl;
			for (int i = 0; i < timenum * Msli - 1; i++) {
				outFileM << MM[i] << ",";
			}
			outFileM << MM[timenum * Msli - 1] << endl;
			for (int i = 0; i < timenum * slice - 1; i++) {
				outFilearr << Arrive_rate[i] << ",";
			}
			outFilearr << Arrive_rate[timenum * slice - 1] << endl;
		}


	}
	outFilearr.close();
	outFiledoc.close();
	outFileM.close();
	outFilewt.close();
}
//shake医生数（可行）
void generatedata1(int number = 1) {
	//输入有 到达率/每小时医生数/内队列人数
	//输出有 平均等待时间
	string pathdoc = "doc" + to_string(number) + ".csv";
	string pathM = "M" + to_string(number) + ".csv";
	string patharr = "arr" + to_string(number) + ".csv";
	string pathwt = "wt" + to_string(number) + ".csv";
	ofstream outFiledoc(pathdoc, ofstream::app);
	ofstream outFileM(pathM, ofstream::app);
	ofstream outFilearr(patharr, ofstream::app);
	ofstream outFilewt(pathwt, ofstream::app);
	double Arrive_exp1[day][timenum / day] = {
		{11.63636364,15.04545455,16.27272727,13.54545455,11.04545455,10.45454545,11.81818182,12.31818182,11.31818182,10.63636364,8.318181818,9.681818182,12.54545455,9.454545455,6.818181818,4.818181818,4.727272727,2.636957813,1.874034462,2.221330957,1.341651812,1.297682709,3.056149733,11.31253714},
		{10.28966132,14.19934641,16.33214498,13.45870469,12.53267974,10.57724302,11.74836601,11.79322638,11.62061794,9.65389186,8.981283422,8.820558526,10.63814617,8.993166964,6.718062983,5.091800357,4.147355912,2.455208993,1.727763544,1.318593471,1.318515746,1.591250216,3.818894294,9.138758708},
		{11.18432264,12.95793367,15.18562957,14.23059618,12.13894438,9.456901664,11.41183574,12.7302349,11.32074817,9.956832575,9.229164028,6.092682365,9.95697651,8.501948874,6.410587829,5.364741781,4.546388681,3.428571429,2.904761905,2.142857143,1.428571429,1.571428571,3.80952381,10.57142857},
		{11,15.42857143,18.33333333,14.57142857,11.0952381,10.71428571,12.19047619,14.19047619,10.76190476,10.61904762,8.761904762,6.952380952,8.904761905,8,6.904761905,4.476190476,4.523809524,3.363636364,2.045454545,2.045454545,1.909090909,1.681818182,3.272727273,12.27272727},
		{13.36363636,20.54545455,22.04545455,17.86363636,9.636363636,11.31818182,14.86363636,16.13636364,13.13636364,9.409090909,6.818181818,7.136363636,10.68181818,8.272727273,6.454545455,5.090909091,4.227272727,3.590909091,2.227272727,2.045454545,1.681818182,1.727272727,2.909090909,11.22727273},
		{11.36363636,15.27272727,16.5,16.81818182,12.09090909,11.90909091,12.40909091,13.5,11.72727273,11.13636364,9.136363636,9.136363636,11.68181818,9.227272727,6.909090909,5.045454545,3.954545455,2.909090909,2.454545455,1.318181818,1.727272727,1.136363636,2.409090909,11.09090909},
		{10.72727273,14.04545455,16.04545455,14.40909091,12.81818182,10.31818182,12.31818182,13,11.31818182,11.13636364,8.727272727,8.5,12.45454545,9.090909091,7.181818182,4.818181818,3.545454545,2.636363636,2.318181818,1.909090909,1.272727273,1.727272727,2.954545455,10}
	};
	double Arrive_exp2[day][timenum / day] = {
		{12,14.66666667,15.66666667,10,11.33333333,10.66666667,9,11.33333333,8,7.666666667,6,8.333333333,5.666666667,5,2.333333333,4.666666667,4.333333333,3.333333333,2.666666667,2.333333333,2.333333333,4,14.33333333,13.33333333},
		{10.66666667,12.33333333,15.66666667,9.666666667,9.333333333,10.66666667,14.33333333,13.33333333,11.66666667,10,8.666666667,8,10,8.333333333,5,4.333333333,3,3.333333333,2.666666667,2.666666667,1.333333333,3.333333333,10.33333333,9.333333333},
		{15.33333333,12,18.33333333,16,11,11.66666667,17,9.333333333,13.33333333,8.666666667,11.66666667,13.66666667,9.333333333,5,8,5.333333333,4,1.333333333,2.333333333,0.666666667,2,4.666666667,13.33333333,12},
		{16.66666667,18.66666667,15,13.66666667,13.33333333,14.66666667,15,10,9.666666667,7.666666667,11.33333333,11,7.666666667,8,4,4.333333333,2.666666667,3,3,1.666666667,2,2.333333333,10.66666667,12.33333333},
		{19.33333333,19.66666667,18,9.666666667,13.33333333,12.33333333,13,10.66666667,10.66666667,9,11.33333333,13.33333333,12,7.666666667,8,3.666666667,4.333333333,1.333333333,1.666666667,1.666666667,1.666666667,2.666666667,8.333333333,10.66666667},
		{13,16.66666667,18,9.333333333,11.33333333,13.33333333,11.66666667,13,12.33333333,10,8,12,10.66666667,7.333333333,3.333333333,3.333333333,6.333333333,1,1.333333333,1.333333333,0.666666667,2,12.33333333,10.33333333},
		{17.33333333,20.33333333,16,16,13.66666667,12.66666667,14.66666667,12.33333333,12.66666667,11.66666667,6.666666667,15,10.66666667,6.333333333,6,3,0.666666667,1.666666667,1.333333333,1.333333333,2,2,10,11.66666667}
	};
	double Arrive_exp3[day][timenum / day] = {
		{12,11.33333333,10.66666667,10.66666667,7.666666667,6.333333333,12,10.33333333,9,10,7.666666667,10.33333333,10,7,1.666666667,4.333333333,3.333333333,2,2.333333333,1.333333333,1,3,12.66666667,13},
		{17,14.33333333,12,14,6,12.66666667,10.33333333,9.333333333,12,8.666666667,7.333333333,16.33333333,6.666666667,6,3.666666667,2.666666667,2.333333333,1.333333333,2.333333333,4.333333333,2.666666667,2,13.66666667,8.666666667},
	{10.66666667,13.66666667,18.33333333,12,10.33333333,13.66666667,13.33333333,10,8.333333333,8,5.333333333,15.33333333,8.666666667,7.333333333,4.666666667,3.666666667,4,3.333333333,0.333333333,1.333333333,1.333333333,4,13,14.33333333},
	{15,12.66666667,17,14,11.66666667,11.66666667,17.66666667,13.66666667,13.66666667,7.666666667,8.666666667,14.66666667,10,9.333333333,8.333333333,3,2.666666667,3.333333333,1,2.333333333,1,1.333333333,11,12.66666667},
	{17,14,15.66666667,11.33333333,10.33333333,12.33333333,11,9.666666667,13.66666667,8,10,10.66666667,11,9,6.666666667,2.666666667,2.666666667,2,1.333333333,0.666666667,0.333333333,1.333333333,6.666666667,8.666666667},
	{14.33333333,24.66666667,14,13.33333333,9.666666667,14.66666667,13.66666667,15.33333333,7.333333333,10.33333333,11,7,8.333333333,5.666666667,5.333333333,5,4,3.333333333,1,1,0.666666667,1,11.33333333,8.666666667},
	{14,20,14,12.66666667,16,12.66666667,13.66666667,12,12.33333333,9.666666667,10,14.33333333,8.333333333,7.666666667,4.666666667,3.666666667,1.333333333,1.333333333,1.333333333,1.666666667,1,4,9.666666667,11.66666667}
	};
	double Arrive_exp4[day][timenum / day] = {
		{11,11.66666667,8,9.333333333,6.333333333,9.666666667,12.33333333,11.66666667,10.33333333,6,8.666666667,10.66666667,6.333333333,5.333333333,5.333333333,3.666666667,3.666666667,3.666666667,2,1,2,4.333333333,9.666666667,14.66666667},
	{12.33333333,13.33333333,16.33333333,10,9.666666667,10.33333333,9.666666667,11,7.666666667,7.666666667,8.666666667,8.333333333,8.333333333,4.666666667,4.333333333,4.333333333,2.666666667,2.333333333,3,0.333333333,2.333333333,1.666666667,11.33333333,11.33333333},
	{16,14.33333333,13,16.33333333,11,8.333333333,17,9,15,8.666666667,10.33333333,11,10,7.333333333,7,5.666666667,3.666666667,2,2.666666667,2.666666667,2,4.666666667,10.33333333,12.33333333},
	{17.33333333,17.66666667,13,10.66666667,11.66666667,13,14.66666667,13.66666667,8,9.333333333,11.33333333,14.66666667,7,6,3,6,4,2.666666667,1.666666667,0,2,2,10.33333333,11.33333333},
	{16.33333333,17.66666667,18,7.333333333,9.333333333,16,10.66666667,11,13.66666667,8,10.33333333,14.33333333,10,8.666666667,6,5.333333333,1.666666667,2,2,1.666666667,1,2,9,9.333333333},
	{14.33333333,19,12.66666667,12,11.33333333,14.33333333,11,10.33333333,9,7,10.33333333,14.33333333,12.66666667,7.333333333,4.333333333,3.666666667,2.333333333,0.666666667,1.666666667,0.666666667,2.333333333,1,10.66666667,10.66666667},
	{17.33333333,18.33333333,13.66666667,10,14.33333333,11,11,13.66666667,10.33333333,11,10,15.66666667,11.33333333,9.333333333,4.333333333,4.333333333,1,3.666666667,1,2.666666667,1,5.666666667,10,14.33333333}
	};
	double Arrive_exp5[day][timenum / day] = {
		{13.33333333,13,11.33333333,9,9.666666667,8.333333333,11,10.66666667,8,7,5.666666667,8.333333333,6.333333333,4.333333333,4,3.333333333,3,3.333333333,1.666666667,0.666666667,2.333333333,5.333333333,11,13.66666667},
	{13.33333333,16.66666667,14,10,10,10,11.66666667,10.66666667,11.33333333,7,4.666666667,8.666666667,5,7.333333333,4.666666667,3.666666667,4,2.333333333,1.666666667,2,2,2.666666667,7.333333333,11},
	{17.66666667,18.66666667,17.66666667,8.333333333,11,15.33333333,14.66666667,11,10.66666667,7.333333333,3.333333333,9.666666667,6.333333333,10,6.666666667,4.666666667,3.333333333,2.333333333,2.666666667,2,1.666666667,3,12.66666667,10.33333333},
	{22.33333333,22,15,9.666666667,9.666666667,12.66666667,12.66666667,13,12.66666667,11.66666667,6.666666667,8.666666667,10,5.666666667,3.666666667,5.333333333,2.333333333,2.666666667,2.333333333,1.333333333,2.333333333,4.666666667,10.66666667,12.33333333},
	{12,20,11.66666667,14.66666667,12,14,15.66666667,10.66666667,9.666666667,10.33333333,9,8,10.66666667,8,7,5.666666667,3.666666667,4,2.666666667,1.333333333,0.666666667,1,8.333333333,10.33333333},
	{13,19,17.66666667,11,9.333333333,13,15,8,9,9,9.333333333,7.333333333,10.33333333,5.666666667,2,3.333333333,4,3.666666667,1.666666667,1.333333333,1,4.666666667,13,11.33333333},
	{13.66666667,17.66666667,14.33333333,14,10.66666667,11.33333333,15.66666667,13.66666667,11.66666667,8.666666667,9.333333333,9.333333333,8.333333333,8.333333333,4,4.333333333,2.666666667,1.666666667,2,1,0.666666667,5,13.33333333,7.666666667}
	};
	double Arrive_exp6[day][timenum / day] = {
		{17,15.33333333,10,8,5.333333333,13.33333333,12.33333333,11.66666667,8.333333333,6.333333333,6.333333333,9,6.666666667,5.666666667,2,4.666666667,2.666666667,1.666666667,2.333333333,2.333333333,1.333333333,2,7.6,17.66666667},
	{21,22.66666667,15.33333333,9.666666667,13.66666667,13,14,15.33333333,11.66666667,9,5,8.333333333,8,5.333333333,6.333333333,5,4.333333333,2.666666667,3,2.666666667,3.333333333,3.666666667,12,14.33333333},
	{20.33333333,22,18.33333333,10.33333333,10.33333333,17,17.66666667,15,9,4.666666667,5.666666667,10.66666667,6.666666667,6.666666667,7,5.333333333,5,1,3.333333333,1.666666667,0.666666667,5.666666667,14,12.66666667},
	{24,23.33333333,13.33333333,10.66666667,11.66666667,15.33333333,17,10,9.333333333,6.333333333,10.33333333,8,9.666666667,6,6,4,2.666666667,2,0.333333333,1.333333333,1.333333333,3.333333333,9,15.66666667},
	{22.33333333,26.33333333,24.66666667,7.333333333,9.333333333,17,14.33333333,13.33333333,8.333333333,9,7,13,12.66666667,9,5,3.333333333,2,2.333333333,2,2.666666667,2.333333333,2.333333333,12.66666667,9.333333333},
	{18.33333333,21,21.33333333,10.66666667,9.333333333,14.66666667,20.33333333,13.66666667,9,5.333333333,7.666666667,14,6.666666667,5,6,4.333333333,3.333333333,2.666666667,1,1,2,1.666666667,12.66666667,14.66666667},
	{20.66666667,20.66666667,20.33333333,10,17,14,18,13,9.666666667,6.666666667,9.333333333,10.66666667,9.333333333,7.333333333,4,4,3,2.333333333,2.666666667,1.666666667,1.333333333,4.666666667,11.66666667,11.33333333}
	};
	double Arrive_expp[numcase][day][timenum / day] = {
	{
		{11.63636364,15.04545455,16.27272727,13.54545455,11.04545455,10.45454545,11.81818182,12.31818182,11.31818182,10.63636364,8.318181818,9.681818182,12.54545455,9.454545455,6.818181818,4.818181818,4.727272727,2.636957813,1.874034462,2.221330957,1.341651812,1.297682709,3.056149733,11.31253714},
		{10.28966132,14.19934641,16.33214498,13.45870469,12.53267974,10.57724302,11.74836601,11.79322638,11.62061794,9.65389186,8.981283422,8.820558526,10.63814617,8.993166964,6.718062983,5.091800357,4.147355912,2.455208993,1.727763544,1.318593471,1.318515746,1.591250216,3.818894294,9.138758708},
		{11.18432264,12.95793367,15.18562957,14.23059618,12.13894438,9.456901664,11.41183574,12.7302349,11.32074817,9.956832575,9.229164028,6.092682365,9.95697651,8.501948874,6.410587829,5.364741781,4.546388681,3.428571429,2.904761905,2.142857143,1.428571429,1.571428571,3.80952381,10.57142857},
		{11,15.42857143,18.33333333,14.57142857,11.0952381,10.71428571,12.19047619,14.19047619,10.76190476,10.61904762,8.761904762,6.952380952,8.904761905,8,6.904761905,4.476190476,4.523809524,3.363636364,2.045454545,2.045454545,1.909090909,1.681818182,3.272727273,12.27272727},
		{13.36363636,20.54545455,22.04545455,17.86363636,9.636363636,11.31818182,14.86363636,16.13636364,13.13636364,9.409090909,6.818181818,7.136363636,10.68181818,8.272727273,6.454545455,5.090909091,4.227272727,3.590909091,2.227272727,2.045454545,1.681818182,1.727272727,2.909090909,11.22727273},
		{11.36363636,15.27272727,16.5,16.81818182,12.09090909,11.90909091,12.40909091,13.5,11.72727273,11.13636364,9.136363636,9.136363636,11.68181818,9.227272727,6.909090909,5.045454545,3.954545455,2.909090909,2.454545455,1.318181818,1.727272727,1.136363636,2.409090909,11.09090909},
		{10.72727273,14.04545455,16.04545455,14.40909091,12.81818182,10.31818182,12.31818182,13,11.31818182,11.13636364,8.727272727,8.5,12.45454545,9.090909091,7.181818182,4.818181818,3.545454545,2.636363636,2.318181818,1.909090909,1.272727273,1.727272727,2.954545455,10}
},
{
	{12,14.66666667,15.66666667,10,11.33333333,10.66666667,9,11.33333333,8,7.666666667,6,8.333333333,5.666666667,5,2.333333333,4.666666667,4.333333333,3.333333333,2.666666667,2.333333333,2.333333333,4,14.33333333,13.33333333},
	{10.66666667,12.33333333,15.66666667,9.666666667,9.333333333,10.66666667,14.33333333,13.33333333,11.66666667,10,8.666666667,8,10,8.333333333,5,4.333333333,3,3.333333333,2.666666667,2.666666667,1.333333333,3.333333333,10.33333333,9.333333333},
	{15.33333333,12,18.33333333,16,11,11.66666667,17,9.333333333,13.33333333,8.666666667,11.66666667,13.66666667,9.333333333,5,8,5.333333333,4,1.333333333,2.333333333,0.666666667,2,4.666666667,13.33333333,12},
	{16.66666667,18.66666667,15,13.66666667,13.33333333,14.66666667,15,10,9.666666667,7.666666667,11.33333333,11,7.666666667,8,4,4.333333333,2.666666667,3,3,1.666666667,2,2.333333333,10.66666667,12.33333333},
	{19.33333333,19.66666667,18,9.666666667,13.33333333,12.33333333,13,10.66666667,10.66666667,9,11.33333333,13.33333333,12,7.666666667,8,3.666666667,4.333333333,1.333333333,1.666666667,1.666666667,1.666666667,2.666666667,8.333333333,10.66666667},
	{13,16.66666667,18,9.333333333,11.33333333,13.33333333,11.66666667,13,12.33333333,10,8,12,10.66666667,7.333333333,3.333333333,3.333333333,6.333333333,1,1.333333333,1.333333333,0.666666667,2,12.33333333,10.33333333},
	{17.33333333,20.33333333,16,16,13.66666667,12.66666667,14.66666667,12.33333333,12.66666667,11.66666667,6.666666667,15,10.66666667,6.333333333,6,3,0.666666667,1.666666667,1.333333333,1.333333333,2,2,10,11.66666667}
},
{
	{12,11.33333333,10.66666667,10.66666667,7.666666667,6.333333333,12,10.33333333,9,10,7.666666667,10.33333333,10,7,1.666666667,4.333333333,3.333333333,2,2.333333333,1.333333333,1,3,12.66666667,13},
	{17,14.33333333,12,14,6,12.66666667,10.33333333,9.333333333,12,8.666666667,7.333333333,16.33333333,6.666666667,6,3.666666667,2.666666667,2.333333333,1.333333333,2.333333333,4.333333333,2.666666667,2,13.66666667,8.666666667},
{10.66666667,13.66666667,18.33333333,12,10.33333333,13.66666667,13.33333333,10,8.333333333,8,5.333333333,15.33333333,8.666666667,7.333333333,4.666666667,3.666666667,4,3.333333333,0.333333333,1.333333333,1.333333333,4,13,14.33333333},
{15,12.66666667,17,14,11.66666667,11.66666667,17.66666667,13.66666667,13.66666667,7.666666667,8.666666667,14.66666667,10,9.333333333,8.333333333,3,2.666666667,3.333333333,1,2.333333333,1,1.333333333,11,12.66666667},
{17,14,15.66666667,11.33333333,10.33333333,12.33333333,11,9.666666667,13.66666667,8,10,10.66666667,11,9,6.666666667,2.666666667,2.666666667,2,1.333333333,0.666666667,0.333333333,1.333333333,6.666666667,8.666666667},
{14.33333333,24.66666667,14,13.33333333,9.666666667,14.66666667,13.66666667,15.33333333,7.333333333,10.33333333,11,7,8.333333333,5.666666667,5.333333333,5,4,3.333333333,1,1,0.666666667,1,11.33333333,8.666666667},
{14,20,14,12.66666667,16,12.66666667,13.66666667,12,12.33333333,9.666666667,10,14.33333333,8.333333333,7.666666667,4.666666667,3.666666667,1.333333333,1.333333333,1.333333333,1.666666667,1,4,9.666666667,11.66666667}
},
{
	{11,11.66666667,8,9.333333333,6.333333333,9.666666667,12.33333333,11.66666667,10.33333333,6,8.666666667,10.66666667,6.333333333,5.333333333,5.333333333,3.666666667,3.666666667,3.666666667,2,1,2,4.333333333,9.666666667,14.66666667},
{12.33333333,13.33333333,16.33333333,10,9.666666667,10.33333333,9.666666667,11,7.666666667,7.666666667,8.666666667,8.333333333,8.333333333,4.666666667,4.333333333,4.333333333,2.666666667,2.333333333,3,0.333333333,2.333333333,1.666666667,11.33333333,11.33333333},
{16,14.33333333,13,16.33333333,11,8.333333333,17,9,15,8.666666667,10.33333333,11,10,7.333333333,7,5.666666667,3.666666667,2,2.666666667,2.666666667,2,4.666666667,10.33333333,12.33333333},
{17.33333333,17.66666667,13,10.66666667,11.66666667,13,14.66666667,13.66666667,8,9.333333333,11.33333333,14.66666667,7,6,3,6,4,2.666666667,1.666666667,0,2,2,10.33333333,11.33333333},
{16.33333333,17.66666667,18,7.333333333,9.333333333,16,10.66666667,11,13.66666667,8,10.33333333,14.33333333,10,8.666666667,6,5.333333333,1.666666667,2,2,1.666666667,1,2,9,9.333333333},
{14.33333333,19,12.66666667,12,11.33333333,14.33333333,11,10.33333333,9,7,10.33333333,14.33333333,12.66666667,7.333333333,4.333333333,3.666666667,2.333333333,0.666666667,1.666666667,0.666666667,2.333333333,1,10.66666667,10.66666667},
{17.33333333,18.33333333,13.66666667,10,14.33333333,11,11,13.66666667,10.33333333,11,10,15.66666667,11.33333333,9.333333333,4.333333333,4.333333333,1,3.666666667,1,2.666666667,1,5.666666667,10,14.33333333}
},
{
	{13.33333333,13,11.33333333,9,9.666666667,8.333333333,11,10.66666667,8,7,5.666666667,8.333333333,6.333333333,4.333333333,4,3.333333333,3,3.333333333,1.666666667,0.666666667,2.333333333,5.333333333,11,13.66666667},
{13.33333333,16.66666667,14,10,10,10,11.66666667,10.66666667,11.33333333,7,4.666666667,8.666666667,5,7.333333333,4.666666667,3.666666667,4,2.333333333,1.666666667,2,2,2.666666667,7.333333333,11},
{17.66666667,18.66666667,17.66666667,8.333333333,11,15.33333333,14.66666667,11,10.66666667,7.333333333,3.333333333,9.666666667,6.333333333,10,6.666666667,4.666666667,3.333333333,2.333333333,2.666666667,2,1.666666667,3,12.66666667,10.33333333},
{22.33333333,22,15,9.666666667,9.666666667,12.66666667,12.66666667,13,12.66666667,11.66666667,6.666666667,8.666666667,10,5.666666667,3.666666667,5.333333333,2.333333333,2.666666667,2.333333333,1.333333333,2.333333333,4.666666667,10.66666667,12.33333333},
{12,20,11.66666667,14.66666667,12,14,15.66666667,10.66666667,9.666666667,10.33333333,9,8,10.66666667,8,7,5.666666667,3.666666667,4,2.666666667,1.333333333,0.666666667,1,8.333333333,10.33333333},
{13,19,17.66666667,11,9.333333333,13,15,8,9,9,9.333333333,7.333333333,10.33333333,5.666666667,2,3.333333333,4,3.666666667,1.666666667,1.333333333,1,4.666666667,13,11.33333333},
{13.66666667,17.66666667,14.33333333,14,10.66666667,11.33333333,15.66666667,13.66666667,11.66666667,8.666666667,9.333333333,9.333333333,8.333333333,8.333333333,4,4.333333333,2.666666667,1.666666667,2,1,0.666666667,5,13.33333333,7.666666667}
},
{
	{17,15.33333333,10,8,5.333333333,13.33333333,12.33333333,11.66666667,8.333333333,6.333333333,6.333333333,9,6.666666667,5.666666667,2,4.666666667,2.666666667,1.666666667,2.333333333,2.333333333,1.333333333,2,7.6,17.66666667},
{21,22.66666667,15.33333333,9.666666667,13.66666667,13,14,15.33333333,11.66666667,9,5,8.333333333,8,5.333333333,6.333333333,5,4.333333333,2.666666667,3,2.666666667,3.333333333,3.666666667,12,14.33333333},
{20.33333333,22,18.33333333,10.33333333,10.33333333,17,17.66666667,15,9,4.666666667,5.666666667,10.66666667,6.666666667,6.666666667,7,5.333333333,5,1,3.333333333,1.666666667,0.666666667,5.666666667,14,12.66666667},
{24,23.33333333,13.33333333,10.66666667,11.66666667,15.33333333,17,10,9.333333333,6.333333333,10.33333333,8,9.666666667,6,6,4,2.666666667,2,0.333333333,1.333333333,1.333333333,3.333333333,9,15.66666667},
{22.33333333,26.33333333,24.66666667,7.333333333,9.333333333,17,14.33333333,13.33333333,8.333333333,9,7,13,12.66666667,9,5,3.333333333,2,2.333333333,2,2.666666667,2.333333333,2.333333333,12.66666667,9.333333333},
{18.33333333,21,21.33333333,10.66666667,9.333333333,14.66666667,20.33333333,13.66666667,9,5.333333333,7.666666667,14,6.666666667,5,6,4.333333333,3.333333333,2.666666667,1,1,2,1.666666667,12.66666667,14.66666667},
{20.66666667,20.66666667,20.33333333,10,17,14,18,13,9.666666667,6.666666667,9.333333333,10.66666667,9.333333333,7.333333333,4,4,3,2.333333333,2.666666667,1.666666667,1.333333333,4.666666667,11.66666667,11.33333333}
}
	};
	double Arrive_expp2[numcase][day][timenum / day] = {
			{
			{11.63636364,15.04545455,16.27272727,13.54545455,11.04545455,10.45454545,11.81818182,12.31818182,11.31818182,10.63636364,8.318181818,9.681818182,12.54545455,9.454545455,6.818181818,4.818181818,4.727272727,2.636957813,1.874034462,2.221330957,1.341651812,1.297682709,3.056149733,11.31253714},
			{10.28966132,14.19934641,16.33214498,13.45870469,12.53267974,10.57724302,11.74836601,11.79322638,11.62061794,9.65389186,8.981283422,8.820558526,10.63814617,8.993166964,6.718062983,5.091800357,4.147355912,2.455208993,1.727763544,1.318593471,1.318515746,1.591250216,3.818894294,9.138758708},
			{11.18432264,12.95793367,15.18562957,14.23059618,12.13894438,9.456901664,11.41183574,12.7302349,11.32074817,9.956832575,9.229164028,6.092682365,9.95697651,8.501948874,6.410587829,5.364741781,4.546388681,3.428571429,2.904761905,2.142857143,1.428571429,1.571428571,3.80952381,10.57142857},
			{11,15.42857143,18.33333333,14.57142857,11.0952381,10.71428571,12.19047619,14.19047619,10.76190476,10.61904762,8.761904762,6.952380952,8.904761905,8,6.904761905,4.476190476,4.523809524,3.363636364,2.045454545,2.045454545,1.909090909,1.681818182,3.272727273,12.27272727},
			{13.36363636,20.54545455,22.04545455,17.86363636,9.636363636,11.31818182,14.86363636,16.13636364,13.13636364,9.409090909,6.818181818,7.136363636,10.68181818,8.272727273,6.454545455,5.090909091,4.227272727,3.590909091,2.227272727,2.045454545,1.681818182,1.727272727,2.909090909,11.22727273},
			{11.36363636,15.27272727,16.5,16.81818182,12.09090909,11.90909091,12.40909091,13.5,11.72727273,11.13636364,9.136363636,9.136363636,11.68181818,9.227272727,6.909090909,5.045454545,3.954545455,2.909090909,2.454545455,1.318181818,1.727272727,1.136363636,2.409090909,11.09090909},
			{10.72727273,14.04545455,16.04545455,14.40909091,12.81818182,10.31818182,12.31818182,13,11.31818182,11.13636364,8.727272727,8.5,12.45454545,9.090909091,7.181818182,4.818181818,3.545454545,2.636363636,2.318181818,1.909090909,1.272727273,1.727272727,2.954545455,10}
	},
			{	{11.66666667,12,14.66666667,15.66666667,10,11.33333333,10.66666667,9,11.33333333,8,7.666666667,6,8.333333333,5.666666667,5,2.333333333,4.666666667,4.333333333,3.333333333,2.666666667,2.333333333,2.333333333,4,14.33333333},
	{13.33333333,10.66666667,12.33333333,15.66666667,9.666666667,9.333333333,10.66666667,14.33333333,13.33333333,11.66666667,10,8.666666667,8,10,8.333333333,5,4.333333333,3,3.333333333,2.666666667,2.666666667,1.333333333,3.333333333,10.33333333},
	{9.333333333,15.33333333,12,18.33333333,16,11,11.66666667,17,9.333333333,13.33333333,8.666666667,11.66666667,13.66666667,9.333333333,5,8,5.333333333,4,1.333333333,2.333333333,0.666666667,2,4.666666667,13.33333333},
	{12,16.66666667,18.66666667,15,13.66666667,13.33333333,14.66666667,15,10,9.666666667,7.666666667,11.33333333,11,7.666666667,8,4,4.333333333,2.666666667,3,3,1.666666667,2,2.333333333,10.66666667},
	{12.33333333,19.33333333,19.66666667,18,9.666666667,13.33333333,12.33333333,13,10.66666667,10.66666667,9,11.33333333,13.33333333,12,7.666666667,8,3.666666667,4.333333333,1.333333333,1.666666667,1.666666667,1.666666667,2.666666667,8.333333333},
	{10.66666667,13,16.66666667,18,9.333333333,11.33333333,13.33333333,11.66666667,13,12.33333333,10,8,12,10.66666667,7.333333333,3.333333333,3.333333333,6.333333333,1,1.333333333,1.333333333,0.666666667,2,12.33333333},
	{10.33333333,17.33333333,20.33333333,16,16,13.66666667,12.66666667,14.66666667,12.33333333,12.66666667,11.66666667,6.666666667,15,10.66666667,6.333333333,6,3,0.666666667,1.666666667,1.333333333,1.333333333,2,2,10}
	},
			{
				{11.66666667,12,11.33333333,10.66666667,10.66666667,7.666666667,6.333333333,12,10.33333333,9,10,7.666666667,10.33333333,10,7,1.666666667,4.333333333,3.333333333,2,2.333333333,1.333333333,1,3,12.66666667},
	{13,17,14.33333333,12,14,6,12.66666667,10.33333333,9.333333333,12,8.666666667,7.333333333,16.33333333,6.666666667,6,3.666666667,2.666666667,2.333333333,1.333333333,2.333333333,4.333333333,2.666666667,2,13.66666667},
	{8.666666667,10.66666667,13.66666667,18.33333333,12,10.33333333,13.66666667,13.33333333,10,8.333333333,8,5.333333333,15.33333333,8.666666667,7.333333333,4.666666667,3.666666667,4,3.333333333,0.333333333,1.333333333,1.333333333,4,13},
	{14.33333333,15,12.66666667,17,14,11.66666667,11.66666667,17.66666667,13.66666667,13.66666667,7.666666667,8.666666667,14.66666667,10,9.333333333,8.333333333,3,2.666666667,3.333333333,1,2.333333333,1,1.333333333,11},
	{12.66666667,17,14,15.66666667,11.33333333,10.33333333,12.33333333,11,9.666666667,13.66666667,8,10,10.66666667,11,9,6.666666667,2.666666667,2.666666667,2,1.333333333,0.666666667,0.333333333,1.333333333,6.666666667},
	{8.666666667,14.33333333,24.66666667,14,13.33333333,9.666666667,14.66666667,13.66666667,15.33333333,7.333333333,10.33333333,11,7,8.333333333,5.666666667,5.333333333,5,4,3.333333333,1,1,0.666666667,1,11.33333333},
	{8.666666667,14,20,14,12.66666667,16,12.66666667,13.66666667,12,12.33333333,9.666666667,10,14.33333333,8.333333333,7.666666667,4.666666667,3.666666667,1.333333333,1.333333333,1.333333333,1.666666667,1,4,9.666666667}
	},
			{
				{14.33333333,11,11.66666667,8,9.333333333,6.333333333,9.666666667,12.33333333,11.66666667,10.33333333,6,8.666666667,10.66666667,6.333333333,5.333333333,5.333333333,3.666666667,3.666666667,3.666666667,2,1,2,4.333333333,9.666666667},
	{14.66666667,12.33333333,13.33333333,16.33333333,10,9.666666667,10.33333333,9.666666667,11,7.666666667,7.666666667,8.666666667,8.333333333,8.333333333,4.666666667,4.333333333,4.333333333,2.666666667,2.333333333,3,0.333333333,2.333333333,1.666666667,11.33333333},
	{11.33333333,16,14.33333333,13,16.33333333,11,8.333333333,17,9,15,8.666666667,10.33333333,11,10,7.333333333,7,5.666666667,3.666666667,2,2.666666667,2.666666667,2,4.666666667,10.33333333},
	{12.33333333,17.33333333,17.66666667,13,10.66666667,11.66666667,13,14.66666667,13.66666667,8,9.333333333,11.33333333,14.66666667,7,6,3,6,4,2.666666667,1.666666667,0,2,2,10.33333333},
	{11.33333333,16.33333333,17.66666667,18,7.333333333,9.333333333,16,10.66666667,11,13.66666667,8,10.33333333,14.33333333,10,8.666666667,6,5.333333333,1.666666667,2,2,1.666666667,1,2,9},
	{9.333333333,14.33333333,19,12.66666667,12,11.33333333,14.33333333,11,10.33333333,9,7,10.33333333,14.33333333,12.66666667,7.333333333,4.333333333,3.666666667,2.333333333,0.666666667,1.666666667,0.666666667,2.333333333,1,10.66666667},
	{10.66666667,17.33333333,18.33333333,13.66666667,10,14.33333333,11,11,13.66666667,10.33333333,11,10,15.66666667,11.33333333,9.333333333,4.333333333,4.333333333,1,3.666666667,1,2.666666667,1,5.666666667,10}
	},
			{
				{7.666666667,13.33333333,13,11.33333333,9,9.666666667,8.333333333,11,10.66666667,8,7,5.666666667,8.333333333,6.333333333,4.333333333,4,3.333333333,3,3.333333333,1.666666667,0.666666667,2.333333333,5.333333333,11},
	{13.66666667,13.33333333,16.66666667,14,10,10,10,11.66666667,10.66666667,11.33333333,7,4.666666667,8.666666667,5,7.333333333,4.666666667,3.666666667,4,2.333333333,1.666666667,2,2,2.666666667,7.333333333},
	{11,17.66666667,18.66666667,17.66666667,8.333333333,11,15.33333333,14.66666667,11,10.66666667,7.333333333,3.333333333,9.666666667,6.333333333,10,6.666666667,4.666666667,3.333333333,2.333333333,2.666666667,2,1.666666667,3,12.66666667},
	{10.33333333,22.33333333,22,15,9.666666667,9.666666667,12.66666667,12.66666667,13,12.66666667,11.66666667,6.666666667,8.666666667,10,5.666666667,3.666666667,5.333333333,2.333333333,2.666666667,2.333333333,1.333333333,2.333333333,4.666666667,10.66666667},
	{12.33333333,12,20,11.66666667,14.66666667,12,14,15.66666667,10.66666667,9.666666667,10.33333333,9,8,10.66666667,8,7,5.666666667,3.666666667,4,2.666666667,1.333333333,0.666666667,1,8.333333333},
	{10.33333333,13,19,17.66666667,11,9.333333333,13,15,8,9,9,9.333333333,7.333333333,10.33333333,5.666666667,2,3.333333333,4,3.666666667,1.666666667,1.333333333,1,4.666666667,13},
	{11.33333333,13.66666667,17.66666667,14.33333333,14,10.66666667,11.33333333,15.66666667,13.66666667,11.66666667,8.666666667,9.333333333,9.333333333,8.333333333,8.333333333,4,4.333333333,2.666666667,1.666666667,2,1,0.666666667,5,13.33333333}
	},
			{
				{11.33333333,17,15.33333333,10,8,5.333333333,13.33333333,12.33333333,11.66666667,8.333333333,6.333333333,6.333333333,9,6.666666667,5.666666667,2,4.666666667,2.666666667,1.666666667,2.333333333,2.333333333,1.333333333,2,7.6},
	{17.66666667,21,22.66666667,15.33333333,9.666666667,13.66666667,13,14,15.33333333,11.66666667,9,5,8.333333333,8,5.333333333,6.333333333,5,4.333333333,2.666666667,3,2.666666667,3.333333333,3.666666667,12},
	{14.33333333,20.33333333,22,18.33333333,10.33333333,10.33333333,17,17.66666667,15,9,4.666666667,5.666666667,10.66666667,6.666666667,6.666666667,7,5.333333333,5,1,3.333333333,1.666666667,0.666666667,5.666666667,14},
	{12.66666667,24,23.33333333,13.33333333,10.66666667,11.66666667,15.33333333,17,10,9.333333333,6.333333333,10.33333333,8,9.666666667,6,6,4,2.666666667,2,0.333333333,1.333333333,1.333333333,3.333333333,9},
	{15.66666667,22.33333333,26.33333333,24.66666667,7.333333333,9.333333333,17,14.33333333,13.33333333,8.333333333,9,7,13,12.66666667,9,5,3.333333333,2,2.333333333,2,2.666666667,2.333333333,2.333333333,12.66666667},
	{9.333333333,18.33333333,21,21.33333333,10.66666667,9.333333333,14.66666667,20.33333333,13.66666667,9,5.333333333,7.666666667,14,6.666666667,5,6,4.333333333,3.333333333,2.666666667,1,1,2,1.666666667,12.66666667},
	{14.66666667,20.66666667,20.66666667,20.33333333,10,17,14,18,13,9.666666667,6.666666667,9.333333333,10.66666667,9.333333333,7.333333333,4,4,3,2.333333333,2.666666667,1.666666667,1.333333333,4.666666667,11.66666667}
	}
	};
	int randomnum = 20000;
	//srand(time(NULL));
	primarysolution(0, 8, Arrive_ratio, s, 1, defaultM);
	for (int i = 0; i < day; i++) {
		for (int j = 0; j < timenum / day; j++) {
			Arrive_rate[i * timenum / day + j] = Arrive_expp[4][i][j];
		}
	}
	double value = Run_Case_Manager_1(Schedule, M);
	int value2 = checkrho(Arrive_rate, Schedule, M);

	system("pause");
	for (int j = 0; j < randomnum; j++) {
		cout << j << endl;
		//首先随机产生医生数/产生随机caseload
		shake2(j % K);
		int Tmpshce[timenum * slice] = { 0 };
		convert(workhours1, workhours2, Tmpshce);
		for (int i = 0; i < slice * timenum; i++) {
			Schedule[i] = Tmpshce[i];
		}
		//产生随机到达率
		std::random_device rd;
		std::mt19937 mt(rd());
		for (int i = 0; i < day; i++) {
			for (int j = 0; j < timenum / day; j++) {
				Arrive_rate[i * timenum / day + j] = generateRandomFloat(0.7, 1.3) * Arrive_expp2[1][i][j];
				//Arrive_rate[i * timenum / day + j] =  Arrive_exp[i][j];
			}
		}
		for (int i = 0; i < timenum * slice; i++) {
			//Arrive_rate[i] = generateRandomFloat(1, 25);
			//cout << i << " " << Arrive_rate[i]<< endl;
		}
		//运行
		//system("pause");
		double value = Run_Case_Manager_1(Schedule, M);
		int value2 = checkrho(Arrive_rate, Schedule, M);
		int constent = 1;
		if (value > 4 || value2 == 0 || constent % 1000 == 0) {
			j--;
			primarysolution(0, 8, Arrive_ratio, s, 1, defaultM);
			constent = 1;
			continue;
		}
		else {
			constent++;
			for (int i = 0; i < slice * timenum - 1; i++)
			{
				outFilewt << avewt[i] << ",";
			}
			outFilewt << avewt[slice * timenum - 1] << endl;
			for (int i = 0; i < timenum * slice - 1; i++) {
				outFiledoc << Schedule[i] << ",";
			}
			outFiledoc << Schedule[timenum * slice - 1] << endl;
			for (int i = 0; i < timenum * Msli - 1; i++) {
				//cout << M[i] << endl;
				outFileM << M[i] << ",";
			}
			outFileM << M[timenum * Msli - 1] << endl;
			for (int i = 0; i < timenum * slice - 1; i++) {
				outFilearr << Arrive_rate[i] << ",";
			}
			outFilearr << Arrive_rate[timenum * slice - 1] << endl;
		}


	}
	outFilearr.close();
	outFiledoc.close();
	outFileM.close();
	outFilewt.close();
}
//vns求解的解
void generatedata2(int number = 1) {
	//输入有 到达率/每小时医生数/内队列人数
//输出有 平均等待时间
	string pathdoc = "doc" + to_string(number) + ".csv";
	string pathM = "M" + to_string(number) + ".csv";
	string patharr = "arr" + to_string(number) + ".csv";
	string pathwt = "wt" + to_string(number) + ".csv";
	ofstream outFiledoc(pathdoc, ofstream::app);
	ofstream outFileM(pathM, ofstream::app);
	ofstream outFilearr(patharr, ofstream::app);
	ofstream outFilewt(pathwt, ofstream::app);

	double Arrive_exp[timenum / day] = { 29,14,20,15,11,14,9,11,17,14,11,15,24,29,14,9,2,3,5,4,1,3,20,25 };
	int randomnum = 5000;
	srand(time(NULL));
	primarysolution(0, 8, Arrive_ratio, s, 1, 4);
	int flagk = 0;
	int shakes = 0;
	for (int i = 0; i < randomnum; i++) {
		cout << i << endl;
		std::random_device rd;
		std::mt19937 mt(rd());
		for (int i = 0; i < day; i++) {
			for (int j = 0; j < timenum / day; j++) {
				Arrive_rate[i * timenum / day + j] = 0.8 * generateRandomFloat(0.7, 1.3) * Arrive_exp[j];
			}
		}
		shake1(shakes % K);
		bestvalue = decode(tmp1, tmp2, tmp3);
		cout << bestvalue << endl;
		for (int k = 0; k < K; k++) {
			flagk = 0;
			cout << k << endl;
			lastvalue = neibour1(k);
			if (lastvalue < bestvalue) {
				cout << bestvalue << " " << lastvalue << endl;
				copyarray(bestwk1, tmp1);
				copyarray(bestwk2, tmp2);
				copyarray(bestM, tmp3, Msli * timenum);
				bestvalue = lastvalue;
				flagk = 1;
			}
			cout << "邻域" << k << "最好值为" << bestvalue << " " << wholebest << endl;
			printout(tmp1, tmp2, tmp3);
			if (flagk == 1) {
				k = -1;
			}
		}
		if (bestvalue < wholebest) {
			copyarray(tmp1, workhours1);
			copyarray(tmp2, workhours2);
			copyarray(tmp3, M, Msli * timenum);
			cout << endl;
			shakes = 0;
			wholebest = bestvalue;
		}
		else {
			shakes++;
		}
	}

	for (int j = 0; j < randomnum; j++) {

		int Tmpshce[timenum * slice] = { 0 };
		convert(workhours1, workhours2, Tmpshce);
		for (int i = 0; i < slice * timenum; i++) {
			Schedule[i] = Tmpshce[i];
		}

		double value = Run_Case_Manager_1(Schedule, M);

		int value2 = checkrho(Arrive_rate, Schedule, M);
		int constent = 1;
		if (value > 5.5 || value2 == 0 || constent % 500 == 0) {
			j--;
			primarysolution(0, 8, Arrive_ratio, s, 1, 4);
			constent = 1;
			continue;
		}
		else {
			constent++;
			for (int i = 0; i < slice * timenum - 1; i++)
			{
				outFilewt << avewt[i] << ",";
			}
			outFilewt << avewt[slice * timenum - 1] << endl;
			for (int i = 0; i < timenum * slice - 1; i++) {
				outFiledoc << Schedule[i] << ",";
			}
			outFiledoc << Schedule[timenum * slice - 1] << endl;
			for (int i = 0; i < timenum * Msli - 1; i++) {
				outFileM << M[i] << ",";
			}
			outFileM << M[timenum * Msli - 1] << endl;
			for (int i = 0; i < timenum * slice - 1; i++) {
				outFilearr << Arrive_rate[i] << ",";
			}
			outFilearr << Arrive_rate[timenum * slice - 1] << endl;
		}


	}
	outFilearr.close();
	outFiledoc.close();
	outFileM.close();
	outFilewt.close();
}

int main()
{
	generatedata1(2);
}

