Building Thrive
===============

Required Software
-----------------

### Common

These programs are needed for all platforms:

- Unreal Engine 4 (4.16). 
  Note: on Linux the engine must be compiled from source. Use the `release` branch version.
- (Ruby)[http://ruby-lang.org] (It is recommended to use RubyInstaller on Windows)
- git
- svn
- cmake

Everything except unreal engine must be installed so that it is 
included in the syustem PATH. You can check by opening a command 
prompt/terminal and running the program by typing its name. 
For example `ruby -v` this shouldn't print an error instead you should get something like this:
```
ruby 2.4.1p111
``` 

Note: if you run ruby without any arguments it does nothing and gives you just a blinking cursor. 
That's not an error and ruby is correctly installed.

### Windows

Visual Studio 2015 Community


### Linux

GCC and Clang.

lsb_release

### Mac

As we currently have to one who uses a mac and can compile the game this is a work in progress.

But we would love to help to anyone trying to get a mac version of Thrive working.
Contact us on discord or on the forums for help, or github if you prefer.

#### Ruby Gems
- os
- colorize
Not supported for ruby 2.4 on Windows. 1.8.0 version should add that support. 
- (Windows only) nokogiri

These can be installed by running `gem install os colorize nokogiri`. Running with sudo is probably not required.
If you get errors installing the gems you should try updating your ruby version.

Cloning the Repository
----------------------

Go to a new folder where you want to build Thrive. And run
```
git clone https://github.com/Revolutionary-Games/Thrive.git Thrive
```

This should download the thrive repository to a new folder called `Thrive`.
All following commands assume that they are ran in that folder unless specified otherwise.

Running the Setup Script
------------------------

Windows:
```
ruby SetupThrive.rb thrive
```
Linux
```
./SetupThrive.rb thrive
```

The `thrive` parameter after the script name is the name for the assets svn. 
The thrive account has READ ONLY access and the password for `thrive` is `thrive`.

Once you are on the team you get access to a separate account you can use here that has
write (push) access.


### Not Using the Setup Script 

If you don't want to run the setup script you will need to manually 
run all the commands it would have ran





