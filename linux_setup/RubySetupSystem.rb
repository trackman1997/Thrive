# coding: utf-8
# A ruby script for downloading and installing C++ project dependencies
# Made by Henri Hyyryläinen

# TODO: make cmake use extra find paths on windows and test

require_relative 'RubyCommon.rb'
require_relative 'DepGlobber.rb'

require 'optparse'
require 'fileutils'
require 'etc'
require 'os'
require 'pathname'
require 'zip'

## Used by: verifyVSProjectRuntimeLibrary
#require 'nokogiri' if OS.windows?
## Required for installs on windows
##require 'win32ole' if OS.windows?

#
# Parse options
#


options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: CreatePackages.rb --default"

  opts.on("--[no-]sudo", "Run commands that need sudo. " +
                         "This maybe needed to run successfuly") do |b|
    options[:sudo] = b
  end 
  opts.on("--only-project", "Skip all dependencies setup") do |b|
    options[:onlyProject] = true
  end

  opts.on("--only-deps", "Skip the main project setup") do |b|
    options[:onlyDeps] = true
  end  
  
end.parse!

if !ARGV.empty?
  # Let application specific args to be parsed
  if defined? parseExtraArgs
    parseExtraArgs
  end

  if !ARGV.empty?

    onError("Unkown arguments. See --help. This was left unparsed: " + ARGV.join(' '))

  end
end

### Setup variables
CMakeBuildType = "RelWithDebInfo"
CompileThreads = Etc.nprocessors

# If set to true will install CEGUI editor
# Note: this doesn't work
InstallCEED = false

# If set to false won't install libs that need sudo
DoSudoInstalls = if options.include?(:sudo) then options[:sudo] else true end

# If true dependencies won't be updated from remote repositories
SkipPullUpdates = false

# If true skips all dependencies
OnlyMainProject = if options[:onlyProject] then true else false end

# If true skips the main project
OnlyDependencies = if options[:onlyDeps] then true else false end

# If true new version of depot tools and breakpad won't be fetched on install
NoBreakpadUpdateOnWindows = false

# On windows visual studio will be automatically opened if required
AutoOpenVS = true

# Visual studio version on windows, required for forced 64 bit builds
VSVersion = "Visual Studio 14 2015 Win64"
VSToolsEnv = "VS140COMNTOOLS"

# TODO create a variable for running the package manager on linux if possible

### Commandline handling
# TODO: add this

if OS.linux?
  HasDNF = which("dnf") != nil
else
  HasDNF = false
end

# This verifies that CurrentDir is good and assigns it to CurrentDir
CurrentDir = checkRunFolder Dir.pwd

ProjectDir = projectFolder CurrentDir

ProjectDebDir = File.join ProjectDir, "libraries"

ProjectDebDirLibs = File.join ProjectDebDir, "lib"

ProjectDebDirBinaries = File.join ProjectDebDir, "bin"

ProjectDebDirInclude = File.join ProjectDebDir, "include"



info "Running in dir '#{CurrentDir}'"
puts "Project dir is '#{ProjectDir}'"

if HasDNF
  info "Using dnf package manager"
end

puts "Using #{CompileThreads} threads to compile, configuration: #{CMakeBuildType}"



## Install runner
class Installer
  # basedepstoinstall Is an array of BaseDep derived objects that install
  # the required libraries
  def initialize(basedepstoinstall)

    @Libraries = basedepstoinstall

    if not @Libraries.kind_of?(Array)
      onError("Installer passed something else than an array")
    end
    
  end

  # Adds an extra library
  def addLibrary(lib)

    @Libraries.push lib
  end

  # Runs the whole thing
  # calls onError if fails
  def run()

    if not OnlyMainProject
      @Libraries.each do |x|

        if x.respond_to?(:installPrerequisites)
          
          # Verifying that it works
          begin
            # Require that this list method exists
            deps = x.depsList
          rescue RuntimeError
            # Not used on platform. Should always be used on non-windows
            if !OS.windows?
              onError "Dependency #{x.Name} prerequisites fetch failed. This needs to " + 
                "work on non-windows platforms"
            end
            
            next
          end
          
          onError "empty deps" if !deps
          
          if !DoSudoInstalls

            warning "Automatic dependency installation is disabled!: please install: " +
                    "'#{deps.join(' ')}' manually for #{x.Name}"
          else
          
            # Actually install
            info "Installing prerequisites for #{x.Name}..."
            
            x.installPrerequisites
            
            success "Prerequisites installed."
            
          end
        end
      end
    end

    if not SkipPullUpdates and not OnlyMainProject
      info "Retrieving dependencies"

      @Libraries.each do |x|

        x.Retrieve
        
      end

      success "Successfully retrieved all dependencies. Beginning compile"
    end

    if not OnlyMainProject

      info "Configuring dependencies"

      @Libraries.each do |x|

        x.Setup
        x.Compile
        x.Install
        
      end

      success "Dependencies done, configuring main project"
    end

    if OnlyDependencies

      success "All done. Skipping main project"
      exit 0
    end
    
  end
  
end

# Path helper
# For all tools that need to be in path but shouldn't be installed because of convenience
# TODO: switch to using this everywhere (the old deps are broken because of this)
def runWithModifiedPath(newPathEntries, prependWinPath=false)
    
  if !newPathEntries.kind_of?(Array)
      newPathEntries = [newPathEntries]
  end
    
  oldPath = ENV["PATH"]

  onError "Failed to get env path" if oldPath == nil

  if OS.windows?
    
    if prependWinPath
      newpath = newPathEntries.join(";") + ";" + oldPath
    else
      newpath = oldPath + ";" + newPathEntries.join(";")
    end
  else

    newpath = newPathEntries.join(":") + ":" + oldPath
  end

  info "Setting path to: #{newpath}"
  ENV["PATH"] = newpath
   
  begin
    yield
  ensure
    info "Restored old path"
    ENV["PATH"] = oldPath
  end    
end


#### Windows stuff

# Run visual studio environment configure .bat file
def bringVSToPath()
  if not File.exist? "#{ENV[VSToolsEnv]}VsMSBuildCmd.bat"
    onError "VsMSBuildCMD.bat is missing check is VSToolsEnv variable correct in Setup.rb" 
  end
  "call \"#{ENV[VSToolsEnv]}VsMSBuildCmd.bat\""
end

# Run vsvarsall.bat 
def runVSVarsAll(type = "amd64")

  folder = File.expand_path(File.join ENV[VSToolsEnv], "../../", "VC")

  if not File.exist? "#{folder}/vcvarsall.bat"
    onError "'#{folder}/vcvarsall.bat' is missing check is VSToolsEnv variable correct in Setup.rb" 
  end
  "call \"#{folder}\\vcvarsall.bat #{type}\""
end

# Gets paths to the visual studio link.exe and cl.exe for use in prepending to paths to
# make sure mingw or cygwin link.exe isn't used
# param amd64 if not empty selects 64 bit compiler
def getVSLinkerFolder(amd64 = "amd64")

  onError "visual studio environment variable '#{VSToolsEnv}' is missing" if !ENV[VSToolsEnv]
  
  folder = File.expand_path(File.join ENV[VSToolsEnv], "../../", "VC", if amd64 then "bin/#{amd64}" else "bin" end)
  
  onError "vs linker bin folder doesn't exist" if !File.exists? folder
  
  folder
end

