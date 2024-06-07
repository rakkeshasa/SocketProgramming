#include "pch.h"
#include "CoreTLS.h"

// 스레드별로 전역 공간(TLS)

thread_local uint32 LThreadId = 0;