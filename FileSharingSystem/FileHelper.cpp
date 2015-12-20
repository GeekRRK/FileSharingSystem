#include "StdAfx.h"
#include "FileHelper.h"

FileHelper::FileHelper(void)
{
}


FileHelper::~FileHelper(void)
{
}

/************************************************************************
说明：
    在文件夹中查找文件（仅在指定文件夹查找，不递归）
参数：
    fileName:要查找的文件名。支持通配符*
    directory:要查找的文件所在的目录路径
返回值：
    pair<string,string>类型的数组，map<文件名，文件路径>
************************************************************************/
vector<pair<CString,CString>> FileHelper::FindFilesInDirecotry(CString fileName,CString directory )
{
    vector<pair<CString,CString>> files;
 
    if(directory.Right(1) != "\\") //保证目录是以\结尾的
    {  
        directory += "\\";
    }
    directory += fileName;
    CFileFind finder;
    BOOL success = finder.FindFile(directory);
    while(success)
    {
        success = finder.FindNextFile();
        CString name = finder.GetFileName();
        CString path = finder.GetFilePath();
        pair<CString,CString> fileMap(name,path);
        files.push_back(fileMap);
    }
    finder.Close();
    return files;
}
 
 
/************************************************************************
说明：
    在文件夹中递归查找文件
参数：
    fileName:要查找的文件名。支持通配符*
    directory:要查找的文件所在的目录路径
    recursionCount:递归查找的深度，默认为-1（不限制深度）,0表示只搜索给定的directory目录
返回值：
    pair<string,string>类型的数组，map<文件名，文件路径>
************************************************************************/
vector<pair<CString,CString>> FileHelper::FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount)
{
    vector<pair<CString,CString>> files;
 
    int curRecursionCount = recursionCount;
 
    //先对参数传进来的文件夹查找
    vector<pair<CString,CString>> localFiles =  FindFilesInDirecotry(fileName,directory);
    files.insert(files.end(),localFiles.begin(),localFiles.end());
 
    //然后对此文件夹下面的子文件夹递归查找
    CFileFind finder;
    if(directory.Right(1) != "\\") //保证目录是以\结尾的
    {  
        directory += "\\";
    }
    directory += "*.*";
 
    BOOL res = finder.FindFile(directory);
    while (res)
    {
        res = finder.FindNextFile();
        CString path = finder.GetFilePath();
        CString name = finder.GetFileName();
        curRecursionCount = recursionCount;  //查找同级目录的时候将当前深度设置为给定值
        if(finder.IsDirectory() && !finder.IsDots())
        {
            if(curRecursionCount >= 1 || curRecursionCount == -1 )
            {
                if(curRecursionCount >= 1)
                {
                    curRecursionCount--;
                }
                localFiles = FindFilesInDirecotryRecursion(fileName,path,curRecursionCount); //如果目标是个文件夹，则利用嵌套来遍历
                files.insert(files.end(),localFiles.begin(),localFiles.end());
            }
        }
    }
    finder.Close();
    return files;
}