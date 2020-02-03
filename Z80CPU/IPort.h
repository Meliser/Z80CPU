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
	size_t m_currentPos;
	//ready,busy,off,
	bool m_status;
	// buffer max 256
	OVERLAPPEDPLUS() :	buffer(new unsigned char[64]),
						m_currentPos(0),
						m_status(false){
		refresh();
	}
	~OVERLAPPEDPLUS() {
		delete[]buffer;
		buffer = nullptr;
	}
	unsigned char* getBuffer()const {
		return buffer;
	}
	size_t getCurrentPos()const {
		return m_currentPos;
	}
	void setCurrentPos(size_t currentPos) {
		m_currentPos = currentPos;
	}
	bool getStatus() const {
		return m_status;
	}
	void setStatus(bool status) {
		m_status = status;
	}
	void refresh() {
		memset(&ovl, 0, sizeof(OVERLAPPED));
		memset(buffer, 0, 64);
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
	 OVERLAPPEDPLUS& getOvlp() {
		return ovlp;
	}
	virtual void init() = 0;
	virtual void start() = 0;
	virtual void restart() = 0;
	virtual void close() = 0;
	virtual ~IPort() 
	{}
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
			3,
			NULL,
			&ovlp.ovl);
	}
	void restart() override {
		ovlp.refresh();
		ReadFile(handle,
			ovlp.buffer,
			3,
			NULL,
			&ovlp.ovl);
	}
	void close() override {
		CloseHandle(handle);
	}
};