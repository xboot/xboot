/*
 * framework/xfs/archiver_zip.c
 */

#include <framework/xfs/platform.h>
#include <framework/xfs/archiver.h>
#include <zlib.h>

u16_t PHYSFS_swapULE16(u16_t x) { return(x); }
u32_t PHYSFS_swapULE32(u32_t x) { return(x); }

/*
 * A buffer of ZIP_READBUFSIZE is allocated for each compressed file opened,
 *  and is freed when you close the file; compressed data is read into
 *  this buffer, and then is decompressed into the buffer passed to
 *  PHYSFS_read().
 *
 * Uncompressed entries in a zipfile do not allocate this buffer; they just
 *  read data directly into the buffer passed to PHYSFS_read().
 *
 * Depending on your speed and memory requirements, you should tweak this
 *  value.
 */
#define ZIP_READBUFSIZE   (16 * 1024)


/*
 * Entries are "unresolved" until they are first opened. At that time,
 *  local file headers parsed/validated, data offsets will be updated to look
 *  at the actual file data instead of the header, and symlinks will be
 *  followed and optimized. This means that we don't seek and read around the
 *  archive until forced to do so, and after the first time, we had to do
 *  less reading and parsing, which is very CD-ROM friendly.
 */
typedef enum
{
    ZIP_UNRESOLVED_FILE,
    ZIP_UNRESOLVED_SYMLINK,
    ZIP_RESOLVING,
    ZIP_RESOLVED,
    ZIP_BROKEN_FILE,
    ZIP_BROKEN_SYMLINK
} ZipResolveType;


/*
 * One ZIPentry is kept for each file in an open ZIP archive.
 */
typedef struct _ZIPentry
{
    char *name;                         /* Name of file in archive        */
    struct _ZIPentry *symlink;          /* NULL or file we symlink to     */
    ZipResolveType resolved;            /* Have we resolved file/symlink? */
    u32_t offset;               /* offset of data in archive      */
    u16_t version;              /* version made by                */
    u16_t version_needed;       /* version needed to extract      */
    u16_t compression_method;   /* compression method             */
    u32_t crc;                  /* crc-32                         */
    u32_t compressed_size;      /* compressed size                */
    u32_t uncompressed_size;    /* uncompressed size              */
    s64_t last_mod_time;        /* last file mod time             */
} ZIPentry;

/*
 * One ZIPinfo is kept for each open ZIP archive.
 */
typedef struct
{
    char *archiveName;        /* path to ZIP in platform-dependent notation. */
    u16_t entryCount; /* Number of files in ZIP.                     */
    ZIPentry *entries;        /* info on all files in ZIP.                   */
} ZIPinfo;

/*
 * One ZIPfileinfo is kept for each open file in a ZIP archive.
 */
typedef struct
{
    ZIPentry *entry;                      /* Info on file.              */
    void *handle;                         /* physical file handle.      */
    u32_t compressed_position;    /* offset in compressed data. */
    u32_t uncompressed_position;  /* tell() position.           */
    u8_t *buffer;                 /* decompression buffer.      */
    z_stream stream;                      /* zlib stream state.         */
} ZIPfileinfo;


/* Magic numbers... */
#define ZIP_LOCAL_FILE_SIG          0x04034b50
#define ZIP_CENTRAL_DIR_SIG         0x02014b50
#define ZIP_END_OF_CENTRAL_DIR_SIG  0x06054b50

/* compression methods... */
#define COMPMETH_NONE 0
/* ...and others... */


#define UNIX_FILETYPE_MASK    0170000
#define UNIX_FILETYPE_SYMLINK 0120000


/*
 * Bridge physfs allocation functions to zlib's format...
 */
static voidpf zlibPhysfsAlloc(voidpf opaque, uInt items, uInt size)
{
    return(malloc(items * size));
} /* zlibPhysfsAlloc */

/*
 * Bridge physfs allocation functions to zlib's format...
 */
static void zlibPhysfsFree(voidpf opaque, voidpf address)
{
    free(address);
} /* zlibPhysfsFree */


/*
 * Construct a new z_stream to a sane state.
 */
static void initializeZStream(z_stream *pstr)
{
    memset(pstr, '\0', sizeof (z_stream));
    pstr->zalloc = zlibPhysfsAlloc;
    pstr->zfree = zlibPhysfsFree;
    pstr->opaque = NULL;
} /* initializeZStream */


static const char *zlib_error_string(int rc)
{
    switch (rc)
    {
        case Z_OK: return(NULL);  /* not an error. */
        case Z_STREAM_END: return(NULL); /* not an error. */
#ifndef _WIN32_WCE
        case Z_ERRNO: return(strerror(errno));
#endif
        case Z_NEED_DICT: return("need dictionary");
        case Z_DATA_ERROR: return("data error");
        case Z_MEM_ERROR: return("memory error");
        case Z_BUF_ERROR: return("buffer error");
        case Z_VERSION_ERROR: return("version error");
        default: return("unknown error");
    } /* switch */

    return(NULL);
} /* zlib_error_string */


/*
 * Wrap all zlib calls in this, so the physfs error state is set appropriately.
 */
static int zlib_err(int rc)
{
    const char *str = zlib_error_string(rc);
    if (str != NULL)
        ;//xxx __PHYSFS_setError(str);
    return(rc);
} /* zlib_err */


/*
 * Read an unsigned 32-bit int and swap to native byte order.
 */
static int readui32(void *in, u32_t *val)
{
    u32_t v;
    if(__xfs_platform_read(in, &v, sizeof (v), 1) != 1)
    	return 0;
    *val = PHYSFS_swapULE32(v);
    return(1);
} /* readui32 */


