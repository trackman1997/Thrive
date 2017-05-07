#!/usr/bin/env ruby
# coding: utf-8
# Setup script for Thrive. Basically just checks out the extra svn repository
# Add extra argument to set svn username

require 'fileutils'

require_relative 'linux_setup/RubyCommon.rb'


SVNUser = if ARGV.length == 0
            "thrive"
          else
            ARGV[0]
          end

WantedURL = "https://#{SVNUser}@boostslair.com/svn/Thrive_Content"

puts "Using svn user: #{SVNUser}"

info "Running Thrive folder setup"

if not File.exist? "Content"

  info "Content folder doesn't exist, checking it out"
  
  system "svn co --force #{WantedURL} ."
  onError "Failed to clone repository" if $?.exitstatus > 0

  success "Checkout succeeded"
  
end

success "Content folder exists"

# Make sure svn username is correct
match = `svn info`.strip.match(/.*URL:\s?(.*thrive\S+).*/i)

abort("'svn info' unable to find URL with regex") if !match

currenturl = match.captures[0]

if currenturl != WantedURL

  info "SVN url is not the target url with username, #{currenturl} != #{WantedURL}"

  systemChecked "svn relocate #{WantedURL}"

  success "svn URL updated"
  
end

info "svn URL is correct"


# Prints files that SVN should be set to ignore
def printSVNIgnoreList

  ignoreList = []

  Dir.foreach(".") do |fname|

    if !fname.match(/.*Content.*/i) &&
       fname != "." && fname != ".." && fname != ".svn"
       
       ignoreList.push fname
      
    end
  end

  puts "Suggested SVN ignore list:"
  puts ignoreList.join"\n"
  
end

#printSVNIgnoreList

success "Done"

exit 0

