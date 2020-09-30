#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include "xxtea.h"
#include "path.h"
#include "resolver.h"

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
		return 0;
	}

	if (!dstPath.exists())
	{
		//目标目录不存在,创建目标目录
		create_directories(dstPath);
	}
	
	std::cout << "source absolute path:" << srcPath.make_absolute() << endl;
	std::cout << "target absolute path:" << dstPath.make_absolute() << endl;

	walkPath(srcPath);
	system("pause");
	return 0;
}
