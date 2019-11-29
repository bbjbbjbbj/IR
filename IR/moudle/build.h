#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "typedef.h"
#include "lib.h"

// 外部可见
long long size_of(Trie *root);                // 显示占用内存大小
void serialize(Trie *root, char *filename);   // 保存到某一文件
void deserialize(Trie *root, char *filename); // 从某一文件恢复
void build_index(Trie *root, char *filename); // 根据某一文档建立索引
void free_trie(Trie *root);                   // 释放空间

// 以下为子过程
Trie *_get_trie_node(Trie *root, char *s);                                                                // 查找某个单词对应的节点(若不存在则创建)
void _update(Trie *root, char *s, int document_id, float tf_idf, int *item_position, int position_count); // 建立某一节点到某一文档的倒排索引
void _sp_serialize(Trie *root, char *s, int level, FILE *fp);                                             // 序列化子操作
void _caculate_tf_idf(Trie *root);                                                                        // 计算完整的tf_idf值
void _sp_build_index(Trie *root, int document_id, char *filename);                                        // 逐个建立索引

Trie *_get_trie_node(Trie *root, char *s)
{
    if (s[0] != '\0')
    {
        int off_set = s[0] - 'a';
        if (root->next[off_set] == NULL)
            root->next[off_set] = _trie_init();
        return _get_trie_node(root->next[off_set], ++s);
    }
    else
    {
        return root;
    }
}

void _update(Trie *root, char *s, int document_id, float tf_idf, int *item_position, int position_count)
{
    // 找到对应结点并建立索引
    Trie *temp_trie = _get_trie_node(root, s);
    if (temp_trie->index == NULL)
        temp_trie->index = _index_init();
    Index *index = temp_trie->index;

    // 如果将要溢出, 则加倍数组长度(重复检测存储位置的数组)
    if (index->size == index->max_size)
    {
        index->max_size *= 2;
        int *temp1 = (int *)malloc(index->max_size * sizeof(int));
        int *temp2 = (int *)malloc(index->max_size * sizeof(int));
        float *temp3 = (float *)malloc(index->max_size * sizeof(float));
        for (int i = 0; i < index->size; i++)
            temp1[i] = index->document_id[i], temp2[i] = index->term_count[i], temp3[i] = index->tf_idf[i];
        free(index->document_id), free(index->term_count), free(index->tf_idf);
        index->document_id = temp1, index->term_count = temp2, index->tf_idf = temp3;
    }
    while (index->pos_size + position_count > index->pos_max_size)
    {
        index->pos_max_size *= 2;
        int *temp4 = (int *)malloc(index->pos_max_size * sizeof(int));
        for (int i = 0; i < index->pos_size; i++)
            temp4[i] = index->item_position[i];
        free(index->item_position);
        index->item_position = temp4;
    }

    // term_count[i] 记录的是前i篇文章中item出现的次数
    int start_index = (index->size == 0) ? 0 : index->term_count[index->size - 1];
    index->term_count[index->size] = start_index + position_count;
    // 更新位置信息
    for (int i = 0; i < position_count; i++)
        index->item_position[start_index + i] = item_position[i];
    // 更新其他信息
    index->document_id[index->size] = document_id;
    index->tf_idf[index->size] = tf_idf;
    index->pos_size += position_count;
    index->size++;
}

// 释放空间
void free_trie(Trie *root)
{
    if (root == NULL)
        return;
    if (root->index != NULL)
        free(root->index->document_id), free(root->index->tf_idf);
    for (int i = 0; i < MAX_ALPHABET; i++)
        free_trie(root->next[i]);
    free(root);
}

// 显示占用内存大小
long long size_of(Trie *root)
{
    long long size = sizeof(Trie);
    if (root == NULL)
        return 0;
    if (root->index != NULL) // index中有四个数组
    {
        size += sizeof(Index);
        size += sizeof(int) * root->index->max_size;
        size += sizeof(int) * root->index->max_size;
        size += sizeof(float) * root->index->max_size;
        size += sizeof(int) * root->index->pos_max_size;
    }
    for (int i = 0; i < MAX_ALPHABET; i++)
        size += size_of(root->next[i]); // 循环递归
    return size;
}

