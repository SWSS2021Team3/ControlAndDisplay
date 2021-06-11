#pragma once
#include <opencv2/core/core.hpp>
#include <vector>

class Payload
{
public:
	uint16_t data_id;
	uint16_t data_length;
	std::vector<uchar> data;
};
