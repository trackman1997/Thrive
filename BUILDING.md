Building Thrive
===============

Required Software
-----------------

### Common

These programs are needed for all platforms:

- Unreal Engine 4 (4.16). 
  Note: on Linux the engine must be compiled from source. Use the `release` branch version.
- [Ruby](http://ruby-lang.org) (It is recommended to use RubyInstaller on Windows)
- git
- svn
- cmake

Everything except unreal engine must be installed so that it is 
included in the system PATH. 

You can check whether a program is in PATH by opening a command 
prompt/terminal and running the program by typing its name. 
For example `ruby -v` this shouldn't print an error instead you should get something like this:
```
ruby 2.4.1p111
``` 

Note: if you run ruby without any arguments it does nothing and gives you just a blinking cursor. 
That's not an error and ruby is correctly installed.

### Windows

- Visual Studio 2015 Community (make sure to enable c++ support when installing)
- Visual c++ 2010 runtime
- [Cygwin64](https://cygwin.com/install.html). When Cygwin asks for
  which packages to install make sure the at least the following are
  selected:
  
  - coreutils
  - make
  - bash
  
  You can also optionally install git and svn with cygwin.
  
  Then add the `C:\cygwin64\bin` to the system
  path to make everything visible to the script
  

### Linux

- GCC and Clang.
- lsb_release

### Mac

As we currently have to get a developer who uses a mac and can compile
the game this is a work in progress.

However we would love to help anyone trying to get a mac version of Thrive working.
Contact us on discord or on the forums for help, or github if you prefer.

### Ruby Gems
- os
- colorize
- rubyzip

These can be installed by running `gem install os colorize rubyzip`. Running with sudo is probably not required.
If you get errors installing the gems you should try updating your ruby version.

Cloning the Repository
----------------------

Now that we have the required programs we can start getting Thrive installed.


Go to a new folder where you want to build Thrive 

note: avoid paths with spaces in them on Linux. On Windows paths with spaces work,
for example `C:\Users\USERNAME\Documents\Unreal Projects\` is a good choice.

And run
```
git clone https://github.com/Revolutionary-Games/Thrive.git Thrive
```

This should download the thrive repository to a new folder called `Thrive`.
All following commands assume that they are ran in that folder unless specified otherwise.

Running the Setup Script
------------------------

### Windows

Run the setup script in command prompt
(hint: shift right-click in the Thrive folder and select 'open command prompt here')
```
ruby SetupThrive.rb thrive
```

If you get errors see the troubleshooting section.

### Linux
```
./SetupThrive.rb thrive
```

### Parameters
The `thrive` parameter after the script name is the name for the assets svn. 
The thrive account has READ ONLY access and the password for `thrive` is `thrive`.
So remember to type that in when it is asked by svn.

Once you are on the team you get access to a separate account you can use here that has
write (push) access.


Setting up version control within the editor
--------------------------------------------

By default the Unreal Editor doesn't care about files that have been
modified by others and this is troublesome when it comes to blueprints
and other ue assets. Because they aren't text files they cannot be
merged by svn.

This is why it's important to enable source control within the
editor. To do this click the "Source Control" button once you have
opened the thrive project. Select svn and fill in your svn username
and password ("thrive" for read only, which doesn't work well, so you
might actually NOT want to link to source control in that case).

Now the editor should automatically "check out" (lock for your use)
any files you modify, preventing multiple people from messing with the
same files at the same time. Also the editor allows handling svn
conflicts gracefully with a graphical editor.

Updating to new versions
------------------------

You can run the setup script as many times as you want. 

Thus the recommended procedure to update your copy is running these commands:
```
git pull
git submodule update
svn up
```

And then run the setup script again and then open ue4 editor and recompile the code, 
or recompile before opening the editor.

### Not Using the Setup Script 

If you don't want to run the setup script you will need to manually 
compile and copy all the extra dependencies AND configure the ue4 Thrive module 
build script from `Source/Thrive/Thrive.Build.Source.cs`. And get the svn assets. And...

Basically it's not a fun time so unless you have already worked on Thrive a bunch it's
not recommended to try doing the setup manually.


Troubleshooting
---------------

### Missing command `pr`

If you get an error about `pr` being an unkown command or missing try
installing Cygwin64 'coreutils' package. If that doesn't work either 
install the default set of cygwin packages.

### Incorrect ffmpeg makefile line endings

If you get an error "missing separator. Stop" in common.mak when
trying to build ffmpeg on windows, this is probably caused by line
endings being incorrect.

To fix these change to the FFMPEG FOLDER and run these commands to fix the line endings:
```
git config core.autocrlf false
```
Delete all files except ".git" folder. And then:
```
git checkout .
```

Warning: this will discard all changes in the git repo this is ran in,
so make sure you are in the right folder.


It is also possible to globally change the autocrlf setting but that
isn't recommended to avoid accidentally committing windows line
endings.


### Issues compiling ffmpeg

First, make sure that you are using the standard Windows command
prompt. Using a MinGW shell or equivalent is known to cause
issues. You may need to delete the ffmpeg folder after this to make
sure that there are no cached values or other things messing things
up.


Then make sure that nothing is overriding the programs in cygwin. You
should start by making sure cygwin is the first thing in PATH. And
then that the specific programs are from cygwin. This can be done by
renaming the executables in cygwin/bin, for example renaming `awk.exe`
or `make.exe` to `testawk.exe` and then verifying that the setup
script starts complaining about missing that specific program. If it
doesn't start complaining then this confirms that something is messing
with your PATH and overriding the cygwin tools.  Either delete the
offending things (MSYS, MinGW) or remove them from PATH (and make sure
once again that you are using plain `cmd.exe` to run the setup and not
some other shell).



### ffmpeg build is still failing

For other ffmpeg building issues see these guides:
[windows guide](https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC),
[all guides](https://trac.ffmpeg.org/wiki/CompilationGuide)


