#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

void string_replace(char* string, char target, char replacement) {
    if (string == NULL) {
        return;
    }

    int len = strlen(string);

    for (int i = 0; i < len; i++) {
        if (string[i] == target) {
            string[i] = replacement;
        }
    }
}

bool is_char(char character) {
    return character != ' ' && character != '\n' && character != '\0' && character != EOF && character != '\t';
    // return character > 32;
}

void pad_string(char* string, int start, int len, char character) {
    for (int i = start; i < len; i++) {
        string[i] = character;
    }
}

bool is_utf8(unsigned char character) {
    return character >> 6 == 0b10;
}

bool check_truncated_end(char* line_chunk_content, int w_col, char next_char) {
    return is_char(line_chunk_content[w_col - 1]) && (is_char(next_char) || is_utf8(next_char));
}

char* truncate_string(char* string, int new_size) {
    int original_size = strlen(string);

    if (original_size <= new_size) {
        return string;
    }

    int amount = original_size - new_size;

    if (amount > original_size) {
        return string;
    }

    char* truncated_string = calloc(new_size + 1, sizeof(char));

    if (truncated_string == NULL) {
        return string;
    }

    strncpy(truncated_string, string, new_size);

    free(string);

    return truncated_string;
}

int replace_truncated_chars(char* line_chunk_content, int* w_col) {
    int truncated_chars = 0;

    for (int i = *w_col - 1; is_char(line_chunk_content[i]) && i >= 0; i--) {
        if (is_utf8(line_chunk_content[i])) {
            *w_col -= 1;
        }

        line_chunk_content[i] = ' ';

        truncated_chars++;
    }

    return truncated_chars;
}

bool no_spaces(char* string, int len) {
    for (int i = 0; i < len; i++) {
        if (string[i] == ' ') {
            return false;
        }
    }

    return true;
}

int round_division(int x, int y) {
    return (x + (y / 2)) / y;
}

int count_words(char* string, int len) {
    bool in_word = false;

    int words = 0;

    for (int i = 0; i < len; i++) {
        char curr_char = string[i];
        bool is_space = is_char(curr_char);

        if (is_space && in_word) {
            words++;

            in_word = false;
        } else if (!is_space && !in_word) {
            in_word = true;
        }
    }

    if (in_word) {
        words++;
    }

    return words;
}

int min(int x, int y) {
    return (x < y) ? x : y;
}

int evaluate_curr_spaces(int spaces_end, int spaces_inside, int spaces_done, int ratio) {
    return ratio + 1 == spaces_end && spaces_done + 1 == spaces_inside ? ratio + 1 : min(spaces_end, ratio);
}

void enqueue(char queue[], int len, int* head, int* tail, char element) {
    if (*tail >= 0 && *tail < len - 1) {
        queue[*tail] = element;

        *tail += 1;
    }
}

char dequeue(char queue[], int len, int* head, int* tail) {
    if (*head < *tail) { // TODO: avrebbe comunque senso mettere i controlli anche qui though
        char element = queue[*head];

        queue[*head] = '\0';

        *head += 1;

        return element;
    } else {
        return '\0';
    }
}

void slide_characters(char* string, int len, int spaces_end, int spaces_inside, int ratio) {
    int head = 0;
    int tail = 0;

    char queue[len];

    pad_string(queue, 0, len, '\0');

    int spaces_done = 0;

    if (!strcmp(string, "solo il carattere    ")) {
        printf("%s\n", string);
    }

    for (int i = 1; i < len; i++) {
        char DEBUG = string[i];
        if (tail != 0) {
            if ((is_char(string[i]) && !is_char(queue[tail - 1])) || !is_char(queue[head])) {
                while (is_char(queue[head])) {
                    enqueue(queue, len, &head, &tail, string[i]);

                    string[i] = dequeue(queue, len, &head, &tail);

                    i++;
                }

                enqueue(queue, len, &head, &tail, string[i]);
                string[i] = dequeue(queue, len, &head, &tail);
                i++;
                
                int curr_spaces = evaluate_curr_spaces(spaces_end, spaces_inside, spaces_done, ratio);

                spaces_end -= curr_spaces;

                while (curr_spaces > 0) {
                    enqueue(queue, len, &head, &tail, string[i]);

                    string[i] = ' ';

                    curr_spaces--;

                    i++;
                }

                spaces_done++;

                i--;
            } else {
                enqueue(queue, len, &head, &tail, string[i]);

                string[i] = dequeue(queue, len, &head, &tail);
            }
        } else {
            if (is_char(string[i]) && !is_char(string[i - 1])) {
                int curr_spaces = evaluate_curr_spaces(spaces_end, spaces_inside, spaces_done, ratio);

                spaces_end -= curr_spaces;

                while (curr_spaces > 0) {
                    enqueue(queue, len, &head, &tail, string[i]);

                    string[i] = ' ';

                    curr_spaces--;

                    i++;
                }

                spaces_done++;

                i--;
            }
        }
    }
}

void justify_string(char* string, int len) {
    int spaces_end = 0;

    for (int i = len - 1; string[i] == ' ' && i >= 0; i--) {
        spaces_end++;
    }

    if (spaces_end == 0) {
        return;
    }

    int spaces_inside = count_words(string, len) - 1;

    // printf("%d ", spaces_inside);

    // TODO: ricordati di scrivere che è garantito
    // che gli space chunks abbiano esattamente 1 spazio
    if (spaces_inside == 0) { // una sola parola
        return;
    }

    int ratio = round_division(spaces_end, spaces_inside);

    if (ratio == 0) { // TODO: scrivere che questo è un fix che risolve quando il ratio fa 0
        ratio = 1;
    }

    slide_characters(string, len, spaces_end, spaces_inside, ratio);
}

#endif