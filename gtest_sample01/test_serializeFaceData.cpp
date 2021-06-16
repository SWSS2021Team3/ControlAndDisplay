#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "../desktop_app01/serialize.h"

using namespace std;

class FaceData : public Serializable
{
public:
	size_t serialize_size() const
	{
		return SerializableP<std::string>::serialize_size(userId) +
			SerializableP<std::vector<std::string> >::serialize_size(faceId);
	}
	void serialize(char* buf) const
	{
		buf = SerializableP<std::string>::serialize(buf, userId);
		buf = SerializableP<std::vector<std::string>>::serialize(buf, faceId);
	}
	void deserialize(const char* buf)
	{
		buf = SerializableP<std::string>::deserialize(buf, userId);
		buf = SerializableP<std::vector<std::string>>::deserialize(buf, faceId);
	}
	std::string userId;
	std::vector<std::string> faceId;
};

template <>
class SerializableP<FaceData>
{
public:
	static size_t serialize_size(FaceData fd)
	{
		return SerializableP<std::string>::serialize_size(fd.userId) +
			SerializableP<std::vector<std::string> >::serialize_size(fd.faceId);
	}

	static char* serialize(char* buf, FaceData fd)
	{
		buf = SerializableP<std::string>::serialize(buf, fd.userId);
		buf = SerializableP<std::vector<std::string>>::serialize(buf, fd.faceId);
		return buf;
	}

	static const char* deserialize(const char* buf, FaceData& fd)
	{
		buf = SerializableP<std::string>::deserialize(buf, fd.userId);
		buf = SerializableP<std::vector<std::string>>::deserialize(buf, fd.faceId);
		return buf;
	}
};

TEST(SerializeFaceData, should_be_equal_vector_facedata)
{
	std::vector<FaceData> vf1;
	std::vector<FaceData> vf2;
	FaceData fd1;

	fd1.userId = "userId1";
	fd1.faceId.push_back("faceimage1");
	fd1.faceId.push_back("faceimage2");
	fd1.faceId.push_back("faceimage3");

	vf1.push_back(fd1);
	size_t t = SerializableP<std::vector<FaceData>>::serialize_size(vf1);
	//EXPECT_EQ(t, 50);

	char* buf = new char[t];
	SerializableP<std::vector<FaceData>>::serialize(buf, vf1);
	SerializableP<std::vector<FaceData>>::deserialize(buf, vf2);
	delete[] buf;

	EXPECT_EQ(vf2.size(), 1);
	EXPECT_EQ(vf2[0].userId, "userId1");
	EXPECT_EQ(vf2[0].faceId.size(), 3);
	EXPECT_EQ(vf2[0].faceId[0], "faceimage1");
	EXPECT_EQ(vf2[0].faceId[1], "faceimage2");
	EXPECT_EQ(vf2[0].faceId[2], "faceimage3");

}

const std::string pathFaceDB = "facedb.bin";

bool writeFile(const std::string& path, const char* ptr, const size_t size)
{
	FILE* fp = fopen(path.c_str(), "wb");
	if (fp) {
		fwrite(ptr, 1, size, fp);
		fclose(fp);
		return true;
	}
	return false;
}
std::string readFileA(const std::string& path)
{
	std::string result;
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		result.resize(std::max(ftell(fp), (long int)0));
		fseek(fp, 0, SEEK_SET);
		auto len = fread(&(result[0]), 1, result.size(), fp);
		fclose(fp);
		if (len != result.size()) {
			result.resize(len);
		}
	}
	return result;
}

int getFileSize(const std::string path) {
	std::ifstream in(path, std::ios_base::in | std::ios_base::binary);
	if (!in.good()) {
		std::cout << "Error to open \"" << path << "\" file." << std::endl;
		return 0;
	}
	in.seekg(0, std::ios_base::end);
	std::streampos filesize = in.tellg(); //fixme: need to check exceed 32bit interger
	std::cout << "filesize: " << filesize << std::endl;
	in.close();
	if (filesize > INT_MAX) { return -1; } //fixme: int_max is still too large.
	return filesize;
}

int SMwriteFaceDB(unsigned char* buffer, size_t bufferSize, size_t* writeLen) {
	writeFile(pathFaceDB, reinterpret_cast<char*>(const_cast<unsigned char*>(buffer)), bufferSize);
	return 0; //error code
}

size_t SMgetSizeFaceDB() {
	return getFileSize(pathFaceDB);
}

int SMreadFaceDB(unsigned char* buffer, size_t bufferSize, size_t* readLen) {
	std::string dbenc = readFileA(pathFaceDB);
	memcpy(buffer, dbenc.c_str(), bufferSize);
	return 1;
}


bool saveFaceDB(std::vector<FaceData>& faceDB)
{
	std::cout << "[FaceManager] saveFaceDB" << endl;
	char* buf = new char[SerializableP<vector<FaceData>>::serialize_size(faceDB)];
	SerializableP<vector<FaceData>>::serialize(buf, faceDB);

	size_t dataSize = SerializableP<vector<FaceData>>::serialize_size(faceDB);
	size_t writeLen;
	bool ret = false;

	cout << "dataSize = " << dataSize << endl;

	printf("data: ");
	for (int i = 0; i < dataSize; i++)
		printf("%02x ", buf[i]);
	printf("\n");

	ret = SMwriteFaceDB((unsigned char*)&buf, dataSize, &writeLen);
	cout << "ret = " << ret << endl;
	cout << "writeLen = " << writeLen << endl;

	delete[] buf;
	return ret;
}

void readFaceDB(std::vector<FaceData>& faceDB)
{
	std::cout << "[FaceManager] readFaceDB" << endl;

	size_t readSize;
	size_t readLen;

	readSize = SMgetSizeFaceDB();
	unsigned char* readData = new unsigned char[readSize];

	int ret = SMreadFaceDB(readData, readSize, &readLen);
	if (ret < 0) {
		cout << "[FaceManager] failed to read faceDB" << endl;
		return;
	}

	std::cout << "[FaceManager] readSize : " << readSize << " readLen : " << readLen << endl;

	printf("data: ");
	for (int i = 0; i < readSize; i++)
		printf("%02x ", readData[i]);
	printf("\n");

	SerializableP<vector<FaceData>>::deserialize(reinterpret_cast<char*>(const_cast<unsigned char*>(readData)), faceDB);

	delete[] readData;

	return;
}
TEST(SerializeFaceData, should_be_equal_saveFaceDB_readFaceDB)
{
	std::vector<FaceData> vf1;
	std::vector<FaceData> vf2;
	FaceData fd1;

	fd1.userId = "userId1";
	fd1.faceId.push_back("faceimage1");
	fd1.faceId.push_back("faceimage2");
	fd1.faceId.push_back("faceimage3");

	vf1.push_back(fd1);
	size_t t = SerializableP<std::vector<FaceData>>::serialize_size(vf1);
	//EXPECT_EQ(t, 50);

	saveFaceDB(vf1);

	readFaceDB(vf2);

	EXPECT_EQ(vf2.size(), 1);
	EXPECT_EQ(vf2[0].userId, "userId1");
	EXPECT_EQ(vf2[0].faceId.size(), 3);
	EXPECT_EQ(vf2[0].faceId[0], "faceimage1");
	EXPECT_EQ(vf2[0].faceId[1], "faceimage2");
	EXPECT_EQ(vf2[0].faceId[2], "faceimage3");

}
