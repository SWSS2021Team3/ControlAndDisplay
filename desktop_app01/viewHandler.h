#pragma once

class StudentViewHandler
{
public:
	virtual void onFaceImageUpdate() = 0;
};

class UserAuthViewHandler
{
public:
	virtual void onFoo() = 0;
};
