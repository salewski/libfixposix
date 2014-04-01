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

#include <lfp/dirent.h>
#include <lfp/string.h>
#include <lfp/errno.h>

DSO_PUBLIC DIR*
lfp_opendir(const char *name)
{
    return opendir(name);
}

DSO_PUBLIC int
lfp_readdir(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    int ret = readdir_r(dirp, entry, result);
    if ( ret > 0 ) {
        lfp_set_errno(ret);
        return -1;
    } else if (*result == NULL) {
        return 0;
    } else {
        return 1;
    }
}

DSO_PUBLIC long
lfp_telldir(DIR *dirp)
{
    return telldir(dirp);
}

DSO_PUBLIC void
lfp_seekdir(DIR *dirp, long offset)
{
    return seekdir(dirp, offset);
}

DSO_PUBLIC void
lfp_rewinddir(DIR *dirp)
{
    return rewinddir(dirp);
}

DSO_PUBLIC int
lfp_closedir(DIR *dirp)
{
    return closedir(dirp);
}
