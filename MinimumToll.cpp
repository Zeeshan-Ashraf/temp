//============================================================================
// Name        : MinimumToll.cpp
// Author      : Santosh Patil
// Version     :
// Copyright   : @2016, SKPatil, All rights reserved.
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <sys/timeb.h>

using namespace std;

//#define ENABLE_LOGS
#define INT_MAX 0x7FFFFFFF

#define MAX_ARCS 20

class Orc {
public:
	int count;
	int toll;

public:
	Orc() : count(0), toll(0) {

	}
};


class HiredGroup {
public:
	int battles;
	int orcs;

public:
	HiredGroup() {
		battles = 0;
		orcs = 0;
	}

	HiredGroup(const HiredGroup& h) {
		battles = h.battles;
		orcs = h.orcs;
	}

	void operator=(const HiredGroup& h) {
		battles = h.battles;
		orcs = h.orcs;
	}

	void Dump(int id = -1) const {
		if (id != -1) {
			cout << "Group Id: " << id << "\t";
		}
		cout << "Battles: " << battles << "\t remain: " << orcs << endl;
	}
};


class Fellowship {
public:
	int fellows;
	HiredGroup group[MAX_ARCS];
	int hiredIndex;
	int tollPaid;

public:
	Fellowship() {
		fellows = 0;
		hiredIndex = 0;
		tollPaid = 0;
	}

	Fellowship(const Fellowship& f) {
		fellows = f.fellows;
		hiredIndex = f.hiredIndex;
		tollPaid = f.tollPaid;

		for(int i = 0; i < f.hiredIndex; i++)
			group[i] = f.group[i];
	}

	void operator=(const Fellowship& f) {
		fellows = f.fellows;
		hiredIndex = f.hiredIndex;
		tollPaid = f.tollPaid;

		for(int i = 0; i < f.hiredIndex; i++)
			group[i] = f.group[i];
	}

	void Dump(const string msg) const {
		cout << "<<< " << msg << " >>>>" << endl << "Fellows : " << fellows << "\tToll paid: " << tollPaid << endl;
		for(int i =0; i<hiredIndex; i++) {
			group[i].Dump(i);
		}

		cout << endl;
	}
};


int getMilliCount(){
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int getMilliSpan(int nTimeStart){
	int nSpan = getMilliCount() - nTimeStart;
	if(nSpan < 0)
		nSpan += 0x100000 * 1000;
	return nSpan;
}


#define NOTSURE 3
#define PAYTOLL 0
#define HIRE 1
#define BATTLE 2


Orc orcs[MAX_ARCS];
static int minValue[MAX_ARCS] = { 0, };
static int N = 0;

int minimum_of(int x, int y, int z) {
	int myMin = x;

	if (y < myMin)
		myMin = y;

	if (z < myMin)
		myMin = z;

	return myMin;
}


int getMinimumToll(int i, Fellowship& f) {

	int value1 = INT_MAX, value2 = INT_MAX, value3 = INT_MAX;
	int minVal = INT_MAX;

	if (i == N) { // Base condition.
		return f.tollPaid;
	}

	{ // pass

		Fellowship fellow = f;
		fellow.tollPaid += orcs[i].toll;

#ifdef ENABLE_LOGS
		fellow.Dump("PASS");
#endif
		if (fellow.tollPaid < minValue[i])
			 value1 = getMinimumToll(i+1, fellow);
	}

	{ // hire
		Fellowship fellow = f;
		fellow.tollPaid += orcs[i].toll * 2;
		fellow.group[fellow.hiredIndex++].orcs = orcs[i].count;
		fellow.fellows += orcs[i].count;

#ifdef ENABLE_LOGS
		fellow.Dump("HIRE");
#endif
		if (fellow.tollPaid < minValue[i])
			 value2 = getMinimumToll(i+1, fellow);
	}

	{ // battle

		if (f.fellows >= orcs[i].count) {

			Fellowship fellow = f;
			int j = 0;
			int current_orcs = orcs[i].count;
			for (j = 0; j < fellow.hiredIndex && current_orcs; j++) {

				if (fellow.group[j].battles < 3 && fellow.group[j].orcs) {

					if (current_orcs <= fellow.group[j].orcs) {

						fellow.group[j].orcs -= current_orcs;
						fellow.fellows -= current_orcs;
						current_orcs = 0;

						// extremated or used
						if (fellow.group[j].battles == 3 || fellow.group[j].orcs == 0) {
							fellow.fellows -= fellow.group[j].orcs;
							fellow.group[j].battles = 3;
							fellow.group[j].orcs = 0;
						}

					 } else {

						// Orcs are more than fellows
						int remainOrcs = current_orcs - fellow.group[j].orcs;
						fellow.fellows -= fellow.group[j].orcs;
						fellow.group[j].orcs = 0;
						current_orcs = remainOrcs;

						// extremated or used
						if (fellow.group[j].battles == 3 || fellow.group[j].orcs == 0) {
							fellow.fellows -= fellow.group[j].orcs;
							fellow.group[j].battles = 3;
							fellow.group[j].orcs = 0;
						}
					 }
				}
			}

			for (int k = 0; k < fellow.hiredIndex; k++) {

				if (fellow.group[k].battles < 3) {
					fellow.group[k].battles++;
				}

				if (fellow.group[k].battles == 3 && fellow.group[k].orcs) {
					fellow.fellows -= fellow.group[k].orcs;
					fellow.group[k].orcs = 0;
				}
			}

#ifdef ENABLE_LOGS
			 fellow.Dump("BATLE");
#endif

			if (fellow.tollPaid < minValue[i])
				value3 = getMinimumToll(i+1, fellow);
		}
	}

	minVal = minimum_of(value1, value2, value3);

	if (minVal < minValue[i])
		minValue[i] = minVal;

	return minVal;
}


/* Main program */
int main(int argc, char** argv)
{
	int test_case;
	int T;

	int start = getMilliCount();

	freopen("sample_input.txt", "r", stdin);

	cin >> T;
	for (test_case = 1; test_case <= T; test_case++)
	{
		int Answer = 0;
		cin >> N;

		for (int i = 0; i < N; i++) {
			cin >> orcs[i].count >> orcs[i].toll;
			minValue[i] = INT_MAX;
		}

		Fellowship fellow;
		Answer = getMinimumToll(0, fellow);

		cout << "#" << test_case << " " << Answer << endl;
	}

	cout << endl << "Elapsed time = " << getMilliSpan(start) << " milliseconds";
	return 0;
}

