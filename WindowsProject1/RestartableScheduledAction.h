#pragma once

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class RestartableScheduledAction
{
private:
	std::function<void()> action;
	std::chrono::seconds delay;
	std::thread worker;
	std::mutex mtx;
	std::condition_variable cv;
	bool cancelRequested;
	std::atomic<bool> isRunning;

	void JoinIfJoinable();
	void SetCancelRequested(bool value);
	void CancelCurrent();

public:
	explicit RestartableScheduledAction(std::function<void()> action, std::chrono::seconds delay = std::chrono::seconds(10));
	~RestartableScheduledAction();

	void ScheduleAfterDelay();
	void Cancel();
	bool IsRunning() const;
};
