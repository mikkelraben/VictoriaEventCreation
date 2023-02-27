#pragma once
#include "Log.h"

#define RE_LogMessage(message) Log::LogMessage("Log: ",message)
#define RE_LogWarning(message) Log::LogMessage("Warning: ",message)
#define RE_LogError(message) Log::LogMessage("Error: ",message)

#ifdef _DEBUG
    #define Break() Log::SaveLogs(); __debugbreak()
#else
    #define Break()
#endif

#define AssertFunction(result, file, line, value)                                                   \
if (result != value)                                                                                    \
{                                                                                                   \
    RE_LogError("Assert failed at File: " + std::string(file) + " Line: " + std::to_string(line) + " Expected: " + std::to_string(value) + " Got: " + std::to_string(result));   \
    Break();                                                                                        \
}                                                                                                   \

#define RE_ASSERT_ZERO(booleanReturn) AssertFunction(booleanReturn, __FILE__, __LINE__,0)

#define RE_ASSERT(booleanReturn) AssertFunction(booleanReturn, __FILE__, __LINE__,true)

#ifdef _DEBUG
#define RE_DEBUG_ASSERT_ZERO(booleanReturn) AssertFunction(booleanReturn, __FILE__, __LINE__,0)

#define RE_DEBUG_ASSERT(booleanReturn) AssertFunction(booleanReturn, __FILE__, __LINE__,true)
#else
#define RE_DEBUG_ASSERT_ZERO(booleanReturn)

#define RE_DEBUG_ASSERT(booleanReturn)
#endif // _DEBUG

