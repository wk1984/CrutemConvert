#ifndef L_TASKPRIVATE_H
#define L_TASKPRIVATE_H

#include <algorithm>
#include <deque>
#include <future>
#include <mutex>

using namespace std;


//TODO: create some time helpers?
unsigned long long ms_time();


namespace tb	{

class Repeating;	// fwd decl

enum Priority {
	DEFAULT		= 0,	// back of whichever queue
	IDLE 		= 1,	// back of low priority queue
	LOW 		= 5,	// front of low priority queue
	NORMAL 		= 10,	// back of normal queue
	HIGH 		= 11,	// front of normal queue
	URGENT	 	= 20,	// back of lower latency queue
	SUPER		= 21	// front of lower latency queue
};

	namespace priv	{

/*
	P R I V A T E
*/


class	FutureOwner {
public:
								FutureOwner(){}
	virtual						~FutureOwner(){}

	virtual	void				Execute() = 0;
};

template <typename T>
class FutureOwnerT : public FutureOwner {
public:
								FutureOwnerT(std::shared_future<T>& fut)
								:FutureOwner(),
								fFuture(fut)
								{}

			void				Execute(){fFuture.get();}
private:
			shared_future<T>	fFuture;
};


class	RepeatOwner	{
public:
								RepeatOwner(int, int, std::function<void()>);
								~RepeatOwner();

			bool				Execute();

			void				WatcherMoved		(Repeating*);

	unsigned long long			Last() const;
			int					Interval() const;
			int					Count() const;

			Repeating*			Watcher();

			void				SetInterval(int);
			void				SetCount(int);

	private:
		std::function<void()>	fFunction;
	mutable	mutex				fMutex;

	unsigned long long			fLast;
			int					fCount;
			int					fInterval;	// milliseconds

		Repeating*				fWatcher;
		Repeating*				fOrigWatcher;
};



class	TBTaskGroup {
public:
	explicit					TBTaskGroup	(Priority);
								~TBTaskGroup();

			void				SetPriority(Priority);

			void				Assign(FutureOwner*, Priority prio);
			void				Assign(RepeatOwner*);

			int					Execute();

			int					CountPending();
			void				Finalize(bool = true);

			bool				Cancel(RepeatOwner*);
private:
			FutureOwner*		_Next();
			void				_Thread(bool);
			void				_RepeaterThread();
			void				_SetOSThreadPriority();

			deque<FutureOwner*>	fFutures;
				Priority		fPriority;
				bool			fFinalized;
				mutex			fMutex;
		vector<thread>			fThreads;
			unsigned int		fMaxThreads;
			unsigned int		fMaxInFlight;
			unsigned int		fInFlightResume;

		condition_variable		fThreadWait;
				mutex			fDelayThreadExitMutex;

		condition_variable		fInFlightExceeded;
				mutex			fInFlightMutex;

				thread*			fRepeatThread;
		vector<RepeatOwner*>	fRepeats;
				mutex			fRepeatMutex;
};

extern "C" TBTaskGroup*	gTGLow;
extern "C" TBTaskGroup*	gTGNorm;
extern "C" TBTaskGroup*	gTGHigh;

extern "C" unsigned long long	gTotalExecuted;

	};
};

#endif // L_TASKPRIVATE_H
