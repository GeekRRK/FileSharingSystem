#include "StdAfx.h"
#include "FileHelper.h"

FileHelper::FileHelper(void)
{
}


FileHelper::~FileHelper(void)
{
}

/************************************************************************
˵����
    ���ļ����в����ļ�������ָ���ļ��в��ң����ݹ飩
������
    fileName:Ҫ���ҵ��ļ�����֧��ͨ���*
    directory:Ҫ���ҵ��ļ����ڵ�Ŀ¼·��
����ֵ��
    pair<string,string>���͵����飬map<�ļ������ļ�·��>
************************************************************************/
vector<pair<CString,CString>> FileHelper::FindFilesInDirecotry(CString fileName,CString directory )
{
    vector<pair<CString,CString>> files;
 
    if(directory.Right(1) != "\\") //��֤Ŀ¼����\��β��
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
˵����
    ���ļ����еݹ�����ļ�
������
    fileName:Ҫ���ҵ��ļ�����֧��ͨ���*
    directory:Ҫ���ҵ��ļ����ڵ�Ŀ¼·��
    recursionCount:�ݹ���ҵ���ȣ�Ĭ��Ϊ-1����������ȣ�,0��ʾֻ����������directoryĿ¼
����ֵ��
    pair<string,string>���͵����飬map<�ļ������ļ�·��>
************************************************************************/
vector<pair<CString,CString>> FileHelper::FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount)
{
    vector<pair<CString,CString>> files;
 
    int curRecursionCount = recursionCount;
 
    //�ȶԲ������������ļ��в���
    vector<pair<CString,CString>> localFiles =  FindFilesInDirecotry(fileName,directory);
    files.insert(files.end(),localFiles.begin(),localFiles.end());
 
    //Ȼ��Դ��ļ�����������ļ��еݹ����
    CFileFind finder;
    if(directory.Right(1) != "\\") //��֤Ŀ¼����\��β��
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
        curRecursionCount = recursionCount;  //����ͬ��Ŀ¼��ʱ�򽫵�ǰ�������Ϊ����ֵ
        if(finder.IsDirectory() && !finder.IsDots())
        {
            if(curRecursionCount >= 1 || curRecursionCount == -1 )
            {
                if(curRecursionCount >= 1)
                {
                    curRecursionCount--;
                }
                localFiles = FindFilesInDirecotryRecursion(fileName,path,curRecursionCount); //���Ŀ���Ǹ��ļ��У�������Ƕ��������
                files.insert(files.end(),localFiles.begin(),localFiles.end());
            }
        }
    }
    finder.Close();
    return files;
}