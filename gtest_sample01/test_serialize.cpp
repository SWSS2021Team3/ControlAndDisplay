#include "pch.h"
#include <iostream>
#include "../desktop_app01/MSG_DB.h"
#include "../desktop_app01/TcpSendRecvCmd.h"
#include "../desktop_app01/NetworkTCP.h"
#include "../desktop_app01/serialize.h"

class UserS : public Serializable
{
public:
	size_t serialize_size() const {
		return SerializableP<int>::serialize_size(uid)
			+ SerializableP<int>::serialize_size(age)
			+ SerializableP<std::string>::serialize_size(username)
			+ SerializableP<std::string>::serialize_size(password);
	}
	void serialize(char* buf) const {
		buf = SerializableP<int>::serialize(buf, uid);
		buf = SerializableP<int>::serialize(buf, age);
		buf = SerializableP<std::string>::serialize(buf, username);
		buf = SerializableP<std::string>::serialize(buf, password);
	}
	void deserialize(const char* buf) {
		buf = SerializableP<int>::deserialize(buf, uid);
		buf = SerializableP<int>::deserialize(buf, age);
		buf = SerializableP<std::string>::deserialize(buf, username);
		buf = SerializableP<std::string>::deserialize(buf, password);
	}
	int uid;
	int age;
	std::string username;
	std::string password;
};

TEST(Serialize, should_be_equal_int)
{
	UserS user1;
	UserS user2;

	user1.uid = 99;
	user1.age = 33;

	EXPECT_NE(user2.uid, 99);
	EXPECT_NE(user2.age, 33);

	char* buf = new char[user1.serialize_size()];

	user1.serialize(buf);
	user2.deserialize(buf);

	EXPECT_EQ(user2.uid, 99);
	EXPECT_EQ(user2.age, 33);
}


TEST(Serialize, should_be_equal_string)
{
	UserS user1;
	UserS user2;

	user1.uid = 99;
	user1.age = 33;
	user1.username = "Chidi";
	user1.password = "hello-world";

	EXPECT_NE(user2.uid, 99);
	EXPECT_NE(user2.age, 33);
	EXPECT_NE(user2.username, "Chidi");
	EXPECT_NE(user2.password, "hello-world");

	char* buf = new char[user1.serialize_size()];

	user1.serialize(buf);
	user2.deserialize(buf);

	EXPECT_EQ(user2.uid, 99);
	EXPECT_EQ(user2.age, 33);
	EXPECT_EQ(user2.username, "Chidi");
	EXPECT_EQ(user2.password, "hello-world");
}


static DWORD WINAPI ServerProc(LPVOID lpParam)
{
	UserS* user = (UserS*)lpParam;

	TTcpListenPort* TcpListenPort;
	TTcpConnectedPort* TcpConnectedPort;
	struct sockaddr_in cli_addr;
	socklen_t clilen;

	std::cout << "start server" << std::endl;

	if ((TcpListenPort = OpenTcpListenPort(8086)) == NULL) // Open UDP Network port
	{
		std::cout << "OpenTcpListenPortFailed" << std::endl;
		return (-1);
	}

	clilen = sizeof(cli_addr);

	std::cout << "Listening for connections" << std::endl;

	if ((TcpConnectedPort = AcceptTcpConnection(TcpListenPort, &cli_addr, &clilen)) == NULL)
	{
		std::cout << "AcceptTcpConnection Failed" << std::endl;
		return (-1);
	}

	std::cout << "Accepted connection Request" << std::endl;

	int ret;
	ret = TcpSendObject(TcpConnectedPort, user);
	if (ret < 0) {
		std::cout << "failed to send command" << std::endl;
	}

	return 0;
}
struct TCPServer2
{
	HANDLE hThread;
	DWORD tid;
	UserS user1;

	void listen()
	{
		hThread = CreateThread(NULL, 0, ServerProc, (LPVOID)&user1, 0, &tid);
	}
	void close()
	{
		CloseHandle(hThread);
	}
};

struct TCPClient2
{
	TTcpConnectedPort* TcpConnectedPort = NULL;
	bool retvalue;

	UserS user2;

	int connect(TCPServer2* server)
	{
		if ((TcpConnectedPort = OpenTcpConnection("127.0.0.1", "8086")) == NULL) // Open UDP Network port
		{
			std::cout << "OpenTcpConnection" << std::endl;
			return (-1);
		}
		return 0;
	}
	int recvUser()
	{
		int ret = 0;
		ret = TcpRecvObject(TcpConnectedPort, &user2);
		if (!ret)
		{
			std::cout << "failed to receive payload" << std::endl;
			return -1;
		}
		return 0;
	}
};

TEST(Serialize, should_be_equal_tcp)
{
	TCPServer2 server;
	TCPClient2 client;


	server.user1.uid = 99;
	server.user1.age = 33;
	server.user1.username = "Chidi";
	server.user1.password = "hello-world";

	server.listen();
	EXPECT_EQ(0, client.connect(&server));

	EXPECT_NE(client.user2.uid, 99);
	EXPECT_NE(client.user2.age, 33);
	EXPECT_NE(client.user2.username, "Chidi");
	EXPECT_NE(client.user2.password, "hello-world");

	EXPECT_EQ(0, client.recvUser());

	EXPECT_EQ(client.user2.uid, 99);
	EXPECT_EQ(client.user2.age, 33);
	EXPECT_EQ(client.user2.username, "Chidi");
	EXPECT_EQ(client.user2.password, "hello-world");
	server.close();
}
