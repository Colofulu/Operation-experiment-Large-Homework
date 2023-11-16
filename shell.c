// 头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

// 定义宏
#define MAX_INPUT_SIZE 1024

// 声明函数，这里只有四个自定义函数
void list_files_detailed(const char* path);
void parse_list_command(char* tokens[], int token_count);
void show_help();
void list_files(const char* path);

// 主函数
int main()
{
    char input[MAX_INPUT_SIZE];

    while (1)
    {
        // <MyShell>表示自定义shell
        printf("<MyShell>: ");
        // fgets获取输入的命令
        fgets(input, sizeof(input), stdin);

        // 移除末尾的换行符
        input[strcspn(input, "\n")] = 0;

        // 分词
        char* tokens[10];
        int token_count = 0;
        // 使用strtok将输入的命令和参数分词，将分词结果存储再tokens数组中
        // tokens数组用于保存分割后的命令和参数
        char* token = strtok(input, " ");
        while (token != NULL)
        {
            tokens[token_count] = token;
            token_count++;

            if (token_count >= 10)
            {
                fprintf(stderr, "Error: Too many tokens.\n");
                exit(EXIT_FAILURE);
            }

            token = strtok(NULL, " ");
        }
        // 执行相应命令
        if (token_count > 0)
        {    
            // 根据不同的类型来决定执行不同的命令
            if (strcmp(tokens[0], "help") == 0)
            {
                show_help();
            }
            else if (strcmp(tokens[0], "list") == 0)
            {
                parse_list_command(tokens, token_count);
            }
            else if (strcmp(tokens[0], "cd") == 0)
            {
                // 处理 cd 命令
                // 这里代码还没写空着先
            }
            else if (strcmp(tokens[0], "exit") == 0)
            {
                // 处理 exit 命令
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Error: Unknown command. Type 'help' for available commands.\n");
            }
        }
    }

    return 0;
}


// show_help()这里展示当前已经实现的命令以及其功能
void show_help()
{
    printf("Custom Shell Command Interpreter\n");
    printf("Usage:\n");
    printf("  help           展示help信息\n");
    printf("  list <dir>     列出dir目录下所有文件(dir默认当前目录)\n");
    printf("  list <dir> -l  列出dir目录下所有文件详细信息(dir默认当前目录)\n");
    printf("  exit           退出自定义 shell\n");
    // 如果有新的命令，添加到help中
}

// 解析 list 命令的参数，不同的参数有不同的调用结果
void parse_list_command(char* tokens[], int token_count)
{
    const char* path = "."; // 默认为当前目录
    int detailed_info = 0; // 用来判断是否需要输出详细信息

    // 解析参数
    for (int i = 1; i < token_count; i++)
    {
        // 如果有 -l 则说明需要输出详细信息
        if (strcmp(tokens[i], "-l") == 0)
        {
            detailed_info = 1;
        }
        // 单纯输出文件
        else
        {
            path = tokens[i];
        }
    }

    // 根据参数执行相应操作
    if (detailed_info)
    {
        list_files_detailed(path);
    }
    else
    {
        list_files(path);
    }
}

// list_files() 函数：单纯输出有哪些文件
void list_files(const char* path)
{
    DIR* dir;
    struct dirent* entry;

    // 如果目录不存在，那么输出错误消息，然后返回控制到main函数
    if ((dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s\n", path, strerror(errno));
        return;
    }

    // 如果目录存在，则遍历
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }
    // 退出
    closedir(dir);
}

// list_files_detailed() 函数：输出文件类型等详细信息
void list_files_detailed(const char* path)
{
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s\n", path, strerror(errno));
        return;
    }
    
    // readdir 函数：用于读取目录中的条目。它返回一个指向 dirent 结构体的指针，该结构体包含有关目录中下一个文件或子目录的信息。
    while ((entry = readdir(dir)) != NULL)
    {
        // 构建文件路径：使用 sprintf 函数构建文件的完整路径
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, entry->d_name);

        // 获取文件信息：使用 stat 函数获取文件的详细信息，包括文件类型、权限、所有者、组等
        struct stat file_stat;
        if (stat(file_path, &file_stat) == -1)
        {
            perror("stat");
            exit(EXIT_FAILURE);
        }
        
        // 获取所有者和组信息：使用 getpwuid 和 getgrgid 函数获取文件所有者和组的详细信息
        struct passwd* owner_info = getpwuid(file_stat.st_uid);
        struct group* group_info = getgrgid(file_stat.st_gid);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
        char file_type = '-';
        if (S_ISDIR(file_stat.st_mode))
        {
            file_type = 'd';  // 目录
        }
        else if (S_ISLNK(file_stat.st_mode))
        {
            file_type = 'l';  // 符号链接
        }
        else 
        {
    		file_type = 'f';  // 文件类型    
		}
        
        // 输出文件信息：根据获取到的文件信息，使用 printf 函数输出文件的详细信息
        printf("%c %5lu %s %s %10ld %s %s\n",
               file_type,
               file_stat.st_nlink,
               (owner_info != NULL) ? owner_info->pw_name : "",
               (group_info != NULL) ? group_info->gr_name : "",
               file_stat.st_size,
               time_str,
               entry->d_name);
    }
    closedir(dir);
}
