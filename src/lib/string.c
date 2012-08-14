/*******************************************************************************/
/* Permission is hereby granted, free of charge, to any person or organization */
/* obtaining a copy of the software and accompanying documentation covered by  */
/* this license (the "Software") to use, reproduce, display, distribute,       */
/* execute, and transmit the Software, and to prepare derivative works of the  */
/* Software, and to permit third-parties to whom the Software is furnished to  */
/* do so, all subject to the following:                                        */
/*                                                                             */
/* The copyright notices in the Software and this entire statement, including  */
/* the above license grant, this restriction and the following disclaimer,     */
/* must be included in all copies of the Software, in whole or in part, and    */
/* all derivative works of the Software, unless such copies or derivative      */
/* works are solely in the form of machine-executable object code generated by */
/* a source language processor.                                                */
/*                                                                             */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    */
/* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT   */
/* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE   */
/* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, */
/* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE.                                                   */
/*******************************************************************************/

#include <lfp/string.h>
#include <lfp/stdlib.h>
#include <lfp/errno.h>

#include <limits.h>
#include <inttypes.h>

static int64_t compute_multiplier(enum lfp_memsize_measure_unit unit);

DSO_PUBLIC size_t
lfp_strnlen(const char *s, size_t maxlen)
{
#if defined(HAVE_STRNLEN)
    return strnlen(s, maxlen);
#else
    for (size_t i = 0; i < maxlen; i++) {
        if (s[i] == '\0') {
            return i;
        }
    }
    return maxlen;
#endif
}

DSO_PUBLIC char*
lfp_strndup(const char *s, size_t maxlen)
{
#if defined(HAVE_STRNDUP)
    return strndup(s, maxlen);
#else
    if (s == NULL) {
        return NULL;
    } else {
        size_t len = lfp_strnlen(s, maxlen);
        char *newstr = malloc(len + 1);
        if (newstr != NULL) {
            memcpy(newstr, s, len);
            newstr[len] = '\0';
        }
        return newstr;
    }
#endif
}

static inline int64_t
compute_multiplier(enum lfp_memsize_measure_unit unit)
{
    switch(unit) {
    case LFP_OCTETS: return 1;
    case LFP_KB:     return 1000LL;
    case LFP_KIB:    return 1024LL;
    case LFP_MB:     return 1000LL*1000LL;
    case LFP_MIB:    return 1024LL*1024LL;
    case LFP_GB:     return 1000LL*1000LL*1000LL;
    case LFP_GIB:    return 1024LL*1024LL*1024LL;
    case LFP_TB:     return 1000LL*1000LL*1000LL*1000LL;
    case LFP_TIB:    return 1024LL*1024LL*1024LL*1024LL;
    case LFP_PB:     return 1000LL*1000LL*1000LL*1000LL*1000LL;
    case LFP_PIB:    return 1024LL*1024LL*1024LL*1024LL*1024LL;
    case LFP_EB:     return 1000LL*1000LL*1000LL*1000LL*1000LL*1000LL;
    case LFP_EIB:    return 1024LL*1024LL*1024LL*1024LL*1024LL*1024LL;
    default:         SYSERR(EINVAL);
    }
}

DSO_PUBLIC int64_t
lfp_parse_memsize(const char *s, enum lfp_memsize_measure_unit default_unit)
{
    int64_t default_multiplier, multiplier, amount;

    SYSCHECK(EINVAL, s == NULL || *s == '\0');
    SYSGUARD(default_multiplier = compute_multiplier(default_unit));

    char *endptr = NULL;
    lfp_set_errno(0);
    SYSGUARD(amount = strtoll(s, &endptr, 10));
    SYSCHECK(ERANGE, amount < 0);

    if (amount == 0) {
        return 0;
    } else if (*endptr) {
        if        (strcasecmp(endptr, "KB")  == 0) {
            multiplier = 1000LL;
        } else if (strcasecmp(endptr, "K")   == 0 ||
                   strcasecmp(endptr, "KiB") == 0) {
            multiplier = 1024LL;
        } else if (strcasecmp(endptr, "MB")  == 0) {
            multiplier = 1000LL*1000LL;
        } else if (strcasecmp(endptr, "M")   == 0 ||
                   strcasecmp(endptr, "MiB") == 0) {
            multiplier = 1024LL*1024LL;
        } else if (strcasecmp(endptr, "GB")  == 0) {
            multiplier = 1000LL*1000LL*1000LL;
        } else if (strcasecmp(endptr, "G")   == 0 ||
                   strcasecmp(endptr, "GiB") == 0) {
            multiplier = 1024LL*1024LL*1024LL;
        } else if (strcasecmp(endptr, "TB")  == 0) {
            multiplier = 1000LL*1000LL*1000LL*1000LL;
        } else if (strcasecmp(endptr, "T")   == 0 ||
                   strcasecmp(endptr, "TiB") == 0) {
            multiplier = 1024LL*1024LL*1024LL*1024LL;
        } else if (strcasecmp(endptr, "PB")  == 0) {
            multiplier = 1000LL*1000LL*1000LL*1000LL*1000LL;
        } else if (strcasecmp(endptr, "P")   == 0 ||
                   strcasecmp(endptr, "PiB") == 0) {
            multiplier = 1024LL*1024LL*1024LL*1024LL*1024LL;
        } else if (strcasecmp(endptr, "EB")  == 0) {
            multiplier = 1000LL*1000LL*1000LL*1000LL*1000LL*1000LL;
        } else if (strcasecmp(endptr, "E")   == 0 ||
                   strcasecmp(endptr, "EiB") == 0) {
            multiplier = 1024LL*1024LL*1024LL*1024LL*1024LL*1024LL;
        } else {
            SYSERR(EINVAL);
        }
    } else {
        multiplier = default_multiplier;
    }

    // Check for overflow
    if (amount > (INT64_MAX / multiplier)) {
        SYSERR(ERANGE);
    } else {
        return amount * multiplier;
    }
}
