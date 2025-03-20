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

const char *argp_application_version = "gost34112018_cli ver. 0.1";
const char *argp_application_bug_address = "anufriewwi@rambler.ru";

int g_opt_hash_size = 512;
bool g_opt_big_endian = false;
bool g_opt_no_nline = false;

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
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

#define log_err(__fmt, ...) \
    fprintf(stderr, "[ERROR, %s] " __fmt "\n", __func__, ##__VA_ARGS__)

#define log_debug(__fmt, ...) \
    fprintf(stdout, "[DEBUG, %s] " __fmt "\n", __func__, ##__VA_ARGS__)

enum
{
    BLOCK_SIZE = 64,
};

int main(int argc, char **argv)
{
    int32_t rc = 0;
    uint8_t buffer[BLOCK_SIZE];
    uint8_t hash[BLOCK_SIZE];
    struct GOST34112018_Context ctx;

    struct argp argp = { options, parse_opt, "[ARGS...]", "My program description.", 0, 0, 0 };
    error_t argp_error = argp_parse(&argp, argc, argv, 0, 0, 0);
    if (argp_error != 0)
    {
        exit(EINVAL);
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

    while (!feof(stdin))
    {
        rc = fread(buffer + rc, 1, BLOCK_SIZE - rc, stdin);
        if (rc < 0 && (errno == EINTR))
        {
            continue;
        }

        if (rc < 0)
        {
            log_err("An error occurred while trying to read data from stdin");
            exit(EIO);
        }

        // In some cases fread may not read all 64 bytes. This makes sure it does
        if (rc < BLOCK_SIZE)
        {
            continue;
        }

        log_debug("Read block of size %d", rc);
        GOST34112018_HashBlock(buffer, rc, &ctx);
        rc = 0;
    }

    if (rc != 0)
    {
        log_debug("Read block of size %d", rc);
        GOST34112018_HashBlock(buffer, rc, &ctx);
    }

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

    return 0;
}
