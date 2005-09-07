 #!/bin/sh
 # \
 exec tclsh "$0" "$@"

namespace eval ::jobCreate:: {
}
# We assume jobCreate.tcl is in the same directory that runjobCreate.tcl is.
# Following code is supposed to take care of cases where an alias
# or soft link was used to invoke runJobCreate. Seems to screw up
# in case were we source file from tkcon.
# If you really want to do that you need to set argv0 to the path
# to runjobCreate.tcl.
set originalPath [pwd]
set ::jobCreate::jobCreatePath   $::argv0
set workingPath  [file dirname $::argv0]
while {![catch {file readlink $::jobCreate::jobCreatePath} result]} {
    cd $workingPath
    set ::jobCreate::jobCreatePath  [file join [pwd] $result]
    set workingPath [file dirname $::jobCreate::jobCreatePath]
}
cd [file dirname $::jobCreate::jobCreatePath]
set ::jobCreate::jobCreatePath [pwd]
cd $originalPath

source [file join $::jobCreate::jobCreatePath jobCreate.tcl]
source [file join $::jobCreate::jobCreatePath xsdValidator.tcl]
::jobCreate::getNewSchema [file join $::jobCreate::jobCreatePath batchSubmit.xsd]
bind $::jobCreate::interfaceWindow <Control-q> exit

