/*
	(C) 2015 Joseph D. Groover, Jr
		looncraz@looncraz.net
*/

#if __cplusplus < 201103L
#	error	"C++11 Support Required!"
#else

#include <deque>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sstream>

#include "FileSystem.h"


using namespace std;

namespace fs {


int for_each_file(	const string& _path,
					function<void(const string&)> func,
				 	bool recurse		)
{
	Entry folder(_path);

	if (folder.IsDirectory() == false)
		return 0;

	int count = 0;

	for (const auto& path : folder) {
		if (Entry(path).IsFile()) {
			func(path);
			++count;
		}	else if (recurse)
				count += for_each_file(path, func, recurse);
	}

	return count;
}


int	for_each_directory(	const string& _path,
						function<void(const string&)> func,
						bool recurse	)
{
	Entry folder(_path);
	if (folder.IsDirectory() == false)
		return 0;

	int count = 0;

	for (const auto& path : folder) {
		if (Entry(path).IsDirectory()) {
			func(path);
			++count;
			if (recurse)
				count += for_each_directory(path, func, recurse);
		}
	}

	return count;
}


//#pragma mark	Entry


Entry	::	Entry()
	:
	fIsDir(false),
	fIsFile(false),
	fIsSymlink(false),
	fVectorInit(false)
	{}


Entry	::	Entry(const string& path)
	:
	fPath(path),
	fIsDir(false),
	fIsFile(false),
	fIsSymlink(false),
	fVectorInit(false)
	{
		_DecodeName();
	}


Entry	::	Entry(const Entry& other)
	:
	fPath(other.fPath),
	fName(other.fName),
	fIsDir(other.fIsDir),
	fIsFile(other.fIsFile),
	fIsSymlink(other.fIsSymlink),
	fVector(other.fVector),
	fVectorInit(other.fVectorInit)
	{}


Entry	::	~Entry()	{}


// #pragma mark +  Members

/*
	We don't need to worry too much about Is[Directory][File]()
	being called more than once, adding in two more floats could
	guarantee we only check the file system once, but it is
	expected that one of these will usually be valid and any repeat
	attempts will likely be cached by the operating system...
*/

bool
Entry	::	IsDirectory	() const
{
	if (fIsDir || fIsFile)
		return fIsDir;

	if (IsSymlink())	return false;

	DIR* dir = opendir(fPath.c_str());

	if (dir != nullptr) {
		closedir(dir);

		fIsDir = true;
		return true;
	}

	return false;
}

bool
Entry	::	IsSymlink() const
{
	if (fIsSymlink || fIsFile || fIsDir)
		return fIsSymlink;

	struct stat sb;

	if (lstat(fPath.c_str(), &sb) < 0)
		return false;

	if ((sb.st_mode & S_IFMT) == S_IFLNK) {
		fIsSymlink = true;
		return true;
	}
	return false;
}


bool
Entry	::	IsFile	() const
{
	if (fIsFile || fIsDir || IsSymlink())
		return fIsFile;

	FILE* file = fopen(fPath.c_str(), "r");

	if (file != nullptr) {
		fclose(file);
		fIsFile = !IsDirectory();
		return fIsFile;
	}

	return false;
}


bool
Entry	::	Exists	() const
{
	return IsFile() || IsDirectory();
}


const string&
Entry	::	Name	() const
{
	return fName;
}


const string&
Entry	::	Path	() const
{
	return fPath;
}


int
Entry	::	Count	() const
{
	_InitVector();
	return fVector.size();
}


const Entry*
Entry	::	EntryAt	(int at) const
{
	_InitVector();
	string path = fVector[at];

	if (path == "")
		return nullptr;

	return new Entry(path);

}


//#pragma mark +  Iterators


vector<string>::const_iterator
Entry	::	begin() const
{
	_InitVector();
	return fVector.begin();
}


vector<string>::const_iterator
Entry	::	end() const
{
	_InitVector();
	return fVector.end();
}


vector<string>::const_reverse_iterator
Entry	::	rbegin() const
{
	_InitVector();
	return fVector.rbegin();
}


vector<string>::const_reverse_iterator
Entry	::	rend() const
{
	_InitVector();
	return fVector.rend();
}


vector<string>::const_iterator
Entry	::	cbegin() const
{
	_InitVector();
	return fVector.cbegin();
}


vector<string>::const_iterator
Entry	::	cend() const
{
	_InitVector();
	return fVector.cbegin();
}


vector<string>::const_reverse_iterator
Entry	::	crbegin() const
{
	_InitVector();
	return fVector.crbegin();
}


vector<string>::const_reverse_iterator
Entry	::	crend() const
{
	_InitVector();
	return fVector.crend();
}


//#pragma mark +  Private


void
Entry	::	_DecodeName()
{
	if (fPath.size() == 0)
		return;

	// root path special case
	if (fPath == string(1, Delimiter())) {
		fIsDir = true;
		return;
	}

	// remove trailing delimiters
	if (fPath.back() == Delimiter())
		fPath.resize(fPath.size() -1);

	if (fPath.size() == 0)	return;

	deque<string> sections;;
	string str;

	stringstream pathstr(fPath);
	while (getline(pathstr, str, Delimiter()))
		sections.push_front(str);

	fName = sections[0];
}


void
Entry	::	_InitVector() const
{
	if (fVectorInit)	return;
	if (!IsDirectory())	return ;

	DIR* dir = opendir(fPath.c_str());
	struct dirent* entry;

	string str, name;

	while ((entry = readdir(dir))) {
		str = fPath;
		str.append(1, Delimiter());
		name = entry->d_name;
		str.append(name);
		if (name == "." || name == "..")
			continue;

		// handle "//root/path/etc..."
		if (str[1] == Delimiter())
			str.erase(0,1);

		fVector.push_back(str);
	}

	fVectorInit = true;
	closedir(dir);
}





};	// namespace fs

#endif
