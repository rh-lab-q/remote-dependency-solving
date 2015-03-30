server-side-dependency-solving - SSDS
==============================

Description
===========
Solving dependencies when installing new packages is a process that in some cases can be computationally very intensive. SSDS is designed to overcome this issue. Client device gathers information about installed packages and enabled repos which are then sent to a server. The server then takes care of dependency solving and the result is sent back to client device as a list of packages that are needed to proceed with the install process.

Right now SSDS is a separated program but in the future it might become plugin for dnf.

Dependencies
============
Required package which are needed for successfull compilation and build.

Installation
============
This section describes how to install and compile SSDS project


