#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib.h"
#include "typedef.h"
#include "query_base.h"



void query1(char *query_sentense, int *document_id, float *score)
{
    int query_num, i;
    char **query;
    float *weight;
    split(&query, query_sentense, &query_num);
    weight = (float *)malloc(sizeof(float) * query_num);
    for(i = 0; i < query_num; i++)
        weight[i] = 1;
    _query(query, weight, query_num, document_id, score);
}

void _start1()
{
    tree = (Trie **)malloc(domain_num * sizeof(Trie *));
    char **file_list = (char **)malloc(domain_num * sizeof(char *));
    file_list[0] = "out/title.txt";
    file_list[1] = "out/abstract.txt";
    file_list[2] = "out/content.txt";
    for (int i = 0; i < domain_num; i++)
        tree[i] = _trie_init(), deserialize(tree[i], file_list[i]);
}