/*
 * Read an unsigned 16-bit int and swap to native byte order.
 */
static int readui16(void *in, u16_t *val)
{
    u16_t v;
    if(__xfs_platform_read(in, &v, sizeof (v), 1) != 1)
    	return 0;
    *val = PHYSFS_swapULE16(v);
    return(1);
} /* readui16 */


static s64_t ZIP_read(void *opaque, void *buf,
                              u32_t objSize, u32_t objCount)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) opaque;
    ZIPentry *entry = finfo->entry;
    s64_t retval = 0;
    s64_t maxread = ((s64_t) objSize) * objCount;
    s64_t avail = entry->uncompressed_size -
                          finfo->uncompressed_position;

    if(maxread == 0)
    	return 0;

    if (avail < maxread)
    {
        maxread = avail - (avail % objSize);
        objCount = (u32_t) (maxread / objSize);
        if(objCount == 0)
        	return 0;
        //xxx __PHYSFS_setError(ERR_PAST_EOF);   /* this is always true here. */
    } /* if */

    if (entry->compression_method == COMPMETH_NONE)
    {
        retval = __xfs_platform_read(finfo->handle, buf, objSize, objCount);
    } /* if */

    else
    {
        finfo->stream.next_out = buf;
        finfo->stream.avail_out = objSize * objCount;

        while (retval < maxread)
        {
            u32_t before = finfo->stream.total_out;
            int rc;

            if (finfo->stream.avail_in == 0)
            {
                s64_t br;

                br = entry->compressed_size - finfo->compressed_position;
                if (br > 0)
                {
                    if (br > ZIP_READBUFSIZE)
                        br = ZIP_READBUFSIZE;

                    br = __xfs_platform_read(finfo->handle,
                                               finfo->buffer,
                                               1, (u32_t) br);
                    if (br <= 0)
                        break;

                    finfo->compressed_position += (u32_t) br;
                    finfo->stream.next_in = finfo->buffer;
                    finfo->stream.avail_in = (u32_t) br;
                } /* if */
            } /* if */

            rc = zlib_err(inflate(&finfo->stream, Z_SYNC_FLUSH));
            retval += (finfo->stream.total_out - before);

            if (rc != Z_OK)
                break;
        } /* while */

        retval /= objSize;
    } /* else */

    if (retval > 0)
        finfo->uncompressed_position += (u32_t) (retval * objSize);

    return(retval);
} /* ZIP_read */


static s64_t ZIP_write(void *opaque, const void *buf,
                               u32_t objSize, u32_t objCount)
{
	return -1;
}

static int ZIP_eof(void *opaque)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) opaque;
    return(finfo->uncompressed_position >= finfo->entry->uncompressed_size);
} /* ZIP_eof */


static s64_t ZIP_tell(void *opaque)
{
    return(((ZIPfileinfo *) opaque)->uncompressed_position);
} /* ZIP_tell */


static int ZIP_seek(void *opaque, u64_t offset)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) opaque;
    ZIPentry *entry = finfo->entry;
    void *in = finfo->handle;

    if(offset > entry->uncompressed_size)
    	return 0;

    if (entry->compression_method == COMPMETH_NONE)
    {
        s64_t newpos = offset + entry->offset;
        if(!__xfs_platform_seek(in, newpos))
        	return 0;
        finfo->uncompressed_position = (u32_t) offset;
    } /* if */

    else
    {
        /*
         * If seeking backwards, we need to redecode the file
         *  from the start and throw away the compressed bits until we hit
         *  the offset we need. If seeking forward, we still need to
         *  decode, but we don't rewind first.
         */
        if (offset < finfo->uncompressed_position)
        {
            /* we do a copy so state is sane if inflateInit2() fails. */
            z_stream str;
            initializeZStream(&str);
            if (zlib_err(inflateInit2(&str, -MAX_WBITS)) != Z_OK)
                return(0);

            if (!__xfs_platform_seek(in, entry->offset))
                return(0);

            inflateEnd(&finfo->stream);
            memcpy(&finfo->stream, &str, sizeof (z_stream));
            finfo->uncompressed_position = finfo->compressed_position = 0;
        } /* if */

        while (finfo->uncompressed_position != offset)
        {
            u8_t buf[512];
            u32_t maxread;

            maxread = (u32_t) (offset - finfo->uncompressed_position);
            if (maxread > sizeof (buf))
                maxread = sizeof (buf);

            if (ZIP_read(finfo, buf, maxread, 1) != 1)
                return(0);
        } /* while */
    } /* else */

    return(1);
} /* ZIP_seek */


static s64_t ZIP_fileLength(void *opaque)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) opaque;
    return(finfo->entry->uncompressed_size);
} /* ZIP_fileLength */


static int ZIP_fileClose(void *opaque)
{
    ZIPfileinfo *finfo = (ZIPfileinfo *) opaque;
    if(!__xfs_platform_close(finfo->handle))
    	return 0;

    if (finfo->entry->compression_method != COMPMETH_NONE)
        inflateEnd(&finfo->stream);

    if (finfo->buffer != NULL)
        free(finfo->buffer);

    free(finfo);
    return(1);
} /* ZIP_fileClose */