// 根据文件信息建立索引(这里假设文件信息是预先定义好的)
void build_index(Trie *root, char *filename)
{
    char file_name[MAX_LENGTH];
    FILE *fp = fopen(filename, "r");
    // 读取文件总数(静态全局变量由此赋值)
    fscanf(fp, "%d", &document_count);

    int *document_id = (int *)malloc(document_count * sizeof(int));
    for (int i = 0; i < document_count; i++)
        fscanf(fp, "%d", document_id + i);
    for (int i = 0; i < document_count; i++)
    {
        fscanf(fp, "%s", file_name);
        _sp_build_index(root, document_id[i], file_name);
        if (i % 50 == 0)
            printf("%d %d %lld\n", i, document_id[i], size_of(root)); //  每隔20个文件输出一次
    }
    _caculate_tf_idf(root); // 遍历完整个文档, 开始计算tf_idf
    fclose(fp);
}

// 根据一个paper建立索引
void _sp_build_index(Trie *root, int document_id, char *filename)
{
    char s[MAX_LENGTH];
    int document_len = 0;
    FILE *fp = fopen(filename, "r");
    Link *head = _link_init("head");
    Link *cur = NULL;
    // 读取文件中所有的单词, 使用链表进行存储
    while (fscanf(fp, "%s", s) != EOF)
    {
        // 判断是否包含非英文字符
        int j = 0, flag = 0;
        while (s[j] != '\0')
        {
            if (s[j] < 'a' || s[j] > 'z')
                flag = 1;
            j++;
        }
        if (flag)
        {
            document_len++;
            continue;
        }
        // 将单词插入链表
        cur = head;
        while (cur->next != NULL && !same_str(cur->next->word, s))
            cur = cur->next;
        if (cur->next == NULL)
            cur->next = _link_init(s);
        if (cur->next->size == cur->next->max_size) // 如果将要溢出, 则加倍数组
        {
            cur->next->max_size *= 2;
            int *temp_trie = (int *)malloc(cur->next->max_size * sizeof(int));
            for (int i = 0; i < cur->next->size; i++)
                temp_trie[i] = cur->next->position[i];
            free(cur->next->position);
            cur->next->position = temp_trie;
        }
        cur->next->position[cur->next->size] = document_len;
        cur->next->size++;
        document_len++;
    }
    // 文档扫描完成后, 开始逐个处理term
    cur = head->next;
    Link *next = NULL;
    while (cur != NULL)
    {
        // 计算tf * len_norm
        float len_norm = 1 / sqrt(document_len);
        float tf = sqrt(cur->size);
        _update(root, cur->word, document_id, tf * len_norm, cur->position, cur->size);
        next = cur->next;
        // 同时释放该item所占用的内存资源
        free(cur->position);
        free(cur);
        cur = next;
    }
    free(head);
    fclose(fp);
}

// 计算tf_idf值
void _caculate_tf_idf(Trie *root)
{
    if (root == NULL)
        return;
    if (root->index != NULL)
    {
        float idf = log((float)document_count / (root->index->size + 1)) + 1;
        for (int i = 0; i < root->index->size; i++)
            root->index->tf_idf[i] *= idf * idf;
    }
    for (int i = 0; i < MAX_ALPHABET; i++)
        _caculate_tf_idf(root->next[i]); // 递归调用
}

// 序列化操作
void serialize(Trie *root, char *filename)
{
    FILE *fp = fopen(filename, "w");
    char s[MAX_LENGTH];
    _sp_serialize(root, s, 0, fp);
    fclose(fp);
}

