/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*              Matthew Covington                                  */
/* and                                                             */
/*              Alex Alarcon                                       */
/*                                                                 */


/** 
 * COSC 3250 - Project 10
 * fileDelete implementation
 * @author Matthew Covington Alex Alarcon
 * Instructor Sabirat Rubya
 * TA-BOT:MAILTO matthew.covington@marquette.edu alex.alarcon@marquette.edu
*/


#include <kernel.h>
#include <memory.h>
#include <file.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

    syscall result;

    if ((NULL == supertab) || (NULL == filetab) || (isbadfd(fd)) || (FILE_FREE == filetab[fd].fn_state)) {
        return SYSERR;
    }

    if ((fd < 0) || (fd >= DISKBLOCKTOTAL)) {
        return SYSERR;
    }

    if (wait(supertab -> sb_dirlock) != OK) {
        return SYSERR;
    }

    result = sbFreeBlock(supertab, supertab -> sb_dirlst -> db_fnodes[fd].fn_blocknum); 

    if (result != OK) {
        signal(supertab -> sb_dirlock);
        return SYSERR;
    }

    supertab -> sb_dirlst -> db_fnodes[fd].fn_state = FILE_FREE; 

    seek(DISK0, supertab -> sb_dirlst -> db_blocknum);

    result = write(DISK0, supertab -> sb_dirlst, sizeof(struct dirblock));

    signal(supertab -> sb_dirlock);

    if (result != OK) {
        return SYSERR;
    }
    
    return OK;
    
}