static s64_t zip_find_end_of_central_dir(void *in, s64_t *len)
{
    u8_t buf[256];
    u8_t extra[4] = { 0, 0, 0, 0 };
    s32_t i = 0;
    s64_t filelen;
    s64_t filepos;
    s32_t maxread;
    s32_t totalread = 0;
    int found = 0;

    filelen = __xfs_platform_length(in);
    if(filelen == -1)
    	return 0;
    if(filelen > 0xFFFFFFFF)
    	return 0;

    /*
     * Jump to the end of the file and start reading backwards.
     *  The last thing in the file is the zipfile comment, which is variable
     *  length, and the field that specifies its size is before it in the
     *  file (argh!)...this means that we need to scan backwards until we
     *  hit the end-of-central-dir signature. We can then sanity check that
     *  the comment was as big as it should be to make sure we're in the
     *  right place. The comment length field is 16 bits, so we can stop
     *  searching for that signature after a little more than 64k at most,
     *  and call it a corrupted zipfile.
     */

    if (sizeof (buf) < filelen)
    {
        filepos = filelen - sizeof (buf);
        maxread = sizeof (buf);
    } /* if */
    else
    {
        filepos = 0;
        maxread = (u32_t) filelen;
    } /* else */

    while ((totalread < filelen) && (totalread < 65557))
    {
        if(!__xfs_platform_seek(in, filepos))
        	return -1;

        /* make sure we catch a signature between buffers. */
        if (totalread != 0)
        {
            if (__xfs_platform_read(in, buf, maxread - 4, 1) != 1)
                return(-1);
            memcpy(&buf[maxread - 4], &extra, sizeof (extra));
            totalread += maxread - 4;
        } /* if */
        else
        {
            if (__xfs_platform_read(in, buf, maxread, 1) != 1)
                return(-1);
            totalread += maxread;
        } /* else */

        memcpy(&extra, buf, sizeof (extra));

        for (i = maxread - 4; i > 0; i--)
        {
            if ((buf[i + 0] == 0x50) &&
                (buf[i + 1] == 0x4B) &&
                (buf[i + 2] == 0x05) &&
                (buf[i + 3] == 0x06) )
            {
                found = 1;  /* that's the signature! */
                break;  
            } /* if */
        } /* for */

        if (found)
            break;

        filepos -= (maxread - 4);
        if (filepos < 0)
            filepos = 0;
    } /* while */

    if(!found)
    	return -1;

    if (len != NULL)
        *len = filelen;

    return(filepos + i);
} /* zip_find_end_of_central_dir */


static int ZIP_isArchive(const char *filename, int forWriting)
{
    u32_t sig;
    int retval = 0;
    void *in;

    in = __xfs_platform_open_read(filename);
    if(in == NULL)
    	return 0;

    /*
     * The first thing in a zip file might be the signature of the
     *  first local file record, so it makes for a quick determination.
     */
    if (readui32(in, &sig))
    {
        retval = (sig == ZIP_LOCAL_FILE_SIG);
        if (!retval)
        {
            /*
             * No sig...might be a ZIP with data at the start
             *  (a self-extracting executable, etc), so we'll have to do
             *  it the hard way...
             */
            retval = (zip_find_end_of_central_dir(in, NULL) != -1);
        } /* if */
    } /* if */

    __xfs_platform_close(in);
    return(retval);
} /* ZIP_isArchive */


static void zip_free_entries(ZIPentry *entries, u32_t max)
{
    u32_t i;
    for (i = 0; i < max; i++)
    {
        ZIPentry *entry = &entries[i];
        if (entry->name != NULL)
            free(entry->name);
    } /* for */

    free(entries);
} /* zip_free_entries */


/*
 * This will find the ZIPentry associated with a path in platform-independent
 *  notation. Directories don't have ZIPentries associated with them, but 
 *  (*isDir) will be set to non-zero if a dir was hit.
 */
static ZIPentry *zip_find_entry(ZIPinfo *info, const char *path, int *isDir)
{
    ZIPentry *a = info->entries;
    s32_t pathlen = strlen(path);
    s32_t lo = 0;
    s32_t hi = (s32_t) (info->entryCount - 1);
    s32_t middle;
    const char *thispath = NULL;
    int rc;

    while (lo <= hi)
    {
        middle = lo + ((hi - lo) / 2);
        thispath = a[middle].name;
        rc = strncmp(path, thispath, pathlen);

        if (rc > 0)
            lo = middle + 1;

        else if (rc < 0)
            hi = middle - 1;

        else /* substring match...might be dir or entry or nothing. */
        {
            int i;

            if (isDir != NULL)
            {
                *isDir = (thispath[pathlen] == '/');
                if (*isDir)
                    return(NULL);
            } /* if */

            if (thispath[pathlen] == '\0') /* found entry? */
                return(&a[middle]);

            /* substring match; search remaining space to find it... */
            for (i = lo; i < hi; i++)
            {
                thispath = a[i].name;
                if (strncmp(path, thispath, pathlen) == 0)
                {
                    if (isDir != NULL)
                    {
                        *isDir = (thispath[pathlen] == '/');
                        if (*isDir)
                            return(NULL);
                    } /* if */

                    if (thispath[pathlen] == '\0') /* found entry? */
                        return(&a[i]);
                } /* if */
            } /* for */
            break;

        } /* else */
    } /* while */

    if (isDir != NULL)
        *isDir = 0;

    return NULL;
} /* zip_find_entry */


/* Convert paths from old, buggy DOS zippers... */
static void zip_convert_dos_path(ZIPentry *entry, char *path)
{
    u8_t hosttype = (u8_t) ((entry->version >> 8) & 0xFF);
    if (hosttype == 0)  /* FS_FAT_ */
    {
        while (*path)
        {
            if (*path == '\\')
                *path = '/';
            path++;
        } /* while */
    } /* if */
} /* zip_convert_dos_path */


