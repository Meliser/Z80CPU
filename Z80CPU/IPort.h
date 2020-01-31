#pragma once
#include <Windows.h>
#include <stdio.h>
enum CompletionKeys
{
	ThreadExitKey = 1
};
struct OVERLAPPEDPLUS
{
	OVERLAPPED ovl;

	//additional data
	unsigned char* buffer;
	//ready,busy,off,
	bool isReady;

	OVERLAPPEDPLUS() :buffer(new unsigned char[64]), isReady(false) {
		memset(&ovl, 0, sizeof(OVERLAPPED));
	}
	~OVERLAPPEDPLUS() {
		delete[]buffer;
		buffer = nullptr;
	}
};

class IPort {
	//LPCWSTR name;
protected:
	HANDLE handle;
	OVERLAPPEDPLUS ovlp;

public:
	IPort() :handle(0), ovlp()
	{}
	const HANDLE getHandle() const {
		return handle;
	}
	unsigned char* getBuffer()const {
		return ovlp.buffer;
	}
	bool isReady() const {
		return ovlp.isReady;
	}
	virtual void init() = 0;
	virtual void start() = 0;
	virtual void close() = 0;
	virtual ~IPort() {}
};
class StdIn :public IPort {
public:
	void init() override {
		handle = CreateFile(L"CONIN$",
			GENERIC_READ,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			0);

		if (handle == INVALID_HANDLE_VALUE) {
			printf("BAD HANDLE");
		}
	}
	void start() override {

		ReadFile(handle,
			ovlp.buffer,
			64,
			NULL,
			&ovlp.ovl);
	}
	void close() override {
		CloseHandle(handle);
	}
};