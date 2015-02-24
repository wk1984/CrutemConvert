#ifndef ID_AVG_ACCUM
#define ID_AVG_ACCUM

#include <future>
#include <vector>
#include <list>
#include <algorithm>
#include "StdTypedefs.h"


/*
	Keeps a running average for multiple entries.

	Example:

		IDAvgAccum<uint32, double> accum;

			// Adding values
		accum.add(1850, 20.0);
		accum.add(1860, 20.0);
		accum.add(1870, 20.0);
		accum.add(1850, 10.0);
		accum.add(1860, 10.0);
		accum.add(1870, 10.0);

			// getting results
		double avg1850 = accum.get(1850);
		double avg1860 = accum.get(1860);
		double avg1870 = accum.get(1870);

		// each average, in this example, will be 15.0

	For the time being, this uses a very naive averaging method prone
	to overflow and has only been tested as shown above.

*/




template <typename K, typename T>
class	IDAvgAccum {
public:
								IDAvgAccum(){}
	virtual						~IDAvgAccum(){}

			void				add(K, T);
			T					get(K);

			int32				size() const;

			void				PrintToStream();

			void				sort();

			void				for_each(std::function<void(K, T, uint32)>) const;
private:
		struct _IDAvgEntry {
			uint32	count;
			T		total;

			_IDAvgEntry()
				:	count(0),	total(0)
				{
				}
		};
		_IDAvgEntry*	_find(K);

	std::vector<std::pair<K, _IDAvgEntry> >
		fData;
};


template <typename K, typename T>
void IDAvgAccum<K, T>	::	add(K id, T val)
{
	using namespace std;
	auto* entry = _find(id);

	if (entry != nullptr) {
		entry->count++;
		entry->total += val;
	}
	else {
		pair<K, _IDAvgEntry> newEntry;
		newEntry.first = id;
		newEntry.second.total = val;
		newEntry.second.count = 1;
		fData.push_back(move(newEntry));
	}
}


template <typename K, typename T>
T IDAvgAccum<K, T> :: get(K id)
{
	using namespace std;
	auto* entry = _find(id);

	if (entry != nullptr)
		return entry->total / (T)entry->count;

	return 0;
}

template <typename K, typename T>
int32 IDAvgAccum<K, T>	::	size() const
{
	return fData.size();
}

template <typename K, typename T>
void IDAvgAccum<K, T>	::	sort()
{
	std::sort(fData.begin(), fData.end(), [](const std::pair<K, _IDAvgEntry>& one,
		 const std::pair<K, _IDAvgEntry>& two)
		{
			return one.first < two.first;
		});
}


template <typename K, typename T>
void IDAvgAccum<K, T> :: PrintToStream()
{
	using namespace std;
	printf("IDAvgAccum: %li items:\n",
		fData.size());

	for (const auto& item : fData) {
		printf("\t%lu: %lu for %.2f (%.2f average)\n", item.first, item.second.count,
					item.second.total,
					item.second.total / (T)item.second.count


					);
	}
}


template <typename K, typename T>
void	IDAvgAccum<K, T>	::	for_each( std::function<void(K, T, uint32)> func )const
{
	using namespace std;
	IDAvgAccum<K, T>::_IDAvgEntry* entry = nullptr;

	// func anticipates key & average
	for (auto& p : fData) {	// using ref to prevent unneeded copy
//		entry = &(p.second);
		func(p.first, (p.second.total / (T)p.second.count), p.second.count);
	}
}


template <typename K, typename T>
typename IDAvgAccum<K, T>::_IDAvgEntry*
IDAvgAccum<K, T> :: _find(K id)
{
	IDAvgAccum<K, T>::_IDAvgEntry* entry = nullptr;
	for (auto& p : fData) {
		if (p.first == id) {
			entry = &(p.second);
			break;
		}
	}

	return entry;
}


#endif // ID_AVG_ACCUM

