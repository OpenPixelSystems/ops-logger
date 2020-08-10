#include "logger.h"

void longer_function_name()
{
	LOG_DEBUG("Long function name test");
}

int main()
{
	LOG_DEBUG("Test");
	LOG_INFO("Test");
	LOG_OK("Test");
	LOG_WARN("Test");
	LOG_ERROR("Test");
	longer_function_name();
}
