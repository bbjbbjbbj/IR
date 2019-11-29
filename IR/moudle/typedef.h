#pragma once
#define MAX_ALPHABET 26      // 英文字母数目
#define MAX_LENGTH 1000      // 单词最大长度
#define MAX_DOCUMENT 1000    // 文档最大数目
#define PERMUTED_ALPHABET 27 // 轮排索引单词个数
#define MAX_FINDING 10       // 纠错查询中最大匹配数
#define MATCH_WEIGHT 0.2     // 纠错查询中的权重

typedef struct Index
{
    int id;
    int size;
    int max_size;
    int *document_id;
    float *tf_idf;
    int *term_count;
    int pos_size;
    int pos_max_size;
    int *item_position;
} Index;

typedef struct Trie
{
    struct Trie *next[MAX_ALPHABET];
    struct Index *index;
} Trie;

typedef struct Link
{
    char *word;
    struct Link *next;
    int size;
    int max_size;
    int *position;
} Link;

// query2
typedef struct Stem
{
    char *word;
    int weight;
} Stem;

typedef struct PermutedTrie
{
    struct PermutedTrie *next[PERMUTED_ALPHABET];
    struct Stem *origin;
} PermutedTrie;

typedef struct PermutedLink
{
    struct PermutedLink *next;
    char *origin_word;
    struct Stem *stem;
} PermutedLink;

static int term_count = 0;
static int document_count = 0;
static Trie **tree = NULL;
static PermutedTrie *permutedTrie = NULL;
static int domain_num = 3;
static float weights[] = {0.3, 0.4, 0.3};

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