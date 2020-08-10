#include "logger-v3.h"

int longer_function_name()
{
	LOG_DEBUG("Long function name test");
}

int main()
{
	int test;
	LOG_DEBUG("Test");
	LOG_INFO("Test");
	LOG_OK("Test");
	LOG_WARN("Test");
	LOG_ERROR("Test");
	longer_function_name();
}
