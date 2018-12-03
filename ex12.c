#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct student {
    char path[100];
    char pathToCFile[100];
    char name[40];
    char reason_of_grade[45];
    int grade;
}student;

int static countStudents = 0;


/**
 * Exp: Checking if a file is a directory or just a file [duplicate] (Use the S_ISDIR macro).
 * Used from stackoverflow: stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
 * @param path
 * @return 0 - if non-exist or non-directory
 */
int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}


/**
 * Exp: get a file extension from a filename in C
 * Used from stackoverflow : stackoverflow.com/questions/5309471/getting-file-extension-in-c
 * @param filename
 * @return (file extension) dot+1, else ""(empty)
 */
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}


/**
 * Exp: get directory name from the full path
 * @param path2Dir
 * @return
 */
student* fromStudentPath(char* path2Dir){
    DIR *dir;
    struct dirent *dirnt;
    strcat(path2Dir,"/");
    int i = 0;
    student* students = (student*)malloc(sizeof(student));
    if ((dir = opendir (path2Dir)) == NULL) {
        perror ("cannot open this directory");
        exit(1);
    }
    else {
        while ((dirnt = readdir (dir)) != NULL) {
            if(strcmp(dirnt->d_name,".") && strcmp(dirnt->d_name,"..")){
                char* path = (char*)malloc(120);
                strcpy(path,path2Dir);
                strcat(path,dirnt->d_name);
                if(isDirectory(path)){
                    strcpy((students + i)->name, dirnt->d_name);
                    strcpy((students + i)->path, path);
                    i++;
                    students = (student*)realloc(students,((i+1)*sizeof(student)));
                }
                free(path);
            }
        }
        closedir (dir);
    }
    countStudents = i;
    return students;
}


/**
 * Exp: assist function to "checkIfCFileExist" function
 * Using <dirent.h>
 * @param path2Dir
 * @return path, else "NO_C_FILE"
 */
char* inCFile(char* path2Dir){
    if(isDirectory(path2Dir)){
        DIR *dir;
        struct dirent *dirnt;
        strcat(path2Dir,"/");
        student* students = (student*)malloc(sizeof(student));
        if ((dir = opendir (path2Dir)) == NULL) {
            perror ("cannot open directory");
            exit(1);
        }
        else {
            while ((dirnt = readdir (dir)) != NULL) {
                if(strcmp(dirnt->d_name,".") && strcmp(dirnt->d_name,"..")){
                    char* path = (char*)malloc(120);
                    strcpy(path,path2Dir);
                    strcat(path,dirnt->d_name);
                    if(!isDirectory(path)){
                        if(!strcmp(get_filename_ext(path),"c")){
                            return path;
                        }
                    }
                    else{
                        inCFile(path);
                    }
                    free(path);
                }
            }
            closedir (dir);
        }
    }
    return "NO_C_FILE";
}

/**
 * Exp: checking if c file exist in students folder, if non-exist, write "NO_C_FILE" in reason_of_grade.
 * @param students
 */
void checkIfCFileExist(student* students){
    for(size_t i = 0 ; i< countStudents; i++){
        strcpy((students + i)->pathToCFile,inCFile((students + i)->path));
        if(!strcmp((students + i)->pathToCFile,"NO_C_FILE")){
            strcpy((students + i)->reason_of_grade,"NO_C_FILE");
        }
    }
}


/**
 * Exp: compile c file of student
 * @param students
 */
void compile(student* students){
    for(size_t i = 0 ; i< countStudents; i++){
        if(strcmp((students + i)->pathToCFile,"NO_C_FILE")){
            pid_t child_pid = fork();
            int childSt;
            if(child_pid){
                waitpid(child_pid,&childSt,0);
                if (WIFEXITED(childSt)&& WEXITSTATUS(childSt)) {
                    strcpy((students + i)->reason_of_grade,"COMPILATION_ERROR");
                };
            }
            else{
                char name[25] = "";
                strcpy(name,(students + i)->name);
                strcat(name,".out");
                char *args[]={"gcc",(students + i)->pathToCFile,"-o",name,NULL};
                execvp(args[0],args);
            }
        }
    }
}


/**
 * Exp: compare between global output and output of a student
 * @param students
 * @param outputFile
 */
