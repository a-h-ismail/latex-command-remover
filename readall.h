/*
Copyright (C) 2025 Ahmad Ismail
SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef READALL_H
#define READALL_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/**
 * @brief Reads all contents of the input_file into the text buffer. Stores the total file size in total_bytes_read.
 * @return 0 on success, -1 if any error occurs.
 */
int fread_all(FILE *input_file, char **text, size_t *total_bytes_read);

#endif