static void zip_expand_symlink_path(char *path)
{
    char *ptr = path;
    char *prevptr = path;

    while (1)
    {
        ptr = strchr(ptr, '/');
        if (ptr == NULL)
            break;

        if (*(ptr + 1) == '.')
        {
            if (*(ptr + 2) == '/')
            {
                /* current dir in middle of string: ditch it. */
                memmove(ptr, ptr + 2, strlen(ptr + 2) + 1);
            } /* else if */

            else if (*(ptr + 2) == '\0')
            {
                /* current dir at end of string: ditch it. */
                *ptr = '\0';
            } /* else if */

            else if (*(ptr + 2) == '.')
            {
                if (*(ptr + 3) == '/')
                {
                    /* parent dir in middle: move back one, if possible. */
                    memmove(prevptr, ptr + 4, strlen(ptr + 4) + 1);
                    ptr = prevptr;
                    while (prevptr != path)
                    {
                        prevptr--;
                        if (*prevptr == '/')
                        {
                            prevptr++;
                            break;
                        } /* if */
                    } /* while */
                } /* if */

                if (*(ptr + 3) == '\0')
                {
                    /* parent dir at end: move back one, if possible. */
                    *prevptr = '\0';
                } /* if */
            } /* if */
        } /* if */
        else
        {
            prevptr = ptr;
            ptr++;
        } /* else */
    } /* while */
} /* zip_expand_symlink_path */

/* (forward reference: zip_follow_symlink and zip_resolve call each other.) */
static int zip_resolve(void *in, ZIPinfo *info, ZIPentry *entry);

/*
 * Look for the entry named by (path). If it exists, resolve it, and return
 *  a pointer to that entry. If it's another symlink, keep resolving until you
 *  hit a real file and then return a pointer to the final non-symlink entry.
 *  If there's a problem, return NULL. (path) is always free()'d by this
 *  function.
 */
static ZIPentry *zip_follow_symlink(void *in, ZIPinfo *info, char *path)
{
    ZIPentry *entry;

    zip_expand_symlink_path(path);
    entry = zip_find_entry(info, path, NULL);
    if (entry != NULL)
    {
        if (!zip_resolve(in, info, entry))  /* recursive! */
            entry = NULL;
        else
        {
            if (entry->symlink != NULL)
                entry = entry->symlink;
        } /* else */
    } /* if */

    free(path);
    return(entry);
} /* zip_follow_symlink */


static int zip_resolve_symlink(void *in, ZIPinfo *info, ZIPentry *entry)
{
    char *path;
    u32_t size = entry->uncompressed_size;
    int rc = 0;

    /*
     * We've already parsed the local file header of the symlink at this
     *  point. Now we need to read the actual link from the file data and
     *  follow it.
     */

    if(!__xfs_platform_seek(in, entry->offset))
    	return 0;

    path = (char *) malloc(size + 1);
    if(path == NULL)
    	return 0;
    
    if (entry->compression_method == COMPMETH_NONE)
        rc = (__xfs_platform_read(in, path, size, 1) == 1);

    else  /* symlink target path is compressed... */
    {
        z_stream stream;
        u32_t complen = entry->compressed_size;
        u8_t *compressed = (u8_t*) malloc(complen);
        if (compressed != NULL)
        {
            if (__xfs_platform_read(in, compressed, complen, 1) == 1)
            {
                initializeZStream(&stream);
                stream.next_in = compressed;
                stream.avail_in = complen;
                stream.next_out = (unsigned char *) path;
                stream.avail_out = size;
                if (zlib_err(inflateInit2(&stream, -MAX_WBITS)) == Z_OK)
                {
                    rc = zlib_err(inflate(&stream, Z_FINISH));
                    inflateEnd(&stream);

                    /* both are acceptable outcomes... */
                    rc = ((rc == Z_OK) || (rc == Z_STREAM_END));
                } /* if */
            } /* if */
            free(compressed);
        } /* if */
    } /* else */

    if (!rc)
        free(path);
    else
    {
        path[entry->uncompressed_size] = '\0';    /* null-terminate it. */
        zip_convert_dos_path(entry, path);
        entry->symlink = zip_follow_symlink(in, info, path);
    } /* else */

    return(entry->symlink != NULL);
} /* zip_resolve_symlink */


/*
 * Parse the local file header of an entry, and update entry->offset.
 */
static int zip_parse_local(void *in, ZIPentry *entry)
{
    u32_t ui32;
    u16_t ui16;
    u16_t fnamelen;
    u16_t extralen;

    /*
     * crc and (un)compressed_size are always zero if this is a "JAR"
     *  archive created with Sun's Java tools, apparently. We only
     *  consider this archive corrupted if those entries don't match and
     *  aren't zero. That seems to work well.
     */

    if(!__xfs_platform_seek(in, entry->offset))
    	return 0;
    if(!readui32(in, &ui32))
    	return 0;
    if(ui32 != ZIP_LOCAL_FILE_SIG)
    	return 0;
    if(!readui16(in, &ui16))
    	return 0;
    if(ui16 != entry->version_needed)
    	return 0;
    if(!readui16(in, &ui16))
    	return 0;
    if(!readui16(in, &ui16))
    	return 0;
    if(ui16 != entry->compression_method)
    	return 0;
    if(!readui32(in, &ui32))
    	return 0;
    if(!readui32(in, &ui32))
    	return 0;
    if(ui32 && (ui32 != entry->crc))
    	return 0;
    if(!readui32(in, &ui32))
    	return 0;
    if(ui32 && (ui32 != entry->compressed_size))
    	return 0;
    if(!readui32(in, &ui32))
    	return 0;
    if(ui32 && (ui32 != entry->uncompressed_size))
    	return 0;
    if(!readui16(in, &fnamelen))
    	return 0;
    if(!readui16(in, &extralen))
    	return 0;

    entry->offset += fnamelen + extralen + 30;
    return(1);
} /* zip_parse_local */


