#pragma once
#include "Log.h"

#define RE_LogMessage(message) Log::LogMessage("Log: ",message)
#define RE_LogWarning(message) Log::LogMessage("Warning: ",message)
#define RE_LogError(message) Log::LogMessage("Error: ",message)

#ifdef _DEBUG
    #define Break() __debugbreak()
#else
    #define Break()
#endif

#define AssertFunction(result, file, line, value)                                                   \
if (result != value)                                                                                    \
{                                                                                                   \
    RE_LogError("Assert failed at File: " + std::string(file) + "Line: " + std::to_string(line));   \
    Break();                                                                                        \
}                                                                                                   \

#define RE_ASSERT_ZERO(booleanReturn) AssertFunction(booleanReturn, __FILE__, __LINE__,0)

#define RE_ASSERT(booleanReturn) AssertFunction(booleanReturn, __FILE__, __LINE__,true)