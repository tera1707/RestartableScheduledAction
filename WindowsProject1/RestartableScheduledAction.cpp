#include "RestartableScheduledAction.h"
#include <iostream>
#include <utility>

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
		std::cout << "Action is already running. Restarting..." << std::endl;
		CancelCurrent();
	}

	isRunning = true;
	std::cout << "Action started." << std::endl;

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
			std::cout << "Action was cancelled before execution." << std::endl;

		isRunning = false;
	});
}

void RestartableScheduledAction::Cancel()
{
	if (isRunning)
	{
		CancelCurrent();
		std::cout << "Action stopped." << std::endl;
	}
	else
	{
		std::cout << "Action is not running." << std::endl;
	}
}

bool RestartableScheduledAction::IsRunning() const
{
	return isRunning;
}
