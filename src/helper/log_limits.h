#ifndef LOGGER_HELPER_LOG_LIMITS_H
#define LOGGER_HELPER_LOG_LIMITS_H

/* c headers */
#include <stdint.h>
#include <limits.h>

# if __WORDSIZE == 64
#  ifndef __INT64_C
#   define __INT64_C(c)  c ## L
#  endif
#  ifndef __UINT64_C
#   define __UINT64_C(c) c ## UL
#  endif
# else
#  ifndef __INT64_C
#   define __INT64_C(c)  c ## LL
#  endif
#  ifndef __UINT64_C
#   define __UINT64_C(c) c ## ULL
#  endif
# endif

/* Limits of integral types.  */

/* Minimum of signed integral types.  */
#ifndef INT8_MIN
# define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
# define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
# define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT64_MIN
# define INT64_MIN              (-__INT64_C(9223372036854775807)-1)
#endif

/* Maximum of signed integral types.  */
#ifndef INT8_MAX
# define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
# define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
# define INT32_MAX              (2147483647)
#endif
#ifndef INT64_MAX
# define INT64_MAX              (__INT64_C(9223372036854775807))
#endif

/* Maximum of unsigned integral types.  */
#ifndef UINT8_MAX
# define UINT8_MAX              (255)
#endif
#ifndef UINT16_MAX
# define UINT16_MAX             (65535)
#endif
#ifndef UINT32_MAX
# define UINT32_MAX             (4294967295U)
#endif
#ifndef UINT64_MAX
# define UINT64_MAX             (__UINT64_C(18446744073709551615))
#endif

#endif /* LOGGER_HELPER_LOG_LIMITS_H */