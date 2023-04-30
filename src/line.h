#include <stdio.h>
#include <stdlib.h>

#include "line_chunk.h"

#ifndef LINE_H
#define LINE_H

typedef struct {
    struct LineChunk* line_chunks_head;
    struct LineChunk* curr_line_chunk;

    struct Line* next_line;
} Line;

Line* new_line(LineChunk* line_chunk) {
    Line* new_line = calloc(1, sizeof(Line));

    if (new_line == NULL) {
        return NULL;
    }

    new_line->line_chunks_head = (struct LineChunk*) line_chunk;
    new_line->curr_line_chunk = (struct LineChunk*) line_chunk;

    new_line->next_line = NULL;

    return new_line;
}

Line* append_line(Line* curr_line, LineChunk* line_chunk) {
    Line* next_line = new_line(line_chunk);

    if (next_line == NULL) {
        return NULL;
    }

    if (curr_line == NULL) {
        curr_line = next_line;
    } else {
        curr_line->next_line = (struct Line*) next_line;
    }

    return next_line;
}

void set_line_chunks_head(Line* line) {
    if (line != NULL && line->line_chunks_head == NULL) {
        line->line_chunks_head = line->curr_line_chunk;
    }
}

void print_line(Line* line, int spacing, char spacing_char) {
    if (line == NULL) {
        return;
    }

    LineChunk* curr_line_chunk = (LineChunk*) line->line_chunks_head;

    while (curr_line_chunk != NULL) {
        print_line_chunk(curr_line_chunk);

        for (int i = 0; i < spacing; i++) {
            printf("%c", spacing_char);
        }

        curr_line_chunk = (LineChunk*) curr_line_chunk->next_line_chunk;
    }

    printf("\n");
}

void free_lines(Line* line) {
    if (line == NULL) {
        return;
    }

    free_lines((Line*) line->next_line);

    free_line_chunks((LineChunk*) line->line_chunks_head);

    free(line);
}

#endif