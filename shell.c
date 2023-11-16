// ͷ�ļ�
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

// �����
#define MAX_INPUT_SIZE 1024

// ��������
void list_files_detailed(const char* path);
void parse_list_command(char* tokens[], int token_count);
void show_help();
void list_files(const char* path);

// ������
int main()
{
    char input[MAX_INPUT_SIZE];

    while (1)
    {
        printf("<MyShell>: ");
        fgets(input, sizeof(input), stdin);

        // �Ƴ�ĩβ�Ļ��з�
        input[strcspn(input, "\n")] = 0;

        // �ִ�
        char* tokens[10];
        int token_count = 0;

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
        // ִ����Ӧ����
        if (token_count > 0)
        {
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
                // ���� cd ����
                // ...
            }
            else if (strcmp(tokens[0], "exit") == 0)
            {
                // ���� exit ����
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


// show_help()
void show_help()
{
    printf("Custom Shell Command Interpreter\n");
    printf("Usage:\n");
    printf("  help           չʾhelp��Ϣ\n");
    printf("  list <dir>     �г�dirĿ¼�������ļ�\n");
    // ������µ������ӵ�help��
}

// ���� list ����Ĳ���
void parse_list_command(char* tokens[], int token_count)
{
    const char* path = "."; // Ĭ��Ϊ��ǰĿ¼
    int detailed_info = 0;

    // ��������
    for (int i = 1; i < token_count; i++)
    {
        if (strcmp(tokens[i], "-l") == 0)
        {
            detailed_info = 1;
        }
        else
        {
            path = tokens[i];
        }
    }

    // ���ݲ���ִ����Ӧ����
    if (detailed_info)
    {
        list_files_detailed(path);
    }
    else
    {
        list_files(path);
    }
}

// list_files() �����޸�
void list_files(const char* path)
{
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

// list_files_detailed() �������
void list_files_detailed(const char* path)
{
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, entry->d_name);

        struct stat file_stat;
        if (stat(file_path, &file_stat) == -1)
        {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        struct passwd* owner_info = getpwuid(file_stat.st_uid);
        struct group* group_info = getgrgid(file_stat.st_gid);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));

        printf("%s %5lu %s %s %10ld %s %s\n",
            (S_ISDIR(file_stat.st_mode)) ? "file" : "-",
            file_stat.st_nlink,
            (owner_info != NULL) ? owner_info->pw_name : "",
            (group_info != NULL) ? group_info->gr_name : "",
            file_stat.st_size,
            time_str,
            entry->d_name);
    }

    closedir(dir);
}