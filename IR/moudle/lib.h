#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "typedef.h"

// 快速排序(key为关键字, id为附加信息)
void quick_sort(int *id, float *key, int low, int high)
{
    if (low >= high)
        return;
    int key1 = id[low];
    float key2 = key[low];
    int i = low, j = high;
    int temp1;
    float temp2;
    while (i < j)
    {
        while (i < j && key2 >= key[j])
            j--;
        while (i < j && key2 <= key[i])
            i++;
        if (i < j)
        {
            temp1 = id[i];
            temp2 = key[i];
            id[i] = id[j];
            key[i] = key[j];
            id[j] = temp1;
            key[j] = temp2;
            i++, j--;
        }
        if (i >= j)
        {
            if (key2 > key[j])
                j--;
            id[low] = id[j];
            key[low] = key[j];
            id[j] = key1;
            key[j] = key2;
        }
    }
    quick_sort(id, key, low, j - 1);
    quick_sort(id, key, j + 1, high);
}

// 判断两个字符串是否相等
int same_str(char *s1, char *s2)
{
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0')
    {
        if (s1[i] != s2[i])
            return 0;
        i++;
    }
    if (s2[i] == s1[i])
        return 1;
    return 0;
}

// 按空格拆分
void split(char ***split_result, char *sentense, int *split_num)
{
    if(sentense == NULL || sentense[0] == '\0')
    {
        *split_num = 0;
        *split_result = NULL;
        return;
    }
    int word_num = 1;
    for (int i = 0; sentense[i] != '\0'; i++)
        if (sentense[i] == ' ')
            word_num += 1;
    char **words = (char **)malloc(word_num * sizeof(char *));
    for (int i = 0; i < word_num; i++)
        words[i] = (char *)malloc(MAX_LENGTH * sizeof(char));
    int i = 0, j = 0, k = 0;
    for (; sentense[i] != '\0'; i++)
    {
        if (sentense[i] != ' ')
            words[j][k++] = sentense[i];
        else
            words[j][k] = '\0', j++, k = 0;
    }
    words[j][k] = '\0';
    *split_result = words, *split_num = word_num;
}

// 将str2拷贝到str1
void str_copy(char *str1, char *str2)
{
    while(*str1++ = *str2++);
}