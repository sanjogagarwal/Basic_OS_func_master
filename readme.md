#contributers -:dharmesh chourasiya
                tarun genwa
                aman agarwal
The code of myMain.c implement the basic os shell functions
      1.cd
      2.rm
      3.running a program as a child process
      4.history n -:giving the list of the last n commands     executed
      5.issue n -:issue the nth command in the history
      6.rmexcept
      7.<program_name> m-:killing a child process forcefully after m seconds.
      8.exit-: exits the program.

maxIPsize-: defined so that the length of a input string is defined.
tokensize-: size of various arguments or tokens.
deliminator -: the characters by which the arguments are separated .
stackSize-:  the number of commands that history is going to save
timeout-: global variable to be used as flag for alarm signal
commands-:all the built in commands are in this array.
commandsResponse-:response of all the builtin arguments.


1.commmand_cd(char **args)-:
        this function is used to change directory. The arguments string after
        tokenizing is passed to the function. The args[1] contains the address
        of the directory. First checks weather args[1] is not NULL.

2.command_ls(char **args)-:
        Here this functions uses a subroutine of "scandir" that returns all the
        names of files in a directory in an array "namelist". This returns the
        count of the total number of files or the directories.

3.command_rm(char **args)-:
        Here the functions first checks weather there is a argument given or not.
        Then it checks for the various kinds of special characters such as "-r"
        "-v","-f". Then there is specified routine for each and every condition.
        In "-v" there is  descriptive deletion takes place.
        In "-r" the files inside the directories are deleted in a recursive manner.
        In "-f" we forcefully remove the file/directory.

4.command_history(char **args)-:
        We maintain a global variable named "commandStack" that saves the last
        commands. If a command like "history m" is given then the last m number
        of commands are displayed. The commands are saved in the "commandStack"
        during the time of the execution of that particular command.
        We are increasing the size of the "commandStack" by using "realloc()" and
        increasing size by "stackSize"(all this happen in myLoop()).

5.command_issue(char **args)-:
        This function checks for a numerical entry and then runs the nth command
        "commandStack" again.

6.command_rmexcept(char **args)-:
        Here args will contain the list of the files that we dont want to remove .
        "namelist" contains the name of all the files in that directory.We use
        basic search algorithm to check which of the files of the "namelist " are
        not in the "args" and then forcefully removing them using "remove()" subroutine.

7.command_exit(char **args)-:
        exits the shell.
        returns 0.

8.numberOfCommands()-:
        returns number of built in or predefined commands.

9.copyString(char *)-:
        It is a helper function that is used for generating the copy of a string.

10.getLastArgs(char *)-:
        It is also a helper function that returns index of the first NULL args cell.

11. launch(char **args)-:       
        This function is used to run a child process when a parent process is running. "fork()" subroutine creates a new process with a pid . If pid ==0 then the process is a child process ,if pid<0 then its a invalid process. If pid> 0 then process is parent process.
        Here we checks first weather a termination time is given or not if it is not given then execute the process with a "waitpid()" otherwise use a alarm and "SIGALRM"
        signal to interrupt at the time alarm is finished.
        At that time the program checks weather the status of the child process is finished or not, if not then kill the process with the pid ,otherwise return that process is terminated in a particular time limit.
        "signal()" is used to register the signal. "alarm()"
        has the time given by the user as input(in seconds).

12.read_line()-:
        This function gets the input given by the user.
        It keeps on taking the input until a null character or a new line character is given.
        The buffer "ipLine" is dynamically reallocated memory if the memory provided is not sufficient or it exceeds the max size.The size is increased by the value "maxIPsize".

13. read_args(char *lineIP)-:
        This function takes the input string as a argument and then it tokenize them using the subroutine "strtok".We tokenize them using the deliminator " \t\n". After that we save the token into a single array of arguments.
        Here the first cell will always contains the built in commands and after that we have rest of the arguments.

14.execute(char **tokens)-:
        This function takes the tokenized arguments and then compare them with the builtin commands and run the respective function for that command.
        If we dont have any matching built in command for the argument given then we run it as a normal program and then it goes into a "launch()" subroutine. In launch() subroutine the process given is executed as a child process.

15.myLoop()-:
        This is the main infinite loop in which shell is present. We save the commands in "commandStack" in this function along with that :
                  read_line()
                  read_args()
                  execute()
        All of the three function takes place in this sequence until status in FALSE.          

16.alarm_handler()-:
        make timeout TRUE to indicate that alarm interrupt has occured..
# A-Basic-Shell-in-C
