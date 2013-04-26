SeniorProject
=============

Kernel and OS development project using:

The following tutorials/guides:

  http://wiki.osdev.org/
  
  http://www.osdever.net/bkerndev/Docs/intro.htm
  
  http://www.jamesmolloy.co.uk/tutorial_html/
  
  http://www.acm.uiuc.edu/sigops/roll_your_own/

The following tools:

  Bochs Virtual Machine software
  
  GCC compiler
  
  LD linker
  
  NASM assembler
  
  A few other assorted tools and plugins for them

Building Project
================
1. Run build.bat to compile, assemble and link code together
2. Run bochs (which should find the text config file in the directory and load in the floppy image)
3. Give grub the location of the kernel (currently 200+24 in this state)
4. Type boot
5. Poof!
