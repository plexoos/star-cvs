#
#	installStaf script to create RL99.x STAF libraries

set NODEBUG =
if ($1 == "NODEBUG") set NODEBUG = "NODEBUG=Yess"
if ($1 == "nodebug") set NODEBUG = "NODEBUG=Yess"

# define input area where directories asu,sdd,... are
set INP = $cwd/asps/staf
echo INP_DIR = $INP
if ( ! -d $INP) then
  echo *** $INP does not exist ***
  exit
endif

# define output area where result will be placed
set OUT = $cwd
echo OUT_DIR = $OUT

# define where all Make*.mk are. Here the place is the same as input area
if ( ! $?STAR_MAKE_HOME ) then
#  if ( -e $cwd/mgr/MakeArch.mk ) then
#    setenv STAR_MAKE_HOME $cwd/mgr
#  else
    setenv STAR_MAKE_HOME ${STAR}/mgr
#  endif
endif
echo installStaf: STAR_MAKE_HOME = $STAR_MAKE_HOME
#
# setup stage: create OUTPUT directories and logon file makestaflogon.mk
#  info about INP_DIR and OUT_DIR is saved into this file

gmake -f ${STAR_MAKE_HOME}/MakeStaf.mk INP_DIR=${INP} OUT_DIR=${OUT} setup

#
if (-f asps/staf/dsl/star.mk) then
# prepare standalone dsl package for MakeStaf.mk
(cd asps/staf/dsl; gmake -f star.mk prepare)
endif
#real run of makefile. It creates all the libraries ans all executables
# if you want only one ASP , "asu" for instance, add in line PKG=asu
# if you add "libs" only libraries will be created (no executables)
# if you add "exes" only executables will be created (no libraries)


gmake -f ${STAR_MAKE_HOME}/MakeStaf.mk INP_DIR=${INP} OUT_DIR=${OUT} $NODEBUG


# Makefile does not touch input area
# But in current environment should be "inc" directory on the same level
# as ASPs. So this directory is making by link to created "inc" directory 
#
  echo '*** DONE ***'

#
# All comments to perev@bnl.gov
#
