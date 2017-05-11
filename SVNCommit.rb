#!/usr/bin/env ruby
# Helper for commiting the Content folder to svn
# without accidentally committing anything from Source

require_relative 'linux_setup/RubyCommon.rb'

if ARGV.length < 1
  
  onError "Commit message required for committing"
  exit 2
  
end


COMMIT_MESSAGE = ARGV.join(" ").gsub(/\"/, "'")

if COMMIT_MESSAGE.empty?

  onError "Commit message is empty"
  exit 2
  
end

# Detect Source stuff about to be committed
SOURCE_CHECK = Regexp.new(/a\s+Source/i).freeze

statusStr = `svn status`.strip

if SOURCE_CHECK =~ statusStr

  puts statusStr
  warning "Was about to commit Source folder contents to svn"

  cmd = "svn rm --keep-local Source"
  
  info "Running '#{cmd}'"

  systemChecked cmd
  
end

# Don't commit if nothing there
actualFilesStatus = `svn status`.strip

commitCount = 0

actualFilesStatus.each_line{|line|

  if line.length > 0 && line[0] != '?'

    #puts "thing to commit: #{line}"
    commitCount += 1
    
  end
}

if commitCount == 0

  warning "Nothing to commit"
  exit 0
  
end

# Detect username
match = `svn info`.strip.match(/.*URL:\s?.+\/\/(.+)@.*thrive\S+.*/i)

if !match
  asUser = "thrivedev"
  warning "Could not detect username, using 'thrivedev'"
else
  asUser = match.captures[0]
end


info "committing #{commitCount} changes as user '#{asUser}' with message: \"#{COMMIT_MESSAGE}\""


system "svn commit --username #{asUser} -m \"#{COMMIT_MESSAGE}\""
if $?.exitstatus > 0
  onError "Committing failed!"
  exit 1
end

success "Committed"