static int zip_resolve(void *in, ZIPinfo *info, ZIPentry *entry)
{
    int retval = 1;
    ZipResolveType resolve_type = entry->resolved;

    /* Don't bother if we've failed to resolve this entry before. */
    if(resolve_type == ZIP_BROKEN_FILE)
    	return 0;
    if(resolve_type == ZIP_BROKEN_SYMLINK)
    	return 0;

    /* uhoh...infinite symlink loop! */
    if(resolve_type == ZIP_RESOLVING)
    	return 0;

    /*
     * We fix up the offset to point to the actual data on the
     *  first open, since we don't want to seek across the whole file on
     *  archive open (can be SLOW on large, CD-stored files), but we
     *  need to check the local file header...not just for corruption,
     *  but since it stores offset info the central directory does not.
     */
    if (resolve_type != ZIP_RESOLVED)
    {
        entry->resolved = ZIP_RESOLVING;

        retval = zip_parse_local(in, entry);
        if (retval)
        {
            /*
             * If it's a symlink, find the original file. This will cause
             *  resolution of other entries (other symlinks and, eventually,
             *  the real file) if all goes well.
             */
            if (resolve_type == ZIP_UNRESOLVED_SYMLINK)
                retval = zip_resolve_symlink(in, info, entry);
        } /* if */

        if (resolve_type == ZIP_UNRESOLVED_SYMLINK)
            entry->resolved = ((retval) ? ZIP_RESOLVED : ZIP_BROKEN_SYMLINK);
        else if (resolve_type == ZIP_UNRESOLVED_FILE)
            entry->resolved = ((retval) ? ZIP_RESOLVED : ZIP_BROKEN_FILE);
    } /* if */

    return(retval);
} /* zip_resolve */


static int zip_version_does_symlinks(u32_t version)
{
    int retval = 0;
    u8_t hosttype = (u8_t) ((version >> 8) & 0xFF);

    switch (hosttype)
    {
            /*
             * These are the platforms that can NOT build an archive with
             *  symlinks, according to the Info-ZIP project.
             */
        case 0:  /* FS_FAT_  */
        case 1:  /* AMIGA_   */
        case 2:  /* VMS_     */
        case 4:  /* VM_CSM_  */
        case 6:  /* FS_HPFS_ */
        case 11: /* FS_NTFS_ */
        case 14: /* FS_VFAT_ */
        case 13: /* ACORN_   */
        case 15: /* MVS_     */
        case 18: /* THEOS_   */
            break;  /* do nothing. */

        default:  /* assume the rest to be unix-like. */
            retval = 1;
            break;
    } /* switch */

    return(retval);
} /* zip_version_does_symlinks */


static int zip_entry_is_symlink(const ZIPentry *entry)
{
    return((entry->resolved == ZIP_UNRESOLVED_SYMLINK) ||
           (entry->resolved == ZIP_BROKEN_SYMLINK) ||
           (entry->symlink));
} /* zip_entry_is_symlink */


static int zip_has_symlink_attr(ZIPentry *entry, u32_t extern_attr)
{
    u16_t xattr = ((extern_attr >> 16) & 0xFFFF);

    return (
              (zip_version_does_symlinks(entry->version)) &&
              (entry->uncompressed_size > 0) &&
              ((xattr & UNIX_FILETYPE_MASK) == UNIX_FILETYPE_SYMLINK)
           );
} /* zip_has_symlink_attr */


static s64_t zip_dos_time_to_physfs_time(u32_t dostime)
{
//xxx jjj
	return -1;
#if 0
#ifdef _WIN32_WCE
    /* We have no struct tm and no mktime right now.
       FIXME: This should probably be fixed at some point.
    */
    return -1;
#else
    u32_t dosdate;
    struct tm unixtime;
    memset(&unixtime, '\0', sizeof (unixtime));

    dosdate = (u32_t) ((dostime >> 16) & 0xFFFF);
    dostime &= 0xFFFF;

    /* dissect date */
    unixtime.tm_year = ((dosdate >> 9) & 0x7F) + 80;
    unixtime.tm_mon  = ((dosdate >> 5) & 0x0F) - 1;
    unixtime.tm_mday = ((dosdate     ) & 0x1F);

    /* dissect time */
    unixtime.tm_hour = ((dostime >> 11) & 0x1F);
    unixtime.tm_min  = ((dostime >>  5) & 0x3F);
    unixtime.tm_sec  = ((dostime <<  1) & 0x3E);

    /* let mktime calculate daylight savings time. */
    unixtime.tm_isdst = -1;

    return((s64_t) mktime(&unixtime));
#endif
#endif
} /* zip_dos_time_to_physfs_time */


