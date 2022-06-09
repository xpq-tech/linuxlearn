#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

//模拟实现ls指令

#define WRONG_COMMAND 112
#define TOO_LONG_COMMAND 113

void printErr(int eid);
void getFileDetail(char * fileName);
char ** ls(char * path, int * num);

int main(int argc, char *argv[])
{
    char currentPath[256];
    int fileNumber = 0;
    //获取当前目录
    getcwd(currentPath,256);//getwd() is dangerous and should not be used. 因为没有限制长度？
    //判断输入的参数是否正确
    char **files = ls(currentPath,&fileNumber);
    if (argc < 2)
    {
        //ls命令
        if(files == NULL){
            printf("failed\n");
            return -1;
        }else{
            int i;
            for( i = 0; i < fileNumber; i++){
                printf("%s ",files[i]);
                if(i%9 == 0){
                    printf("\n");
                }
            }
            if(i%9 != 0){
                printf("\n");
            }

        }
        
    }else if(argc < 3){
        //ls -l 命令
        if (strcmp(argv[1], "-l") != 0){
            printErr(WRONG_COMMAND);
            return -1;
        }
        for(int i = 0; i < fileNumber; i ++){
            getFileDetail(files[i]);
        }

    }else{
        printErr(TOO_LONG_COMMAND);
        return -1;
    }
    return 0;
}
//获取一个文件的详细信息
void getFileDetail(char * fileName){
    //通过stat函数获取用户传入文件的信息
    struct stat st;
    int ret = stat(fileName, &st);
    if (ret == -1)
    {
        perror("stat");
        exit(-1);
    }

    //获取文件类型和文件权限
    char perms[11] = {0}; //用于保存文件类型和文件权限的字符串
    switch (st.st_mode & S_IFMT)
    {
    case S_IFLNK:
        perms[0] = 'l';
        break;
    case S_IFDIR:
        perms[0] = 'd';
        break;
    case S_IFREG:
        perms[0] = '-';
        break;
    case S_IFBLK:
        perms[0] = 'b';
        break;
    case S_IFCHR:
        perms[0] = 'c';
        break;
    case S_IFSOCK:
        perms[0] = 's';
        break;
    case S_IFIFO:
        perms[0] = 'f';
        break;
    default:
        perms[0] = '?';
        break;
    }
    //文件的访问权限

    //文件所有者
    perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';

    //文件所在组
    perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';

    //其他人
    perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    //硬链接数
    int linkNum = st.st_nlink;

    //文件所有者
    char *fileUser = getpwuid(st.st_uid)->pw_name;

    //文件所在组
    char *fileGrp = getgrgid(st.st_gid)->gr_name;

    //文件大小
    long int fileSize = st.st_size;

    //修改时间
    char *time = ctime(&st.st_mtime);
    char mtime[512] = {0};
    strncpy(mtime, time, strlen(time) - 1);

    //char buf[1024];
    printf("%s %d %s %s %ld %s %s\n", perms, linkNum, fileUser, fileGrp, fileSize, mtime, fileName);

    //return buf;

}

void printErr(int eid){
    switch (eid)
    {
    case 112:
        printf("wrong command\n");
        break;
    case 113:
        printf("command too long\n");
        break;
    default:
        break;
    }
}
//ls命令
char ** ls(char * path, int * num){
    //打开一个目录
    DIR * dir = opendir(path);
    if(dir == NULL){
        perror("opendir");
        return NULL;
    }
    //char files[256][256]={0};错误，函数调用结束之后内存被释放，返回的是空指针
    char ** files = (char **)malloc(sizeof(char *)*256);
    //一个目录里面可能有多个文件，因此需要循环来读取，直到末尾
    struct dirent * drt = NULL;
    *num = 0;
    while((drt = readdir(dir))!=NULL){
        char * name = drt->d_name;
        if(strcmp(".",name)!=0 && strcmp("..",name) != 0){
            files[*num] = (char *)malloc(sizeof(char *)*256);//一定不要忘记在这里再开辟内存！！
            sprintf(files[(*num)++],"%s", name);
        }
    }
    
    closedir(dir);

    return files;

}