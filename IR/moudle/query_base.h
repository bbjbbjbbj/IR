#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib.h"
#include "typedef.h"
#include "build.h"

void _query(char **query, float *weight, int query_num, int *document_id, float *score)
{
    printf("querying...\n");
    for(int i = 0; i < query_num; i++)
        printf("%s\t%f\t%d\n", query[i], weight[i], i);
    Index *temp = NULL;
    for (int i = 0; i < MAX_DOCUMENT; i++)
        document_id[i] = i, score[i] = 0;
    // 每个部分的得分和计数器
    int *temp_coord = (int *)malloc(MAX_DOCUMENT * sizeof(int));   // 不用初始化
    float *temp_score = (float *)malloc(MAX_DOCUMENT * sizeof(float)); // 不用初始化
    // 考虑多个domain
    for (int i = 0; i < domain_num; i++)
    {
        // 因为每个domain都有不同的coord和score，所以要进行初始化
        for (int j = 0; j < MAX_DOCUMENT; j++)
            temp_coord[j] = 0, temp_score[j] = 0;
        for (int j = 0; j < query_num; j++)
        {
            temp = _get_trie_node(tree[i], query[j])->index;
            if (temp == NULL)
                continue;
            // 根据到排索引计算得分
            for (int k = 0; k < temp->size; k++)
                temp_score[temp->document_id[k]] += temp->tf_idf[k] * weight[j], temp_coord[temp->document_id[k]]++;
        }
        // 本domain得分为tf_idf得分*coord得分*domain的权重
        for (int j = 0; j < MAX_DOCUMENT; j++)
            score[j] += temp_coord[j] * temp_score[j] * weights[i];
    }

    // 排序
    quick_sort(document_id, score, 0, MAX_DOCUMENT - 1);

    // 归一化
    if (score[0] == 0)
        return;
    float sqrt_max_score = sqrt(score[0]);
    for (int i = 0; i < MAX_DOCUMENT; i++)
    {
        if (score[i] == 0)
            break;
        score[i] = sqrt(score[i]) / sqrt_max_score;
    }
}