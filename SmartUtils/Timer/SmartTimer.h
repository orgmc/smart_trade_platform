/*
 * CSmartTimers.h
 *
 *  Created on: Apr 1, 2015
 *      Author: rock
 */

#ifndef SMARTTIMERS_H_
#define SMARTTIMERS_H_

#include <cstdint>
#include <memory>
#include <thread>
#include <set>
#include <atomic>
#include <mutex>
#include "../Common/Defines.h"

#define MAX_TIMERS (1024)

namespace NSSmartUtils
{

enum ETimerType
{
	ETT_REALTIME = 0, ETT_MONOTONIC = 1
};

enum EErrCode
{
	EEC_SUC = 0, EEC_ERR = -1
};

class CSmartTimers;

class ITimer
{
	friend CSmartTimers;

	DISABLE_COPY(ITimer)
	DISABLE_MOVE(ITimer)

protected:
	explicit ITimer(const ETimerType timer_type, int64_t interval_seconds,
			int64_t interval_nanos) :
			Registered_(false), FileDescriptor_(-1), TimerType_(timer_type), InitExpireSeconds_(
					interval_seconds), InitExpireNanos_(interval_nanos), IntervalSeconds_(
					interval_seconds), IntervalNanos_(interval_nanos)
	{
	}

	virtual ~ITimer();

	int32_t Create();

	bool IsRegistered()
	{
		return Registered_;
	}
	void Registered()
	{
		Registered_ = true;
	}

	int32_t GetFD()
	{
		return FileDescriptor_;
	}

	virtual void HandleTimerEvent(uint64_t ui64Times) = 0;

public:

	static inline uint32_t GetMaxTimers()
	{
		return MAX_TIMERS;
	}

	int64_t GetIntervalSeconds()
	{
		return IntervalSeconds_;
	}

	int64_t GetIntervalNanos()
	{
		return IntervalNanos_;
	}

private:
	bool Registered_;
	int32_t FileDescriptor_;
	int32_t TimerType_;
	int64_t InitExpireSeconds_;
	int64_t InitExpireNanos_;
	int64_t IntervalSeconds_;
	int64_t IntervalNanos_;
};
typedef std::shared_ptr<NSSmartUtils::ITimer> TimerPtr_t;

class CSmartTimers
{
	DISABLE_COPY(CSmartTimers)
	DISABLE_MOVE(CSmartTimers)

public:
	explicit CSmartTimers();
	virtual ~CSmartTimers();

public:

	int32_t Start();
	int32_t Stop();

	int32_t RegisterTimer(TimerPtr_t &ptimer);
	uint32_t GetMaxTimer();

	void HandleTimers();

private:
	volatile bool StopFlag_;
	typedef std::set<TimerPtr_t> TimersSet_t;
	TimersSet_t Timers_;
	typedef std::shared_ptr<std::thread> ThreadPtr_t;
	ThreadPtr_t pThread_;
	std::mutex StateLock_, TimersLock_;
	int32_t EpollFD_;
};

} /* namespace ns_utils */

#endif /* SMARTTIMERS_H_ */
