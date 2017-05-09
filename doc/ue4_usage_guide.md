Unreal Engine 4 Usage in Thrive
===============================


Importing assets
----------------

The raw asset files need to be placed in `Thrive/RawContent` and
imported from there. This way we won't lose the sources for any of the
assets.

Some files that cannot be directly imported and need to be converted
are placed in `Thrive/RawContent/Converted`. Some sound files and
others that can be automatically converted are handled by scripts in
the `Thrive/RawContent/Scripts` folder. The folder
`Thrive/RawContent/Converted` should NOT be in source control because
if the source assets are modified they need to be regenerated and
imported. Long sound files should not be imported as assets. They
should instead be in `Thrive/Content/ExtraContent` where they can be
played with the custom audio player.


Committing SVN content
----------------------

All the code is in the Source subdirectory which is handled by git. So
you need to make sure that you never commit that folder to svn. There
is a helper script called `SVNCommit.rb` which automatically ignores
the Source folder when committing. Also new files that aren't binaries
(assets, blueprints) should be comitted to git.


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
