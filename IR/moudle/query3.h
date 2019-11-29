#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib.h"
#include "typedef.h"
#include "query_base.h"

#define LOW2 0.1
#define DICTIONARY_MAX_LENGTH 20
#define DICTIONARY_MAX_ITEM 18000

static char origin_word[DICTIONARY_MAX_LENGTH * DICTIONARY_MAX_ITEM];
static char stem_word[DICTIONARY_MAX_LENGTH * DICTIONARY_MAX_ITEM];
static int origin_word_length[DICTIONARY_MAX_ITEM];
static int dictionary_item = 0;

void query3(char *normal_query, char *correcting_query, char *response, int *document_id, float *score);
void _spell_check(char **query, int query_num, char *o_word, char *s_word);
int _levenshtein_distance(char *str1, char *str2, int str1_len, int str2_len, int *array1, int *array2);
void _start3();

void query3(char *normal_query, char *correcting_query, char *response, int *document_id, float *score)
{
    int i, s;
    char **normal = NULL, **correcting = NULL;
    int number1 = 0, number2 = 0;
    split(&normal, normal_query, &number1), split(&correcting, correcting_query, &number2);
    char origin_word[1000], stem_word[1000];
    _spell_check(correcting, number2, origin_word, stem_word);
    // 生成标准查询语句和查询反馈结果
    int query_num = number1 + 10;
    char **query = (char **)malloc(sizeof(char *) * query_num);
    for (i = 0; i < number1; i++)
        query[i] = normal[i];
    for (i = 0; i < 10; i++)
        query[number1 + i] = stem_word + i * 20;
    float weight[query_num];
    for (i = 0; i < number1; i++)
        weight[i] = 1;
    for (i = 0; i < 10; i++)
        weight[i + number1] = LOW2;
    s = 0;
    for (int i = 0; i < 10; i++)
    {
        for (int k = 0; origin_word[20 * i + k] != '\0'; k++)
            response[s++] = origin_word[20 * i + k];
        response[s++] = ' ';
    }
    response[s - 1] = '\0';
    _query(query, weight, query_num, document_id, score);
}

// 拼写校验 并取前10个结果
void _spell_check(char **query, int query_num, char *o_word, char *s_word)
{
    int str_len[query_num];
    for (int i = 0; i < query_num; i++)
        str_len[i] = 0;
    int temp1[DICTIONARY_MAX_ITEM], temp2[DICTIONARY_MAX_ITEM];
    int total_item = dictionary_item * query_num;
    for (int i = 0; i < query_num; i++)
        while (query[i][str_len[i]] != '\0')
            str_len[i]++;
    float similarity[total_item];
    int similarity_id[total_item];
    for (int i = 0; i < query_num; i++)
        for (int j = 0; j < dictionary_item; j++)
        {
            similarity_id[dictionary_item * i + j] = j;
            similarity[dictionary_item * i + j] = _levenshtein_distance(origin_word + 20 * j, query[i], origin_word_length[j], str_len[i], temp1, temp2);
        }
    quick_sort(similarity_id, similarity, 0, total_item - 1);
    int count = 0;
    int i = total_item - 1;

    while (count < 10)
    {
        if (origin_word_length[similarity_id[i]] > 3)
        {
            str_copy(o_word + 20 * count, origin_word + 20 * similarity_id[i]);
            str_copy(s_word + 20 * count, stem_word + 20 * similarity_id[i]);
            for (int k = 0; k < count; k++)
                if (same_str(s_word + DICTIONARY_MAX_LENGTH * count, s_word + DICTIONARY_MAX_LENGTH * k))
                    count--; // count 最多减少一次
            count++;
        }
        i--;
    }
}
// 最小编辑距离 节省空间的算法
int _levenshtein_distance(char *str1, char *str2, int str1_len, int str2_len, int *array1, int *array2)
{
    int temp = 0, cost1 = 0, cost2 = 0, cost3 = 0;
    int *past = array1, *cur = array2, *swap;
    int i, j;
    for (i = 0; i < str1_len + 1; i++)
        cur[i] = i;
    for (i = 1; i < str2_len + 1; i++)
    {
        swap = cur, cur = past, past = swap;
        cur[0] = i;
        for (j = 1; j < str1_len + 1; j++)
        {
            if (str2[i - 1] == str1[j - 1])
                cur[j] = past[j - 1];
            else
            {
                cost1 = cur[j - 1] + 1, cost2 = past[j] + 1, cost3 = past[j - 1] + 2;
                temp = cost1 > cost2 ? cost2 : cost1;
                cur[j] = temp > cost3 ? cost3 : temp;
            }
        }
    }
    return cur[str1_len];
}

void _start3()
{
    if(tree == NULL)
        _start1();
    FILE *f = fopen("./out/dictionary.txt", "r");
    int weight, count = 0;
    char *o_word = origin_word, *s_word = stem_word;
    while (fscanf(f, "%s", o_word) != EOF)
    {
        fscanf(f, "%s", s_word);
        fscanf(f, "%d", &weight);
        int i = 0;
        while (o_word[i] != '\0')
            i++;
        origin_word_length[count] = i;
        count++, o_word += 20, s_word += 20;
    }
    dictionary_item = count;
    fclose(f);
}