// 序列化操作子过程
void _sp_serialize(Trie *root, char *s, int level, FILE *fp)
{
    if (root == NULL)
        return;
    if (level == 0)
        fprintf(fp, "%d %d\n", term_count, document_count); // 如果是根结点, 则打印全局信息
    // 打印这个结点的所有数据到文件中
    if (root->index != NULL)
    {
        s[level] = '\0';
        fprintf(fp, "%s %d %d %d\n", s, root->index->id, root->index->size, root->index->pos_size);
        fprintf(fp, "%d", root->index->document_id[0]);
        for (int i = 1; i < root->index->size; i++)
            fprintf(fp, " %d", root->index->document_id[i]);
        fprintf(fp, "\n");
        fprintf(fp, "%f", root->index->tf_idf[0]);
        for (int i = 1; i < root->index->size; i++)
            fprintf(fp, " %f", root->index->tf_idf[i]);
        fprintf(fp, "\n");
        fprintf(fp, "%d", root->index->term_count[0]);
        for (int i = 1; i < root->index->size; i++)
            fprintf(fp, " %d", root->index->term_count[i]);
        fprintf(fp, "\n");
        fprintf(fp, "%d", root->index->item_position[0]);
        for (int i = 1; i < root->index->pos_size; i++)
            fprintf(fp, " %d", root->index->item_position[i]);
        fprintf(fp, "\n");
    }
    for (int i = 0; i < MAX_ALPHABET; i++)
    {
        s[level] = 'a' + i;
        _sp_serialize(root->next[i], s, level + 1, fp); // 递归调用
    }
}

// 反序列化操作
void deserialize(Trie *root, char *filename)
{
    char s[MAX_LENGTH];
    int id = 0;
    int size = 0;
    int position_size = 0;
    FILE *fp = fopen(filename, "r");
    // 读取全局信息
    fscanf(fp, "%d", &term_count);
    fscanf(fp, "%d", &document_count);
    // 读取整个文件内容
    int count = 0;
    while (fscanf(fp, "%s", s) != EOF)
    {
        fscanf(fp, "%d", &id);
        fscanf(fp, "%d", &size);
        fscanf(fp, "%d", &position_size);
        Index *index = (Index *)malloc(sizeof(Index));
        index->size = size;
        index->max_size = size;
        index->pos_size = position_size;
        index->pos_max_size = position_size;
        index->id = id;
        index->document_id = (int *)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++)
            fscanf(fp, "%d", index->document_id + i);
        index->tf_idf = (float *)malloc(size * sizeof(float));
        for (int i = 0; i < size; i++)
            fscanf(fp, "%f", index->tf_idf + i);
        index->term_count = (int *)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++)
            fscanf(fp, "%d", index->term_count + i);
        index->item_position = (int *)malloc(position_size * sizeof(int));
        for (int i = 0; i < position_size; i++)
            fscanf(fp, "%d", index->item_position + i);
        _get_trie_node(root, s)->index = index;
        count += 1;
    }
    // 关闭文件
    fclose(fp);
}



PermutedTrie *_get_permuted_trie_node(PermutedTrie *root, char *s)
{
    if (s[0] != '\0')
    {
        int off_set = s[0] - 'a';
        if (root->next[off_set] == NULL)
            root->next[off_set] = _permuted_trie_init();
        return _get_permuted_trie_node(root->next[off_set], ++s);
    }
    else
    {
        return root;
    }
}

void update(PermutedTrie *root, char *origin_word, char *stem_word, int weight)
{
    Stem *temp = _stem_init(stem_word, weight);
    int i = 0;
    while (origin_word[++i] != '\0')
        ;
    char st[i + 2];
    for (int j = 0; j <= i; j++)
    {
        for (int k = 0; k < i - j; k++)
            st[k] = origin_word[k + j];
        st[i - j] = 'z' + 1;
        for (int k = 0; k < j; k++)
            st[k + i - j + 1] = origin_word[k];
        st[i + 1] = '\0';
        _get_permuted_trie_node(root, st)->origin = temp;
    }
}

void build()
{
    tree = (Trie **)malloc(domain_num * sizeof(Trie *));
    char **file_list = (char **)malloc(domain_num * sizeof(char *));
    file_list[0] = "in/title.txt";
    file_list[1] = "in/abstract.txt";
    file_list[2] = "in/content.txt";
    for (int i = 0; i < domain_num; i++)
        tree[i] = _trie_init(), build_index(tree[i], file_list[i]);
}
