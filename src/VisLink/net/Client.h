#ifndef VISLINK_CLIENT_H_
#define VISLINK_CLIENT_H_

#include <string>
#include <iostream>

#include "VisLink/net/NetInterface.h"
#include "VisLink/VisLinkAPI.h"

namespace vislink {

class ClientMessageQueue : public MessageQueue {
public:
	ClientMessageQueue(NetInterface* net, SOCKET socketFD, int id);
	virtual ~ClientMessageQueue() {}
	int getId();
	void waitForMessage();
	void sendMessage();
	int sendData(const unsigned char *buf, int len);
	int receiveData(unsigned char *buf, int len);
private:
	NetInterface* net;
	SOCKET socketFD;
	int id;
};

class Client : public NetInterface  {
public:
	Client(const std::string &serverIP = "127.0.0.1", int serverPort = 3457);
	~Client();

	virtual void createSharedTexture(const std::string& name, const TextureInfo& info, int deviceIndex) {
	    sendMessage(socketFD, MSG_createSharedTexture, (const unsigned char*)name.c_str(), name.size());
	    sendData(socketFD, (unsigned char*)& deviceIndex, sizeof(int));
	    sendData(socketFD, (unsigned char*)& info, sizeof(TextureInfo));
	}

	virtual Texture getSharedTexture(const std::string& name, int deviceIndex) { 
	    sendMessage(socketFD, MSG_getSharedTexture, (const unsigned char*)name.c_str(), name.size());
	    sendData(socketFD, (unsigned char*)& deviceIndex, sizeof(int));
	    Texture tex;
#ifdef WIN32
		//HANDLE currentProcess = GetCurrentProcess();
		//sendData(socketFD, (unsigned char*)&currentProcess, sizeof(HANDLE));
		int pid = GetCurrentProcessId();
		receiveData(socketFD, (unsigned char*)& pid, sizeof(int));
		std::cout << "Pid client: " << pid << std::endl;
        receiveData(socketFD, (unsigned char*)& tex, sizeof(tex));

        HANDLE serverProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid);
		tex.externalHandle = getExternalHandle(serverProcess, tex.externalHandle);
		//tex.externalHandle = 0;
#else
		int fd = NetInterface::recvfd(socketFD);
		receiveData(socketFD, (unsigned char*)& tex, sizeof(tex));
		tex.externalHandle = fd;
#endif
		return tex;
	}

#ifdef WIN32
	HANDLE getExternalHandle(HANDLE serverProcess, HANDLE externalHandle) {
		HANDLE externalHandleDup;
		std::cout << externalHandle << " eh1" << std::endl;
		DuplicateHandle(serverProcess,
			externalHandle,
			GetCurrentProcess(),
			&externalHandleDup,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS);
		std::cout << externalHandleDup << " eh2" << std::endl;
		return externalHandleDup;
	}
#endif

	MessageQueue* getMessageQueue(const std::string& name) { 
		sendMessage(socketFD, MSG_getMessageQueue, (const unsigned char*)name.c_str(), name.size());
		int queueId;
		receiveData(socketFD, (unsigned char*)& queueId, sizeof(queueId));
		MessageQueue* queue = new ClientMessageQueue(this, socketFD, queueId);
		messageQueues.push_back(queue);
		return queue;
	}

	virtual Semaphore getSemaphore(const std::string& name, int deviceIndex) { 
	    sendMessage(socketFD, MSG_getSemaphore, (const unsigned char*)name.c_str(), name.size());
	    sendData(socketFD, (unsigned char*)& deviceIndex, sizeof(int));
	    Semaphore sem;
#ifdef WIN32
		int pid = GetCurrentProcessId();
		receiveData(socketFD, (unsigned char*)& pid, sizeof(int));
        receiveData(socketFD, (unsigned char*)& sem, sizeof(sem));

        HANDLE serverProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid);
		sem.externalHandle = getExternalHandle(serverProcess, sem.externalHandle);
#else
		int fd = NetInterface::recvfd(socketFD);
		int semaphoreFDs[NUM_TEXTURE_SEMAPHORES];
		receiveData(socketFD, (unsigned char*)& sem, sizeof(sem));
		sem.externalHandle = fd;
#endif
		return sem;
	}

private:
	std::vector<MessageQueue*> messageQueues;
	SOCKET socketFD;
};

}

#endif
