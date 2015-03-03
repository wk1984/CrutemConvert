#include <cstdint>
#include <deque>
#include <limits>
#include <iostream>
#include <pthread.h>
#include <string>

#include "Task.h"

#ifndef INT_MAX
	#	define INT_MAX	std::numeric_limits<int>::max()
#endif

unsigned long long ms_time()
{
	using namespace std::chrono;
	auto timePoint = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(timePoint.time_since_epoch());
	return duration.count();
}




namespace tb {


/*
	erase_if
*/

template<typename C, typename R>
bool	erase_if(C& c, R func)
{
	auto it = remove_if(c.begin(), c.end(), func);

	if (it != c.end()) {
		c.erase(it);
		return true;
	}

	return false;
}


//#pragma mark Repeating


Repeating	::		Repeating	()
	:
	fOwner(nullptr),
	fDeathIsBliss(false)
	{}


Repeating	::	Repeating(priv::RepeatOwner* owner)
	:
	fOwner(owner),
	fDeathIsBliss(false)
	{}


Repeating	::		Repeating	(Repeating&& other)
	:
	fOwner(nullptr),
	fDeathIsBliss(false)
	{
		lock_guard<recursive_mutex> _(other.fMutex);
		fOwner = other.fOwner;
		fDeathIsBliss = other.fDeathIsBliss;

		other.fOwner = nullptr;

		if (fOwner != nullptr)
			fOwner->WatcherMoved(this);
	}


Repeating	::~Repeating	()
{
	fMutex.try_lock();

	if (fOwner != nullptr)
		fOwner->WatcherMoved(nullptr);
}


unsigned long long
Repeating	::	Last() const
{
	if (fOwner != nullptr)
		return fOwner->Last();
	else
		return 0;
}


int
Repeating	::	CountRemaining	() const
{
	if (fOwner != nullptr)
		return fOwner->Count();
	else
		return 0;
}


int
Repeating	::	Interval 		() const
{
	if (fOwner != nullptr)
		return fOwner->Interval();

	return 0;
}


bool
Repeating	::	Cancel()
{
	lock_guard<recursive_mutex> _(fMutex);
	if (fOwner != nullptr
		&&(	priv::gTGLow->Cancel(fOwner)
			||	priv::gTGNorm->Cancel(fOwner)
			||	priv::gTGHigh->Cancel(fOwner)	)	) {
		// DO NOT ACCESS fOwner, it is now invalid!
		fOwner = nullptr;
		return true;
	}

	return false;
}


bool
Repeating	::	SetInterval		(int _int)
{
	lock_guard<recursive_mutex> _(fMutex);
	if (fOwner != nullptr) {
		fOwner->SetInterval(_int);
		return true;
	}

	return false;
}


bool
Repeating	::	SetCount		(int _cnt)
{
	lock_guard<recursive_mutex> _(fMutex);
	if (fOwner != nullptr) {
		fOwner->SetCount(_cnt);
		return true;
	}

	return false;
}


void
Repeating	::	WaitOnce		() const
{
	unique_lock<recursive_mutex> _(fMutex);
	fCondition.wait(_);
}


void
Repeating	::	OwnerDestroyed ()
{
	/*
		Worst case scenario is we don't get the lock and a thread
		just returned from a WaitOnce() call and has just passed
		the fOwner != nullptr test.

		We can't acquire the mutex here reliably (resource deadlock),
		because of the call chain:

			Cancel()// locks mutex
				gTGNorm->Cancel(fOwner)
						~RepeatOwner();
							fWatcher->OwnerDestroyed()
								// can't lock mutex!

		While yielding on WaitOnce() is unlikely while Cancel() is
		called (you have to have passed the object by pointer to
		another thread... but that's something I want to permit!), it
		is very possible that the owner will be destroyed as it completes
		its intended number of runs and a thread returning from WaitOnce()
		could call our methods.

		For this reason we use a recursive_mutex and have to call
		notify_*() twice on fCondition, since condition_variable_any will
		only step back once on the recursive mutex locks and we usually
		want every waiter to be notified, assuming we are being used as
		a timing mechanism.
	*/
	lock_guard<recursive_mutex> _(fMutex);
	fOwner = nullptr;
	fCondition.notify_one();
	fCondition.notify_all();
}

//#pragma mark AutoMutexLocker - for debugging




class	AutoMutexLocker {
public:
			AutoMutexLocker(mutex& mut, const char* caller)
			:
			fMutex(mut),
			fCaller(caller)
			{
//				cout << fCaller << ": lock attempt\n";
				fMutex.lock();
//				cout << fCaller << ": lock acquired\n";
			}

