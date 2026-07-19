#include "RestartableScheduledAction.h"
#include <iostream>
#include <utility>

extern void LogDebug(const wchar_t* format, ...);

void RestartableScheduledAction::JoinIfJoinable()
{
	if (worker.joinable())
	{
		worker.join();
	}
}

void RestartableScheduledAction::SetCancelRequested(bool value)
{
	std::lock_guard<std::mutex> lock(mtx);
	cancelRequested = value;
}

void RestartableScheduledAction::CancelCurrent()
{
	SetCancelRequested(true);
	cv.notify_all();
	JoinIfJoinable();
}

RestartableScheduledAction::RestartableScheduledAction(std::function<void()> action, std::chrono::seconds delay)
	: action(std::move(action)), delay(delay), cancelRequested(false), isRunning(false) {}

RestartableScheduledAction::~RestartableScheduledAction()
{
	CancelCurrent();
}

//--------------------------------------------------------------

void RestartableScheduledAction::ScheduleAfterDelay()
{
	if (!action)
		return;

	if (isRunning)
	{
		LogDebug(L"Action is already running. Restarting...");
		CancelCurrent();
	}

	isRunning = true;
	LogDebug(L"Action started.");

	JoinIfJoinable();
	SetCancelRequested(false);

	worker = std::thread([this]
	{
		std::unique_lock<std::mutex> lock(mtx);
		bool cancelled = cv.wait_for(lock, delay, [this] { return cancelRequested; });
		lock.unlock();

		if (!cancelled)
			action();
		else
			LogDebug(L"Action was cancelled before execution.");

		isRunning = false;
	});
}

void RestartableScheduledAction::Cancel()
{
	if (isRunning)
	{
		CancelCurrent();
		LogDebug(L"Action stopped.");
	}
	else
	{
		LogDebug(L"Action is not running.");
	}
}

bool RestartableScheduledAction::IsRunning() const
{
	return isRunning;
}
