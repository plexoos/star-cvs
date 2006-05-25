#!/bin/sh
# \
exec tclsh "$0" "$@"

# We assume jobMonitor.tcl is in the same directory that runJobMonitor.tcl is.
# Following code is supposed to take care of cases where an alias
# or soft link was used to invoke runJobMonitor. Seems to screw up
# in case were we source file from tkcon.
# If you really want to do that you need to set argv0 to the path
# to runJobMonitor.tcl.
set originalPath [pwd]
set jobMonitorPath   $::argv0
set workingPath  [file dirname $::argv0]
while {![catch {file readlink $jobMonitorPath} result]} {
    cd $workingPath
    set jobMonitorPath  [file join [pwd] $result]
    set workingPath [file dirname $jobMonitorPath]
}
cd [file dirname $jobMonitorPath]
set jobMonitorPath [pwd]
cd $originalPath

source [file join $jobMonitorPath jobMonitor.tcl]



set ::jobMonitor::scriptDir  [pwd]/scripts
set ::jobMonitor::logDir     [pwd]/logs
set ::jobMonitor::pattern    {}
set ::jobMonitor::ignoreCase 1
set ::jobMonitor::tagColor   [list #ffff00 #ffcc00 #ccff00 #ff6600 #ff2200]
set ::jobMonitor::LOGTYPE    .log
set ::jobMonitor::IOwnProcess true

wm withdraw .
::jobMonitor::createWindow $originalPath

bind $::jobMonitor::bWindow <Control-q> ::jobMonitor::exit
