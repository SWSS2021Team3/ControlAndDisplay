#include "pch.h"
#include <iostream>
#include "../desktop_app01/MSG_DB.h"
#include "../desktop_app01/TcpSendRecvCmd.h"
#include "../desktop_app01/NetworkTCP.h"

Payload* createCmdPacket(int cmd)
{
	Payload* payload = new Payload();
	payload->data_id = cmd;
	payload->data_length = 0;
	// payload->data = NULL; // TODO: set to NULL
	return payload;
}

DWORD WINAPI ServerProc(LPVOID lpParam)
{
	std::ostream* os = (std::ostream *)lpParam;

	TTcpListenPort* TcpListenPort;
	TTcpConnectedPort* TcpConnectedPort;
	struct sockaddr_in cli_addr;
	socklen_t clilen;

	*os << "start server" << std::endl;

	if ((TcpListenPort = OpenTcpListenPort(8087)) == NULL) // Open UDP Network port
	{
		*os << "OpenTcpListenPortFailed" << std::endl;
		return (-1);
	}

	clilen = sizeof(cli_addr);

	*os << "Listening for connections" << std::endl;

	if ((TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen)) == NULL)
	{
		*os << "AcceptTcpConnection Failed" << std::endl;
		return (-1);
	}

	*os << "Accepted connection Request" << std::endl;
	Payload* payload = createCmdPacket(SIGNAL_FM_RESP_GET_FACES);
	if (payload == NULL) {
		std::cout << "unable to create a payload" << std::endl;
	}
	int ret;
	ret = TcpSendCommand(TcpConnectedPort, payload);
	if (ret < 0) {
		std::cout << "failed to send command" << std::endl;
	}

	return 0;
}
struct TCPServer
{
	HANDLE hThread;
	DWORD tid;
	void listen()
	{
		hThread = CreateThread(NULL, 0, ServerProc, (LPVOID) &std::cerr, 0, &tid);
	}
	void close()
	{
		CloseHandle(hThread);
	}
};

struct TCPClient
{
	TTcpConnectedPort* TcpConnectedPort = NULL;
	bool retvalue;

	int connect(TCPServer* server)
	{
		if ((TcpConnectedPort = OpenTcpConnection("127.0.0.1", "8087")) == NULL) // Open UDP Network port
		{
			std::cout << "OpenTcpConnection" << std::endl;
			return (-1);
		}
		return 0;
	}
	int recvCommand()
	{
		Payload payload;

		int ret = 0;
		ret = TcpRecvCommand(TcpConnectedPort, &payload);
		if (!ret)
		{
			std::cout << "failed to receive payload" << std::endl;
			return -1;
		}
		return payload.data_id;
	}
};


TEST(TCPCommand, should_be_send_recv_tcp_cmd)
{
	TCPServer server;
	TCPClient client;

	server.listen();
	EXPECT_EQ(client.connect(&server), 0);
	EXPECT_EQ(client.recvCommand(), SIGNAL_FM_RESP_GET_FACES);
	server.close();
}