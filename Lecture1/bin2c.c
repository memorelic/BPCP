/*
 *  bin2c - compresses data files & converts the result to C source code
 *  Copyright (C) 1998-2000  Anders Widell  <awl@hem.passagen.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * This command uses the zlib library to compress each file given on
 * the command line, and outputs the compressed data as C source code
 * to the file 'data.c' in the current directory
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_LIBZ
#include <zlib.h>
#else
typedef unsigned char Bytef;
typedef unsigned long uLongf;
#endif

#define BUFSIZE 16384            /* Increase buffer size by this amount */

#define SUFFIXLEN 8



static const char *programName="";
static const char *usage =
"\nUsage: ./bin2c -o <output-file> file1 [file2 [file3 [...]]]\n\n"
"    Example: ./bin2c -o data.c a.bmp b.jpg c.png\n\n";


typedef struct _export_list {
char                *export_data;
struct _export_list *next;
} export_list_t;

static export_list_t *exports_head = NULL;

struct file_info {
char    **input_file_list;
int     input_file_count;
char    *output_file;
};



/*
 * Replacement for strrchr in case it isn't present in libc
 *
 */
static char *my_strrchr(char *s, int c)
{
        char *ptr = NULL;

        while (*s)
        {
                if (*s == c)
                {
                        ptr = s;
                }

                s++;
        }

        return ptr;
}

#ifdef USE_LIBZ
/*
 * NOTE: my_compress2 is taken directly from zlib 1.1.3
 *
 * This is for compability with early versions of zlib that
 * don't have the compress2 function.
 *
 */

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int my_compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
{
        z_stream stream;
        int err;

        stream.next_in = (Bytef*)source;
        stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
        /* Check for source > 64K on 16-bit machine: */
        if ((uLong)stream.avail_in != sourceLen)
        {
                return Z_BUF_ERROR;
        }
#endif
        stream.next_out = dest;
        stream.avail_out = (uInt)*destLen;
        if ((uLong)stream.avail_out != *destLen)
        {
                return Z_BUF_ERROR;
        }

        stream.zalloc = (alloc_func)0;
        stream.zfree = (free_func)0;
        stream.opaque = (voidpf)0;

        err = deflateInit(&stream, level);
        if (err != Z_OK)
        {
                return err;
        }

        err = deflate(&stream, Z_FINISH);
        if (err != Z_STREAM_END)
        {
                deflateEnd(&stream);
                return err == Z_OK ? Z_BUF_ERROR : err;
        }
        *destLen = stream.total_out;

        err = deflateEnd(&stream);
        return err;
}
#endif


struct file_info *create_file_info(int input_file_count)
{
        struct file_info *ret = NULL;

        ret = (file_info*)malloc(sizeof(file_info));

        if (ret)
        {
                ret->input_file_list = (char**)calloc(input_file_count, sizeof(char*));
                ret->input_file_count = input_file_count;
                ret->output_file = (char*)malloc(sizeof(char*));
        }

        return ret;
}

void destory_file_info(file_info *info)
{
        if (info != NULL)
        {
                for (int i = 0; i < info->input_file_count; i++)
                {
                        free(info->input_file_list + i);
                }

                free(info->output_file);
        }
}



static const char *add_export(const char *filename)
{
	int idx = 0;
	int i;
	const char* begin;
	const char* ext;
        static char strname[1024];
	begin = strrchr(filename, '/');
	if (begin == NULL)
        {
                begin = filename;
        }

	ext = strrchr(begin, '.');
	i = (ext ? (ext-begin) : strlen(begin)) + 10;
	//char* strname = (char*)malloc(i);

	strname[idx++] = '_';
	if(ext)
        {
		for(i=1; ext[i]; i++)
                {
                        strname[idx++] = ext[i];
                }

		strname[idx++] = '_';
	}

	for(i=0; (ext && &begin[i] < ext) || (!ext && begin[i]); i++)
	{
		if(isalnum(begin[i]))
                {
                        strname[idx++] = begin[i];
                }
		else
                {
                        strname[idx++] = '_';
                }

	}

	if(strname[idx-1] == '_')
        {
                strcpy(strname+idx, "data");
        }
	else
        {
                strcpy(strname+idx, "_data");
        }


	export_list_t *els = (export_list_t*)calloc(1, sizeof(export_list_t));
	els->export_data = strname;
	els->next = exports_head;
	exports_head = els;

	return strname;
}

static void print_exports(FILE *f)
{
	export_list_t *els = exports_head;

	while (els)
	{
		fprintf(f,"\t%s\n", els->export_data);
		els = els->next;
	}
}

static int parser_args(int argc, char *argv[], struct file_info *info)
{
	int i;
	int list_idx = 0;

	if(argc < 4)
	{
		goto parse_failed;
	}


	i=1;

	while(i<argc)
	{
		switch(argv[i][0])
		{
		case '-':
			if(argv[i][1] == 'o')
                        {
				info->output_file = argv[++i];
				break;
			}
			else if(argv[i][1] == 'h' || argv[i][1] == '?')
                        {
				goto parse_failed;
			}
		default:
			info->input_file_list[i] = argv[i];
			break;
		}

		i++;
	}

        parse_failed:
                return 0;
        parse_success:
	        return 1;
}