static int zip_load_entry(void *in, ZIPentry *entry, u32_t ofs_fixup)
{
    u16_t fnamelen, extralen, commentlen;
    u32_t external_attr;
    u16_t ui16;
    u32_t ui32;
    s64_t si64;

    /* sanity check with central directory signature... */
    if(!readui32(in, &ui32))
    	return 0;

    if(ui32 != ZIP_CENTRAL_DIR_SIG)
    	return 0;

    /* Get the pertinent parts of the record... */
    if(!readui16(in, &entry->version))
    	return 0;
    if(!readui16(in, &entry->version_needed))
    	return 0;
    if(!readui16(in, &ui16))
    	return 0;
    if(!readui16(in, &entry->compression_method))
    	return 0;
    if(!readui32(in, &ui32))
    	return 0;
    entry->last_mod_time = zip_dos_time_to_physfs_time(ui32);
    if(!readui32(in, &entry->crc))
    	return 0;
    if(!readui32(in, &entry->compressed_size))
    	return 0;
    if(!readui32(in, &entry->uncompressed_size))
    	return 0;
    if(!readui16(in, &fnamelen))
    	return 0;
    if(!readui16(in, &extralen))
    	return 0;
    if(!readui16(in, &commentlen))
    	return 0;
    if(!readui16(in, &ui16))
    	return 0;
    if(!readui16(in, &ui16))
    	return 0;
    if(!readui32(in, &external_attr))
    	return 0;
    if(!readui32(in, &entry->offset))
    	return 0;
    entry->offset += ofs_fixup;

    entry->symlink = NULL;  /* will be resolved later, if necessary. */
    entry->resolved = (zip_has_symlink_attr(entry, external_attr)) ?
                            ZIP_UNRESOLVED_SYMLINK : ZIP_UNRESOLVED_FILE;

    entry->name = (char *) malloc(fnamelen + 1);
    if(entry->name == NULL)
    	return 0;

    if (__xfs_platform_read(in, entry->name, fnamelen, 1) != 1)
        goto zip_load_entry_puked;

    entry->name[fnamelen] = '\0';  /* null-terminate the filename. */
    zip_convert_dos_path(entry, entry->name);

    si64 = __xfs_platform_tell(in);
    if (si64 == -1)
        goto zip_load_entry_puked;

        /* seek to the start of the next entry in the central directory... */
    if (!__xfs_platform_seek(in, si64 + extralen + commentlen))
        goto zip_load_entry_puked;

    return(1);  /* success. */

zip_load_entry_puked:
    free(entry->name);
    return(0);  /* failure. */
} /* zip_load_entry */


static int zip_entry_cmp(void *_a, size_t one, size_t two)
{
    if (one != two)
    {
        const ZIPentry *a = (const ZIPentry *)_a;
        return strcmp(a[one].name, a[two].name);
    } /* if */

    return 0;
} /* zip_entry_cmp */


static void zip_entry_swap(void *_a, size_t one, size_t two)
{
    if (one != two)
    {
        ZIPentry tmp;
        ZIPentry *first = &(((ZIPentry *) _a)[one]);
        ZIPentry *second = &(((ZIPentry *) _a)[two]);
        memcpy(&tmp, first, sizeof (ZIPentry));
        memcpy(first, second, sizeof (ZIPentry));
        memcpy(second, &tmp, sizeof (ZIPentry));
    } /* if */
} /* zip_entry_swap */


static int zip_load_entries(void *in, ZIPinfo *info,
                            u32_t data_ofs, u32_t central_ofs)
{
    u32_t max = info->entryCount;
    u32_t i;

    if(!__xfs_platform_seek(in, central_ofs))
    	return 0;

    info->entries = (ZIPentry *) malloc(sizeof (ZIPentry) * max);
    if(info->entries == NULL)
    	return 0;

    for (i = 0; i < max; i++)
    {
        if (!zip_load_entry(in, &info->entries[i], data_ofs))
        {
            zip_free_entries(info->entries, i);
            return(0);
        } /* if */
    } /* for */

    __xfs_platform_sort(info->entries, max, zip_entry_cmp, zip_entry_swap);
    return(1);
} /* zip_load_entries */


static int zip_parse_end_of_central_dir(void *in, ZIPinfo *info,
                                        u32_t *data_start,
                                        u32_t *central_dir_ofs)
{
    u32_t ui32;
    u16_t ui16;
    s64_t len;
    s64_t pos;

    /* find the end-of-central-dir record, and seek to it. */
    pos = zip_find_end_of_central_dir(in, &len);
    if(pos == -1)
    	return 0;

    if(!__xfs_platform_seek(in, pos))
    	return 0;

    /* check signature again, just in case. */
    if(!readui32(in, &ui32))
    	return 0;

    if(ui32 != ZIP_END_OF_CENTRAL_DIR_SIG)
    	return 0;

    /* number of this disk */
    if(!readui16(in, &ui16))
    	return 0;

    if(ui16 != 0)
    	return 0;

    /* number of the disk with the start of the central directory */
    if(!readui16(in, &ui16))
    	return 0;

    if(ui16 != 0)
    	return 0;

    /* total number of entries in the central dir on this disk */
    if(!readui16(in, &ui16))
    	return 0;

    /* total number of entries in the central dir */
    if(!readui16(in, &info->entryCount))
    	return 0;

    if(ui16 != info->entryCount)
    	return 0;

    /* size of the central directory */
    if(!readui32(in, &ui32))
    	return 0;

    /* offset of central directory */
    if(!readui32(in, central_dir_ofs))
    	return 0;

    if(pos < *central_dir_ofs + ui32)
    	return 0;

    /*
     * For self-extracting archives, etc, there's crapola in the file
     *  before the zipfile records; we calculate how much data there is
     *  prepended by determining how far the central directory offset is
     *  from where it is supposed to be (start of end-of-central-dir minus
     *  sizeof central dir)...the difference in bytes is how much arbitrary
     *  data is at the start of the physical file.
     */
    *data_start = (u32_t) (pos - (*central_dir_ofs + ui32));

    /* Now that we know the difference, fix up the central dir offset... */
    *central_dir_ofs += *data_start;

    /* zipfile comment length */
    if(!readui16(in, &ui16))
    	return 0;

    /*
     * Make sure that the comment length matches to the end of file...
     *  If it doesn't, we're either in the wrong part of the file, or the
     *  file is corrupted, but we give up either way.
     */
    if((pos + 22 + ui16) != len)
    	return 0;

    return(1);  /* made it. */
} /* zip_parse_end_of_central_dir */


