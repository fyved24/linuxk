#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define CMD_MAX_LEN 256
#define CMD_MAX_ARGV_NUM 256
#define BUFFSIZE 255
char buf[BUFFSIZE];

int argc = 0;
char *argv[CMD_MAX_ARGV_NUM];
// 获取参数
void input()
{
    argc = 0;
    fgets(buf, BUFFSIZE, stdin);
    char argv_liat[CMD_MAX_ARGV_NUM][CMD_MAX_LEN];
    memset(argv_liat, 0, sizeof(argv_liat));
    buf[strlen(buf) - 1] = '\0';
    int len = strlen(buf);
    for (int i = 0, j = 0; i < len; ++i)
    {
        if (buf[i] != ' ')
        {
            argv_liat[argc][j] = buf[i];
            j++;
        }
        else
        {
            if (j != 0)
            {
                argv_liat[argc][j + 1] = '\0';
                j = 0;
                // 将每一个参数所在的字符串地址，赋值给argv数组
                argv[argc] = argv_liat[argc];
                argc++;
            }
        }
    }
    if (buf[len - 1] != ' ')
    {
        argv[argc] = argv_liat[argc];
    }
    argv[argc + 1] = NULL;
}
int main()
{
    while (1)
    {
        printf("->");
        input();
        // 如果是 exit 就退出
        if(strcmp(argv[0], "exit") == 0) {
            return 0;
        }
        int pid = fork();

        if (pid == 0)
        {
            execvp(argv[0], argv);
        }

        waitpid(pid, NULL, 0);
    }
    return 0;
}
