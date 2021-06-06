#include "pch.h"
#include "../desktop_app01/client.h"

TEST(Connection, should_be_false_when_connect_failed) {
	Client c;

	bool connectionResult = c.connect("no_where", "not_port");
	EXPECT_EQ(connectionResult, false);
}