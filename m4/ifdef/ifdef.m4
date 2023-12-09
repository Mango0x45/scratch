m4_dnl Execute this file with ‘m4 -P ifdef.m4 -’
m4_changequote([,])m4_dnl
m4_dnl
This file has an implementation of ‘__ifdef’ and ‘__endif’ macros.  The two
macros act like the ‘#ifdef’ and ‘#endif’ macros from cpp(1).  The usage looks
as follows:

__ifdef(`foo')
This is some text
and some more text
__ifdef(`bar')
This is also some
text on two lines
__endif

TODO: Add __elifdef()?

m4_define([__endif], [m4_divert(0)m4_dnl])m4_dnl
m4_define([__ifdef], [m4_ifdef([$1], [__endif], [m4_divert(-1)])])m4_dnl
