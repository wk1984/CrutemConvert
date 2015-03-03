/*
	(C) 2015 Joseph D. Groover, Jr
		looncraz@looncraz.net
*/

#if __cplusplus < 201103L
#	error	"C++11 Support Required!"
#	define L_DIR_ITERATOR_H
#endif


#ifndef L_DIR_ITERATOR_H
#define L_DIR_ITERATOR_H


/*
	Directory / file system iteration utilities

	Example:

			int count = fs::for_each_file("/tmp/",
				[](const string& path)
				{
					printf("%s\n", path.c_str());
				}, true	);

			printf("Count: %i\n", count);

	On Haiku this prints out:
	/tmp/tracker/virtual-directories/0b151713-d0d9-4b1a-9364-6d6241ffdf18/Applications
	/tmp/tracker/virtual-directories/0b151713-d0d9-4b1a-9364-6d6241ffdf18/Demos
	/tmp/tracker/virtual-directories/0b151713-d0d9-4b1a-9364-6d6241ffdf18/Desktop applets
	/tmp/tracker/virtual-directories/0b151713-d0d9-4b1a-9364-6d6241ffdf18/Preferences

	Pretty sweet, eh? C++11 lambdas FTW!

	There are no file system modifcation facilities and every Entry function is
	const.
*/


#include <future>
#include <string>
#include <vector>

using namespace std;

namespace fs {

int	for_each_file	(	const string& path,
						function<void(const string&)>,
						bool recurse = false	);


int	for_each_directory(	const string& path,
						function<void(const string&)>,
						bool recurse = false	);



/*
	Entry
*/


class Entry {
public:
								Entry();
								Entry(const string&);
								Entry(const Entry&);

	virtual						~Entry();

			bool				IsDirectory	() const;
			bool				IsSymlink	() const;
			bool				IsFile		() const;

			bool				Exists		() const;

	const	string&				Name		() const;
	const	string&				Path		() const;

			int					Count		() const;

	const	Entry*				EntryAt		(int) const;

// directory listing (if applicable)

	vector<string>::const_iterator			begin() const;

	vector<string>::const_iterator			end() const;

	vector<string>::const_reverse_iterator	rbegin() const;

	vector<string>::const_reverse_iterator	rend() const;

	vector<string>::const_iterator			cbegin() const;
	vector<string>::const_iterator			cend() const;

	vector<string>::const_reverse_iterator	crbegin() const;
	vector<string>::const_reverse_iterator	crend() const;

private:
			void				_DecodeName	();
			void				_InitVector	() const;

			string				fPath;
			string				fName;

	mutable	bool				fIsDir;
	mutable	bool				fIsSymlink;
	mutable	bool				fIsFile;

	mutable	vector<string>		fVector;
	mutable	bool				fVectorInit;
};


constexpr char Delimiter()
{
#if _WIN32
	return '\\';
#else
	return '/';
#endif
}


};	// namespace fs


#endif // L_DIR_ITERATOR_H