void cmp(student* students, char* outputFile){
    for(size_t i = 0 ; i< countStudents; i++){
        if(!strcmp((students + i)->reason_of_grade,"")){
            pid_t child_pid = fork();
            int childSt;
            char studentOutput[25] = "";
            strcpy(studentOutput,(students + i)->name);
            strcat(studentOutput,".txt");
            if(child_pid){
                waitpid(child_pid,&childSt,0);
                if (WIFEXITED(childSt)) {
                    if(WEXITSTATUS(childSt) == 1){
                        strcpy((students + i)->reason_of_grade,"BAD_OUTPUT");
                    }
                    else if(WEXITSTATUS(childSt) == 2){
                        strcpy((students + i)->reason_of_grade,"GREAT_JOB");
                        (students + i)->grade = 100;
                    }
                    else{
                        perror("Error: this is not 1 and not 2");
                        exit(1);
                    }
                    char cmpP[25] = "";
                    strcat(cmpP,(students + i)->name);
                    strcat(cmpP,".out");
                    if(unlink(studentOutput) < 0){
                        perror("Error : file cannot deleted");
                        exit(1);
                    }
                    if(unlink(cmpP) < 0){
                        perror("Error : file cannot deleted");
                        exit(1);
                    }
                }
            }
            else{
                char *args[]={"./bin/comp.out",studentOutput,outputFile,NULL};
                execvp(args[0],args);
            }
        }
    }
}


/**
 * Exp: run the files that compiled
 * @param students
 * @param inputFile
 */
void FuncRun(student* students, char* inputFile){
    for(size_t i = 0 ; i< countStudents; i++){      //if student has no comment yet
        if(!strcmp((students + i)->reason_of_grade,"")){
            pid_t child_pid = fork();
            int childSt;
            if(child_pid){
                waitpid(child_pid,&childSt,0);
            }
            else{
                char cmd[25] = "./";
                strcat(cmd,(students + i)->name);
                strcat(cmd,".out");
                char filename[22] = "";
                strcpy(filename,(students + i)->name);
                strcat(filename,".txt");
                int inp = open(inputFile,O_RDONLY,0);
                int out = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
                if (inp < 0){
                    perror("Error : cannot open input file");
                    exit(1);
                }
                if (out < 0){
                    perror("Error : cannot open output file");
                    exit(1);
                }
                if ((dup2(inp,0)) < 0){
                    perror("Error : cannot redirect");
                    exit(1);
                }
                if ((dup2(out,1)) < 0){
                    perror("Error : cannot redirect");
                    exit(1);
                }
                if(close(inp) < 0){
                    perror("Error : file cannot close");
                    exit(1);
                }
                if(close(out) < 0){
                    perror("Error : file cannot close");
                    exit(1);
                }
                char *args[]={cmd,NULL};
                execvp(args[0],args);
            }
        }
    }
}


/**
 * Exp: writes all students results to csv file
 * @param students
 */
void write2Csv(student* students){
    int csv_FD = open("results.csv", O_APPEND | O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
    if(csv_FD < 0){
        perror("Error : cannot open results.csv");
        exit(1);
    }
    for(size_t i = 0 ; i< countStudents; i++){
        char line[50]="";
        char student_num[5];
        sprintf(student_num,"%d",(students + i)->grade);
        strcat(line,(students + i)->name);
        strcat(line,",");
        strcat(line,student_num);
        strcat(line,",");
        strcat(line,(students + i)->reason_of_grade);
        strcat(line,"\n");
        if(!write(csv_FD,line,strlen(line))){
            perror("Error : cannot write to results.csv");
            exit(1);
        }
    }
    if(close(csv_FD) < 0){
        perror("Error : File cannot close");
        exit(1);
    }
}


int main(int argc, char *argv[]){
    if(argc!=2){
        perror("Error : have not only one path");
        exit(1);
    }
    else{
        int configFD = open(argv[1],O_RDONLY,0);
        char globalPath[80], inputPath[80] , outputPath[80];
        if(configFD<0){
            perror("Error : cannot open configuration file");
            exit(1);
        }
        if((configFD = dup2(configFD,0)) < 0){
            perror("Error : cannot redirect");
            exit(1);
        }
        scanf("%s",globalPath);
        scanf("%s",inputPath);
        scanf("%s",outputPath);
        if(isDirectory(globalPath)){
            student* students = fromStudentPath(globalPath);
            checkIfCFileExist(students);
            compile(students);
            FuncRun(students,inputPath);
            cmp(students,outputPath);
            write2Csv(students);
            free(students);
        }
        if(close(configFD)<0){
            perror("Error : file cannot closed");
            exit(1);
        }
    }
    return 0;
}