static int dump_files(struct file_info *info)
{
        FILE *outfile = NULL;
        FILE *infile = NULL;

        /* Buffer containing uncompressed data */
        Bytef *source=NULL;
        /* Buffer containing compressed data */
        Bytef *dest=NULL;
        /* uncompressed Buffer size */
        uLongf sourceBufSize = 0;
        /* Length of uncompressed data */
        uLongf sourceLen = 0;
        /* Length of compressed data */
        uLongf destLen = 0;

#ifdef USE_LIBZ
        int result;
        /* compressed Buffer size */
        uLongf destBufSize = 0;
#endif
        int i;
        unsigned j;

        outfile = fopen(info->output_file, "w");

        if (outfile == NULL)
        {
                goto open_output_file_failed;
        }

        /* Process each file given on command line */
        for (i = 0; i < argc - 3; i++)
        {
                infile = fopen(info->input_file_list[i], "rb");

                if (infile == NULL)
                {
                        goto open_input_file_failed;
                }

                while (!feof(infile))
                {
                        if (sourceLen + BUFSIZE > sourceBufSize)
                        {
                                sourceBufSize += BUFSIZE;
                                source = realloc(source, sourceBufSize);
                                if (source == NULL)
                                {
                                        goto memory_exhausted;

                                }
                        }

                        sourceLen += fread(source + sourceLen, 1, BUFSIZE, infile);

                        if (ferror(infile))
                        {
                                goto error_input_reading;
                        }
                }

                fclose(infile);

#ifdef USE_LIBZ

                /* (Re)allocate dest buffer */
                destLen = sourceBufSize + (sourceBufSize + 9) / 10 + 12;
                if (destBufSize < destLen)
                {
                        destBufSize = destLen;
                        dest = realloc(dest, destBufSize);
                        if (dest == NULL)
                        {
                                goto compressed_memory_exhausted;
                        }
                }

                /* Compress dest buffer */
                destLen = destBufSize;
                result = my_compress2(dest, &destLen, source, sourceLen, 9);
                if (result != Z_OK)
                {
                        goto error_compress;
                }

#else

                destLen = sourceLen;
                dest = source;

#endif

                /* Output dest buffer as C source code to outfile */

                fprintf(outfile, "static const unsigned char %s[] = {\n",
                                add_export(info->input_file_list[i]));

                for (j=0; j<destLen-1; j++)
                {
                        switch (j%8)
                        {
                        case 0:
                                fprintf(outfile, "  0x%02x, ",
                                        ((unsigned) dest[j]) & 0xffu);
                                break;
                        case 7:
                                fprintf(outfile, "0x%02x,\n",
                                        ((unsigned) dest[j]) & 0xffu);
                                break;
                        default:
                                fprintf(outfile, "0x%02x, ",
                                        ((unsigned) dest[j]) & 0xffu);
                                break;
                        }
                }

                if ((destLen-1)%8 == 0)
                {
                        fprintf(outfile, "  0x%02x\n};\n\n",
                                ((unsigned) dest[destLen-1]) & 0xffu);
                }
                else
                {
                        fprintf(outfile, "0x%02x\n};\n\n",
                                ((unsigned) dest[destLen-1]) & 0xffu);
                }
        }

        fprintf(outfile, "/*********************************************\n");
        fprintf(outfile, "Export:\n");
        print_exports(outfile);
        fprintf(outfile,"**********************************************/\n");

        fclose (outfile);
#ifdef USE_LIBZ
        free (dest);
#endif
        free (source);

        return 0;

#ifdef USE_LIBZ
        error_compress:
                fclose(outfile);
                fclose(infile);
                free(source);
                free(dest);
                fprintf(stderr, "error compressing '%s'\n",
                                info->input_file_list[i]);
                return 1;
        compressed_memory_exhausted:
                fclose(outfile);
                fclose(infile);
                free(source);
                free(dest);
                fprintf(stderr, "memory exhausted.\n");
                return 1;
#endif
        error_input_reading:
                fclose(outfile);
                fclose(infile);
                free(source);
                fprintf(stderr, "error reading '%s'\n",
                                info->input_file_list[i]);
                return 1;
        memory_exhausted:
                fclose(outfile);
                fclose(infile);
                fprintf(stderr, "memory exhausted.\n");
                return 1;
        open_input_file_failed:
                fclose(outfile);
                fprintf(stderr, "can't open '%s' for reading",
                                info->input_file_list[i]);
                return 1;
        open_output_file_failed:
                fprintf(stderr, "%s: can't open '%s' for writing\n",
                                argv[0], info->output_file);
                return 1;
}


int main(int argc, char *argv[])
{
        programName = argv[0];

        struct file_info *info = create_file_info(argc - 3);

        if (info == NULL)
        {
                goto lack_of_memory:
        }

        if (!parser_args(argc, argv, info))
        {
                goto parse_error;
        }

        int failed = dump_files(info);
        destory_file_info(info);
        return failed;

        parse_error:
                destory_file_info(info);
                printf(usage);
                return 1;
        lack_of_memory:
                fprintf(stderr, "memory exhausted.\n");
                return 1;
}
