Program Structure in Keil IDE
=============================

Program structure contains 2 parts: **File Structure** and **IDE Structure**

File Structure
--------------

The name of top level directory is `JoyCar` and sub-directory structure is:

- `project` including project related files and generating files.
  * `list` symbol files and map files.
  * `obj` object files and binary files.
- `Libraries` including the library files of stm32.
- `basic_drivers` including basic drivers which are **on-board** modules such as _Interrupt_, _System Clock_, _UART_ etc.
- `external_drivers` including sensors and modules which connect board **externally**. Every module should has its own directory.
- `user` including control/logic modules. **Don't coupling logic part into drivers**.
- `README.md` is the description of file structure and usage of program.

IDE Structure
-------------

The project name is `joycar` and group structure is:

- `user` the same as _File Structure_ part.
- `basic_drivers` the same as _File Structure_ part.
- `module` every module of external drivers should has its own group.
- `startup` including the start-up code.
- `library` the same as _File Structure_ part.
- `doc` the descriptions and definitions of project.
