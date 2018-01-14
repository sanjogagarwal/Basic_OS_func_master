#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>


#define maxIPsize 256
#define tokenSize 64
#define deliminator " \n\t"
#define stackSize 100

int command_cd(char **args);
int command_ls(char **args);
int command_exit(char **args);
int command_rm(char **args);
int command_history(char **args);
int command_issue(char **args);
int command_rmexcept(char **args);
char ** read_args(char *line);
int execute(char **args);
int getLastArgs(char **args);

int timeout=0;
char **commandStack;

char *commands[]={
    "cd",
    "ls",
    "rm",
    "history",
    "issue",
    "rmexcept",
    "exit"
};

int (*commandResponse[])(char **args)={
    &command_cd,
    &command_ls,
    &command_rm,
    &command_history,
    &command_issue,
    &command_rmexcept,
    &command_exit
};

void alarm_handler(int sig)
{
    timeout = 1;
}

int command_rmexcept(char **args){
    int n;
    struct dirent **namelist;
    int last=getLastArgs(args);
    n=scandir(".",&namelist,NULL,alphasort);              //scanning dir entities
    if(n<0){
        perror("scandir error");
        exit(0);
    }
    else{
        int flag=0;
        for (int i =0;i<n;i++){
            flag=0;
            for(int j=1;j<last;j++){
                
                if(strcmp(namelist[i]->d_name,args[j])==0){
                    flag=1;
                }
            }
            if(!flag){
                printf("delteting: %s \n",namelist[i]->d_name);
                remove(namelist[i]->d_name);
            }

        }

        
        free(namelist);
    }
    return 1;
}

int command_issue(char **args){
    if(args[1]==NULL){
        fprintf(stderr,"issue n where 'n' is a integer\n");
        return 1;
    }
    else{
        int length = strlen (args[1]),i;
        for (i=0;i<length; i++)
            if (!isdigit(args[1][i]))
            {
                printf ("issue n -:'n' is a integer not a character.\n");
                return 1;
            }

        int n=atoi(args[1]);
        if(commandStack[n]==NULL){
            fprintf(stderr,"no Nth command in the stack\n"); 
        }
        else{
            char **tempArgs=read_args(commandStack[n]);
            execute(tempArgs);
            return 1;
        }
        
    }
    return 1;
}

int command_history(char **args){
    if(args[1]==NULL){
        int iter=0;
        while(commandStack[iter]!=NULL){
            printf("%s\n",commandStack[iter]);
            iter++;
        }
        return 1;
    }
    else{
        int length = strlen (args[1]),i;
        for (i=0;i<length; i++)
            if (!isdigit(args[1][i]))
            {
                printf ("history n -:'n' is a integer not a character.\n");
                return 1;
            }

        int his_n=atoi(args[1]);
        int last=getLastArgs(commandStack);
        //printf("%d\n",last);
        //printf("%d\n",last);
        while(his_n>0){
            if(commandStack[his_n]!=NULL){
                printf("%s\t",commandStack[last-his_n-1]);
            }
            printf("\n");
            his_n--;
        }    
        return 1;
    }

    return 1;
}

int command_exit(char **args){
    return 0;
}

int command_cd(char **args){
    if (args[1] == NULL) {
        fprintf(stderr, "no expected argument to \"cd\" given.... \n");
    } else {
        if (chdir(args[1]) != 0) {
             perror("dharmesh");
        }
    }
    return 1;
}

