#include "pack.h"
#include <utils.h>

#include <archive.h>
#include <archive_entry.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>

//! @brief Warn stream.
static FILE *WARN = NULL;


//! @brief Wrapper for libarhive's functions, which rerurn ARCHIVE_* status.
#define WRP(_cmd) do{                                                                       \
    int res = (_cmd);                                                                       \
    if (res != ARCHIVE_OK)                                                                  \
    {                                                                                       \
        if (res == ARCHIVE_WARN && WARN)                                                    \
            fprintf(WARN, "libarchive warning: %s\n", archive_error_string(ar));            \
        else                                                                                \
        {                                                                                   \
            err = archive_errno(ar);                                                        \
            goto CleanUp;                                                                   \
        }                                                                                   \
    }                                                                                       \
}while(0)


#define READ_CHK_ERR(_buf, _buf_len, _stream, _len, _err) do{   \
    (_len) = fread((_buf), 1, (_buf_len), (_stream));           \
    if ( ferror(_stream) )                                      \
    {                                                           \
        (_err) = EIO;                                           \
        goto CleanUp;                                           \
    }                                                           \
}while(0)

//! @brief Copies the whole file's data into `entry` of the `ar`. All preceeding
//! routine (including writing a header) must be done before.
inline int copy_file_data_into_ar(const char *filename, archive *ar, archive_entry *entry)
{
    assert(filename);
    assert(ar);
    assert(entry);

    int err = 0;

    size_t len = 0;
    char buf[4096] = {};

    FILE *file = fopen(filename, "r");
    if (!file)
    {
        err = errno;
        goto CleanUp;
    }

    READ_CHK_ERR(buf, sizeof(buf), file, len, err);
    while (len > 0)
    {
        archive_write_data(ar, buf, len);
        READ_CHK_ERR(buf, sizeof(buf), file, len, err);  
    }

CleanUp:
    FCLOSE(file);
    return err;
}

static int add_file_to_ar(const char *filename, archive *ar, archive_entry *entry);

//! @brief ONLY FOR USE IN 'add_file_to_ar()'.
//! Adds given dir to archive with all of it's files. Empty dirs aren't added.
//! @param[in] dirname  Dir, which contents are to be added to the archive.
//! @param[in] ar       The archive, it must be prepared for adding files.
//! @param[in] entry    Entry object, receieved from `archive_entry_new()`. No 
//!                     other preps are needed, just `archive_entry_free()` it
//!                     afterwards.
//! @return Error code. Equals 0, if everything is ok.
//! @note If one of files to be archived can't be found (stat() returned ENOENT),
//! corresponding warning message is printed into WARN if WARN != NULL; This isn't
//! considered to be an error, so 0 is returned.
static int _add_dir_to_ar_rec(const char* dirname, archive *ar, archive_entry *entry)
{
    assert(dirname);
    assert(ar);
    assert(entry);

    int err = 0;

    dirent *ent     = NULL;
    char *filename  = NULL;

    size_t dirname_len = strlen(dirname);
    DIR *dir = opendir(dirname);
    if (!dir)
    {
        err = errno;
        goto CleanUp;
    }

    while ((ent = readdir(dir)) != 0)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        size_t filename_len = dirname_len + strlen(ent->d_name) + (sizeof("/") - 1)+1;
        filename = (char *) calloc(filename_len, sizeof(char));
        if (!filename)
        {
            err = ENOMEM;
            goto CleanUp;
        }

        snprintf(filename, filename_len, "%s/%s", dirname, ent->d_name);

        if ( (err = add_file_to_ar(filename, ar, entry)) != 0)
            goto CleanUp;
    }

CleanUp:
    free(filename);
    closedir(dir);
    return err;
}

//! @brief Adds given file into archive `ar`. If given file is a directory,
//! All it's files are added recursively.
//! @param[in] filename File (maybe dir) to add to the archive.
//! @param[in] ar       The archive, it must be prepared for adding files.
//! @param[in] entry    Entry object, receieved from `archive_entry_new()`. No 
//!                     other preps are needed, just `archive_entry_free()` it
//!                     afterwards.
//! @return Error code. Equals 0, if everything is ok.
//! @note If one of files to be archived can't be found (stat() returned ENOENT),
//! corresponding warning message is printed into WARN if WARN != NULL; This isn't
//! considered to be an error, so 0 is returned.
static int add_file_to_ar(const char *filename, archive *ar, archive_entry *entry)
{
    assert(filename);
    assert(ar);
    assert(entry);

    int err = 0;

    struct stat st = {};
    if (stat(filename, &st) == -1)
    {
        if (errno == ENOENT)
        {
            if (WARN) fprintf(WARN, "Warning: File to archive not found: %s\n", filename);
            return 0;
        }
        else
        {
            err = errno;
            goto CleanUp;
        }
    }

    // following recursive calls might use `entry`, but they all must clean it afterwards.
    if (S_ISDIR(st.st_mode))
        return _add_dir_to_ar_rec(filename, ar, entry);

    // since now no recursive calls happen, so we can freely use `entry` ourselves.
    archive_entry_copy_stat(entry, &st);
    archive_entry_set_pathname(entry, filename);
    WRP(archive_write_header(ar, entry));

    if (archive_entry_size(entry) > 0)
    {
        if ( (err = copy_file_data_into_ar(filename, ar, entry)) != 0)
            goto CleanUp;
    }

CleanUp:
    entry = archive_entry_clear(entry);
    return err;
}

bool archive_files(const char *out, const char * const *filenames, 
                   int *error_code_p, FILE *warn_stream)
{
    assert(out);

    int err = 0;

    if (warn_stream)
        WARN = warn_stream;
    else
        WARN = NULL;

    const char * const *cur_file_p = filenames;
    archive       *ar    = archive_write_new();
    archive_entry *entry = archive_entry_new();

    WRP(archive_write_add_filter_gzip(ar));
    WRP(archive_write_set_format_pax_restricted(ar)); // recommended by authors of libarchive
    WRP(archive_write_open_filename(ar, out));

    while(*cur_file_p)
    {
        add_file_to_ar(*cur_file_p, ar, entry);
        cur_file_p++;
    }

CleanUp:
    if (entry) archive_entry_free(entry);
    if (ar) archive_write_close(ar);
    if (ar) archive_write_free(ar);

    bool res = true;
    if (err) res = false;
    if (error_code_p) *error_code_p = err;
    return res;
}