static ZIPinfo *zip_create_zipinfo(const char *name)
{
    char *ptr;
    ZIPinfo *info = (ZIPinfo *) malloc(sizeof (ZIPinfo));
    if(info == NULL)
    	return NULL;
    memset(info, '\0', sizeof (ZIPinfo));

    ptr = (char *) malloc(strlen(name) + 1);
    if (ptr == NULL)
    {
        free(info);
        return NULL;
    } /* if */

    info->archiveName = ptr;
    strcpy(info->archiveName, name);
    return(info);
} /* zip_create_zipinfo */


static void *ZIP_openArchive(const char *name, int forWriting)
{
    void *in = NULL;
    ZIPinfo *info = NULL;
    u32_t data_start;
    u32_t cent_dir_ofs;

    if(forWriting)
    	return NULL;

    if ((in = __xfs_platform_open_read(name)) == NULL)
        goto zip_openarchive_failed;
    
    if ((info = zip_create_zipinfo(name)) == NULL)
        goto zip_openarchive_failed;

    if (!zip_parse_end_of_central_dir(in, info, &data_start, &cent_dir_ofs))
        goto zip_openarchive_failed;

    if (!zip_load_entries(in, info, data_start, cent_dir_ofs))
        goto zip_openarchive_failed;

    __xfs_platform_close(in);
    return(info);

zip_openarchive_failed:
    if (info != NULL)
    {
        if (info->archiveName != NULL)
            free(info->archiveName);
        free(info);
    } /* if */

    if (in != NULL)
        __xfs_platform_close(in);

    return(NULL);
} /* ZIP_openArchive */


static s32_t zip_find_start_of_dir(ZIPinfo *info, const char *path,
                                            int stop_on_first_find)
{
    s32_t lo = 0;
    s32_t hi = (s32_t) (info->entryCount - 1);
    s32_t middle;
    u32_t dlen = strlen(path);
    s32_t retval = -1;
    const char *name;
    int rc;

    if (*path == '\0')  /* root dir? */
        return(0);

    if ((dlen > 0) && (path[dlen - 1] == '/')) /* ignore trailing slash. */
        dlen--;

    while (lo <= hi)
    {
        middle = lo + ((hi - lo) / 2);
        name = info->entries[middle].name;
        rc = strncmp(path, name, dlen);
        if (rc == 0)
        {
            char ch = name[dlen];
            if ('/' < ch) /* make sure this isn't just a substr match. */
                rc = -1;
            else if ('/' > ch)
                rc = 1;
            else 
            {
                if (stop_on_first_find) /* Just checking dir's existance? */
                    return(middle);

                if (name[dlen + 1] == '\0') /* Skip initial dir entry. */
                    return(middle + 1);

                /* there might be more entries earlier in the list. */
                retval = middle;
                hi = middle - 1;
            } /* else */
        } /* if */

        if (rc > 0)
            lo = middle + 1;
        else
            hi = middle - 1;
    } /* while */

    return(retval);
} /* zip_find_start_of_dir */


/*
 * Moved to seperate function so we can use alloca then immediately throw
 *  away the allocated stack space...
 */
static void doEnumCallback(xfs_enumerate_callback cb, void *callbackdata,
                           const char *odir, const char *str, s32_t ln)
{
    char *newstr = malloc(ln + 1);
    if (newstr == NULL)
        return;

    memcpy(newstr, str, ln);
    newstr[ln] = '\0';
    cb(callbackdata, odir, newstr);
    free(newstr);
} /* doEnumCallback */


static void ZIP_enumerateFiles(void *opaque, const char *dname,
                               xfs_enumerate_callback cb,
                               const char *origdir, void *callbackdata)
{
    ZIPinfo *info = ((ZIPinfo *) opaque);
    s32_t dlen, dlen_inc, max, i;

    i = zip_find_start_of_dir(info, dname, 0);
    if (i == -1)  /* no such directory. */
        return;

    dlen = strlen(dname);
    if ((dlen > 0) && (dname[dlen - 1] == '/')) /* ignore trailing slash. */
        dlen--;

    dlen_inc = ((dlen > 0) ? 1 : 0) + dlen;
    max = (s32_t) info->entryCount;
    while (i < max)
    {
        char *e = info->entries[i].name;
        if ((dlen) && ((strncmp(e, dname, dlen) != 0) || (e[dlen] != '/')))
            break;  /* past end of this dir; we're done. */

        //xxx if ((omitSymLinks) && (zip_entry_is_symlink(&info->entries[i])))
        if ((1) && (zip_entry_is_symlink(&info->entries[i])))
            i++;
        else
        {
            char *add = e + dlen_inc;
            char *ptr = strchr(add, '/');
            s32_t ln = (s32_t) ((ptr) ? ptr-add : strlen(add));
            doEnumCallback(cb, callbackdata, origdir, add, ln);
            ln += dlen_inc;  /* point past entry to children... */

            /* increment counter and skip children of subdirs... */
            while ((++i < max) && (ptr != NULL))
            {
                char *e_new = info->entries[i].name;
                if ((strncmp(e, e_new, ln) != 0) || (e_new[ln] != '/'))
                    break;
            } /* while */
        } /* else */
    } /* while */
} /* ZIP_enumerateFiles */


static int ZIP_exists(void *opaque, const char *name)
{
    int isDir;    
    ZIPinfo *info = (ZIPinfo *) opaque;
    ZIPentry *entry = zip_find_entry(info, name, &isDir);
    return((entry != NULL) || (isDir));
} /* ZIP_exists */


