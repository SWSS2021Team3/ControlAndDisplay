#include "pch.h"
#include "../desktop_app01/attendanceChecker.h"
#include "../desktop_app01/commManager.h"
#include "../desktop_app01/userAuthManager.h"

TEST(Connection, should_be_false_when_connect_failed) {
	AttendanceChecker acs;

	bool connectionResult = acs.connect("no_where", "not_port");
	EXPECT_EQ(connectionResult, false);
}
