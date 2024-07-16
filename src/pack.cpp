#include "pack.h"
#include <utils.h>

#include <archive.h>
#include <archive_entry.h>
#include <errno.h>

//! @brief Wrapper for libarhive's functions, which rerurn ARCHIVE_* status.
#define WRP(_cmd) do{                                                                       \
    int res = (_cmd);                                                                       \
    if (res != ARCHIVE_OK)                                                                  \
    {                                                                                       \
        if (res == ARCHIVE_WARN && warn_stream)                                             \
            fprintf(warn_stream, "libarchive warning: %s\n", archive_error_string(arr));    \
        else                                                                                \
        {                                                                                   \
            err = archive_errno(arr);                                                       \
            fprintf(warn_stream, "libarchive error: %s\n", archive_error_string(arr));      \
            goto CleanUp;                                                                   \
        }                                                                                   \
    }                                                                                       \
}while(0)


#define READ_CHK_ERR(_buf, _buf_len, _stream, _len, _err) do{   \
    (_len) = fread((_buf), 1, (_buf_len), (_stream));                   \
    if ( ferror(_stream) )                                      \
    {                                                           \
        (_err) = EIO;                                             \
        goto CleanUp;                                           \
    }                                                           \
}while(0)


bool archive_files(const char *out, const char * const *filenames, 
                   int *error_code_p, FILE *warn_stream)
{
    int err = 0;

    const char * const *cur_file_p = filenames;
    
    FILE *file = NULL;
    size_t len = 0;
    char buf[4096] = {};

    archive       *arr   = archive_write_new();
    archive_entry *entry = archive_entry_new();

    WRP(archive_write_add_filter_gzip(arr));
    WRP(archive_write_set_format_pax_restricted(arr)); // recommended by authors of libarchive
    WRP(archive_write_open_filename(arr, out));

    while(*cur_file_p)
    {
        struct stat st = {};
        if (stat(*cur_file_p, &st) == -1)
        {
            err = errno;
            goto CleanUp;
        }

        archive_entry_copy_stat(entry, &st);
        archive_entry_set_pathname(entry, *cur_file_p);
        WRP(archive_write_header(arr, entry));

        file = fopen(*cur_file_p, "r");
        if (!file)
        {
            err = errno;
            goto CleanUp;
        }

        READ_CHK_ERR(buf, sizeof(buf), file, len, err);
        while (len > 0)
        {
            archive_write_data(arr, buf, len);
            READ_CHK_ERR(buf, sizeof(buf), file, len, err);  
        }

        FCLOSE(file);
        entry = archive_entry_clear(entry);
        cur_file_p++;
    }


CleanUp:
    if (file) FCLOSE(file);
    if (entry) archive_entry_free(entry);
    if (arr) archive_write_close(arr);
    if (arr) archive_write_free(arr);

    bool res = true;
    if (err) res = false;
    if (error_code_p) *error_code_p = err;
    return res;
}