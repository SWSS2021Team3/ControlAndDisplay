#include "pch.h"

#include "../ACS/user.h"
#include "../ACS/studentController.h"
#include "mockCommManager.h"
#include "mockUserAuthManager.h"

using namespace std;
using testing::Return;
using testing::_;

TEST(FaceImage, should_be_received) {
	User user = { 501, "user1" };

	MockCommManager commManager;
	MockUserAuthManager uam;

	StudentController sc(&commManager, &uam);

	EXPECT_CALL(uam, getCurrentUser()).WillOnce(Return(user));
	EXPECT_CALL(commManager, requestFaces(user.uid))
		.WillOnce(
			[&sc](const int uid) {
				cv::Mat image;
				for (int i = 0; i < sc.NUMBER_OF_FACES_REQ; i++)
					sc.onFaceImageReceive(image);

				return true;
			});

	EXPECT_TRUE(sc.fetchFaces());

	EXPECT_EQ(sc.getFaces().size(), sc.NUMBER_OF_FACES_REQ);
}

// TODO: edge case
