Date 10/13/2023

This program, that I am personally calling rbat, is a batch interpeter meant to proccess normal command line commands with added controls and tools. 

Variables
    All variables are booleans, they can be created in three ways.

    1. With the $ command, this will create the variable initalized to true,
    unless the variable name is followed by off/false or the = symbol.

    2. With the args(variable_names) function. These are initalized to false.

    3. By specifying it when calling a file by using a dash as a prefix.
    These are initalized to true;

Flags
    Flags can be operated on and used the same way as variables but are always defined.

    "echo" Controls whether anything is printed to the console. On by default.

    "fbf" Stands for "file by file" and ask before proccessing every file.

    "lbl" Stands for "line by line" and ask before proccesing evey line.

    "coe" Stands for "continue on error" and will stop the program from terminating,
    and go to the next line after an error.

    "aoe" Stands for "ask on error" and will ask if you want to continue after an error occurs.

    "ebf" Stands for "error on batch fail" and will count a non-zero return from running a program 
    on the command line as an error. 

Functions/Controls
    args - Takes in variable names as argumentsand will intizalize them to false if they are not already defined.

    check - Takes two groups of string literal arguments seperated by a semicolon representing file paths.
    If any of the files in the first group have been edited or created later than any of the second group,
    then it will return true and run the scope associated with it if there is one. 

    out - Takes a string literal as an argument and will print it to the console,
    regardless of whether echo in on or not.

    echo - Takes a string literal as an argument and will print it to the console if echo is on.

    ask - Takes a string literal as an argument, will print that message and then wait for the user
    to input a y/Y or a n/N returns the result as a boolean and will run the scope associated with it,
    if there is one. Will run regardless of whether or not echo is on.

    echo ask - Same as ask, except it will automaticly return false if echo is on.

    for - takes a positive integer as an argument, and will execute the contents of the scope
    associated with it that number of time.

    if/else if/else - Takes a boolean expression as an argument and will run the scope associated with it
    if the result is true. 'else if' and 'else' can also be used after the check, ask, and echo ask functions.
    The 'if' in 'else if' can also be replaced by any of those functions.

    exit - If exit() is called the entire program will close. If it is called with a number as an argument exit(n),
    then if the number is positive then it will exit scopes until it arrives at the nth scope,
    if the number is negative it will exit that many scopes. Passing 0 closes the program.
    
Commands
    '~' toggles the state of a variable
    '!' makes a variable false
    '-' makes a variable true
    '$' defines a variable
    '&' defines a macro
    '#' includes the contents of the files specified
    '*' runs the following line directly in the command line
    note: this is the defualt, so the '*' command is generally not needed
    unless there are naming conflicts with the program to be run.

    The Push Command:
        The Push Command is indicated by a set of bracket followed by either '->' or '~>'.
        The set of brackets are to contain string literals seperated by commas these strings
        can represent filenames to read or text to use. The '->' or '~>' symbols are to be 
        followed by a single string literal representing the folder to write in, to write in
        the current folder just set this as empty '""'. The program will read the data in the 
        brackets and write inside the foler specified. If the '->' is used, then the contents of
        any file already there will be replaced, if the '~>' is used, the the file will be appended to.
        The strings inside the brackets can be joined by another string inside of parentheses,
        indicating the name to use when writing the file. To indicate that the part to read is not
        a filename, but rather actual data to be written, a colon must be placed outside of 
        the string and outside of parentheses. If this option is used then a file path must be
        specified inside of parentheses.

        Example:
            ["file_name.txt","other_file.exe"("new_name.exe")]->"folder_path";
            ["\n//comment":("file_to_write_to.cpp"),
            "other_other_file.txt"("file_to_write_to.cpp")]~>"folder_path";

        This writes writes "file_name.txt" into the folder, writes "other_file.exe" into the folder as 
        "new_name.exe", appends "\n//comment" and the contents of "other_other_file.txt" to "file_to_write_to.cpp"
        inside of the folder.

Macros
    Macros can be defined with the '&' command followed by the name of the macro, the argument
    and the code it represents:
        &macro_name(arg){
            if(arg){
                //stuff
            }else{
                //other stuff
            }
            //more stuff
        }note: contents of macro not called on definition.
    Macros are called by the name of the macro:
        $var=false;
        macro_name(var);
    Macros can contain anything except macro definitions including calling itself. RECURSION BEWARE!!!

Including Files
    A file can be included by starting a line with '#' followed by the path to that file as a string literal. That string can be followed by parentheses inside of which can can contain variable names to be initalized to true. These variable names can also be followed by an equal sign and a boolean expression and will be initalized to the result of said boolean expression instead. Included files operate inside of the current scope not there own.

Boolean Expressions
    Boolean Expressions can consist of variable or lines of batch code that are seperated by boolean operators.
    Boolean operators include: !(not), &(and), |(or), ~(the same), ^(different). A period will ensure the following part of the expression is proccessed as a normal line, so if we have a variable named foo that is true then !foo will evaluate to false and foo will not be effected, but .!foo will make foo false and then evaluate.