int command_ls(char **args){
    int n;
    struct dirent **namelist;
    n=scandir(".",&namelist,NULL,alphasort);              //scanning dir entities
    if(n<0){
        perror("scandir error");
        exit(0);
    }
    else{
        while(n--){
            printf("%s\n",namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }
    return 1;

}

int command_rm(char ** args){

    if (args[1] == NULL) {
        fprintf(stderr, "no expected argument to \"rm ( -f, -r )\" given.... \n");
    } else {
        if(strcmp("-r",args[1]) && strcmp("-v",args[1])  && strcmp("-f",args[1])  ){
            //printf("entring hre\n");
            remove(args[1]);    
            return 1;  
        }
        else if (strcmp("-f",args[1])==0){
            remove(args[2]);  
            return 1;
        }
        else if(strcmp("-r",args[1])==0){
            if(args[2]==NULL){
                fprintf(stderr, "no expected argument to \"rm\" given.... \n");
            }else{
                struct dirent *dirVar;
                DIR *dir;
                char buf[256];
                dir = opendir(args[2]);
                dirVar = readdir(dir);
                while(dirVar)
                {               
                    sprintf(buf, "%s/%s", args[2], dirVar->d_name);
                    printf("%s\n",dirVar->d_name);
                    remove(buf);
                    dirVar = readdir(dir);
                }
                remove(args[2]);
                free(dirVar);
                return 1;
               
            }
            
        }
        else if(strcmp("-v",args[1])==0){
            if(args[2]==NULL){
                fprintf(stderr, "no expected argument to \"rm -r\" given.... \n");            
            }
            else{
                struct stat buf;
                stat(args[2], &buf);
                if(S_ISDIR(buf.st_mode)==0){
                    remove(args[2]);
                    printf("sucessfully removed '%s'.\n",args[2]);
                }
                else{
                    fprintf(stderr,"the given file is a directory not a file.\n");
                }

            }

            return 1;
        }

    }  
    return 1;     
}


int numberOfCommands(){
    return sizeof(commands)/sizeof(char *);
}

char *copyString(char *line){
    char *temp;
    int size=sizeof(line)/sizeof(char);
    temp=(char *)malloc(sizeof(char)*size);

    for(int i=0;i<size;i++){
        temp[i]=line[i];
    }
    return temp;
}


int getLastArgs(char **args){
    int i=0;
    while(args[i]!=NULL){
        i++;
    }
    return i;
}

int launch(char **args)
{
    pid_t pid;
    int status;

    pid =fork();
    if (pid==0){
    
        int fd0,fd1,i,in=0,out=0;
        char input[64],output[64];

    // finds where '<' or '>' occurs and make that argv[i] = NULL , to ensure that command wont't read that

    for(i=0;args[i]!='\0';i++)
    {
        if(strcmp(args[i],"<")==0)
        {        
            args[i]=NULL;
            strcpy(input,args[i+1]);
            in=2;           
        }               

        if(strcmp(args[i],">")==0)
        {      
            args[i]=NULL;
            strcpy(output,args[i+1]);
            out=2;
        }         
    }

    //if '<' char was found in string inputted by user
    if(in)
    {   

        // fdo is file-descriptor
        int fd0;
        if ((fd0 = open(input, O_RDONLY, 0)) < 0) {
            perror("Couldn't open input file");
            exit(0);
        }           
        // dup2() copies content of fdo in input of preceeding file
        dup2(fd0, 0); // STDIN_FILENO here can be replaced by 0 

        close(fd0); // necessary
    }

    //if '>' char was found in string inputted by user 
    if (out)
    {

        int fd1 ;
        if ((fd1 = creat(output , 0644)) < 0) {
            perror("Couldn't open the output file");
            exit(0);
        }           

        dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
        close(fd1);
    }


        if(execvp(args[0],args)==-1){
            perror("program error");
        }
        
        exit(0);
    }
    else if(pid<0){
        perror("not a possible process");
    }
    else{
        //converting last argument to integer...
        int last=getLastArgs(args);
        int length = strlen (args[last-1]),i;

        for (i=0;i<length; i++){
            if (!isdigit(args[last-1][i]))
            {
                do{
                waitpid(pid,&status,WUNTRACED);
                }while(!WIFEXITED(status) && !WIFSIGNALED(status));
                
                return 1;
            }
        }    
        int n=atoi(args[last-1]);     
        //printf("%d\n",n);   
        signal(SIGALRM, alarm_handler);        
        alarm(n); 
        pause();
        if(timeout){
            timeout=0;
            printf("alarm triggered\n");
            int result = waitpid(pid, NULL, WNOHANG);
            if (result == 0) {
                // child still running, so kill it
                printf("killing child\n");
                kill(pid, 9);
            }else {
                printf("alarm triggered, but child finished normally\n");
            }
        }
    }

    return 1;
}



//reading the line
char* read_line(){
    int pos=0;
    int flag=1;
    int bufferSize=maxIPsize;
    char *ipLine;
    char c;
    ipLine=(char *)malloc(sizeof(char)*maxIPsize );
     if(!ipLine){
                fprintf(stderr,"memory not allocated");
                exit(0);   
            }
    
    while(flag){
        c=getchar();

        if(c!='\n'){
            ipLine[pos]=c;
        }
        else if (c=='\n'){
            ipLine[pos]='\0';
            flag=0;
        }
        pos++;

        if (pos>=maxIPsize){
            bufferSize+=maxIPsize;
            ipLine=realloc(ipLine,bufferSize);
            if(!ipLine){
                fprintf(stderr,"memory not allocated");
                exit(0);   
            }
        }
    }


    return ipLine;
}


//reading the aruments from line
char **read_args(char *lineIP){
    char **tokens,*token;
    int pos=0,tokenBufferSize=tokenSize;
    tokens=(char **)malloc(sizeof(char *)*tokenSize);
    for(int i=0;i<tokenSize;i++){
        tokens[i]=NULL;
    }
    token=strtok(lineIP,deliminator);

    while(token!=NULL){
        tokens[pos]=token;
        pos++;
        if(pos>tokenBufferSize){
            tokenBufferSize+=tokenSize;
            tokens=realloc(tokens,tokenBufferSize);
        }
        token=strtok(NULL,deliminator);
    }

    return tokens;
}

//executing the commands given
int execute(char **tokens){
    int i;

    if(tokens[0]==NULL)
        return 1;

    for(i=0;i<numberOfCommands();i++){
        if(strcmp(tokens[0],commands[i])==0){
            return (*commandResponse[i])(tokens);
        }
    }
    return launch(tokens);
}

//main loop
int myLoop(){
    int status=1;
    char *line,*lineBackup;
    char **args; 
    int stackElemetsCount=stackSize;
    int stackTop=0;

    commandStack=(char **)malloc(sizeof(char*)*stackSize);
    for (int i=0;i<stackSize;i++){
        commandStack[i]=NULL;
    }


    while(status){
        
        printf(">");
        line=read_line();
        if(stackTop<stackElemetsCount){
            lineBackup=copyString(line);
            commandStack[stackTop]=lineBackup;
            stackTop++;
        }
        else{
            stackElemetsCount+=stackSize;
            commandStack=realloc(commandStack,stackElemetsCount);
            lineBackup=copyString(line);
            commandStack[stackTop]=lineBackup;
            stackTop++;            
        }
        args=read_args(line);
        status=execute(args);
    }
    free(commandStack);
    free(line);
    free(args);
return 1;
}


int main(int argc,char **argv){

    myLoop();

    return 0;
}
