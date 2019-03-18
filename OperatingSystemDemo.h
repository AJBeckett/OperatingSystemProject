#pragma once

#include "targetver.h"
#include <Windows.h>

#include <crtdbg.h>
#ifdef _DEBUG
#define ASSERT(stmt) _ASSERT(stmt)
// ASSERT_EXP
#define VERIFY(stmt) _ASSERT(stmt)
#else
#define ASSERT(stmt)
#define VERIFY(stmt) stmt
#endif