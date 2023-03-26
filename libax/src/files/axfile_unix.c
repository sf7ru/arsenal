// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id$
// HISTORY
//      $Log$
// ***************************************************************************

#define _FILE_OFFSET_BITS       64

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <arsenal.h>
#include <stdio.h>
#include <axfile.h>
#include <pvt_unix.h>


// ---------------------------------------------------------------------------
// ------------------------------- DEFINITIONS -------------------------------
// -----|-------------------|-------------------------------------------------

#define DEFAULT_BUFF_SIZE   (64 * 1024)

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------

// ***************************************************************************
// FUNCTION
//      axfile_close
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file --
// RESULT
//      HAXFILE  --
// ***************************************************************************
HAXFILE axfile_close(HAXFILE h_file)
{
    PAXFILE         pst_file    = (PAXFILE)h_file;

    ENTER(pst_file);

    if (pst_file->i_fd != -1)
        close(pst_file->i_fd);

    FREE(pst_file);

    RETURN(NULL);
}
// ***************************************************************************
// FUNCTION
//      axfile_open
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_fname --
//      PSTR   psz_mode  --
// RESULT
//      HAXFILE  --
// ***************************************************************************
HAXFILE axfile_open(PCSTR   psz_fname,
                    UINT    u_mode)
{
    PAXFILE         pst_file    = NULL;

    ENTER(psz_fname);

    if ((pst_file = CREATE(AXFILE)) != NULL)
    {
        pst_file->i_fd = -1;

        if ((pst_file->i_fd = open(psz_fname, u_mode, 0644)) == -1)
        {
            pst_file = (PAXFILE)axfile_close((HAXFILE)pst_file);
        }
    }

    RETURN((HAXFILE)pst_file);
}
// ***************************************************************************
// FUNCTION
//      axfile_write
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file   --
//      PVOID     p_buffer --
//      INT       i_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axfile_write(HAXFILE        h_file,
                 PVOID          p_buffer,
                 INT            i_size)
{
    INT             i_result        = -1;
    PAXFILE         pst_file        = (PAXFILE)h_file;

    ENTER(pst_file && p_buffer && i_size);

    i_result = write(pst_file->i_fd, p_buffer, i_size);

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_read
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file   --
//      PVOID     p_buffer --
//      INT       i_size   --
// RESULT
//      INT  --
// ***************************************************************************
INT axfile_read(HAXFILE         h_file,
                PVOID           p_buffer,
                INT             i_size)
{
    INT             i_result        = -1;
    PAXFILE         pst_file        = (PAXFILE)h_file;

    ENTER(pst_file && p_buffer && i_size);

    do
    {
        i_result = read(pst_file->i_fd, p_buffer, i_size);

        if (i_result == -1)
        {
            perror("@@@@@@@@@@@@@@@@@@@@@@@@@ ");
        }
        else
        {
//            if (i_result != i_size)
//            {
//                printf("@@@@@@@@@@@@@@@@@@@@@@@@@ axfile_read: %d != %d\n", i_result, i_size);
//            }
        }

    } while ((i_result == -1) && (errno == EINTR));

    RETURN(i_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_move
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_old     --
//      PSTR   psz_new     --
//      BOOL   b_overwrite --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_move(PCSTR      psz_new,
                 PCSTR      psz_old,
                 BOOL       b_overwrite)
{
    BOOL b_result   = false;

    ENTER(psz_new && psz_old);

    if (rename(psz_old, psz_new))
    {
        switch (errno)
        {
            case EEXIST:
                if (b_overwrite)
                {
                    if (!remove(psz_new) && !rename(psz_old, psz_new))
                    {
                        b_result = axfile_move(psz_new, psz_old, b_overwrite);
                    }
                }
                break;

            case EXDEV:
                if (axfile_copy(psz_new, psz_old, b_overwrite))
                {
                    remove(psz_old);
                    b_result = true;
                }
                break;
        }
    }
    else
        b_result = true;

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_remove
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_filename --
// RESULT
//      BOOL
//            true:   Ok
//            false:  Error
// ***************************************************************************
BOOL axfile_remove(PCSTR       psz_filename)
{
    BOOL            b_result        = false;

    ENTER(true);

    b_result = !remove(psz_filename);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_copy
// PURPOSE
//
// PARAMETERS
//      PSTR   psz_old     --
//      PSTR   psz_new     --
//      BOOL   b_overwrite --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_copy(PCSTR      psz_new,
                 PCSTR      psz_old,
                 BOOL       b_overwrite)
{
    BOOL            b_result        = false;
    PU8	            p_buff          = NULL;
    UINT            u_size;
    UINT            u_amount;
    UINT            u_buff_size;
    HAXFILE         h_old;
    HAXFILE         h_new;

    ENTER(psz_new && psz_old);

    if ((h_old = axfile_open(psz_old, O_RDONLY)) != NULL)
    {
        if (b_overwrite || !axfile_exist(psz_new))
        {
            if ((h_new = axfile_open(psz_new, O_RDWR | O_CREAT)) != NULL)
            {
                if ((u_size = axfile_size_get(h_old)) > 0)
                {
                    u_buff_size = u_size < DEFAULT_BUFF_SIZE ?
                                  u_size : DEFAULT_BUFF_SIZE;

                    if((p_buff = MALLOC(u_buff_size)) != NULL)
                    {
                        for(    b_result = true;
                                b_result && u_size;
                                u_size -= u_amount)
                        {
                            u_amount = u_size > u_buff_size   ?
                                       u_buff_size            :
                                       u_size;

                            if ( !( (axfile_read(h_old,
                                      p_buff, u_amount) == u_amount)    &&
                                    (axfile_write(h_new,
                                      p_buff, u_amount) == u_amount)    ))
                            {
                                b_result = false;
                            }
                        }

                        FREE(p_buff);
                    }
                    else
                        errno = ENOMEM;
                }
                else
                    b_result = true;

                axfile_close(h_new);

                if (!b_result)
                    remove(psz_new);
            }
        }
        else
            errno = EEXIST;

        axfile_close(h_old);
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_seek
// PURPOSE
//
// PARAMETERS
//      HAXFILE       h_file   --
//      FILESIZEINT   i_offs   --
//      UINT          u_whence --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_seek(HAXFILE            h_file,
                 FILESIZEINT        i_offs,
                 UINT               u_whence)
{
    PAXFILE         pst_file        = (PAXFILE)h_file;
    BOOL            b_result        = false;

    ENTER(pst_file);

    if (lseek(pst_file->i_fd, i_offs, u_whence) != -1)
    {
        b_result = true;
    }

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_seek
// PURPOSE
//
// PARAMETERS
//      HAXFILE       h_file   --
//      FILESIZEINT   i_offs   --
//      UINT          u_whence --
// RESULT
//      BOOL  --
// ***************************************************************************
FILESIZEINT axfile_tell(HAXFILE     h_file)
{
    FILESIZEINT     i_offset        = -1;
    PAXFILE         pst_file        = (PAXFILE)h_file;

    ENTER(pst_file);

    i_offset = lseek(pst_file->i_fd, 0, SEEK_CUR);

    RETURN(i_offset);
}
// ***************************************************************************
// FUNCTION
//      axfile_size_set
// PURPOSE
//
// PARAMETERS
//      HAXFILE       h_file --
//      FILESIZEINT   i_size --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_size_set(HAXFILE            h_file,
                     FILESIZEINT        i_size)
{
    BOOL            b_result        = false;
    PAXFILE         pst_file        = (PAXFILE)h_file;

    ENTER(pst_file);

    b_result = !ftruncate(pst_file->i_fd, i_size);

    RETURN(b_result);
}
// ***************************************************************************
// FUNCTION
//      axfile_size
// PURPOSE
//
// PARAMETERS
//      HAXFILE   h_file --
// RESULT
//      FILESIZEINT  --
// ***************************************************************************
FILESIZEINT axfile_size_get(HAXFILE     h_file)
{
    FILESIZEINT     i_size          = -1;
    PAXFILE         pst_file        = (PAXFILE)h_file;
    struct stat     st_st;

    ENTER(pst_file);

    if (!fstat(pst_file->i_fd, &st_st))
    {
        i_size = st_st.st_size;
    }

    RETURN(i_size);
}
// ***************************************************************************
// FUNCTION
//      axfile_unlock
// PURPOSE
//
// PARAMETERS
//      HAXHANDLE h_lock --
// RESULT
//      void --
// ***************************************************************************
void axfile_unlock(HAXHANDLE h_lock)
{
    int *           p_fd;

    ENTER(true);

    if ((p_fd = (int*)h_lock) != NULL)
    {
        close(*p_fd);

        FREE(p_fd);
    }

    QUIT;
}
// ***************************************************************************
// FUNCTION
//      axfile_lock
// PURPOSE
//
// PARAMETERS
//      PCSTR psz_lock_file --
// RESULT
//      HAXHANDLE --
// ***************************************************************************
HAXHANDLE axfile_lock(PCSTR psz_lock_file)
{
    int *           result          = NULL;
    int             fd;
    int             notused;
    CHAR            buff            [ 32 ];

    ENTER(psz_lock_file);

    if ((fd = open(psz_lock_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) != 0)
    {
        if (  (!lockf(fd, F_TLOCK, 0))                          &&
              ((result = (int*)MALLOC(sizeof(int))) != NULL)    )
        {
//            if ( errno == EACCES || errno == EAGAIN )

            notused = ftruncate(fd, 0);
            sprintf(buff, "%d", (int)getpid());
            notused = write(fd, buff, strlen(buff));

            notused = notused;

            *result = fd;
        }
        else
            close(fd);
    }

    RETURN(result);
}

#if (TARGET_SYSTEM == __AX_linux__)

#include <sys/vfs.h>

// ***************************************************************************
// FUNCTION
//      axfile_statfs
// PURPOSE
//
// PARAMETERS
//      PAXSTATFS   pst_stat --
//      PSTR        psz_path --
// RESULT
//      BOOL  --
// ***************************************************************************
BOOL axfile_statfs(PAXSTATFS    pst_stat,
                   PSTR         psz_path)
{
    BOOL            b_result                    = false;
    struct statfs   st_st;

    ENTER(pst_stat && psz_path);

    if (!statfs(psz_path, &st_st))
    {
        pst_stat->u_type    = (UINT)st_st.f_type;
        pst_stat->u_total   = (FILESIZEINT)st_st.f_blocks * st_st.f_bsize;
        pst_stat->u_free    = (FILESIZEINT)st_st.f_bfree  * st_st.f_bsize;
        pst_stat->u_avail   = (FILESIZEINT)st_st.f_bavail * st_st.f_bsize;
        b_result            = true;
    }

    RETURN(b_result);
}

#endif  // #if (TARGET_SYSTEM == __AX_linux__)

// ***************************************************************************
// FUNCTION
//      axfile_times_get
// PURPOSE
//      Retrieve file creation, access and modification time
// PARAMETERS
//      PAXDATE pst_c_time   -- Pointer to structure for creation time
//      PAXDATE pst_a_time   -- Pointer to structure for access time
//      PAXDATE pst_m_time   -- Pointer to structure for modification time
//      PSTR     psz_filename -- Pointer to path and file name
// RESULT
//      BOOL  -- true if all is ok or false if error has occured
// ***************************************************************************
BOOL axfile_times_get(PAXDATE        pst_c_time,
                      PAXDATE        pst_a_time,
                      PAXDATE        pst_m_time,
                      PSTR            psz_filename)
{
    BOOL            b_result        = false;
    struct stat     st_st;

    if (!stat(psz_filename, &st_st))
    {
        b_result   = (!pst_c_time ||
                        axdate_from_utime(pst_c_time, st_st.st_ctime))  &&
                     (!pst_a_time ||
                        axdate_from_utime(pst_a_time, st_st.st_atime))  &&
                     (!pst_m_time ||
                        axdate_from_utime(pst_m_time, st_st.st_mtime))  ;
    }

    return b_result;
}
