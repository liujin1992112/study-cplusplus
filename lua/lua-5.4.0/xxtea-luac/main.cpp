#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "xxtea.h"
#include "path.h"
#include "resolver.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

using namespace std;
using namespace filesystem;

typedef struct CompileArgs {
	std::string strSrcPath;
	std::string strDstPath;
	std::string strKey;
	std::string strSign;

	void print()
	{
		std::cout << "source path:" << strSrcPath.c_str() << std::endl;
		std::cout << "target path:" << strDstPath.c_str() << std::endl;
		std::cout << "compile key:" << strKey.c_str() << std::endl;
		std::cout << "compile sign:" << strSign.c_str() << std::endl;
	}

	void parseArgs(int argc, char* argv[])
	{
		for (int i = 1; i < argc; i += 2)
		{
			if (*argv[i] == '-')
			{
				if (strcmp(&argv[i][1], "s") == 0)
				{
					this->strSrcPath = argv[i + 1];
				}

				if (strcmp(&argv[i][1], "d") == 0)
				{
					this->strDstPath = argv[i + 1];
				}

				if (strcmp(&argv[i][1], "key") == 0)
				{
					this->strKey = argv[i + 1];
				}

				if (strcmp(&argv[i][1], "sign") == 0)
				{
					this->strSign = argv[i + 1];
				}
			}
		}
	}
} CompileArgs;

void dfsFolder(string folderPath, std::vector<std::string>&files, int depth = 0)
{
#ifdef WIN32
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);


	if (Handle == -1L)
	{
		cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			//这个语句很重要
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				string newPath = folderPath + "\\" + FileInfo.name;
				dfsFolder(newPath, files);
			}
		}
		else
		{
			string filename = (folderPath + "\\" + FileInfo.name);
			files.push_back(filename);
			//cout << folderPath << "\\" << FileInfo.name << " " << endl;
		}
	} while (_findnext(Handle, &FileInfo) == 0);


	_findclose(Handle);
#else
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if ((dp = opendir(folderPath.c_str())) == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", folderPath.c_str());
		return;
	}
	chdir(folderPath.c_str());
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {


			if (strcmp(".", entry->d_name) == 0 ||
				strcmp("..", entry->d_name) == 0)
				continue;
			printf("%*s%s/\n", depth, "", entry->d_name);
			dfsFolder(entry->d_name, files, depth + 4);
		}
		else {
			string filename = entry->d_name;
			printf("%*s%s\n", depth, "", entry->d_name);
		}
	}
	chdir("..");
	closedir(dp);
#endif
}

void walkPath(filesystem::path rootPath)
{
	if (rootPath.is_directory())
	{
		//源目录当前所在路径
		walkPath(rootPath);
	}
	else
	{
		//源目录为文件
		std::fstream fs;
		std::string str;
		fs.open(rootPath.str(), std::fstream::in | std::fstream::out | std::fstream::app);
		fs >> str;
		cout << str;
	}
}

static const std::string BYTECODE_FILE_EXT = ".luac";
static const std::string NOT_BYTECODE_FILE_EXT = ".lua";

int main(int argc, char* argv[])
{
	std::cout << "program current path:" << argv[0] << std::endl;
	filesystem::path exePath = path(argv[0]);
	std::cout << exePath.extension() << endl;
	std::string programDir = exePath.parent_path().str();
	std::cout << "program current directory:" << programDir << std::endl;

	CompileArgs args;
	args.parseArgs(argc, argv);
	args.print();

	
	filesystem::path srcPath = path(args.strSrcPath.c_str());
	filesystem::path dstPath = path(programDir + "\\" + args.strDstPath.c_str());

	if (!srcPath.exists()) 
	{	//源目录不存在
		cout << "source path is not exists!" << endl;
		system("pause");
		return 0;
	}

	if (!dstPath.exists())
	{
		//目标目录不存在,创建目标目录
		create_directories(dstPath);
	}
	
	std::cout << "source absolute path:" << srcPath.make_absolute() << endl;
	std::cout << "target absolute path:" << dstPath.make_absolute() << endl;

	std::string srcAbsolutePath = srcPath.make_absolute().str();
	std::string targetAbsolutePath = dstPath.make_absolute().str();

	std::vector<std::string> files;
	dfsFolder(srcPath.str(), files);


	for (size_t i = 0; i < files.size(); i++)
	{
		std::string file = filesystem::path(files[i]).make_absolute().str();
		cout << file << endl;
		std::fstream fs;
		std::stringstream buffer;
		fs.open(file, std::fstream::in | std::fstream::out | std::fstream::app);

		buffer << fs.rdbuf();
		std::string contents(args.strSign + buffer.str());//将签名写入到luac文件头部
		//cout << contents << endl;

		xxtea_long len = 0;
		unsigned char* pData = xxtea_encrypt((unsigned char *)contents.c_str(), (xxtea_long)strlen(contents.c_str()), (unsigned char *)args.strKey.c_str(), strlen(args.strKey.c_str()), &len);
		
		filesystem::path writePath = filesystem::path(targetAbsolutePath + file.erase(0, srcAbsolutePath.size()) + "c");
		cout << writePath.make_absolute() << endl;

		std::fstream ws;
		ws.open(writePath.str(), std::fstream::in | std::fstream::out | std::fstream::app);
		ws << pData;
		free(pData);
	}
	system("pause");
	return 0;
}
