Unreal Engine 4 Usage in Thrive
===============================


Importing assets
----------------

The raw asset files need to be placed in Thrive/RawContent and
imported from there. This way we won't lose the sources for any of the
assets.


Committing SVN content
----------------------

All the code is in the Source subdirectory which is handled by git. So
you need to make sure that you never commit that folder to svn. There
is a helper script called `SVNCommit.rb` which automatically ignores
the Source folder when committing.


Committing not working from editor
----------------------------------

The editor might not work for committing changes to assets. You will
need to use the command line `svn commit` command directly or use the
helper script described in the section `Committing SVN content` above


Code style
----------

Naming scheme and variable naming conventions should match the one
used in Unreal Engine. Namely classes should start with either A, U or
F depending on what their base class is. Enums start with E and
variables are always CamelCase starting with a capital. See the source
code for more details or other descriptions about Unreal Engine naming
conventions.
