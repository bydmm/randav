/*
 * ===========================================================================
 *
 *       Filename:  list.c
 *
 *    Description:  list the files
 *
 *        Version:  1.0
 *        Created:  2010.12.28
 *       Compiler:  gcc
 *         Author:  puresky
 *
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/** Defines and Macros */
#define MATCH      1
#define NOT_MATCH  0
#define random(x) (rand()%x)
/* 匹配一个字符的宏 */
#define MATCH_CHAR(c1,c2,ignore_case)  ( (c1==c2) || ((ignore_case==1) &&(tolower(c1)==tolower(c2))) )
/*p匹配到的文件*/
char filelist[256][256];
int  end = 0;
/*---------------------------------------------------------------------------*/
/*  通配符匹配算法
 *        src      字符串
 *        pattern  含有通配符( * 或 ? 号)的字符串
 *        ignore_case 是否区分大小写，1 表示不区分,  0 表示区分
 *
 *  返回1表示 src 匹配 pattern，返回0表示不匹配
 */
int WildCharMatch(char *src, char *pattern, int ignore_case)
{
        int result;

        while (*src)
          {
                if (*pattern == '*')
                    {   /* 如果 pattern 的当前字符是 '*' */
                    	/* 如果后续有多个 '*', 跳过 */
                        while ((*pattern == '*') || (*pattern == '?'))
                              pattern++;
                              
                        /* 如果 '*" 后没有字符了，则正确匹配 */
                        if (!*pattern) return MATCH;

                        /* 在 src 中查找一个与 pattern中'*"后的一个字符相同的字符*/
                        while (*src && (!MATCH_CHAR(*src,*pattern,ignore_case)))
                              src++;
                        
                        /* 如果找不到，则匹配失败 */        
                        if (!*src) return NOT_MATCH;

                        /* 如果找到了，匹配剩下的字符串*/
                        result = WildCharMatch (src, pattern, ignore_case);
                        /* 如果剩下的字符串匹配不上，但src后一个字符等于pattern中'*"后的一个字符 */
                        /* src前进一位，继续匹配 */
                        while ( (!result) && (*(src+1)) && MATCH_CHAR(*(src+1),*pattern,ignore_case) )
                           result = WildCharMatch (++src, pattern, ignore_case);

                        return result;

                    }
                else
                    {
                    	/* 如果pattern中当前字符不是 '*' */
                    	/* 匹配当前字符*/
                        if ( MATCH_CHAR(*src,*pattern,ignore_case) || ('?' == *pattern))
                          { 
                            /* src,pattern分别前进一位，继续匹配 */
                            return WildCharMatch (++src, ++pattern, ignore_case);
                          }
                        else
                          {
                             return NOT_MATCH;
                          }
                    }
            }


       /* 如果src结束了，看pattern有否结束*/       
       if (*pattern)  
         { 
            /* pattern没有结束*/        	 
           if ( (*pattern=='*') && (*(pattern+1)==0) ) /* 如果pattern有最后一位字符且是'*' */
             return MATCH;
           else
             return NOT_MATCH;
         }
       else
         return MATCH;
}


//判断是否为目录
int IS_DIR(const char* path)
{
         struct stat st;
         lstat(path, &st);
         return S_ISDIR(st.st_mode);
}

//遍历文件夹de递归函数
void List_Files_Core(const char *path, int recursive)
{
         DIR *pdir;
         struct dirent *pdirent;
         char temp[256];
         pdir = opendir(path);
         if(pdir)
         {
                 while(pdirent = readdir(pdir))
                 {
                           //跳过"."和".."
                           if(strcmp(pdirent->d_name, ".") == 0
                                     || strcmp(pdirent->d_name, "..") == 0
																		 || strcmp(pdirent->d_name, "..") == 0)
                                   continue;
													 if(WildCharMatch(pdirent->d_name, "*.rmvb", 1)
													           || WildCharMatch(pdirent->d_name, "*.mp4", 1)
																		 || WildCharMatch(pdirent->d_name, "*.avi", 1)
																		 || WildCharMatch(pdirent->d_name, "*.wmv", 1))
													 {
														 sprintf(filelist[end], "%s/%s", path, pdirent->d_name);
														 end++;
													 }	
                           //sprintf(temp, "%s/%s", path, pdirent->d_name);
                           //printf("%s\n", temp);
                           //当temp为目录并且recursive为1的时候递归处理子目录
                           if(IS_DIR(temp) && recursive)
                           {
                                   List_Files_Core(temp, recursive);
                           }
                 }
          }
         else
         {
                  printf("opendir error:%s\n", path);
          }
          closedir(pdir);
}

//遍历文件夹的驱动函数
void
List_Files(const char *path, int recursive)
{
          int len;
          char temp[256];
          //去掉末尾的'/'
          len = strlen(path);
          strcpy(temp, path);
          if(temp[len - 1] == '/') temp[len -1] = '\0';
 
          if(IS_DIR(temp))
          {
                   //处理目录
                   List_Files_Core(temp, recursive);
          }
         else   //输出文件
         {
                  printf("%s\n", path);
         }
}

//小朋友们大家好，你还在为选哪个片子来一发而苦恼么？ 让本程序帮你从云海一般的av库中随机翻一个牌子，从此妈妈再也不担心我的撸管了！
//使用方法 “./randaav a片路径” 比如 ./randav ~/摇杆驱动
int
main(int argc, char** argv)
{
				if(argc != 2)
				{
					printf("Usage: ./program absolutePath\n");
					exit(0);
				}
				List_Files(argv[1], 1);
				char command[999];
				srand((int)time(0));
				int randindex = random(end);
				sprintf(command, "open %s -a MPlayerX", filelist[randindex]);
				printf("%s\n",command);
				system(command);
				return 0;
}

