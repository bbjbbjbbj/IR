#pragma once
#include <stdlib.h>
#include "typedef.h"

Trie *_trie_init()
{
    Trie *temp = (Trie *)malloc(sizeof(Trie));
    temp->index = NULL;
    for (int i = 0; i < MAX_ALPHABET; i++)
        temp->next[i] = NULL;
    return temp;
}

Index *_index_init()
{
    Index *temp = (Index *)malloc(sizeof(Index));
    temp->id = term_count, term_count++;
    temp->size = 0;
    temp->max_size = 10;
    temp->pos_size = 0;
    temp->pos_max_size = 50;
    temp->document_id = (int *)malloc(temp->max_size * sizeof(int));
    temp->tf_idf = (float *)malloc(temp->max_size * sizeof(float));
    temp->term_count = (int *)malloc(temp->max_size * sizeof(int));
    temp->item_position = (int *)malloc(temp->pos_max_size * sizeof(int));
    return temp;
}

Link *_link_init(char *s)
{
    Link *temp = (Link *)malloc(sizeof(Link));
    temp->size = 0;
    temp->max_size = 10;
    temp->position = (int *)malloc(10 * sizeof(int));
    temp->next = NULL;
    temp->word = (char *)malloc(MAX_LENGTH * sizeof(char));
    int j = 0;
    while (s[j] != '\0')
        temp->word[j] = s[j], j++;
    temp->word[j] = '\0';
    return temp;
}

PermutedTrie *_permuted_trie_init()
{
    PermutedTrie *temp = (PermutedTrie *)malloc(sizeof(PermutedTrie));
    temp->origin = NULL;
    for (int i = 0; i < PERMUTED_ALPHABET; i++)
        temp->next[i] = NULL;
    return temp;
}

Stem *_stem_init(char *s, int weight)
{
    Stem *temp = (Stem *)malloc(sizeof(Stem));
    temp->weight = weight;
    int i = 0;
    while (s[++i] != '\0')
        ;
    temp->word = (char *)malloc(sizeof(char) * (i + 1));
    for (int j = 0; j <= i; j++)
        temp->word[j] = s[j];
    return temp;
}