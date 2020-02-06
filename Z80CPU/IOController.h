#pragma once
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include "IPort.h"
#include "workerThread.h"

using namespace std;
struct IOController {
	HANDLE hComPort;
	vector<HANDLE> hThreads;
	//use std::array instead
	vector<IPort*> ports;
	IOController() {
		hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
			NULL,
			0,
			0);
		//thread number should be changeble
		for (int i = 0; i < 1; i++)
		{
			hThreads.push_back((HANDLE)_beginthreadex(NULL, 0, &workerThread, hComPort, 0, NULL));
		}
	}
	void attachPort(IPort* port) {
		CreateIoCompletionPort(port->getHandle(), hComPort, 0, 0);
		port->start();
		ports.push_back(port);
	}
	IPort* getPort(size_t index) const {
		return ports.at(index);
	}
	vector<IPort*>& getPorts() {
		return ports;
	}
	void postExitKeys() {
		printf("Try to terminate threads...\n");
		Sleep(3000);
		OVERLAPPED DUMMY;
		for (auto threadCounter : hThreads) {
			PostQueuedCompletionStatus(hComPort, 0, ThreadExitKey, &DUMMY);
		}
	}
	~IOController() {
		postExitKeys();
		for (auto hThread : hThreads) {
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		for (auto port : ports) {
			port->close();
		}
		//need to close files first
		CloseHandle(hComPort);
	}
};