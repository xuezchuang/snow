/* SPDX-FileCopyrightText: 2001-2002 NaN Holding BV. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

 /** \file
  * \ingroup datatoc
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <iostream>
#include <filesystem>


  /* #define VERBOSE */

#define MAX2(x, y) ((x) > (y) ? (x) : (y))
#define MAX3(x, y, z) MAX2(MAX2((x), (y)), (z))

static char* basename(char* string)
{
	char* lfslash,* lbslash;

	lfslash = strrchr(string,'/');
	lbslash = strrchr(string,'\\');
	if(lbslash)
	{
		lbslash++;
	}
	if(lfslash)
	{
		lfslash++;
	}

	return MAX3(string,lfslash,lbslash);
}

void gen(char** argv)
{
	FILE* fpin,* fpout;
	long size;
	int i;
	int argv_len;
	fpin = fopen(argv[1],"rb");
	if(!fpin)
	{
		printf("Unable to open input <%s>\n",argv[1]);
		exit(1);
	}

	char* temp = basename(argv[1]);

	fseek(fpin,0L,SEEK_END);
	size = ftell(fpin);
	fseek(fpin,0L,SEEK_SET);

	if(temp[0] == '.')
	{
		temp++;
}

#ifdef VERBOSE
	printf("Making C file <%s>\n",argv[2]);
#endif

	argv_len = (int)strlen(temp);
	for(i = 0; i < argv_len; i++)
	{
		if(temp[i] == '.')
		{
			temp[i] = '_';
		}
	}

	fpout = fopen(argv[2],"w");
	if(!fpout)
	{
		fprintf(stderr,"Unable to open output <%s>\n",argv[2]);
		exit(1);
	}

	fprintf(fpout,"/* DataToC output of file <%s> */\n\n",temp);

	/* Quiet 'missing-variable-declarations' warning. */
	fprintf(fpout,"extern const int datatoc_%s_size;\n",temp);
	fprintf(fpout,"extern const char datatoc_%s[];\n\n",temp);

	fprintf(fpout,"const int datatoc_%s_size = %d;\n",temp,(int)size);
	fprintf(fpout,"const char datatoc_%s[] = {\n",temp);
	while(size--)
	{
		/* Even though this file is generated and doesn't need new-lines,
		 * these files may be loaded by developers when looking up symbols.
		 * Avoid a very long single line that may lock-up some editors. */
		if(size % 32 == 31)
		{
			fprintf(fpout,"\n");
		}

		// fprintf(fpout, "\\x%02x", getc(fpin));
		fprintf(fpout,"%3d,",getc(fpin));
	}

	/* Trailing NULL terminator, this isn't needed in some cases and
	 * won't be taken into account by the size variable, but its useful when dealing with
	 * NULL terminated string data */
	fprintf(fpout,"0\n};\n\n");

	fclose(fpin);
	fclose(fpout);
}


using namespace std::filesystem;
int main(int argc,char** argv)
{
	argv = new char* [3];
	argv[0] = new char[MAX_PATH];
	argv[1] = new char[MAX_PATH];
	argv[2] = new char[MAX_PATH];
	path currentPath = current_path();
	currentPath = currentPath / "../Engine/shaders";
	for(const auto& entry : directory_iterator(currentPath))
	{
		//if(filesystem::is_directory(entry.status()))
		//{
		//	// 如果是目录，则递归遍历子目录
		//	//TraverseDirectory(entry.path());
		//}
		if(is_regular_file(entry.status()) && entry.path().extension() == ".glsl")
		{
			// 如果是文件，则输出文件路径
			std::string b = entry.path().string();
			strcpy_s(argv[1],MAX_PATH,b.c_str());
			std::string a = entry.path().parent_path().string() + "\\..\\glGpu\\" + entry.path().stem().string() + ".c";
			strcpy_s(argv[2],MAX_PATH,a.c_str());
			gen(argv);
			
		}
	}
	system("pause");
	return 0;
}