			~AutoMutexLocker()
			{
//				cout << fCaller << ": unlock\n";
				fMutex.unlock();
			}

	mutex&	fMutex;
	string	fCaller;

};


void	SetDefaultPriority(Priority prio)
{
	priv::gTGNorm->SetPriority(prio);
}


int	CountPending(Priority prio)
{
	int count = 0;

	switch (prio) {
		case Priority::DEFAULT:	// counts all
		case Priority::SUPER:
		case Priority::URGENT:
			count += priv::gTGHigh->CountPending();
		case Priority::HIGH:
		case Priority::NORMAL:
			count += priv::gTGNorm->CountPending();
		case Priority::LOW:
		case Priority::IDLE:
			count += priv::gTGLow->CountPending();
	}
	return count;
}


void	Finalize(Priority prio)
{
	switch (prio) {
		case Priority::DEFAULT:	// counts all
		case Priority::SUPER:
		case Priority::URGENT:
			priv::gTGHigh->Finalize();
		case Priority::HIGH:
		case Priority::NORMAL:
			priv::gTGNorm->Finalize();
		case Priority::LOW:
		case Priority::IDLE:
			priv::gTGLow->Finalize();
	}
}


void	ReInitialize(Priority prio)
{
	switch (prio) {
		case Priority::DEFAULT:	// counts all
		case Priority::SUPER:
		case Priority::URGENT:
			priv::gTGHigh->Finalize(false);
		case Priority::HIGH:
		case Priority::NORMAL:
			priv::gTGNorm->Finalize(false);
		case Priority::LOW:
		case Priority::IDLE:
			priv::gTGLow->Finalize(false);
	}
}


unsigned long long 	TotalExecuted()
{
	return priv::gTotalExecuted;
}

void	ResetTotalExecuted()
{
	priv::gTotalExecuted = 0;
}


using namespace std;

namespace priv {


//#pragma mark RepeatOwner


RepeatOwner	::	RepeatOwner(int _int, int _cnt, std::function<void()> _func)
	:
	fFunction(_func),
	fLast(0),
	fCount(_cnt),
	fInterval(_int),
	fWatcher(new Repeating(this)),
	fOrigWatcher(fWatcher)
{
}


RepeatOwner	::	~RepeatOwner()
{
	lock_guard<mutex> _(fMutex);

	if (fWatcher != nullptr)
		fWatcher->OwnerDestroyed();

	delete fOrigWatcher;
}


bool
RepeatOwner	::	Execute()
{
	lock_guard<mutex> _(fMutex);
	fLast = ms_time();
	--fCount;
	if (fCount < 0)
		return false;

	fFunction();

	if (fWatcher != nullptr)
		fWatcher->fCondition.notify_all();

	return true;
}


void
RepeatOwner	::	WatcherMoved(Repeating* to)
{
	lock_guard<mutex> _(fMutex);

	// we don't need to do any checks
	delete fOrigWatcher;
	fOrigWatcher = nullptr;

	fWatcher = to;
}


unsigned long long
RepeatOwner	::	Last() const
{
	lock_guard<mutex> _(fMutex);
	return fLast;
}


int
RepeatOwner	::	Interval() const
{
	lock_guard<mutex> _(fMutex);
	return fInterval;
}


int
RepeatOwner	::	Count() const
{
	lock_guard<mutex> _(fMutex);
	return fCount;
}


Repeating*
RepeatOwner	::	Watcher()
{
	lock_guard<mutex> _(fMutex);
	return fWatcher;
}


void
RepeatOwner	::	SetInterval(int _int)
{
	lock_guard<mutex> _(fMutex);
	fInterval = _int;
}


void
RepeatOwner	::	SetCount(int _cnt)
{
	lock_guard<mutex> _(fMutex);
	fCount = _cnt;
}




//#pragma mark -



unsigned long long gTotalExecuted = 0;

// Handles a group of threads and a single task queue
// held as a vector of shared_futures
TBTaskGroup	::	TBTaskGroup		(Priority prio)
	:
	fPriority(prio == Priority::DEFAULT ? Priority::NORMAL : prio),
	fFinalized(false),
	fMaxThreads(thread::hardware_concurrency()),
	fMaxInFlight(fMaxThreads * 10),
	fInFlightResume(fMaxThreads * 3)
	{
		if (fPriority <= Priority::LOW) {
			fMaxThreads = thread::hardware_concurrency() / 2;
			fMaxInFlight = fMaxThreads * 10;
			fInFlightResume = fMaxThreads * 3;
		}
	}


TBTaskGroup	::	~TBTaskGroup	()
{
	// kill threads cleanly
	Finalize(true);

	while (fThreads.size() != 0) {
		fThreadWait.notify_all();
		this_thread::yield();
	}

	while (fRepeatThread != nullptr)
		this_thread::yield();
}


void
TBTaskGroup	::	SetPriority(Priority prio)
{
	if (prio == Priority::DEFAULT)
		fPriority = Priority::NORMAL;
	else
		fPriority = prio;

	if (prio > Priority::LOW)
		fMaxThreads = thread::hardware_concurrency();
	else
		fMaxThreads = thread::hardware_concurrency()/2;

	fMaxInFlight = fMaxThreads * 10;
	fInFlightResume = fMaxThreads * 3;

	// newly created OS threads will be at the new priority and
	// threads will die off through attrition while additional
	// threads will be created as needed

	// Overriding the default priority is the only time when this
	// will be called and users should do so before loading up the
	// queues.
}


void
TBTaskGroup	::	Assign(FutureOwner* fut, Priority prio)
{
	if (fFutures.size() > fMaxInFlight) {
		unique_lock<mutex> locker(fInFlightMutex);
		fInFlightExceeded.wait_for(locker, chrono::milliseconds(15));
	}

	AutoMutexLocker _(fMutex, "Assign");

	if (fFinalized)
		throw	"Assignment attempted after Finalize()";

	switch (prio) {
		case Priority::SUPER:
		case Priority::HIGH:
		case Priority::LOW:
			fFutures.push_front(fut);
			break;

		case Priority::DEFAULT:
		case Priority::IDLE:
		case Priority::NORMAL:
		case Priority::URGENT:
			fFutures.push_back(fut);
	}

	// TODO: consider time instead of the future count
	// That means I need to track the time before executions
	// for each FutureOwner, of course.
	if (fFutures.size() > 5 || fThreads.size() == 0) {
		if (fThreads.size() < fMaxThreads)
			fThreads.emplace_back(thread(&TBTaskGroup::_Thread, this,
				fThreads.size() == 0));
	}

	fThreadWait.notify_one();
}


void
TBTaskGroup	::	Assign(RepeatOwner* fut)
{
	AutoMutexLocker _(fRepeatMutex, "Assign-Repeat");

	if (fFinalized)
		throw	"Assignment attempted after Finalize()";

	fRepeats.push_back(fut);

	// sorting is fast
	sort(fRepeats.begin(), fRepeats.end(), [](RepeatOwner* a, RepeatOwner* b){
		return a->Interval() < b->Interval();
	});

	if (fRepeatThread == nullptr)
		fRepeatThread = new thread(&TBTaskGroup::_RepeaterThread, this);
}



int
TBTaskGroup	::	Execute()
{
	auto fut = _Next();
	if (fut == nullptr)
		return 0;

	fut->Execute();
	delete fut;
	return CountPending();
}


int
TBTaskGroup	::	CountPending()
{
//	AutoMutexLocker _(fMutex, "CountPending");
	return fFutures.size();
}


void
TBTaskGroup	::	Finalize(bool final)
{
	unique_lock<mutex> _(fMutex);
	fFinalized = final;
	if (!final)	return;
	_.unlock();

	while (fFutures.size() != 0)
		Execute();	// uses current thread to help out!

	// prevent threads from waiting:
	fInFlightExceeded.notify_all();
	fThreadWait.notify_all();
	lock_guard<mutex> __(fRepeatMutex);
	fRepeats.clear();
}


bool
TBTaskGroup	::	Cancel(RepeatOwner* rpt)
{
	if (rpt == nullptr)
		return false;

	lock_guard<mutex> _(fRepeatMutex);
	if (fRepeats.size() == 0)
		return false;

	bool success = false;

	erase_if(fRepeats,
			[rpt, &success](RepeatOwner* owner){
			if (rpt == owner) {
				delete rpt;
				success = true;
				return true;
			}
			return false;
		});

	return success;
}


FutureOwner*
TBTaskGroup	::	_Next()
{
	AutoMutexLocker _(fMutex, "_Next");
	if (fFutures.size() == 0)
		return nullptr;

	auto fut = fFutures.front();
	fFutures.pop_front();

	if (fFutures.size() < fInFlightResume)
		fInFlightExceeded.notify_one();

	return fut;
}


#if __HAIKU__
#	include <OS.h>
#endif

void
TBTaskGroup	::	_Thread(bool warrior)
{
#if __HAIKU__
	if (warrior)
		rename_thread(find_thread(NULL), "I am the warrior!");
	else if (fPriority < Priority::NORMAL)
		rename_thread(find_thread(NULL), "LOW PRIO");
	else if (fPriority == Priority::NORMAL
		|| fPriority < Priority::HIGH)
		rename_thread(find_thread(NULL), "NORMAL PRIO");
	else
		rename_thread(find_thread(NULL), "HIGH PRIO");
#endif
	/*
		This thread function is designed to balance polling and duration
		of its existence with its priority.

		Regardless of priority, however, one thread will always exist after
		we have been used unless Finalize(true) is issued. I call this the
		warrior thread.

		Those threads will likely be sleeping on fThreadWait.wait(), but
		their existence takes care of an edge case as well as reducing
		latency for future tb::async() executions.
	*/

	_SetOSThreadPriority();
	int dryRuns = 0;
	auto id = this_thread::get_id();

	auto lastWakeup = ms_time();

	while (!fFinalized && (warrior || dryRuns < 15)) {
		if (Execute() <= 0) {
			unique_lock<mutex>	locker(fDelayThreadExitMutex);
			fThreadWait.wait_for(locker, chrono::milliseconds(250));
			++dryRuns;
		} else {
			dryRuns = 0;
			++gTotalExecuted;
		}

			// wake up the underling threads every five seconds
			// to see if they deserve to die
		if (warrior && ms_time() - lastWakeup > 2500) {
			fThreadWait.notify_all();
			lastWakeup = ms_time();
		}
	}

	// each thread cleans up after itself!
	fThreadWait.notify_all();
	AutoMutexLocker _(fMutex, "_Thread");

	fThreads.erase(remove_if(fThreads.begin(), fThreads.end(),
	[this, id](thread& thd) {
		if (thd.get_id() == id) {
			thd.detach();
			return true;
		}
		return false;
	}));


}



void	// only one repeater thread per priority
TBTaskGroup	::	_RepeaterThread()
{
	_SetOSThreadPriority();

#if __HAIKU__
		rename_thread(find_thread(NULL), "repeaters");
#endif

	int size = 1;
	int delay = 0, minDelay = 0;
	long long delta = 0, nextDelta = 0;

	if (fPriority > Priority::HIGH)
		minDelay = 1;
	else if (fPriority > Priority::LOW)
		minDelay = 2;
	else
		minDelay = 5;

	while (size != 0) {
		//printf("\nSLEEP: %i ms\n", delay);
		this_thread::sleep_for(chrono::milliseconds(delay));

		nextDelta = 1000;	// controls minmum polling interval
		fRepeatMutex.lock();

		for (auto owner : fRepeats) {
			delta = ms_time() - owner->Last();
			if (delta >= owner->Interval()) {
				if (! owner->Execute() ) {
					// false from Execute() signals the repeater is done
					erase_if(fRepeats, [&owner](RepeatOwner* _cmp){
						if (owner == _cmp) {
							delete owner;
							return true;
						}
						return false;
					});
					nextDelta = 0;
					break;	// invalidated fRepeats iteration
				}

			++gTotalExecuted;
			}

			if (nextDelta > owner->Interval())
				nextDelta = owner->Interval();
		}

		size = fRepeats.size();
		fRepeatMutex.unlock();

		delay = nextDelta ;
		if (delay < minDelay)
			delay = minDelay;
	};

	AutoMutexLocker _(fRepeatMutex, "_RepeaterThread");
	fRepeatThread->detach();
	delete fRepeatThread;
	fRepeatThread = nullptr;
}


void
TBTaskGroup	::	_SetOSThreadPriority()
{
	sched_param param;
	int policy;
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = fPriority;
	pthread_setschedparam(pthread_self(), policy, &param);

}


TBTaskGroup		_lowOnStack(Priority::LOW);
TBTaskGroup		_normOnStack(Priority::NORMAL);
TBTaskGroup		_highOnStack(Priority::URGENT);

TBTaskGroup*	gTGLow 	= &_lowOnStack;
TBTaskGroup*	gTGNorm = &_normOnStack;
TBTaskGroup*	gTGHigh = &_highOnStack;


}; // namespace priv



}; // end namespace tb
