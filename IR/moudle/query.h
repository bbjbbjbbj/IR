#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "build.h"
#include "query1.h"
#include "query2.h"
#include "query3.h"
#include "query4.h"

void start()
{
    _start1();
    _start2();
    _start3();
    _start4();
    printf("Initialization finished!\n");
}

int query(char *normal_query, char *other_query, char *response, int *document_id, float *score, int mode)
{
    if (tree == NULL)
        start();
    switch (mode)
    {
    case 1:
        query1(normal_query, document_id, score);
        break;
    case 2:
        query2(normal_query, other_query, response, document_id, score);
        break;
    case 3:
        query3(normal_query, other_query, response, document_id, score);
        break;
    case 4:
        query4(normal_query, other_query, response, document_id, score);
        break;
    default:
        break;
    }
    int i;
    for (i = 0; i < MAX_DOCUMENT; i++){
        if(score[i] < 0.01)
            break;
    }
    return i;
}

void quit()
{
    char **file_list = (char **)malloc(domain_num * sizeof(char *));
    file_list[0] = "out/title.txt";
    file_list[1] = "out/abstract.txt";
    file_list[2] = "out/content.txt";
    for (int i = 0; i < domain_num; i++)
        free_trie(tree[i]); // serialize(tree[i], file_list[i])
}