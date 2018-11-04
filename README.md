# ComputerNetworksProject
A C++ console program which implements a communication protocol among processes and creates custom UNIX commands.

* **"login: username"** - whose existence is validated by using a configuration file
* **"myfind file"** - a command that allows finding a file and displaying information associated with that file; the displayed information will contain the creation date, date of change, file size, file access rights, etc.
* **"mystat file"** - a command that allows you to view the attributes of a file
* **"quit"**

# Other project specifications:

* communication among processes is done using the following communication mechanisms: pipes, fifos, and socketpairs
* communication is done by executing commands read from the keyboard in the parent process and executed in child processes
* the result obtained from the execution of any command will be displayed on screen by the parent process
* no function in the "exec()" family will be used to implement "myfind" or "mystat" commands in order to execute some system commands that offer the same functionalities.

![](https://github.com/AlexandruSte/ComputerNetworksProject/blob/master/1.PNG)
