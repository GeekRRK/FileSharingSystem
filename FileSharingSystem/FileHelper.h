#pragma once
#include <vector>
using namespace std;

class FileHelper
{
public:
	FileHelper(void);
	~FileHelper(void);

	vector<pair<CString,CString>> FindFilesInDirecotry(CString fileName,CString directory );
	vector<pair<CString,CString>> FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount);
};