static s64_t ZIP_getLastModTime(void *opaque,
                                        const char *name,
                                        int *fileExists)
{
    int isDir;
    ZIPinfo *info = (ZIPinfo *) opaque;
    ZIPentry *entry = zip_find_entry(info, name, &isDir);

    *fileExists = ((isDir) || (entry != NULL));
    if (isDir)
        return(1);  /* Best I can do for a dir... */

    if(entry == NULL)
    	return -1;
    return(entry->last_mod_time);
} /* ZIP_getLastModTime */


static int ZIP_isDirectory(void *opaque, const char *name, int *fileExists)
{
    ZIPinfo *info = (ZIPinfo *) opaque;
    int isDir;
    ZIPentry *entry = zip_find_entry(info, name, &isDir);

    *fileExists = ((isDir) || (entry != NULL));
    if (isDir)
        return(1); /* definitely a dir. */

    /* Follow symlinks. This means we might need to resolve entries. */
    if(entry == NULL)
    	return 0;

    if (entry->resolved == ZIP_UNRESOLVED_SYMLINK) /* gotta resolve it. */
    {
        int rc;
        void *in = __xfs_platform_open_read(info->archiveName);
        if(in == NULL)
        	return 0;
        rc = zip_resolve(in, info, entry);
        __xfs_platform_close(in);
        if (!rc)
            return(0);
    } /* if */

    if(entry->resolved == ZIP_BROKEN_SYMLINK)
    	return 0;
    if(entry->symlink == NULL)
    	return 0;

    return(zip_find_start_of_dir(info, entry->symlink->name, 1) >= 0);
} /* ZIP_isDirectory */


static int ZIP_isSymLink(void *opaque, const char *name, int *fileExists)
{
    int isDir;
    const ZIPentry *entry = zip_find_entry((ZIPinfo *) opaque, name, &isDir);
    *fileExists = ((isDir) || (entry != NULL));
    if(entry == NULL)
    	return 0;
    return(zip_entry_is_symlink(entry));
} /* ZIP_isSymLink */


static void *zip_get_file_handle(const char *fn, ZIPinfo *inf, ZIPentry *entry)
{
    int success;
    void *retval = __xfs_platform_open_read(fn);
    if(retval == NULL)
    	return NULL;

    success = zip_resolve(retval, inf, entry);
    if (success)
    {
        s64_t offset;
        offset = ((entry->symlink) ? entry->symlink->offset : entry->offset);
        success = __xfs_platform_seek(retval, offset);
    } /* if */

    if (!success)
    {
        __xfs_platform_close(retval);
        retval = NULL;
    } /* if */

    return(retval);
} /* zip_get_file_handle */


static void *ZIP_openRead(void *opaque, const char *fnm, int *fileExists)
{
    ZIPinfo *info = (ZIPinfo *) opaque;
    ZIPentry *entry = zip_find_entry(info, fnm, NULL);
    ZIPfileinfo *finfo = NULL;
    void *in;

    *fileExists = (entry != NULL);
    if(entry == NULL)
    	return NULL;

    in = zip_get_file_handle(info->archiveName, info, entry);
    if(in == NULL)
    	return NULL;

    finfo = (ZIPfileinfo *) malloc(sizeof (ZIPfileinfo));
    if (finfo == NULL)
    {
        __xfs_platform_close(in);
        return NULL;
    } /* if */

    memset(finfo, '\0', sizeof (ZIPfileinfo));
    finfo->handle = in;
    finfo->entry = ((entry->symlink != NULL) ? entry->symlink : entry);
    initializeZStream(&finfo->stream);
    if (finfo->entry->compression_method != COMPMETH_NONE)
    {
        if (zlib_err(inflateInit2(&finfo->stream, -MAX_WBITS)) != Z_OK)
        {
            ZIP_fileClose(finfo);
            return(NULL);
        } /* if */

        finfo->buffer = (u8_t *) malloc(ZIP_READBUFSIZE);
        if (finfo->buffer == NULL)
        {
            ZIP_fileClose(finfo);
            return NULL;
        } /* if */
    } /* if */

    return(finfo);
} /* ZIP_openRead */


static void *ZIP_openWrite(void *opaque, const char *filename)
{
	return NULL;
}

static void *ZIP_openAppend(void *opaque, const char *filename)
{
	return NULL;
}

static void ZIP_dirClose(void *opaque)
{
    ZIPinfo *zi = (ZIPinfo *) (opaque);
    zip_free_entries(zi->entries, zi->entryCount);
    free(zi->archiveName);
    free(zi);
}

static int ZIP_remove(void *opaque, const char *name)
{
	return 0;
}

static int ZIP_mkdir(void *opaque, const char *name)
{
	return 0;
}

const struct xfs_archiver_t __xfs_archiver_zip = {
	.extension			= "zip",
	.description		= "PkZip/WinZip/Info-Zip compatible",

	.is_archive			= ZIP_isArchive,
	.open_archive		= ZIP_openArchive,
	.enumerate			= ZIP_enumerateFiles,
	.exists				= ZIP_exists,
	.is_directory		= ZIP_isDirectory,
	.is_symlink			= ZIP_isSymLink,
	.get_last_modtime	= ZIP_getLastModTime,
	.open_read			= ZIP_openRead,
	.open_write			= ZIP_openWrite,
	.open_append		= ZIP_openAppend,
	.remove				= ZIP_remove,
	.mkdir				= ZIP_mkdir,
	.dir_close			= ZIP_dirClose,
	.read				= ZIP_read,
	.write				= ZIP_write,
	.eof				= ZIP_eof,
	.tell				= ZIP_tell,
	.seek				= ZIP_seek,
	.length				= ZIP_fileLength,
	.file_close			= ZIP_fileClose,
};
