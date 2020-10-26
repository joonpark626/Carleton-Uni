# Tcl Scripting in Vivado

The Tcl script auto-generates an HDL wrapper. Delete and generate a new HDL wrapper, in case of any modifications to the design, to have your changes saved.

## 1 | Generate Tcl script through Vivado

Assuming you have created a project, you can go through the following steps to generate a Tcl script.

- **Step 1:** From _'File'_ menu on the left top of Vivado 2016.3, select _'Write Project Tcl'_.
- **Step 2:** Choose a name and location for the output Tcl script file, generally use the name `build.tcl`\
(i.e. "C:/Users/Ghassan_Arnouk/verilog_labs/build.tcl").

***Note:*** Ensure to have *'Do not import sources'* checked off.

## 2 | Run Tcl script through Vivado Tcl Console

You can go through the following steps to run a Tcl script.

- **Step 1:** Start Vivado 2016.3. You can see the Tcl Console on the left bottom of Vivado 2016.3.
- **Step 2:** Click Tcl Console. Move cursor to the bottom of the line (i.e to _'type a tcl command here'_).
- **Step 3:** Switch to your folder location where you are having the Tcl script using the `cd` command\
(i.e. "cd C:/Users/Ghassan_Arnouk/laboratories/lab6_1_1").
- **Step 4:** Once you changed your directory, make sure you have the Tcl script in that location using `dir` command\
(i.e. "dir").
- **Step 5:** Run the script using the command `source filename.tcl` (_'build.tcl'_ is generally used).
- **Step 6:** Now you can see Vivado 2016.3 started to create the project.
- **Step 7:** If all the steps so far went as planned, you will have your project open.

***Note:*** Ensure Tcl script is located in a directory with **NO space** in it.
If not, ensure to use `{}` for the path of the specified directory (i.e. "cd {C:/Users/Ghassan Arnouk/laboratories/lab6_1_1}").\
***Note:*** Ensure to have all design and simulation sources `filename.v` and constrains `filename.xdc` in a folder (i.e. _src_ ) outside the project created by Vivado **BEFORE** generating the Tcl script.
