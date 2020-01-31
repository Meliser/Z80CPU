#pragma once
#include <Windows.h>
#include <stdio.h>

#include <thread>
static unsigned __stdcall workerThread(void* CompletionPortID)
{

	HANDLE	CompletionPort = (HANDLE)CompletionPortID;
	DWORD	BytesTransferred,
		completionKey;

	LPOVERLAPPED ovl;
	OVERLAPPEDPLUS* ovlPlus;
	printf("Worker thread with id %u initialized\n", std::this_thread::get_id());
	while (true)
	{
		printf("Worker thread with id %u is waiting\n", std::this_thread::get_id());
		GetQueuedCompletionStatus(CompletionPort,
			&BytesTransferred,
			&completionKey,
			&ovl,
			INFINITE);
		printf("Worker thread with id %u woke up\n", std::this_thread::get_id());
		if (completionKey == ThreadExitKey) {
			printf("Worker thread with id %u is terminating...\n", std::this_thread::get_id());
			_endthreadex(0);
			return 0;
		}
		ovlPlus = CONTAINING_RECORD(ovl, OVERLAPPEDPLUS, ovl);
		if (BytesTransferred != 0) {
			printf("Worker thread with id %u finished, buffer - > %s\n",
				std::this_thread::get_id(),
				ovlPlus->buffer);
			ovlPlus->isReady = true;
		}
		else {
			printf("Worker thread with id %u detected empty file\n",
				std::this_thread::get_id());
			//Release the handle 
		}

	}

}

