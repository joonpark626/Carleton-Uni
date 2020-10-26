#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "restart.h"
#include "fileio.h"
#include "util.h"

static int translate_errno_for_file(int errnum)
{
    return errnum == ENOENT ? IOERR_INVALID_PATH : IOERR_POSIX;
}
static int translate_errno_for_dir(int errnum)
{
    return (errnum == ENOENT || errnum == ENOTDIR || errnum == EEXIST)
	? IOERR_INVALID_PATH : IOERR_POSIX;
}

static int safeclose(int fd, int retval)
{
    if (fd < 0)
	return retval;
    int save_errno = errno;
    int close_val = r_close(fd);
    return retval < 0 ? errno = save_errno, retval
	: close_val ? translate_errno_for_file(errno) : retval;
}

/* PUBLIC API. Refer to assignment5_tests.c and fileio.h for specs and errors*/

/* Read upto 'bufbytes' into buffer 'buffer' from file 'path' starting from 'offset' */
int file_read(char *path, int offset, void *buffer, size_t bufbytes)
{
    if (!path || offset < 0 || !buffer || bufbytes < 1)
	return IOERR_INVALID_ARGS;

    int fd = r_open2(path, O_RDONLY);

    if (fd == -1)
	return translate_errno_for_file(errno);


    if (offset && (lseek(fd, offset, SEEK_SET) == (off_t) - 1))
	return safeclose(fd, IOERR_POSIX);


    int bytes_read = r_read(fd, buffer, bufbytes);

    return safeclose(fd, bytes_read);
}

/* Return a status line about an existing file */
int file_info(char *path, void *buffer, size_t bufbytes)
{
    if (!path || !buffer || bufbytes < 1)
	return IOERR_INVALID_ARGS;
    struct stat s;
    if (-1 == stat(path, &s))
	return translate_errno_for_file(errno);

    char *ftype = S_ISREG(s.st_mode) ? "f"
	: S_ISDIR(s.st_mode) ? "d" : "?";

    int accessed = s.st_atime;
    int modified = s.st_mtime;

    snprintf(buffer, (int) bufbytes,
	     "Size:%d Accessed:%d Modified:%d Type %s", (int) s.st_size,
	     (int) accessed, modified, ftype);

    *((char *)buffer + bufbytes - 1) = '\0';	//safety termination

    return 0;
}

/* Write bytes into a file. */
int file_write(char *path, int offset, void *buffer, size_t bufbytes)
{
    if (!path || !buffer || bufbytes < 1)
	return IOERR_INVALID_ARGS;

    int fd;

    if (-1 == (fd = r_open3(path, O_CREAT | O_WRONLY, S_IRWXU)))
	return translate_errno_for_file(errno);

    if (offset && (lseek(fd, offset, SEEK_SET) == (off_t) - 1))
	return safeclose(fd, IOERR_POSIX);

    int bytes_written = r_write(fd, buffer, bufbytes);

    return safeclose(fd, bytes_written);
}

/* Create a file based on a pattern */
int file_create(char *path, char *pattern, int repeatcount)
{
    int r, offset = 0;
    if (!path || !pattern || repeatcount < 1)
	return IOERR_INVALID_ARGS;


    if ((r = file_remove(path)) && r != IOERR_INVALID_PATH)
	return r;


    while (repeatcount--) {
	r = file_write(path, offset, pattern, strlen(pattern));
	if (r < 1) {
	    return r;
	}
	offset += r;
    }

    return 0;
}

/* Remove a file */
int file_remove(char *path)
{
    if (!path)
	return IOERR_INVALID_ARGS;
    return !unlink(path) ? 0 : translate_errno_for_file(errno);
}

/* Create a directory */
int dir_create(char *path)
{
    if (!path)
	return IOERR_INVALID_ARGS;
    return !mkdir(path, 0755) ? 0 : translate_errno_for_dir(errno);
}

/* List the contents of a directory */
int dir_list(char *path, void *buffer, size_t bufbytes)
{
    struct dirent *direntp;
    DIR *dirp;
    char *buf = (char *) buffer;

    if (!path || !buffer || bufbytes < 1)
	return IOERR_INVALID_ARGS;

    if ((dirp = opendir(path)) == NULL)
	return translate_errno_for_dir(errno);

    *buf = 0;
    int used = 1, ln;
    while (used > 0 && ((direntp = readdir(dirp)) != NULL)) {
	if ((used + (ln = strlen(direntp->d_name)) + 2) < bufbytes) {
	    strcat(buf + used - 1, direntp->d_name);
	    strcat(buf + used - 1 + ln, "\n");
	    used += ln + 1;
	} else
	    used = -1;		// return with error
    }

    while ((closedir(dirp) == -1) && (errno == EINTR));

    return used < 0 ? IOERR_BUFFER_TOO_SMALL : 0;
}


/* Calculate the checksum value for a file */
int file_checksum(char *path)
{
    if (!path)
	return IOERR_INVALID_ARGS;
    char b[1];
    int offset = 0;
    unsigned short sum = 0;
    while (1) {
	int r = file_read(path, offset, b, sizeof(b));
	if (r < 0)
	    return r;
	if (r == 0)
	    return sum;
	sum = checksum(b, r, sum);
	offset += r;
    }
    return 0;
}

/* Recursively calculate the checksum value for a directory */
int dir_checksum(char *path)
{
    struct dirent *direntp;
    DIR *dirp;

    if (!path)
	return IOERR_INVALID_ARGS;
    if ((dirp = opendir(path)) == NULL) {
	return (errno == ENOENT
		|| errno == ENOTDIR) ? IOERR_INVALID_PATH : IOERR_POSIX;
    }
    unsigned short sum = 0;
    struct stat s;
    int r;

    while ((direntp = readdir(dirp)) != NULL) {
	if (!strcmp(".", direntp->d_name))
	    continue;
	if (!strcmp("..", direntp->d_name))
	    continue;
	char path2[255];
	strcpy(path2, path);
	strcat(path2, "/");
	strcat(path2, direntp->d_name);
	if (-1 == stat(path2, &s)) {
	    fprintf(stderr, "stat failed");
	    return translate_errno_for_file(errno);
	}
	if (S_ISDIR(s.st_mode)) {
	    sum = checksum(direntp->d_name, strlen(direntp->d_name), sum);
	    r = dir_checksum(path2);
	} else {
	    r = file_checksum(path2);
	}
	if (r < 0)
	    return r;
	sum += r;
    }
    while ((closedir(dirp) == -1) && (errno == EINTR));

    return sum;
}
