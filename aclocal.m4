# aclocal.m4 generated automatically by aclocal 1.6.3 -*- Autoconf -*-

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# AST_EXT_LIB([NAME], [FUNCTION], [package header], [package symbol name], [package friendly name], [additional LIB data])

AC_DEFUN([AST_EXT_LIB],
[
AC_ARG_WITH([$1], AC_HELP_STRING([--with-$1=PATH],[use $5 files in PATH]),[
case ${withval} in
     n|no)
     USE_$1=no
     ;;
     y|ye|yes)
     $1_MANDATORY="yes"
     ;;
     *)
     $1_DIR="${withval}"
     $1_MANDATORY="yes"
     ;;
esac
])

PBX_LIB$1=0

if test "${USE_$1}" != "no"; then
   libdir=""
   if test "x${$1_DIR}" != "x"; then
      libdir="-L${$1_DIR}/lib"
   fi
   AC_CHECK_LIB([$1], [$2], [:], [], ${libdir} $6)

   if test "${ac_cv_lib_$1_$2}" = "yes"; then
      $1_LIB="-l$1 $6"
      $4_HEADER_FOUND="1"
      if test "x${$1_DIR}" != "x"; then
         $1_LIB="${libdir} ${$1_LIB}"
	 $1_INCLUDE="-I${$1_DIR}/include"
	 if test "x$3" != "x" ; then
	    AC_CHECK_HEADER([${$1_DIR}/include/$3], [$4_HEADER_FOUND=1], [$4_HEADER_FOUND=0] )
	 fi
      else
	 if test "x$3" != "x" ; then
            AC_CHECK_HEADER([$3], [$4_HEADER_FOUND=1], [$4_HEADER_FOUND=0] )
	 fi
      fi
      if test "x${$4_HEADER_FOUND}" = "x0" ; then
         if test ! -z "${$1_MANDATORY}" ;
         then
            echo " ***"
            echo " *** It appears that you do not have the $1 development package installed."
            echo " *** Please install it to include $5 support, or re-run configure"
            echo " *** without explicitly specifying --with-$1"
            exit 1
         fi
         $1_LIB=""
         $1_INCLUDE=""
         PBX_LIB$1=0
      else
         PBX_LIB$1=1
         AC_DEFINE_UNQUOTED([HAVE_$4], 1, [Define to indicate the $5 library])
      fi
   elif test ! -z "${$1_MANDATORY}";
   then
      echo "***"
      echo "*** The $5 installation on this system appears to be broken."
      echo "*** Either correct the installation, or run configure"
      echo "*** without explicity specifying --with-$1"
      exit 1
   fi
fi
AC_SUBST([$1_LIB])
AC_SUBST([$1_INCLUDE])
AC_SUBST([PBX_LIB$1])
])


AC_DEFUN(
[AST_CHECK_GNU_MAKE], [AC_CACHE_CHECK(for GNU make, GNU_MAKE,
   GNU_MAKE='Not Found' ;
   for a in make gmake gnumake ; do
      if test -z "$a" ; then continue ; fi ;
      if ( sh -c "$a --version" 2> /dev/null | grep GNU  2>&1 > /dev/null ) ;  then
         GNU_MAKE=$a ;
         break;
      fi
   done ;
) ;
if test  "x$GNU_MAKE" = "xNot Found"  ; then
   echo " *** Please install GNU make.  It is required to build Asterisk!"
   exit 1
fi
AC_SUBST([GNU_MAKE])
])

