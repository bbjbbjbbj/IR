#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "lib.h"
#include "typedef.h"
#include "query_base.h"
#include "build.h"

void query2(char *normal_sentence, char *match_sentence, char *response, int *document_id, float *score);
void _permuted_match(PermutedLink *link, char *query_word);
void _sp_permuted_match(PermutedTrie *root, PermutedLink *link, char *s, int level);
void _start2();




void query2(char *normal_sentence, char *match_sentence, char *response, int *document_id, float *score)
{
    PermutedLink *link, *cur;
    int normal_num, match_num, norm_query_num;
    int i = 0, j = 0, k = 0, flag = 0;
    char **normal_query, **match_query;
    split(&normal_query, normal_sentence, &normal_num);
    split(&match_query, match_sentence, &match_num);
    printf("%s\t%d\n%s\t%d\n", normal_sentence, normal_num, match_sentence, match_num);
    norm_query_num = normal_num + MAX_FINDING;
    char **norm_query = (char **)malloc(sizeof(char *) * norm_query_num);
    for (i = 0; i < normal_num; i++)
        norm_query[i] = normal_query[i];
    float *weight = (float *)malloc(sizeof(float) * norm_query_num);
    for (i = 0; i < normal_num; i++)
        weight[i] = 1;
    for (i = normal_num; i < norm_query_num; i++)
        weight[i] = MATCH_WEIGHT;

    link = (PermutedLink *)malloc(sizeof(PermutedLink)), link->next = NULL, link->origin_word = NULL, link->stem = NULL;
    for (i = 0; i < match_num; i++)
        _permuted_match(link, match_query[i]);

    i = 0, j = normal_num, flag = 0, cur = link->next;
    while (cur != NULL && i < MAX_FINDING)
    {
        k = 0;
        while (cur->origin_word[k] != '\0')
            response[flag++] = cur->origin_word[k], k++;
        response[flag++] = ' ';
        norm_query[j] = cur->stem->word;
        j++, i++;
        cur = cur->next;
    }
    response[flag] = '\0'; // 打上结束标签
    norm_query_num = j;    // 重置查询个数
    // 调用基本查询
    _query(norm_query, weight, norm_query_num, document_id, score);
}

void _permuted_match(PermutedLink *link, char *query_word)
{
    char norm_query_word[MAX_LENGTH];
    int i = 0, j = 0, k;
    while (query_word[i] != '\0') // 计算单词长度
        i++;
    while (query_word[j] != '*') // 计算通配符位置
        j++;
    {
        for (k = 0; k < i - j - 1; k++)
            norm_query_word[k] = query_word[k + j + 1];
        norm_query_word[i - j - 1] = 'z' + 1;
        for (k = 0; k < j; k++)
            norm_query_word[k + i - j] = query_word[k];
        norm_query_word[i] = '\0';
    } // 生成标准查询单词
    _sp_permuted_match(_get_permuted_trie_node(permutedTrie, norm_query_word), link, norm_query_word, i);
}

void _sp_permuted_match(PermutedTrie *root, PermutedLink *link, char *s, int level)
{
    int i, j, flag;
    PermutedLink *cur, *pre, *temp;
    if (root == NULL)
        return;
    if (root->origin != NULL)
    {

        char *stem_word = root->origin->word;
        int weight = root->origin->weight;
        cur = link->next, pre = link;
        // 判断该词项是否已经在考虑列表中
        flag = 1;
        while (cur != NULL)
        {
            if (same_str(cur->stem->word, stem_word))
            {
                flag = 0;
                break;
            }
            if (weight >= cur->stem->weight)
                break;
            pre = cur, cur = cur->next;
        }
        if (flag)
        {
            temp = (PermutedLink *)malloc(sizeof(PermutedLink));
            temp->next = pre->next;
            pre->next = temp;
            temp->origin_word = (char *)malloc(sizeof(char) * (level));
            temp->stem = root->origin;
            // 生成原单词
            {
                s[level] = '\0';
                i = 0;
                while (s[i] != 'z' + 1)
                    i++;
                char origin_word[MAX_LENGTH];
                for (j = 0; j < level - i - 1; j++)
                    origin_word[j] = s[j + i + 1];
                for (j = 0; j < i; j++)
                    origin_word[j + level - i - 1] = s[j];
                origin_word[level - 1] = '\0';
                str_copy(temp->origin_word, origin_word);
            }
            
        }
    }
    // 递归查找
    for (int i = 0; i < PERMUTED_ALPHABET; i++)
        s[level] = 'a' + i, _sp_permuted_match(root->next[i], link, s, level + 1);
}

void _start2()
{
    if(tree == NULL)
        _start1();
    FILE *f = fopen("./out/dictionary.txt", "r");
    permutedTrie = _permuted_trie_init();
    char origin_word[MAX_LENGTH];
    char stem_word[MAX_LENGTH];
    int weight;
    while (fscanf(f, "%s", origin_word) != EOF)
    {
        fscanf(f, "%s", stem_word);
        fscanf(f, "%d", &weight);
        update(permutedTrie, origin_word, stem_word, weight);
    }
    fclose(f);
}