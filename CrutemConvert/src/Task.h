/*
	This system is designed to replace std::async() calls so you can
	customize the executation of those calls in a more flexible and
	performant manner.

	Anywhere you would use std::async(), you should use tb::async.

	You will need to remove any std::launch:: business, though, since
	tb::async is always asynchronous (unless you call get() on the
	shared_future first, of course.  But, by that time, you have
	apparently have nothing better to do anyway.
*/

#ifndef L_TB_TASK_H
#define L_TB_TASK_H

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>

using namespace std;

#include "TaskPrivate.h"

namespace tb	{

/*
	Thread prioritization:
		tb::Priority::

	DEFAULT	- same as NORMAL, but can be changed w/SetDefaultPriority(...)
	IDLE	- back of high latency queue
	LOW		- front of high latency queue
	NORMAL	- back of normal latency queue
	HIGH	- front of normal latency queue
	URGENT	- back of low latency queue
	SUPER	- front of low latency queue

	With OS support

*/

/*
	Use these rather than TBTaskGroup's interface!
*/

void	SetDefaultPriority	(Priority = Priority::DEFAULT);
int		CountPending		(Priority = Priority::DEFAULT);
void	Finalize			(Priority = Priority::DEFAULT);
void	ReInitialize		(Priority = Priority::DEFAULT);
	// must call ReInitialize() after Finalize() to use


unsigned long long 	TotalExecuted();
void				ResetTotalExecuted();


class	Repeating	{
public:
								Repeating	();
								Repeating	(priv::RepeatOwner*);
								Repeating	(Repeating&&);
								~Repeating	();

	unsigned long long			Last			() const;
			int					CountRemaining	() const;
			int					Interval 		() const;

			bool				Cancel();
			bool				SetInterval		(int);
			bool				SetCount		(int);

			void				WaitOnce		() const;

private:
		friend class priv::RepeatOwner;
			void				OwnerDestroyed	();
		Repeating(const Repeating&){/*no copy, move only!*/}

		mutable	recursive_mutex	fMutex;
		priv::RepeatOwner*		fOwner;
mutable	condition_variable_any	fCondition;
			bool				fDeathIsBliss;
};



template <typename T>
shared_future<T>	assign(shared_future<T> fut,
							Priority prio = Priority::DEFAULT,
							int interval = -1, int count = 0)
{
	using namespace priv;
	auto fown =	new FutureOwnerT<T>(fut);

	switch (prio) {
		case Priority::DEFAULT:
		case Priority::NORMAL:
		case Priority::HIGH:
			priv::gTGNorm->Assign(fown, prio);
			break;
		case Priority::IDLE:
		case Priority::LOW:
			priv::gTGLow->Assign(fown, prio);
			break;
		default:
			priv::gTGHigh->Assign(fown, prio);
	}

	return fut;
}


template <typename T, typename... A>
shared_future<typename result_of<T(A...)>::type>
	async(Priority prio, T&& func, A&&... args)
{
	return assign<typename result_of<T(A...)>::type>
		(async(launch::deferred, func, args...).share(), prio);
}


template <typename T, typename... A>
shared_future<typename result_of<T(A...)>::type>
	async(T&& func, A&&... args)
{
	return async(Priority::DEFAULT, func, args...);
}


template <typename T, typename... A>
Repeating&&
	async(Priority prio, int interval, int count, T&& _func, A&&... args)
{	// called by all other repeater async() calls

	std::function<void()> func = std::bind(_func, args...);

	auto fown = new priv::RepeatOwner(interval, count, func);

	switch (prio) {
		case Priority::DEFAULT:
		case Priority::NORMAL:
		case Priority::HIGH:
			priv::gTGNorm->Assign(fown);
			break;
		case Priority::IDLE:
		case Priority::LOW:
			priv::gTGLow->Assign(fown);
			break;
		default:
			priv::gTGHigh->Assign(fown);
	}

	return move(*(fown->Watcher()));
}


template <typename T, typename... A>
Repeating&&
	async(int interval, T&& func, A&&... args)
{
	return std::move(async(Priority::DEFAULT, interval, INT_MAX, func, args...));
}


template <typename T, typename... A>
Repeating&&
	async(int interval, int count, T&& func, A&&... args)
{
	return std::move(async(Priority::DEFAULT, interval, count, func, args...));
}


template <typename T, typename... A>
Repeating&&
	async(Priority prio, int interval, T&& func, A&&... args)
{
	return std::move(async(prio, interval, INT_MAX, func, args...));
}


}; // namespace tb

#endif // L_TB_TASK_H
