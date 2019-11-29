#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib.h"
#include "typedef.h"
#include "query_base.h"

#define S_THRESHOLD 1
#define M_THRESHOLD 10
#define L_THRESHOLD 30

static float domain_weights[] = {0.6, 0.3, 0.1};
static float shreshold_weights[] = {0.6, 0.3, 0.1};

void query4(char *normal_query, char *other_query, char *responsee, int *document_id, float *score);
void _phrase_query(Index **index, int query_num, int threshold, float gain, float *score);


void query4(char *normal_query, char *other_query, char *responsee, int *document_id, float *score)
{
    char **query = NULL;
    int query_num = 0;
    split(&query, normal_query, &query_num);
    printf("querying...\n");
    for(int i = 0; i < query_num; i++)
        printf("%s\n", query[i]);
    Index **index = NULL;
    int pp = 1;
    // 初始化
    for (int i = 0; i < MAX_DOCUMENT; i++)
        document_id[i] = i, score[i] = 0;
    for(int i = 0; i < domain_num; i++)
    {
        index = (Index **)malloc(sizeof(Index *) * query_num);
        pp = 1;
        for (int j = 0; j < query_num; j++)
        {
            index[j] = _get_trie_node(tree[i], query[j])->index;
            if (index[j] == NULL)
            {
                pp = 0;
                break;
            }
        }
        // 如果该单词不在域中, 则不再进行计算
        if(!pp)
            continue;
        
        _phrase_query(index, query_num, S_THRESHOLD, shreshold_weights[0] * domain_weights[i], score);
        _phrase_query(index, query_num, M_THRESHOLD, shreshold_weights[1] * domain_weights[i], score);
        _phrase_query(index, query_num, L_THRESHOLD, shreshold_weights[2] * domain_weights[i], score);
    }
    quick_sort(document_id, score, 0, MAX_DOCUMENT - 1);
}

void _phrase_query(Index **index, int query_num, int threshold, float gain, float *score)
{
    // 不对score进行初始化
    // 哪些文档出现了足够的次数
    int flag[MAX_DOCUMENT];
    for (int i = 0; i < MAX_DOCUMENT; i++)
        flag[i] = 0;
    for (int i = 0; i < query_num; i++)
        for (int j = 0; j < index[i]->size; j++)
            flag[index[i]->document_id[j]]++;
    // 分别记录start和end
    int start_index[MAX_DOCUMENT][query_num], end_index[MAX_DOCUMENT][query_num]; // 不用初始化
    for (int i = 0; i < query_num; i++)
    {
        for (int j = 0; j < index[i]->size; j++)
            if (flag[index[i]->document_id[j]] == query_num)
            {
                start_index[index[i]->document_id[j]][i] = j > 0 ? index[i]->term_count[j - 1] : 0;
                end_index[index[i]->document_id[j]][i] = index[i]->term_count[j];
            }
    }
    int window_start = 0;
    for (int i = 0; i < MAX_DOCUMENT; i++)
    {
        // 如果记录个数不满足, 则直接查找下一个文档
        if(!(flag[i] == query_num))
            continue;
        for(; start_index[i][0] < end_index[i][0]; start_index[i][0]++)
        {
            window_start = index[0]->item_position[start_index[i][0]];
            int p = 1;
            for(int j = 1; j < query_num; j++)
            {
                while(start_index[i][j] < end_index[i][j] && index[j]->item_position[start_index[i][j]] < window_start)
                    start_index[i][j]++;
                // 如果检查到了边界或超过了阈值
                if(start_index[i][j] == end_index[i][j] || index[j]->item_position[start_index[i][j]] - window_start > threshold)
                {
                    p = 0;
                    break;
                }else
                {
                    window_start = index[j]->item_position[start_index[i][j]];
                }
            }
            if(p)
                score[i] += gain;
        }
    }
}

void _start4(){
    if(tree == NULL)
        _start1();
}