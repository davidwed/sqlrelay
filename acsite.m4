AC_DEFUN([AC_CHECK_PS],
[AC_MSG_CHECKING(whether ps -efa works)
INVALID="`ps -efal 2>&1 | grep illegal | grep -v grep`"
if ( test -n "$INVALID" ); then
	PS="ps\ aux"
	AC_MSG_RESULT(no)
else
	PS="ps\ \-efal"
	AC_MSG_RESULT(yes)
fi
AC_SUBST(PS)])

AC_DEFUN([AC_CHECK_INLINE],
[AC_MSG_CHECKING(inline)
INLINE="inline"
# intel optimizing compiler doesn't have inlines, assume that CC doesn't either
# even though it might, this test needs to be more robust
if ( test "$CXX" = "icc" -o "$CXX" = "CC" ); then
	INLINE=""
else 
	# redhat's gcc 2.96 has problems with inlines
	CXX_VERSION=`$CXX --version`
	if ( test "$CXX_VERSION" = "2.96" ); then
		INLINE=""
	fi
fi
if ( test "$INLINE" = "inline" ); then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AC_DEFINE_UNQUOTED(INLINE,$INLINE,Some compliers don't support the inline keyword)])
