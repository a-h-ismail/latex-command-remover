/*
Copyright (C) 2025 Ahmad Ismail
SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "readall.h"

int fread_all(FILE *input_file, char **text, size_t *total_bytes_read)
{
    // All parameters received should point to valid variables
    if (input_file == NULL || text == NULL || total_bytes_read == NULL)
        return -1;

    // Make sure we are at the file start
    if (ferror(input_file) == 0)
        rewind(input_file);
    else
        return -1;

    size_t next_char_index = 0;
    // Read text in blocks of 1MB
    size_t batch_size = 1e6, bytes_in_batch;
    char *txt_tmp = malloc(batch_size * sizeof(char));

    while (1)
    {
        bytes_in_batch = fread(txt_tmp + next_char_index, 1, batch_size, input_file);

        // An error occured?
        if (ferror(input_file) != 0)
        {
            free(txt_tmp);
            return -1;
        }
        // Reading completed
        if (bytes_in_batch == 0 || feof(input_file) != 0)
        {
            *total_bytes_read = next_char_index + bytes_in_batch;
            txt_tmp[*total_bytes_read] = '\0';
            *text = realloc(txt_tmp, *(total_bytes_read + 1) * sizeof(char));
            return 0;
        }
        else
        {
            // More bytes to go
            next_char_index += bytes_in_batch;
            txt_tmp = realloc(txt_tmp, (next_char_index + batch_size) * sizeof(char));
        }
    }
}
