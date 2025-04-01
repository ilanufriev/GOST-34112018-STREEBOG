// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

/**
    gost34112018_cli - a simple utility to compute a GOST 34.11-2018 hashsum of the
    given data.
 */

#include "gost34112018.h"
#include "stdio.h"
#include "stdint.h"
#include "errno.h"
#include "stdlib.h"
#include "argp.h"
#include "stdbool.h"
#include "sys/resource.h"
#include <time.h>

#define log_err(__fmt, ...) \
    fprintf(stderr, "[ERROR, %s] " __fmt "\n", __func__, ##__VA_ARGS__)

#define log_debug(__fmt, ...) \
    fprintf(stdout, "[DEBUG, %s] " __fmt "\n", __func__, ##__VA_ARGS__)

const char *argp_application_version = "gost34112018_cli ver. 0.1";
const char *argp_application_bug_address = "anufriewwi@rambler.ru";

int g_opt_hash_size     = 512;
bool g_opt_big_endian   = false;
bool g_opt_no_nline     = false;
bool g_opt_file_mode    = false;
char *g_filename        = NULL;

enum
{
    INTERNAL_BUFFER_SIZE = 65536,
    BLOCK_SIZE = 64,
};

static struct argp_option options[] = {
    {
        "hash-size",
        's',
        "HASH_SIZE",
        0,
        "Size of the hash (256 or 512). 512 by default.",
        0
    },
    {
        "big-endian",
        'b',
        0,
        0,
        "Print hash in big endian format (by default hash is printed in little endian format).",
        0
    },
    {
        "no-newline",
        'n',
        0,
        0,
        "Print hash with no newline character at the end.",
        0
    },
    {
        "file",
        'f',
        "FILENAME",
        0,
        "Compute hash of the file with name FILENAME.",
        0
    },
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    switch (key) {
        case 's':
            sscanf(arg, "%d", &g_opt_hash_size);
            break;
        case 'b':
            g_opt_big_endian = true;
            break;
        case 'n':
            g_opt_no_nline = true;
            break;
        case 'f':
            g_opt_file_mode = true;
            g_filename = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *fin = NULL;

    int32_t rc = 0;
    uint8_t hash[BLOCK_SIZE];
    static uint8_t buffer[INTERNAL_BUFFER_SIZE];
    struct GOST34112018_Context ctx;

    struct argp argp = { options, parse_opt, "[ARGS...]", "My program description.", 0, 0, 0 };
    error_t argp_error = argp_parse(&argp, argc, argv, 0, 0, 0);
    if (argp_error != 0)
    {
        exit(EINVAL);
    }

    if (g_opt_file_mode)
    {
        fin = fopen(g_filename, "rb");
        if (!fin)
        {
            fprintf(stderr, "Could not open file %s", g_filename);
            exit(ENOENT);
        }
    }
    else
    {
        fin = stdin;
    }

    if (g_opt_hash_size == 512)
    {
        GOST34112018_InitContext(&ctx, GOST34112018_Hash512);
    }
    else if (g_opt_hash_size == 256)
    {
        GOST34112018_InitContext(&ctx, GOST34112018_Hash256);
    }
    else
    {
        log_err("Unsupported hash size");
        exit(EINVAL);
    }

    while (!feof(fin))
    {
        rc = fread(buffer + rc, 1, INTERNAL_BUFFER_SIZE - rc, fin);
        if (rc < 0 && (errno == EINTR))
        {
            continue;
        }

        if (rc < 0)
        {
            log_err("An error occurred while trying to read data");
            exit(EIO);
        }

        // In some cases fread may not read all 64 bytes. This makes sure it does
        if (rc < INTERNAL_BUFFER_SIZE)
        {
            continue;
        }

        for (int i = 0; i < rc; i += BLOCK_SIZE)
        {
            GOST34112018_HashBlock(buffer + i, BLOCK_SIZE, &ctx);
        }
        rc = 0;
    }

    // we hit feof, but rc is not empty
    if (rc != 0)
    {
        int i;
        for (i = 0; (i + BLOCK_SIZE) < rc; i += BLOCK_SIZE)
        {
            GOST34112018_HashBlock(buffer + i, BLOCK_SIZE, &ctx);
        }

        // Hash the rest of the message
        GOST34112018_HashBlock(buffer + i, rc - i, &ctx);
    }

    // Finish the hashing process correctly
    GOST34112018_HashBlockEnd(&ctx);

    GOST34112018_GetHashFromContext(&ctx, hash);

    if (g_opt_big_endian)
    {
        for (int i = 0; i < (g_opt_hash_size / 8); i++)
        {
            printf("%02x", hash[(g_opt_hash_size / 8) - 1 - i]);
        }
    }
    else
    {
        for (int i = 0; i < (g_opt_hash_size / 8); i++)
        {
            printf("%02x", hash[i]);
        }
    }

    if (!g_opt_no_nline)
    {
        putc('\n', stdout);
    }

    if (g_opt_file_mode)
        fclose(fin);

    return 0;
}
