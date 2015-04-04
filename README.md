server-side-dependency-solving - SSDS
==============================

Description
===========
Solving dependencies when installing new packages is a process that in some cases can be computationally very intensive. SSDS is designed to overcome this issue. Client device gathers information about installed packages and enabled repos which are then sent to a server. The server then takes care of dependency solving and the result is sent back to client device as a list of packages that are needed to proceed with the install process.

Right now SSDS is a separated program but in the future it might become plugin for dnf.

Dependencies
============
Required package which are needed for successfull compilation and build.

git
gcc-c++
cmake
boost
libsolv
librepo-devel
hawkey-devel
libxml2-devel
xml2
glib2.0-devel

Installation
============
This section describes how to install and compile SSDS project

There is new module in librepo, but it is not among updates yet so this is what needs to be done to be able to use it:
  -from http://koji.fedoraproject.org/koji/taskinfo?taskID=8948750 download these:
   librepo, python-librepo, librepo-devel - all in the right architecture
   these three should be ebough but just in case you can download also librepo-debuinfo and python3-librepo
   there is new version with some fixes - 1.7.13-4.fc21.x86_64
  -install these packages at the same time like this:
   $sudo dnf update <pkg.rpm> <pkg.rpm> ... - put them all into the same command

Then enter /usr/include/hawkey/ dir and apply patch located in src/bugzilla/bug.patch:
  $patch -p1 -R < bug.patch

Now everything should be in order to build ssds:
  -enter build/ and type:
    $cmake .
    $make
  -two files are created - ssds-client and ssds-server
  
Enjoy :)
