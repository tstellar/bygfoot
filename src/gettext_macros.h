/**
 * Standard gettext macros.
 */
#ifndef G_OS_UNIX
  #define ENABLE_NLS 
#endif
 
#ifdef ENABLE_NLS
#  ifndef G_OS_UNIX
#    define PACKAGE "bygfoot"
#    define GETTEXT_PACKAGE "bygfoot"
#    define PACKAGE_LOCALE_DIR "./locale"
#  endif
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