def getVSBaseFolder()
  onError "visual studio environment variable '#{VSToolsEnv}' is missing" if !ENV[VSToolsEnv]
  
  folder = File.expand_path(File.join ENV[VSToolsEnv], "../../")
  
  onError "vs folder doesn't exist" if !File.exists? folder
  
  folder
end

# Makes sure that the wanted value is specified for all targets that match the regex
def verifyVSProjectRuntimeLibrary(projFile, matchRegex, wantedRuntimeLib)
  # Very parameters
  onError "Call verifyVSProjectRuntimeLibrary only on windows!" if not OS.windows?
  onError "Project file: #{projFile} doesn't exist" if not File.exist? projFile
  
  # Load xml with nokogiri
  doc = File.open(projFile) { |f| Nokogiri::XML(f) }
  
  doc.css("Project ItemDefinitionGroup").each do |group|
    if not matchRegex.match group['Condition'] 
      next
    end
    
    info "Checking that project target '#{group['Condition']}' Has RuntimeLibrary of type #{wantedRuntimeLib}"
    
    libType = group.at_css("ClCompile RuntimeLibrary")
    
    if not libType
      warning "Couldn't verify library type. Didn't find RuntimeLibrary node"
      next
    end
    
    if libType.content != wantedRuntimeLib
      
      onError "In file '#{projFile}' target '#{group['Condition']}' "+
        "Has RuntimeLibrary of type #{libType.content} which is not #{wantedRuntimeLib}. "+
        "Please open the visual studio solution in the folder and modify the Runtime Library to be #{wantedRuntimeLib}." +
        "If you don't know how google: 'visual studio set project runtime library'"
    end
  end
  
  success "All targets had correct runtime library types"
end

def runWindowsAdmin(cmd)
  shell = WIN32OLE.new('Shell.Application')
  
  shell.ShellExecute("ruby.exe", 
                     "\"#{CurrentDir}/Helpers/WinInstall.rb\" " +
                     "\"#{cmd.gsub( '"', '\\"')}\"", 
                     "#{Dir.pwd}", 'runas')
  
  # TODO: find a proper way to wait here
  info "Please wait while the install script runs and then press any key to continue"
  system "pause"
end

def askToRunAdmin(cmd)
  puts "."
  puts "."
  info "You need to open a new cmd window as administrator and run the following command: "
  info cmd
  info "Sorry, windows is such a pain in the ass"
  system "pause"
end

### Linux only stuff
def getLinuxOS()

  if OS.mac?
    return "mac"
  end
  
  if OS.windows?
    raise "getLinuxOS called on Windows!"
  end

  osrelease = `lsb_release -is`.strip

  onError "Failed to run 'lsb_release'. Make sure you have it installed" if osrelease.empty?

  osrelease.downcase

end

# Returns true if sudo should be enabled
def shouldUseSudo(localOption, warnIfMismatch = true)

  if localOption

    if !DoSudoInstalls

      if warnIfMismatch

         warning "Sudo is globally disabled, but a command should be ran as sudo.\n" +
                 "If something breaks please rerun with sudo allowed."
      end

      return false
      
    end

    return true
    
  end

  return false
end

# Returns "sudo" or "" based on options
def getSudoCommand(localOption, warnIfMismatch = true)

  if(!shouldUseSudo localOption, warnIfMismatch)
    return ""
  end
  
  return "sudo "
end

### Standard stuff

# CMake configure
def runCMakeConfigure(additionalArgs)
    
  if OS.windows?

    system "cmake .. -G \"#{VSVersion}\" #{additionalArgs}"
        
  else
  
    system "cmake .. -DCMAKE_BUILD_TYPE=#{CMakeBuildType} #{additionalArgs}"
        
  end
end

# Running make or msbuild
def runCompiler(threads)
    
  if OS.windows?
    #system "start \"ms\" \"MSBuild.exe\" "
    # Would use this if used project.sln file: /target:ALL_BUILD 
    system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{threads} /p:Configuration=RelWithDebInfo"
        
  else
    
    system "make -j #{threads}"
        
  end
end

# Run msbuild with specific target and configuration
def runVSCompiler(threads, project = "ALL_BUILD.vcxproj", configuration = "RelWithDebInfo",
  platform = "x64")
  
  onError "runVSCompiler called on non-windows os" if !OS.windows?
  system "#{bringVSToPath} && MSBuild.exe #{project} /maxcpucount:#{threads} " + 
    "/p:Configuration=#{configuration} /p:Platform=\"#{platform}\""

end

# Running platform standard cmake install
def runInstall()
    
  if OS.windows?
  
    info "Running install script as Administrator"

    # Requires admin privileges
    runWindowsAdmin("#{bringVSToPath} && MSBuild.exe INSTALL.vcxproj /p:Configuration=RelWithDebInfo")
        
  else
    
    system "sudo make install"

  end
end

def runGlobberAndCopy(glob, targetFolder)
  onError "globbing for library failed #{glob.LibName}" if not glob.run
  
  FileUtils.cp_r glob.getResult, targetFolder
end

def isInSubdirectory(directory, possiblesub)

  path = Pathname.new(possiblesub)

  if path.fnmatch?(File.join(directory, '**'))
    true
  else
    false
  end
  
end

def createDependencyTargetFolder()

  FileUtils.mkdir_p ProjectDebDirLibs
  
  FileUtils.mkdir_p ProjectDebDirBinaries
  
  FileUtils.mkdir_p ProjectDebDirInclude
  
end


def createLinkIfDoesntExist(source, linkfile)

  if File.exist? linkfile
    return
  end

  FileUtils.ln_sf source, linkfile
  
end

# Installs a list of dependencies
def installDepsList(deps)

  os = getLinuxOS

  if os == "fedora" || os == "centos" || os == "rhel"

    if HasDNF
      askRunSudo "sudo dnf install #{deps.join(' ')}"
    else
      askRunSudo "sudo yum install #{deps.join(' ')}"
    end

    return
  end

  if os == "ubuntu"

    askRunSudo "sudo apt-get install #{deps.join(' ')}"

    return
  end

  onError "unkown linux os '#{os}' for package manager installing " +
          "dependencies: #{deps.join(' ')}"
  
end

