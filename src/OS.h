/**
 * Definition of the os class
 *
 * @authors Dylan Allbee, Taylor Sanchez
 * @version .1
 * @date 10 May, 2013
 * @file os.h
 */
#ifndef OS_H
#define OS_H

#include "vm/VirtualMachine.h"
#include "sys/assembler/Assembler.h"
#include <fstream>
#include <list>
#include <queue>

using namespace std;

class PCB {
public:
	PCB(string name);
private:
	friend class OS;
	string name;

	// Program execution information
	short reg[VirtualMachine::regSize];
	short pc;
	short sr;
	short sp;
	short base;
	short limit;

	// Accounting Information
	unsigned execTime;
	unsigned waitTime;
	unsigned turnTime;
	unsigned ioTime;
	unsigned largestStack;

	// File Streams
	fstream oFile;
	fstream outFile;
	fstream inFile;
	fstream stFile;
};

class OS {
public:
	OS();
	void run();
	void load();
	void loadState();
	void saveState();
	void scheduler();
	void processFinish();
	void finish();

private:
	VirtualMachine VM;
	list<PCB *> progs;
	queue<PCB *> readyQ, waitQ;
	PCB* running;
	int osClock;
	short exitCode;

	//System Information
	int userTotal;
	int idleTotal;
	int systemCpuUtil;
	int userCpuUtil;
	int throughput;

	fstream osOut;
	fstream processStack;

	int asLine;
	short limit;
	string programAs;
};

#endif
