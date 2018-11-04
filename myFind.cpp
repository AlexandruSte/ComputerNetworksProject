#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

#define FIFO "fifo1"

int waitt=3;
static const size_t npos = -1;

ifstream fin("fisier.in");

void getInfo(char* path, int fd1);

void find_file(char* initPath,string filename, int fd1, char result[]){
    DIR *dir;
    struct dirent *de;
    struct stat fileStat;
    string path;
    if(stat(initPath,&fileStat)==0){
        if(S_ISDIR(fileStat.st_mode))
        {
            if((dir=opendir(initPath))==NULL){
                cout<<"Nu s-a putut deschide directorul "<<initPath<<endl;
            }
            else {
                while((de=readdir(dir))!=NULL){
                    if(strcmp(de->d_name,".") && strcmp(de->d_name,".."))
                    {
                        if(strcmp(de->d_name,filename.c_str())==0){
                            path = initPath;
                            path = path+"/"+filename;
                            char *pathPtr = &path[0];
                            strcat(result,path.c_str());
                            strcat(result,"\n");
                            getInfo(pathPtr, fd1);
                        }
                        string nextPath_string = initPath;
                        nextPath_string = nextPath_string + "/" + de->d_name;
                        char *nextPath = &nextPath_string[0];
                        find_file(nextPath, filename, fd1, result);
                    }
                }
                closedir(dir);
            }
        }
    }
}

string typeOfFile(char* path){
    struct stat fileStat;
    if(stat(path,&fileStat)==0){
        if(S_ISBLK(fileStat.st_mode)!=0)
            return "block special file";
        if(S_ISCHR(fileStat.st_mode)!=0)
            return "character special file";
        if(S_ISDIR(fileStat.st_mode)!=0)
            return "directory";
        if(S_ISFIFO(fileStat.st_mode)!=0)
            return "pipe or FIFO special file";
        if(S_ISREG(fileStat.st_mode)!=0)
            return "regular file";
        if(S_ISLNK(fileStat.st_mode)!=0)
            return "symbolic link";
        if(S_ISSOCK(fileStat.st_mode)!=0)
            return "socket";
    }
    return "";
}

void getInfo(char* path, int fd1){
    struct stat fileStat;
    struct passwd *pwd;
    if(stat(path,&fileStat)==0){
        char result[1000] = "";
        char buf[30];
        strcat(result,"Name: ");
        strcat(result,path);
        string s = typeOfFile(path);
        strcpy(buf,s.c_str());
        pwd = getpwuid(fileStat.st_uid);
        if(pwd!=NULL){
            strcat(result,"\nOwner of the file: ");
            strcat(result, pwd->pw_name);
            strcat(result,"\nDirectory: ");
            strcat(result, pwd->pw_dir);
            strcat(result,"\nShell: ");
            strcat(result, pwd->pw_shell);
            strcat(result,"\nPassword: ");
            strcat(result, pwd->pw_passwd);
        }
        strcat(result,"\nType of file: ");
        strcat(result,buf);
        strcat(result,"\n");
        strcat(result,"Access: ");
        strcat(result,((S_ISDIR(fileStat.st_mode)) ? "d" : "-"));
        strcat(result,((S_IRUSR & fileStat.st_mode) ? "r" : "-"));
        strcat(result,((S_IWUSR & fileStat.st_mode) ? "w" : "-"));
        strcat(result,((S_IXUSR & fileStat.st_mode) ? "x" : "-"));
        strcat(result,((S_IRGRP & fileStat.st_mode) ? "r" : "-"));
        strcat(result,((S_IWGRP & fileStat.st_mode) ? "w" : "-"));
        strcat(result,((S_IXGRP & fileStat.st_mode) ? "x" : "-"));
        strcat(result,((S_IROTH & fileStat.st_mode) ? "r" : "-"));
        strcat(result,((S_IWOTH & fileStat.st_mode) ? "w" : "-"));
        strcat(result,((S_IXOTH & fileStat.st_mode) ? "x \n" : "- \n"));
        strcat(result,"Access: ");
        strcat(result,ctime(&fileStat.st_atim.tv_sec));
        strcat(result,"Modify: ");
        strcat(result,ctime(&fileStat.st_mtim.tv_sec));
        strcat(result,"Change: ");
        strcat(result,ctime(&fileStat.st_ctim.tv_sec));
        strcat(result,"\n");
        write(fd1, result, strlen(result));
    }
}