class GitVersionType

  UNSPECIFIED=1
  BRANCH=2
  HASH=3
  # A specific commit
  TAG=4
  # A remote repo?
  REMOTE=5

  # Must be in the folder where git can find the current repo
  def GitVersionType.detect(versionStr)

    if !versionStr || versionStr.length == 0
      # Default branch
      return BRANCH
    end

    output = %x{git show-ref --verify refs/heads/#{versionStr}}
    
    if $?.exitstatus == 0
      return BRANCH
    end

    output = %x{git rev-parse --verify "#{versionStr}^{commit}"}
    
    if $?.exitstatus == 0
      return HASH
    end
    
    output = %x{git show-ref --verify refs/tags/#{versionStr}}
    
    if $?.exitstatus == 0
      return TAG
    end

    output = %x{git show-ref --verify refs/remote/#{versionStr}}
    
    if $?.exitstatus == 0
      return REMOTE
    end

    UNSPECIFIED
  end

  def GitVersionType.typeToStr(type)
    k = GitVersionType.constants.find {|k| GitVersionType.const_get(k) == type}
    return nil unless k
    "GitVersionType::#{k}"
  end

  
end

# Detect whether a given git is a branch or a hash and then do a pull
# if on a branch
def gitPullIfOnBranch(version)

  versionType = GitVersionType.detect(version)

  puts "Doing git pull updates. '#{version}' is #{GitVersionType.typeToStr versionType}"

  case versionType
  when GitVersionType::UNSPECIFIED, GitVersionType::BRANCH
    system "git pull origin #{version}"
  end
end

# Copies files to a directory following all symlinks but also copying the symlinks if their
# names are different
# Also if stripfiles is true will run strip on each of the files
def copyPossibleSymlink(path, target, stripfiles = false, log = false)

  if not File.exist?(path)
    warning "Skipping copying non-existant file: #{path}" if log
    return
  end

  info "Copying file: #{path} to #{target}" if log 
  
  if File.lstat(path).symlink?
    
    link = File.join(File.dirname(path), File.readlink(path))

  else
    link = nil
  end

  if link

    info "File #{path} is a symlink to #{link}" if log

    if not File.basename(link) == File.basename(path)

      # Symlink target has a different name, so copy the symlink
      #FileUtils.cp path, target
      linkname = File.join(target, File.basename(path)) 
      FileUtils.ln_sf File.basename(link), linkname

      if not File.lstat(linkname).symlink?
        onError "Link creation failed (#{linkname} => #{File.basename(link)})"
      end
      
      info "Created symlink #{linkname} pointing to #{File.basename(link)}" if log
    end

    # Follow the link
    info "Following symlink to #{link}" if log
    copyPossibleSymlink(link, target, stripfiles, log)
    
  else

    info "Copying plain file: #{path}" if log
    
    # Plain old file
    FileUtils.cp path, target
    
    if stripfiles
      system "strip #{File.join(target, File.basename(path))}"
      info "Stripped file #{File.join(target, File.basename(path))}" if log
    end
  end
end

HandledLibraries = []

# Copies a dependency library to target directory following all symlinks along the way
# Ignores some common things that CMake adds that aren't actually libraries
def copyDependencyLibraries(libs, target, strip, log)

  libs.each do |lib|

    # Skip empty stuff
    if not lib or lib.empty? or lib == "optimized" or lib == "debug" or lib =~ /-l.*/
      next
    end

    # Skip duplicates
    if HandledLibraries.include? lib
      next
    end

    onError "Dependency library file #{lib} doesn't exist" if not File.exists? lib

    copyPossibleSymlink(lib, target, strip, log)
    HandledLibraries.push lib
    
  end
end

# Finds library matching regex and returns that folder
def findLibraryFolder(libs, regex)

  libs.each do |lib|

    if lib =~ regex

      return File.dirname lib
      
    end
  end

  "didn't find library matching regex"
end

# Finds CEGUI plugins and adds them to the list
def findCEGUIPlugins(libs, ceguiversion)

  ceguiDir = findLibraryFolder(libs, /.*CEGUIBase.*/i)

  ceguiDir = File.join(ceguiDir, "cegui-#{ceguiversion}.0")
  
  if not File.exists? ceguiDir
    onError "Failed to find CEGUI root directory for dynamic libs (#{ceguiDir})"
  end

  info "Looking for CEGUI plugins in #{ceguiDir}"

  Dir.chdir(ceguiDir) do

    Dir["*.so"].each do |ceguilib|

      libs.push File.absolute_path(ceguilib)
      
    end
    
  end
  
end

# Finds Ogre plugins and adds them to the list
# Plugin names is an array containing names like 'RenderSystem_GL' and 'Plugin_ParticleFX'
def findOgrePlugins(libs, pluginnames)

  ogreDir = File.join(findLibraryFolder(libs, /.*OgreMain.*/i), "OGRE")

  if not File.exists? ogreDir
    onError "Failed to find Ogre root directory for plugins (#{ogreDir})"
  end

  pluginnames.each do |lib|

    # This is a symbolic link but the dependency copy function should figure it out
    file = File.absolute_path(File.join(ogreDir, lib + ".so"))

    onError "Ogre library #{file} doesn't exist" if not File.exists? file
    
    libs.push file
    
  end
  
end

# Boost thread library is a ld script. This finds the actual libraries
def findRealBoostThread(libs)

  boostDir = findLibraryFolder(libs, /.*boost_thread.*/i)

  if not File.exists? boostDir
    onError "Failed to find boost_thread directory for getting actual libraries (#{boostDir})"
  end
  
  # Copy the actual files
  Dir.chdir(boostDir) do

    Dir["libboost_thread.*"].each do |lib|

      libs.push File.absolute_path(lib)
      
    end
    
  end
end

# Uses ldd on a file to find dependency libraries
def lddFindLibraries(binary)

  result = []

  libs = `ldd "#{binary}"`

  libs.each_line do |line|

    line.strip!

    if line.empty?
      next
    end

    if match = line.match(/\s+=>\s+(.*?\.so[^\s]*)/i)
      
      lib = match.captures[0]

      # Skip non-existing filles
      if not File.exist? lib or not Pathname.new(lib).absolute?
        next
      end

      if not isGoodLDDFound lib
        next
      end

      # And finally skip ones that are in the staging or build directory
      if not isInSubdirectory(CurrentDir, lib)

        puts "ldd found library: " + lib 

        result.push lib
        
      end
    end
  end

  result
end


#
# Dependency base class
#
### Download settings ###
# Standard args that are handled:
# :version => standard git version select
# :installPath => path where to install the built files (if this dep uses install)
# :noInstallSudo => don't use sudo when installing (on windows administrator might be
#     used instead if this isn't specified)
# :options => override project configure options (extraOptions should be used instead)
# :extraOptions => extra things to add to :options, see the individual dependencies
#     as to what specific options they support
# :preCreateInstallFolder => If installPath is specified will create
#     the folder if it doesn't exist
class BaseDep
  attr_reader :Name, :Folder
  
  def initialize(name, foldername, args)

    @Name = name
    
    @Folder = File.join(CurrentDir, foldername)
    @FolderName = foldername

    # Standard args handling
    if args[:options]

      @Options = args[:options]
      puts "#{@Name}: using options: #{@Options}"
    else

      @Options = self.getDefaultOptions
      
    end

    raise AssertionError if !@Options.kind_of?(Array)

    if args[:extraOptions]
      @Options += args[:extraOptions]
      puts "#{@Name}: using extra options: #{args[:extraOptions]}"
    end

    if args[:version]
      @Version = args[:version]
      puts "#{@Name}: using version: #{@Version}"
    end

    if args[:installPath]
      @InstallPath = args[:installPath]
      puts "#{@Name}: using install prefix: #{@InstallPath}"
    end

    if args[:noInstallSudo]
      @InstallSudo = false
      puts "#{@Name}: installing without sudo"
    else
      @InstallSudo = true
    end

    if args[:preCreateInstallFolder] && @InstallPath

      puts "#{@Name}: precreating install folder: #{@InstallPath}"
      FileUtils.mkdir_p @InstallPath
    end
  end

  def RequiresClone
    not File.exist?(@Folder)
  end
  
  def Retrieve
    info "Retrieving #{@Name}"

    Dir.chdir(CurrentDir) do
      
      if self.RequiresClone
        
        info "Cloning #{@Name} into #{@Folder}"

        if not self.DoClone
          onError "Failed to clone repository"
        end

      end

      if not File.exist?(@Folder)
        onError "Retrieve Didn't create a folder for #{@Name} at #{@Folder}"
      end

      if not self.Update
        # Not fatal
        warning "Failed to update dependency #{@Name}"
      end

    end
    
    success "Successfully retrieved #{@Name}"
  end

  def Update
    Dir.chdir(@Folder) do
      self.DoUpdate
    end
  end

  def Setup
    info "Setting up build files for #{@Name}"
    Dir.chdir(@Folder) do
      if not self.DoSetup
        onError "Setup failed for #{@Name}. Is a dependency missing? or some other cmake error?"
      end
    end
    success "Successfully created project files for #{@Name}"
  end
  
  def Compile
    info "Compiling #{@Name}"
    Dir.chdir(@Folder) do
      if not self.DoCompile
        onError "#{@Name} Failed to Compile. Are you using a broken version? or has the setup process"+
                " changed between versions"
      end
    end
    success "Successfully compiled #{@Name}"
  end

  def Install
    info "Installing #{@Name}"
    Dir.chdir(@Folder) do
      if not self.DoInstall
        onError "#{@Name} Failed to install. Did you type in your sudo password?"
      end
    end
    success "Successfully installed #{@Name}"
  end

  #
  # Helpers for subclasses
  #
  def linuxMakeInstallHelper
    
    if shouldUseSudo(@InstallSudo)

      askRunSudo "sudo make install"
      
    else

      if @InstallSudo
        warning "Dependency '#{@name}' should have been installed with sudo"
      end
      
      system "make install"
    end

    $?.exitstatus == 0
  end

  def standardGitUpdate

    system "git fetch"
    
    system "git checkout #{@Version}"
    
    gitPullIfOnBranch @Version
    
    $?.exitstatus == 0    
  end
end






#
#### Library Install Definitions ###
# These are all the libraries that this script can install
#

# Supported extra options:
# :enablePIC => enable -fPIC when building
# :buildShared => only shared versions of the libraries are built
# :enableSmall => optimize for size
# :noStrip => disable stripping
class FFMPEG < BaseDep
  def initialize(args)

    super("FFmpeg", "ffmpeg", args)

    if @InstallPath

      @Options.push "--prefix='" + if OS.windows? then 
        makeWindowsPathIntoMinGWPath @InstallPath else @InstallPath end + "'"
      
    end

    if args[:enablePIC]
      @Options.push "--enable-pic"
      @Options.push "--extra-ldexeflags=-pie"
    end

    if args[:buildShared]

      @Options.push "--disable-static"
      @Options.push "--enable-shared"
      
    end

    if args[:enableSmall]
      @Options.push "--enable-small"
    end

    if args[:noStrip]
      @Options.push "--disable-stripping"
    end
    
    if OS.windows?
      @Options.push "--toolchain=msvc"
      @YasmFolder = File.expand_path(File.join @Folder, "../", "ffmpeg-win-tools")
      puts "#{@Name} using msvc toolchain"
      # This may or may not be required as the used compiler is chosen manually by
      # the user by running 'vcvarsall.bat amd64'
      @Options.push "--arch=x86_64"
      puts "#{@Name} using 64-bit build"
    end
    
  end

  def getDefaultOptions
    [
      "--disable-doc",
      # This is useful to comment when testing which components need to
      # be compiled
      "--disable-programs"
    ]
  end

  def depsList
    os = getLinuxOS

    if os == "fedora" || os == "centos" || os == "rhel"
      
      return [
        "autoconf", "automake", "bzip2", "cmake", "freetype-devel", "gcc", "gcc-c++",
        "git", "libtool", "make", "mercurial", "nasm", "pkgconfig", "zlib-devel", "yasm"
      ]
      
    end

    if os == "ubuntu"
      
      return [
        "autoconf", "automake", "build-essential",
        "libass-dev", "libfreetype6-dev", "libsdl2-dev", "libtheora-dev",
        "libtool", "libva-dev", "libvdpau-dev", "libvorbis-dev", "libxcb1-dev",
        "libxcb-shm0-dev", "libxcb-xfixes0-dev", "pkg-config", "texinfo",
        "zlib1g-dev"
      ]
    end
    
    onError "#{@name} unknown packages for os: #{os}"

  end

  def installPrerequisites

    installDepsList depsList
    
  end

  def DoClone
    system "git clone https://github.com/FFmpeg/FFmpeg.git ffmpeg"
    $?.exitstatus == 0
  end

  def DoUpdate
    self.standardGitUpdate
  end

  def DoSetup

    if OS.windows?
      
      someFFMPEGMakeFile = "common.mak"
      
      # Check line endings
      if getFileLineEndings(someFFMPEGMakeFile) != "\n"
        warning "ffmpeg makefiles have non-unix line endings. Fixing."
        
        system "git config core.autocrlf off"
        
        puts "Deleting files and re-checking them out"
        `git ls-files`.strip.lines.each{|f|
        
          f = f.chomp.strip
          
          if f
            FileUtils.rm f
          end
        }
        system "git checkout ."
        
        onError("Line endings fix failed. See the troubleshooting in the build guide.") if
          getFileLineEndings(someFFMPEGMakeFile) != "\n"
              
        success "Fixed. If you get 'missing separator' errors see the troubleshooting " +
          "section in the help files"        
      end

    
      # YASM assembler is required, so download that
      yasmExecutable = File.join @YasmFolder, "yasm.exe"
      
      if !File.exists? yasmExecutable
        # We need to download it
        FileUtils.mkdir_p @YasmFolder
        
        downloadURLIfTargetIsMissing(
          "http://www.tortall.net/projects/yasm/releases/yasm-1.3.0-win64.exe",
          yasmExecutable)
        
        onError "yasm tool dl failed" if !File.exists? yasmExecutable
        
      end
      
      #{runVSVarsAll}
      exit 3
      
      puts ""
      puts ""
      error "Windows ffmpeg manual setup section begin."
      puts "Please open #{VSVersion} and then press any key to continue"
      puts
      puts "If for some reason these instructions don't work, see: " +
        "https://trac.ffmpeg.org/wiki/CompilationGuide/MSVC for help"
      
      system "pause"
      
      puts ""
      error "Please do these manual steps and press a key in this window once done:"
      puts ""
      puts "1. In Visual Studio select from the menu bar 'Tools -> Visual Studio Command Prompt'"
      # https://msdn.microsoft.com/en-us/library/x4d2c09s.aspx
      # (How to: Enable a 64-Bit Visual C++ Toolset on the Command Line)
      # that link was very helpful here
      puts "2. Now run these commands in the new cmd window to enable the 64 bit compiler, " +
        "(hint: select text and right click to copy and paste between cmd windows):"
      
      puts ""
      puts "cd \"#{File.join getVSBaseFolder, "VC"}\""
      puts "vcvarsall.bat amd64"
      puts ""
      
      puts "3. Open mingw environment by running 'msys.bat'"
      
      puts "at this point the command prompt should have a '$' character instead of the usual current folder"
      puts "and the title bar should be MINGW32"
      
      puts ""
      puts "4. Run these command in order"
      
      puts ""
      
      # YASM assembler needs to be in path for ffmpeg to be able to use it
      # also visual studio needs to be in path before the mingw linker.exe
      puts "export PATH=\"#{makeWindowsPathIntoMinGWPath @YasmFolder}\":$PATH"
      puts ""
      # Apparently this visual studio path thing doesn't need to be done
      puts "  If you experience problems, THEN try this extra command:"
      puts "  export PATH=\"#{makeWindowsPathIntoMinGWPath getVSLinkerFolder}\":$PATH"
      
      puts ""
      puts "cd \"#{makeWindowsPathIntoMinGWPath @Folder}\""
      puts ""
      
      puts "5. Now run these setup and compile commands"
      
      puts "./configure #{@Options.join(' ')}"
      puts ""
      puts "make"
      puts "make install"
      
      puts ""
      puts "Now ffmpeg should be built and copied to the correct folder"

      puts ""
      puts ""
      error "FOLLOW THE INSTRUCTIONS ABOVE BEFORE CONTINUING"
      system "pause"
      return true

    else

      system "./configure #{@Options.join(' ')}"
      return $?.exitstatus == 0
    end
  end
  
  def DoCompile
    if OS.windows?
    
      if !File.exists? File.join(@Folder, "libavcodec/avcodec.lib")
        
        onError "manual ffmpeg not detected to have succeeded"
        return false
      end

      true
    else

      runCompiler CompileThreads
      return $?.exitstatus == 0
    end
  end

  def DoInstall

    if OS.windows?
    
      if Dir.glob(File.join(@InstallPath, "lib/avcodec*.def")).empty? 
        
        onError "manual ffmpeg install not detected to have succeeded"
        return false
      end
    
      # Copy the .lib files to allow linking properly
      Dir.glob("#{File.join(@InstallPath, 'lib')}/**/*.def") {|file|
        
        puts "Copying .lib file matching: '#{file}'"
        
        found = Dir.glob("#{@Folder}/**/" + 
          File.basename(file, File.extname(file)).split('-')[0] + ".lib")
        
        if found.empty?
          onError "Didn't find lib file for: #{file}. " +
            "Did you follow the build instructions?"
        end
        
        FileUtils.cp found.first, File.dirname(file)
        
        puts "Copied  '#{found.first}' to '#{File.dirname file}'"
      }
      
      puts ""
      
      onError("Copying libs failed") if !File.exists?(File.join(
        @InstallPath, "lib", "avcodec.lib"))
      
      true 
    else

      return self.linuxMakeInstallHelper
    end
  end
end



# Supported extra options:
# :noOSS => Disable OSS support on linux (which is not supported well
#     according to the documentation
class PortAudio < BaseDep
  def initialize(args)

    super("PortAudio", "portaudio", args)

    if @InstallPath
      if usesCMake
        @Options.push "-DCMAKE_INSTALL_PREFIX=\"#{@InstallPath}\""
      else
        @Options.push "--prefix='#{@InstallPath}'"
      end  
    end

    if args[:noOSS]
      if usesCMake
        # Doesn't make sense on windows, and the cmake doesn't seem
        # to have 'OSS' in it so probably doesn't need to be disabled
        # even if we switch linux to also use cmake
      else
        @Options.push "--without-oss"
      end
    end
    
  end
  
  # Returns true if platform uses cmake to configure, and cmake style args
  # should be used
  def usesCMake
    OS.windows?
  end

  
  def getDefaultOptions
    if !usesCMake
      []
    else
      [
        "-DPA_DLL_LINK_WITH_STATIC_RUNTIME=OFF",
      ]
    end

  end

  def depsList
    os = getLinuxOS

    if os == "fedora" || os == "centos" || os == "rhel"
      
      return [
        "alsa-lib-devel"
      ]
      
    end

    if os == "ubuntu"
      
      return [
        "libasound-dev"
      ]
    end
    
    onError "#{@Name} unknown packages for os: #{os}"

  end

  def installPrerequisites

    installDepsList depsList
    
  end

  def DoClone
    system "git clone https://git.assembla.com/portaudio.git portaudio"
    $?.exitstatus == 0
  end

  def DoUpdate
    self.standardGitUpdate
  end

  def DoSetup

    if OS.windows?
        
      FileUtils.mkdir_p "build"
      
      Dir.chdir("build") do
      
        runCMakeConfigure @Options.join(' ')
        return $?.exitstatus == 0
      end
    else

      system "./configure #{@Options.join(' ')}"
      return $?.exitstatus == 0
    end
  end
  
  def DoCompile
    if OS.windows?
    
      Dir.chdir("build") do
        
        # If we ran cmake again here with 32 bit windows (probably in a 'build-32' folder)
        # we could probably also build the 32-bit version as it has a different file name        
        runVSCompiler CompileThreads, "portaudio.vcxproj", "Release", "x64"
        
        return $?.exitstatus == 0
      end
    else

      runCompiler CompileThreads
      return $?.exitstatus == 0
    end
  end

  def DoInstall

    if OS.windows?
      
      buildFolder = File.join(@Folder, "build/Release/")
      
      if Dir.glob(File.join(buildFolder, "*.dll")).empty? 
        onError "portaudio files to be installed are missing"
      end
      
      binFolder = File.join @InstallPath, "bin"
      libFolder = File.join @InstallPath, "lib"

      FileUtils.mkdir_p binFolder
      FileUtils.mkdir_p libFolder
      
      Dir.glob(File.join(buildFolder, "*.dll")) {|file|
        FileUtils.cp file, binFolder
      }
      
      Dir.glob(File.join(buildFolder, "*.lib")) {|file|
        FileUtils.cp file, libFolder
      }
      
      FileUtils.cp_r File.join(@Folder, "include"), @InstallPath
      
      true
      
    else
    
      return self.linuxMakeInstallHelper
    end
  end
end
  
#
##
### TODO: all the following need to be fixed / verified that they still work
##
#

class Newton < BaseDep
  def initialize
    super("Newton Dynamics", "newton-dynamics")
  end

  def DoClone
    system "git clone https://github.com/MADEAPPS/newton-dynamics.git"
    $?.exitstatus == 0
  end

  def DoUpdate
    system "git checkout master"
    system "git pull origin master"
    $?.exitstatus == 0
  end

  def DoSetup
    
    if OS.windows?
      
      return File.exist? "packages/projects/visualStudio_2015_dll/build.sln"
    else
      FileUtils.mkdir_p "build"

      Dir.chdir("build") do
        
        runCMakeConfigure "-DNEWTON_DEMOS_SANDBOX=OFF"
        return $?.exitstatus == 0
      end
    end      
  end
  
  def DoCompile
    if OS.windows?
      cmdStr = "#{bringVSToPath} && MSBuild.exe \"packages/projects/visualStudio_2015_dll/build.sln\" " +
               "/maxcpucount:#{CompileThreads} /p:Configuration=release /p:Platform=\"x64\""
      system cmdStr
      return $?.exitstatus == 0
    else
      Dir.chdir("build") do
        
        runCompiler CompileThreads
        
      end
      return $?.exitstatus == 0
    end
  end
  
  def DoInstall
    
    # Copy files to ProjectDir dependencies folder
    createDependencyTargetFolder

    runGlobberAndCopy(Globber.new("Newton.h", File.join(@Folder, "coreLibrary_300/source")),
                          ProjectDebDirInclude)
    
    if OS.linux?

      runGlobberAndCopy(Globber.new("libNewton.so", File.join(@Folder, "build/lib")),
                            ProjectDebDirLibs)

    elsif OS.windows?

      runGlobberAndCopy(Globber.new("newton.dll",
                                    File.join(@Folder, "coreLibrary_300/projects/windows")),
                            ProjectDebDirBinaries)

      runGlobberAndCopy(Globber.new("newton.lib",
                                    File.join(@Folder, "coreLibrary_300/projects/windows")),
                            ProjectDebDirLibs)
    else
      onError "Unkown os"
    end
    true
  end
end

class OpenAL < BaseDep
  def initialize
    super("OpenAL Soft", "openal-soft")
    onError "Use OpenAL from package manager on linux" if !OS.windows?
  end

  def DoClone
    system "git clone https://github.com/kcat/openal-soft.git"
    $?.exitstatus == 0
  end

  def DoUpdate
    system "git checkout master"
    system "git pull origin master"
    $?.exitstatus == 0
  end

  def DoSetup
    FileUtils.mkdir_p "build"

    Dir.chdir("build") do
      
      runCMakeConfigure "-DALSOFT_UTILS=OFF -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF"
    end
    
    $?.exitstatus == 0
  end
  
  def DoCompile

    Dir.chdir("build") do
      runCompiler CompileThreads
    end
    $?.exitstatus == 0
  end
  
  def DoInstall
    return false if not DoSudoInstalls
    
    Dir.chdir("build") do
      runInstall
      
      if OS.windows? and not File.exist? "C:/Program Files/OpenAL/include/OpenAL"
        # cAudio needs OpenAL folder in include folder, which doesn't exist. 
        # So we create it here
        askToRunAdmin("mklink /D \"C:/Program Files/OpenAL/include/OpenAL\" " + 
                      "\"C:/Program Files/OpenAL/include/AL\"")
      end
    end
    $?.exitstatus == 0
  end
end

class CAudio < BaseDep
  def initialize
    super("cAudio", "cAudio")
  end

  def DoClone
    #system "git clone https://github.com/R4stl1n/cAudio.git"
    # Official repo is broken
    system "git clone https://github.com/hhyyrylainen/cAudio.git"
    $?.exitstatus == 0
  end

  def DoUpdate
    system "git checkout master"
    system "git pull origin master"
    $?.exitstatus == 0
  end

  def DoSetup
    FileUtils.mkdir_p "build"

    Dir.chdir("build") do
      
      if OS.windows?
        # The bundled ones aren't compatible with our compiler setup 
        # -DCAUDIO_DEPENDENCIES_DIR=../Dependencies64
        runCMakeConfigure "-DCAUDIO_BUILD_SAMPLES=OFF -DCAUDIO_DEPENDENCIES_DIR=\"C:/Program Files/OpenAL\" " +
                          "-DCMAKE_INSTALL_PREFIX=./Install"
      else
        runCMakeConfigure "-DCAUDIO_BUILD_SAMPLES=OFF"
      end
    end
    
    $?.exitstatus == 0
  end
  
  def DoCompile

    Dir.chdir("build") do
      runCompiler CompileThreads
    end
    $?.exitstatus == 0
  end
  
  def DoInstall
    
    Dir.chdir("build") do
      if OS.windows?
        
        system "#{bringVSToPath} && MSBuild.exe INSTALL.vcxproj /p:Configuration=RelWithDebInfo"
        
        # And then to copy the libs
        
        FileUtils.mkdir_p File.join(CurrentDir, "cAudio")
        FileUtils.mkdir_p File.join(CurrentDir, "cAudio", "lib")
        FileUtils.mkdir_p File.join(CurrentDir, "cAudio", "bin")
        
        FileUtils.cp File.join(@Folder, "build/bin/RelWithDebInfo", "cAudio.dll"),
                     File.join(CurrentDir, "cAudio", "bin")

        FileUtils.cp File.join(@Folder, "build/lib/RelWithDebInfo", "cAudio.lib"),
                     File.join(CurrentDir, "cAudio", "lib")
        
        FileUtils.copy_entry File.join(@Folder, "build/Install/", "include"),
                             File.join(CurrentDir, "cAudio", "include")
        
      else
        return true if not DoSudoInstalls
        runInstall
      end
    end
    $?.exitstatus == 0
  end
end

class AngelScript < BaseDep
  def initialize
    super("AngelScript", "angelscript")
    @WantedURL = "http://svn.code.sf.net/p/angelscript/code/tags/2.31.2"

    if @WantedURL[-1, 1] == '/'
      onError "Invalid configuraion in Setup.rb AngelScript tag has an ending '/'. Remove it!"
    end
  end

  def DoClone
    system "svn co #{@WantedURL} angelscript"
    $?.exitstatus == 0
  end

  def DoUpdate

    # Check is tag correct
    match = `svn info`.strip.match(/.*URL:\s?(.*angelscript\S+).*/i)

    onError("'svn info' unable to find URL with regex") if !match
    
    currenturl = match.captures[0]

    if currenturl != @WantedURL
      
      info "Switching AngelScript tag from #{currenturl} to #{@WantedURL}"
      
      system "svn switch #{@WantedURL}"
      onError "Failed to switch svn url" if $?.exitstatus > 0
    end
    
    system "svn update"
    $?.exitstatus == 0
  end

  def DoSetup
    if OS.windows?
      
      return File.exist? "sdk/angelscript/projects/msvc2015/angelscript.sln"
    else
      return true
    end
  end
  
  def DoCompile

    if OS.windows?
      info "Verifying that angelscript solution has Runtime Library = MultiThreadedDLL"
      verifyVSProjectRuntimeLibrary "sdk/angelscript/projects/msvc2015/angelscript.vcxproj", 
                                    %r{Release\|x64}, "MultiThreadedDLL"  
      
      success "AngelScript solution is correctly configured. Compiling"
      
      cmdStr = "#{bringVSToPath} && MSBuild.exe \"sdk/angelscript/projects/msvc2015/angelscript.sln\" " +
               "/maxcpucount:#{CompileThreads} /p:Configuration=Release /p:Platform=\"x64\""
      system cmdStr
      $?.exitstatus == 0
      
    else
    
      Dir.chdir("sdk/angelscript/projects/gnuc") do
        
        system "make -j #{CompileThreads}"
        return $?.exitstatus == 0
      end
    end
  end
  
  def DoInstall

    # Copy files to Project folder
    createDependencyTargetFolder

    # First header files and addons
    FileUtils.cp File.join(@Folder, "sdk/angelscript/include", "angelscript.h"),
                 ProjectDebDirInclude

    addondir = File.join(ProjectDebDirInclude, "add_on")

    FileUtils.mkdir_p addondir

    # All the addons from
    # `ls -m | awk 'BEGIN { RS = ","; ORS = ", "}; NF { print "\""$1"\""};'`
    addonnames = Array[
      "autowrapper", "contextmgr", "datetime", "debugger", "scriptany", "scriptarray",
      "scriptbuilder", "scriptdictionary", "scriptfile", "scriptgrid", "scripthandle",
      "scripthelper", "scriptmath", "scriptstdstring", "serializer", "weakref"
    ]

    addonnames.each do |x|

      FileUtils.copy_entry File.join(@Folder, "sdk/add_on/", x),
                           File.join(addondir, x)
    end

    # Then the library
    if OS.linux?

      FileUtils.cp File.join(@Folder, "sdk/angelscript/lib", "libangelscript.a"),
                   ProjectDebDirLibs
      
    elsif OS.windows?
      FileUtils.cp File.join(@Folder, "sdk/angelscript/lib", "angelscript64.lib"),
                   ProjectDebDirLibs
    else
      onError "Unkown OS"
    end
    true
  end
end

class Breakpad < BaseDep
  def initialize
    super("Google Breakpad", "breakpad")
    @DepotFolder = File.join(CurrentDir, "depot_tools")
    @CreatedNewFolder = false
  end

  def RequiresClone
    if File.exist?(@DepotFolder) and File.exist?(@Folder)
      return false
    end
    
    true
  end
  
  def DoClone

    # Depot tools
    system "git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git"
    return false if $?.exitstatus > 0

    if not File.exist?(@Folder)
      
      FileUtils.mkdir_p @Folder
      @CreatedNewFolder = true
      
    end
    
    true
  end

  def DoUpdate
    
    if OS.windows? and NoBreakpadUpdateOnWindows
      info "Windows: skipping Breakpad update"
      if not File.exist?("src")
        @CreatedNewFolder = true
      end
      return true
    end

    # Update depot tools
    Dir.chdir(@DepotFolder) do
      system "git checkout master"
      system "git pull origin master"
    end

    if $?.exitstatus > 0
      return false
    end

    if not @CreatedNewFolder
      
      if not File.exist?("src")
        # This is set to true if we created an empty folder but we didn't get to the pull stage
        @CreatedNewFolder = true
      else
        Dir.chdir(@Folder) do
          # The first source subdir is the git repository
          Dir.chdir("src") do
            system "git checkout master"
            system "git pull origin master"
            system "gclient sync"
          end
        end
      end
    end
    
    true
  end

  def DoSetup
    
    if not @CreatedNewFolder
      return true
    end
    
    # Bring the depot tools to path
    pathedit = PathModifier.new(@DepotFolder)

    # Get source for breakpad
    Dir.chdir(@Folder) do

      system "fetch breakpad"

      if $?.exitstatus > 0
        pathedit.Restore
        onError "fetch breakpad failed"
      end
      
      Dir.chdir("src") do

        # Configure script
        if OS.windows?
          system "src/tools/gyp/gyp.bat src/client/windows/breakpad_client.gyp –no-circular-check"
        else
          system "./configure"
        end
        
        if $?.exitstatus > 0
          pathedit.Restore
          onError "configure breakpad failed" 
        end
      end
    end

    pathedit.Restore
    true
  end
  
  def DoCompile

    # Bring the depot tools to path
    pathedit = PathModifier.new(@DepotFolder)

    # Build breakpad
    Dir.chdir(File.join(@Folder, "src")) do
      
      if OS.windows?
        info "Please open the solution at and compile breakpad client in Release and x64. " +
             "Remember to disable treat warnings as errors first: "+
             "#{CurrentDir}/breakpad/src/src/client/windows/breakpad_client.sln"
        
        system "start #{CurrentDir}/breakpad/src/src/client/windows/breakpad_client.sln" if AutoOpenVS
        system "pause"
      else
        system "make -j #{CompileThreads}"
        
        if $?.exitstatus > 0
          pathedit.Restore
          onError "breakpad build failed" 
        end
      end
    end
    
    pathedit.Restore
    true
  end
  
  def DoInstall

    # Create target folders
    FileUtils.mkdir_p File.join(CurrentDir, "Breakpad", "lib")
    FileUtils.mkdir_p File.join(CurrentDir, "Breakpad", "bin")

    breakpadincludelink = File.join(CurrentDir, "Breakpad", "include")
    
    if OS.windows?

      askToRunAdmin "mklink /D \"#{breakpadincludelink}\" \"#{File.join(@Folder, "src/src")}\""
      
      FileUtils.copy_entry File.join(@Folder, "src/src/client/windows/Release/lib"),
                           File.join(CurrentDir, "Breakpad", "lib")
      
      
      
    # Might be worth it to have windows symbols dumbed on windows, if the linux dumber can't deal with pdbs
    #FileUtils.cp File.join(@Folder, "src/src/tools/linux/dump_syms", "dump_syms"),
    #             File.join(CurrentDir, "Breakpad", "bin")
      
    else
      
      # Need to delete old file before creating a new symlink
      File.delete(breakpadincludelink) if File.exist?(breakpadincludelink)
      FileUtils.ln_s File.join(@Folder, "src/src"), breakpadincludelink
      
      FileUtils.cp File.join(@Folder, "src/src/client/linux", "libbreakpad_client.a"),
                   File.join(CurrentDir, "Breakpad", "lib")

      FileUtils.cp File.join(@Folder, "src/src/tools/linux/dump_syms", "dump_syms"),
                   File.join(CurrentDir, "Breakpad", "bin")

      FileUtils.cp File.join(@Folder, "src/src/processor", "minidump_stackwalk"),
                   File.join(CurrentDir, "Breakpad", "bin")
    end
    true
  end
end

class Ogre < BaseDep
  def initialize
    super("Ogre", "ogre")
  end

  def RequiresClone
    if OS.windows?
      return (not File.exist?(@Folder) or not File.exist?(File.join(@Folder, "Dependencies")))
    else
      return (not File.exist? @Folder)
    end
  end
  
  def DoClone
    if OS.windows?

      system "hg clone https://bitbucket.org/sinbad/ogre"
      if $?.exitstatus > 0
        return false
      end
      
      Dir.chdir(@Folder) do

        system "hg clone https://bitbucket.org/cabalistic/ogredeps Dependencies"
      end
      return $?.exitstatus == 0
    else
      system "hg clone https://bitbucket.org/sinbad/ogre"
      return $?.exitstatus == 0
    end
  end

  def DoUpdate
    
    if OS.windows?
      Dir.chdir("Dependencies") do
        system "hg pull"
        system "hg update"
        
        if $?.exitstatus > 0
          return false
        end
      end
    end
    
    system "hg pull"
    system "hg update v2-0"
    $?.exitstatus == 0
  end

  def DoSetup
    
    # Dependencies compile
    additionalCMake = ""
    
    if OS.windows?
      Dir.chdir("Dependencies") do
        
        system "cmake . -DOGREDEPS_BUILD_SDL2=OFF" 
        
        system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{CompileThreads} /p:Configuration=Debug"
        onError "Failed to compile Ogre dependencies " if $?.exitstatus > 0
        
        runCompiler CompileThreads
        onError "Failed to compile Ogre dependencies " if $?.exitstatus > 0

        info "Please open the solution SDL2 in Release and x64: "+
             "#{@Folder}/Dependencies/src/SDL2/VisualC/SDL_VS2013.sln"
        
        system "start #{@Folder}/Dependencies/src/SDL2/VisualC/SDL_VS2013.sln" if AutoOpenVS
        system "pause"
        
        additionalCMake = "-DSDL2MAIN_LIBRARY=..\SDL2\VisualC\Win32\Debug\SDL2main.lib " +
                          "-DSD2_INCLUDE_DIR=..\SDL2\include"
        "-DSDL2_LIBRARY_TEMP=..\SDL2\VisualC\Win32\Debug\SDL2.lib"
        
      end
    end
    
    FileUtils.mkdir_p "build"
    
    Dir.chdir("build") do

      runCMakeConfigure "-DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON " +
                        "-DOGRE_BUILD_RENDERSYSTEM_D3D9=OFF -DOGRE_BUILD_RENDERSYSTEM_D3D11=OFF "+
                        "-DOGRE_BUILD_COMPONENT_OVERLAY=OFF " +
                        "-DOGRE_BUILD_COMPONENT_PAGING=OFF -DOGRE_BUILD_COMPONENT_PROPERTY=OFF " +
                        "-DOGRE_BUILD_COMPONENT_TERRAIN=OFF -DOGRE_BUILD_COMPONENT_VOLUME=OFF "+
                        "-DOGRE_BUILD_PLUGIN_BSP=OFF -DOGRE_BUILD_PLUGIN_CG=OFF " +
                        "-DOGRE_BUILD_PLUGIN_OCTREE=OFF -DOGRE_BUILD_PLUGIN_PCZ=OFF -DOGRE_BUILD_SAMPLES=OFF " + 
                        additionalCMake
    end
    
    $?.exitstatus == 0
  end
  
  def DoCompile
    Dir.chdir("build") do
      if OS.windows?
        system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{CompileThreads} /p:Configuration=Release"
        system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{CompileThreads} /p:Configuration=RelWithDebInfo"
      else
        runCompiler CompileThreads
      end
    end
    
    $?.exitstatus == 0
  end
  
  def DoInstall

    Dir.chdir("build") do
      
      if OS.windows?

        system "#{bringVSToPath} && MSBuild.exe INSTALL.vcxproj /p:Configuration=RelWithDebInfo"
        ENV["OGRE_HOME"] = "#{@Folder}/build/ogre/sdk"
        
      else
        return true if not DoSudoInstalls
        runInstall
      end
    end

    $?.exitstatus == 0
  end
end

# Windows only CEGUI dependencies
class CEGUIDependencies < BaseDep
  def initialize
    super("CEGUI Dependencies", "cegui-dependencies")
  end

  def DoClone

    system "hg clone https://bitbucket.org/cegui/cegui-dependencies"
    $?.exitstatus == 0
  end

  def DoUpdate
    system "hg pull"
    system "hg update default"
    $?.exitstatus == 0
  end

  def DoSetup

    FileUtils.mkdir_p "build"

    if InstallCEED
      python = "ON"
    else
      python = "OFF"
    end

    Dir.chdir("build") do
      runCMakeConfigure "-DCEGUI_BUILD_PYTHON_MODULES=#{python} "
    end
    
    $?.exitstatus == 0
  end
  
  def DoCompile

    Dir.chdir("build") do
      system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{CompileThreads} /p:Configuration=Debug"
      system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{CompileThreads} /p:Configuration=RelWithDebInfo"
    end
    $?.exitstatus == 0
  end
  
  def DoInstall

    FileUtils.copy_entry File.join(@Folder, "build", "dependencies"),
                         File.join(CurrentDir, "cegui", "dependencies")
    $?.exitstatus == 0
  end
end

# Depends on Ogre to be installed
class CEGUI < BaseDep
  def initialize
    super("CEGUI", "cegui")
  end

  def DoClone

    system "hg clone https://bitbucket.org/cegui/cegui"
    $?.exitstatus == 0
  end

  def DoUpdate
    system "hg pull"
    #system "hg update default"

    # TODO: allow configuring this commit
    system "hg update 6510156"
    
    $?.exitstatus == 0
  end

  def DoSetup

    FileUtils.mkdir_p "build"

    if InstallCEED
      python = "ON"
    else
      python = "OFF"
    end

    Dir.chdir("build") do
      # Use UTF-8 strings with CEGUI (string class 1)
      runCMakeConfigure "-DCEGUI_STRING_CLASS=1 " +
                        "-DCEGUI_BUILD_APPLICATION_TEMPLATES=OFF -DCEGUI_BUILD_PYTHON_MODULES=#{python} " +
                        "-DCEGUI_SAMPLES_ENABLED=OFF -DCEGUI_BUILD_RENDERER_DIRECT3D11=OFF -DCEGUI_BUILD_RENDERER_OGRE=ON " +
                        "-DCEGUI_BUILD_RENDERER_OPENGL=OFF -DCEGUI_BUILD_RENDERER_OPENGL3=OFF"
    end
    
    $?.exitstatus == 0
  end
  
  def DoCompile

    Dir.chdir("build") do
      runCompiler CompileThreads 
    end
    $?.exitstatus == 0
  end
  
  def DoInstall

    return true if not DoSudoInstalls or BuildPlatform == "windows"
    
    Dir.chdir("build") do
      runInstall
    end
    $?.exitstatus == 0
  end
end

class SFML < BaseDep
  def initialize
    super("SFML", "SFML")
  end

  def DoClone
    system "git clone https://github.com/SFML/SFML.git"
    $?.exitstatus == 0
  end

  def DoUpdate
    system "git checkout master"
    system "git pull origin master"
    $?.exitstatus == 0
  end

  def DoSetup
    FileUtils.mkdir_p "build"

    Dir.chdir("build") do
      runCMakeConfigure ""
    end
    
    $?.exitstatus == 0
  end
  
  def DoCompile

    Dir.chdir("build") do
      
      if BuildPlatform == "windows"
        system "#{bringVSToPath} && MSBuild.exe ALL_BUILD.vcxproj /maxcpucount:#{CompileThreads} /p:Configuration=Debug"
      end
      
      runCompiler CompileThreads
    end
    $?.exitstatus == 0
  end
  
  def DoInstall

    return true if not DoSudoInstalls or BuildPlatform == "windows"
    
    Dir.chdir("build") do
      runInstall
    end
    $?.exitstatus == 0
  end

  def LinuxPackages
    if getLinuxOS == "Fedora"
      return Array["xcb-util-image-devel", "systemd-devel", "libjpeg-devel", "libvorbis-devel",
                   "flac-devel"]
    else
      onError "LinuxPackages not done for this linux system"
    end
  end
end


### LDD found libraries that should be included in full package
# Used to filter ldd results
def isGoodLDDFound(lib)

  case lib
  when /.*swresample.*/i
    true
  when /.*vorbis.*/i
    true
  when /.*theora.*/i
    true
  when /.*opus.*/i
    true
  when /.*pcre.*/i
    true
  when /.*ogg.*/i
    true
  when /.*tinyxml.*/i
    true
  when /.*avcodec.*/i
    true
  when /.*avformat.*/i
    true
  when /.*avutil.*/i
    true
  when /.*swscale.*/i
    true
  when /.*rtmp.*/i
    true
  when /.*gsm.*/i
    true
  when /.*soxr.*/i
    true
  when /.*vpx.*/i
    true
  when /.*x2.*/i
    true
  when /.*libstdc++.*/i
    true
  when /.*jpeg.*/i
    true
  when /.*jxrglue.*/i
    true
  when /.*IlmImf.*/i
    true
  when /.*Imath.*/i
    true
  when /.*Half.*/i
    true
  when /.*Iex.*/i
    true
  when /.*IlmThread.*/i
    true
  when /.*openjp.*/i
    true
  when /.*libraw.*/i
    true
  when /.*png.*/i
    true
  when /.*freeimage.*/i
    true
  when /.*gnutls.*/i
    true
  when /.*atomic.*/i
    true
  when /.*zzip.*/i
    true
  when /.*Cg.*/i
    true
  when /.*va.*/i
    true
  when /.*xvid.*/i
    true
  when /.*zvbi.*/i
    true
  when /.*amr.*/i
    true
  when /.*mfx.*/i
    true
  when /.*aac.*/i
    true
  # nvidia stuff for ffmpeg
  when /.*nvcu.*/i
    true
  when /.*cuda.*/i
    true
  when /.*nvidia-fatbinary.*/i
    true
  when /.*vdpau.*/i
    true
  when /.*twolame.*/i
    true
  when /.*h26.*/i
    true
  when /.*mp3.*/i
    true
  when /.*bluray.*/i
    true
  when /.*OpenCL.*/i
    true
  when /.*webp.*/i
    true
  when /.*schroedinger.*/i
    true
  when /.*Xaw.*/i
    true
  when /.*numa.*/i
    true
  when /.*hogweed.*/i
    true
  when /.*jasper.*/i
    true
  else
    false
  end
end

