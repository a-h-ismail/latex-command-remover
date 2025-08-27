/*
Copyright (C) 2025 Ahmad Ismail
SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "readall.h"
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>

#define array_length(z) (sizeof(z) / sizeof(*z))

// A simple strtok wrapper to support use of read-only strings
char *strtok_m(char *_s, const char *delimiter)
{
    static char *text = NULL;
    // Initialization
    if (_s != NULL)
    {
        free(text);
        text = strdup(_s);
    }
    return strtok((_s == NULL ? NULL : text), delimiter);
}

// Finds the closing symbol for the specified opening symbol
// Ignores escaped symbols
size_t find_closing(char *text, size_t i, char opening, char closing)
{
    // Can't find a closing for no opening
    if (text[i] != opening)
        return -1;

    // Initializing depth to 1 because the function receives the index of an open symbol
    size_t depth = 1;
    while (text[i] != '\0' && depth != 0)
    {
        // Skipping over the first bracket
        ++i;
        // Avoid escaped brackets
        if (text[i - 1] != '\\')
        {
            if (text[i] == opening)
                ++depth;
            else if (text[i] == closing)
                --depth;
        }
    }
    // Case where the open symbol has no closing one, return -1 to the calling function
    if (depth != 0)
        return -1;
    else
        return i;
}

// Finds the closing bracket } for the specified open bracket {
size_t find_closing_bracket(char *text, size_t i)
{
    return find_closing(text, i, '{', '}');
}

// Finds the closing square bracket ] for the specified open square bracket [
size_t find_closing_sb(char *text, size_t i)
{
    return find_closing(text, i, '[', ']');
}

// Forward search the keyword in the string, ignoring escaped matches
// Returns the first match starting from "index" or -1 if nothing is found
size_t f_search(char *str, char *keyword, size_t index)
{
    char *match;
    while (1)
    {
        // Search for the needle in the haystack
        match = strstr(str + index, keyword);
        if (match == NULL)
            return -1;
        else
        {
            size_t i = match - str;
            // skip latex escaped matches
            if (i > 0 && str[i - 1] == '\\')
                index = i + strlen(keyword);
            else
                return match - str;
        }
    }
}

int find_arg_bounds(char *text, size_t first_arg, int arg_index, size_t *start, size_t *end)
{
    // Find bounds of the first argument
    if (arg_index == 0)
    {
        *start = first_arg;
        *end = find_closing_bracket(text, first_arg);
        if (*end == -1)
            return -1;
        else
            return 0;
    }

    // Find bounds of the second argument or more
    // Initialize to end of the first argument
    size_t s, e = find_closing_bracket(text, first_arg);
    for (int curr_arg = 0; curr_arg != arg_index; ++curr_arg)
    {
        s = e + 1;
        e = find_closing_bracket(text, s);
        // Incomplete command
        if (e == -1)
            return -1;
    }

    *start = s;
    *end = e;
    return 0;
}

char *remove_tex_command(const char *command, char *text, bool delete_content, int arg_to_keep)
{
    size_t txt_len = strlen(text);
    // Length of the command including the starting backslash
    int cmd_len = strlen(command) + 1;
    char cmd_text[cmd_len + 1];
    // Add a backslash to the command name
    cmd_text[0] = '\\';
    strcpy(cmd_text + 1, command);

    char *cleaned_text = malloc(txt_len + 1 * sizeof(char));

    size_t orig_i = 0, new_i = 0, cmd_start, cmd_end, bytes_to_copy, args_start;
    while ((cmd_start = f_search(text, cmd_text, orig_i)) != -1)
    {
        // Copy the relevant text before the command (between the last copied index and the current match)
        bytes_to_copy = cmd_start - orig_i;
        strncpy(cleaned_text + new_i, text + orig_i, bytes_to_copy);
        new_i += bytes_to_copy;

        // Continue processing from here
        switch (text[cmd_start + cmd_len])
        {
            // Command without options
        case '{':
            args_start = cmd_start + cmd_len;
            break;

            // Command with options
        case '[':
            args_start = find_closing_sb(text, cmd_start + cmd_len) + 1;

            if (args_start == 0)
            {
                puts("Syntax error, expected a closing ]");
                exit(1);
            }
            // Has options but no arguments
            if (text[args_start] != '{')
            {
                orig_i = args_start + 1;
                continue;
            }
            else
                break;

        // Command without arguments or options
        case ' ':
            orig_i = cmd_start + cmd_len;
            continue;

        default:
            // Not an exact match of the command, move to the next match
            orig_i += cmd_len;
            continue;
        }

        // Find the end of the last argument
        size_t next_start = args_start;
        while (1)
        {
            cmd_end = find_closing_bracket(text, next_start);

            if (cmd_end == -1)
            {
                puts("fatal error, unclosed {");
                exit(1);
            }

            // Loop to find the end of the last argument
            if (text[cmd_end + 1] != '{')
                break;
            else
                next_start = cmd_end + 1;
        }

        // Keep only the specified argument as requested
        if (!delete_content)
        {
            // Target argument start/end
            size_t start, end;
            int status = find_arg_bounds(text, args_start, arg_to_keep, &start, &end);
            if (status == -1)
            {
                fputs("Error while processing text, likely unclosed { or the argument to preserve doesn't exist\n", stderr);
                exit(1);
            }
            bytes_to_copy = (end - 1) - (start + 1) + 1;
            strncpy(cleaned_text + new_i, text + start + 1, bytes_to_copy);
            new_i += bytes_to_copy;
        }
        orig_i = cmd_end + 1;
    }

    // Finalization: Copy the remaining text
    strcpy(cleaned_text + new_i, text + orig_i);
    return cleaned_text;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        puts("Usage: apply_tex_changes texfile.tex [command1:action1:preserved_index1 ...]\n");
        puts("The action could be d (delete) or p (preserve)");
        puts("Preserved index is relevant to \"preserve\" mode, indicating which argument content to keep (zero indexed)\n");
        puts("If no command is specified, the default is to remove the easyReview and changes LateX packages commands while applying the stated changes.");
        return EXIT_SUCCESS;
    }

    char *default_commands[] = {"added:p", "deleted:d", "replaced:p:0", "comment:d", "highlight:d", "add:p", "remove:d", "replace:p:1"};
    char **command_list;
    int total_commands;

    // Use default values (for the changes package)
    if (argc == 2)
    {
        command_list = default_commands;
        total_commands = array_length(default_commands);
    }
    else
    {
        command_list = argv + 2;
        total_commands = argc - 2;
    }

    FILE *input_file = NULL, *output_file = NULL;

    input_file = fopen(argv[1], "r");
    if (input_file == NULL)
    {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    char *file_contents;
    size_t len;
    int status = readall(input_file, &file_contents, &len);
    if (status != READALL_OK)
    {
        printf("Failed to read the input file, reason %d\n", status);
        return EXIT_FAILURE;
    }
    fclose(input_file);

    char *out, *cmd_name, *op_char, *arg_to_keep_str;
    bool delete_content;
    int arg_to_keep;

    // Process commands according to the provided command line arguments
    for (int i = 0; i < total_commands; ++i)
    {
        cmd_name = strtok_m(command_list[i], ":");

        op_char = strtok_m(NULL, ":");
        if (op_char == NULL || strlen(op_char) > 1)
        {
            fprintf(stderr, "Expected a single operation letter in argument \"%s\"\n", command_list[i]);
            return EXIT_FAILURE;
        }

        // Set the deletion flag
        switch (op_char[0])
        {
        case 'd':
            delete_content = true;
            break;
        case 'p':
            delete_content = false;
            break;

        default:
            fprintf(stderr, "Invalid operation character in argument \"%s\"\n", command_list[i]);
            return EXIT_FAILURE;
        }

        // Read the index of the argument to keep (index starts at 0)
        arg_to_keep_str = strtok_m(NULL, ":");
        if (arg_to_keep_str == NULL)
            arg_to_keep = 0;
        else
        {
            if (sscanf(arg_to_keep_str, "%d", &arg_to_keep) != 1)
            {
                fprintf(stderr, "Invalid argument to preserve value in \"%s\"\n", command_list[i]);
                return EXIT_FAILURE;
            }
            else if (arg_to_keep < 0)
            {
                fprintf(stderr, "Argument to preserve value in \"%s\" should be > 0\n", command_list[i]);
                return EXIT_FAILURE;
            }
        }

        // Execute the remove algorithm
        out = remove_tex_command(cmd_name, file_contents, delete_content, arg_to_keep);

        // free the old text and place the new text pointer in its place
        // preparing for the next iteration where a new text block will be allocated
        free(file_contents);
        file_contents = out;
    }

    // Conversion complete, rename the old file and write the new file
    char output_filename[strlen(argv[1]) + 30], *input_filename = argv[1];
    char time_str[22];
    sprintf(time_str, ".%lld", (long long)time(NULL));

    // Building the output filename
    strcpy(output_filename, argv[1]);
    strcat(output_filename, time_str);
    strcat(output_filename, ".bak");

    // Rename the original file to avoid data loss if something wrong happened
    status = rename(input_filename, output_filename);

    // Failed to rename the original file
    if (status != 0)
    {
        perror("Failed to rename the source file");
        return EXIT_FAILURE;
    }

    output_file = fopen(input_filename, "w");
    if (output_file == NULL)
    {
        perror("Failed to write the output file");

        // Restore the original file then
        status = rename(output_filename, input_filename);
        // Failed to rename the backup file back to the original name
        if (status != 0)
            perror("Failed to rename the backup file");

        return EXIT_FAILURE;
    }

    fwrite(out, 1, strlen(out), output_file);
    fclose(output_file);

    // Silence leak checkers
    free(file_contents);

    return EXIT_SUCCESS;
}