int main(){
    char line[500];
    int sockp[2], fd1;
    cout<<"Bine ati venit! Mai intai trebuie sa va logati. \n";
    string username;
    string homePath="/home/alexandru";
    char *nextPath = &homePath[0];
    char* commandLine;
    bool userLogged=false;
    mknod(FIFO,S_IFIFO | 0666, 0);
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockp) < 0)
    {
        perror(" [error] socketpair");
        exit(1);
    }
    int parent=fork();
    if(parent){
        while(true){
            cin.getline(line,1000);
            if(strstr(line,"login : ") && strcmp(line,"login : ")!=0)
                break;
            cout<<"Trebuie sa aveti formatul : \"login : username\" pentru a va loga! \n";
        }
        commandLine = strtok(line,": ");
        commandLine = strtok(NULL,": ");
        username = commandLine;
        char usernameChar[200];
        strcpy(usernameChar, username.c_str());
        close(sockp[0]);
        write(sockp[1],usernameChar, sizeof(usernameChar));
        wait(&waitt);
        char msg[50];
        if (read(sockp[1], msg, 8) < 0)
            perror("[copil]Err..read");
        if(msg[0]=='d'){
            cout<<"V-ati logat cu succes! \n";
            cout<<"Comenzile disponibile sunt : \"quit\" , \"myfind file\" si \"mystat file\". \n";
            userLogged=true;
        }
        else
            cout<<"Logarea nu s-a putut face intrucat nu aveti cont! \n";
        close(sockp[1]);
    }
    else{
        char usernamee[300];
        close(sockp[1]);
        if (read(sockp[0], usernamee, 200) < 0)
            perror("[copil]Err..read");
        string fileUsername;
        username = usernamee;
        while(fin>>fileUsername){
            if(fileUsername==username){
                userLogged=true;
                break;
            }
        }
        char msg[3];
        if(userLogged)
            char msg[]="da";
        else
            char msg[]="nu";
        write(sockp[0], msg, sizeof(msg));
        close(sockp[0]);
        exit(waitt);
    }
    if(userLogged){
        reload:
        int p[2];
        if( pipe(p) == -1)
        {
            perror("error: pipe login");
            exit(1);
        }
        int parent=fork();
        if(parent){
            cin.getline(line,255);
            string lineS=line;
            fd1 = open(FIFO, O_RDONLY);
            cout<<endl;
            close(p[0]);
            write(p[1], line, sizeof(line));
            close(p[1]);
            wait(&waitt);
            char result[3000] = "";
            int num;
            if((num = read(fd1,result,2900))==-1){
                cout<<"Nu s-a putut citi din fifo.";
            }
            else{
                result[num]='\0';
                string resultS = result;
                if(resultS=="invalid"){
                    cout<<"Comanda invalida \n";
                }
                else if(resultS=="quit"){
                    goto endd;
                }
                else{
                    cout<<result;
                }
            }
            close(fd1);
            goto reload;
        }
        else{
            char readLine[3000] = "";
            char * path;
            fd1 = open(FIFO,O_WRONLY);
            close(p[1]);
            if (read(p[0], readLine, 2900) < 0)
                perror("[copil]Err..read");
            close(p[0]);
            string comm=readLine;
            if(comm=="quit"){
                write(fd1, "quit", sizeof("quit"));
                close(fd1);
                exit(waitt);
            }
            if(comm.find("myfind ")==npos && comm.find("mystat ")==npos){
                write(fd1, "invalid", sizeof("invalid"));
                close(fd1);
                exit(waitt);
            }
            path = strtok(readLine," ");
            string command = path;
            path = strtok(NULL," ");
            string pathS = path;
            if(command=="myfind"){
                char result[4000] = "";
                find_file(nextPath, pathS, fd1, result);
                strcat(result,"\n");
                write(fd1, result, sizeof(result));
            }
            else if(command=="mystat"){
                getInfo(path, fd1);
            }
            close(fd1);
            exit(waitt);
        }
    }
    endd:
    cout<<"Va mai asteptam!"<<endl;
    return 0;
}
