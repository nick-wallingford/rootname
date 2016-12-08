#pragma once

#include <inttypes.h>
#include <stdio.h>

/**
 * Parses a file, and reads a single unsigned 64 bit integer from it.
 * read_int() rewinds the file before reading.
 *
 * @param f A file descriptor to read from.
 * @returns The integer read from the file. Returns SIZE_MAX if any errors occur.
 */
uint64_t read_int(FILE *restrict f);
