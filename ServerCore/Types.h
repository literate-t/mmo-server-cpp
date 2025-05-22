#pragma once

#include <mutex>
#include <atomic>
#include <array>

// 언리얼 엔진에서도 등장하는 방법
using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

template<typename T>
using Atomic = std::atomic<T>;
using Mutex = std::mutex;
template<typename T, uint32 Size>
using Array = std::array<T, Size>;
using ConditionVariable = std::condition_variable;
using UniqueLock = std::unique_lock<std::mutex>;
using LockGuard = std::lock_guard<std::mutex>;

using SharedIocpObject = std::shared_ptr<class IocpObject>;
using SharedIocpCore = std::shared_ptr<class IocpCore>;
using SharedSession = std::shared_ptr<class Session>;
using SharedSessionManager = std::shared_ptr<class SessionManager>;
using SharedListenHandler = std::shared_ptr<class ListenHandler>;
using SharedService = std::shared_ptr<class Service>;
using SharedServerService = std::shared_ptr<class ServerService>;
using SharedClientService = std::shared_ptr<class ClientService>;
using SharedSendBuffer = std::shared_ptr<class SendBuffer>;
using SharedSendBufferChunk = std::shared_ptr<class SendBufferChunk>;
using SharedPacketSession = std::shared_ptr<class PacketSession>;
using SharedJob = shared_ptr<class Job>;
using SharedJobQueue = shared_ptr<class JobQueue>;

#define sizeof16(val)		static_cast<int16>(sizeof(val))
#define sizeof32(val)		static_cast<int32>(sizeof(val))
#define length16(arr)		static_cast<int16>(sizeof16(arr)/sizeof16(arr[0]))
#define length32(arr)		static_cast<int32>(sizeof32(arr)/sizeof32(arr[0]))

//#define _STOMP