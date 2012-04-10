#ifndef __LV_CONFIG_H__
#define __LV_CONFIG_H__

#define VISUAL_VERSION     "0.5.0"
#define VISUAL_PLUGIN_PATH "/usr/local/lib/libvisual-0.5"

#ifndef __cplusplus
#define LV_HAVE_ISO_C_VARARGS 1
#endif

/* gcc-2.95.x supports both gnu style and ISO varargs, but if -ansi
 * is passed ISO vararg support is turned off, and there is no work
 * around to turn it on, so we unconditionally turn it off.
 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 95
#undef LV_HAVE_ISO_C_VARARGS
#endif

#define LV_HAVE_GNU_C_VARARGS 1

/* Needed for lv/types.h */
#define _LV_HAVE_STDINT_H    1
#define _LV_HAVE_INTTYPES_H  1

#define VISUAL_BIG_ENDIAN    0
#define VISUAL_LITTLE_ENDIAN 1

typedef unsigned int visual_size_t;

#define VISUAL_SIZE_T_FORMAT "u"

/* #undef VISUAL_RANDOM_FAST_FP_RNG */

/* #undef VISUAL_ARCH_MIPS */
/* #undef VISUAL_ARCH_ALPHA */
/* #undef VISUAL_ARCH_SPARC */
/* #undef VISUAL_ARCH_X86 */
/* #undef VISUAL_ARCH_X86_64 */
/* #undef VISUAL_ARCH_POWERPC */
#define VISUAL_ARCH_ARM
/* #undef VISUAL_ARCH_UNKNOWN */

#define VISUAL_OS_LINUX
/* #undef VISUAL_OS_WIN32 */
/* #undef VISUAL_OS_HPUX */
/* #undef VISUAL_OS_AIX */
/* #undef VISUAL_OS_BEOS */
/* #undef VISUAL_OS_OSF */
/* #undef VISUAL_OS_IRIX */
/* #undef VISUAL_OS_SUNOS */
/* #undef VISUAL_OS_SOLARIS */
/* #undef VISUAL_OS_SCO */
/* #undef VISUAL_OS_FREEBSD */
/* #undef VISUAL_OS_NETBSD */
/* #undef VISUAL_OS_OPENBSD */
/* #undef VISUAL_OS_DARWIN */
/* #undef VISUAL_OS_ANDROID */
/* #undef VISUAL_OS_UNKNOWN */

/* #undef VISUAL_WITH_CYGWIN */
/* #undef VISUAL_WITH_MINGW */

/*#define VISUAL_HAVE_THREADS*/
/* #undef VISUAL_THREAD_MODEL_WIN32 */
/*#define VISUAL_THREAD_MODEL_POSIX*/
/* #undef VISUAL_THREAD_MODEL_DCE */
/* #undef VISUAL_THREAD_MODEL_GTHREAD2 */

#endif /* LV_CONFIG_H */
