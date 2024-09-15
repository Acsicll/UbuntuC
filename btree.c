#include <stdlib.h>
#include <stdio.h>

#define SUB_M 3

typedef struct btree_node
{
    int *keys;
    struct btree_node **childrens;
    int num;
    int leaf;
} btree_node;

typedef struct _btree
{
    struct btree_node *root;
} btree;

btree_node *btree_creat_node(int leaf)
{
    // calloc will malloc memory and set value as zero
    btree_node *node = (btree_node *)calloc(1, sizeof(btree_node));
    if (node == NULL)
    {
        printf("running time error fail in creating node\n");
        return NULL;
    }
    node->leaf = leaf;
    node->keys = (int *)calloc(2 * SUB_M - 1, sizeof(int));
    if (node->keys == NULL)
    {
        printf("running time error fail in creating node\n");
        free(node);
        return NULL;
    }
    node->childrens = (btree_node **)calloc(2 * SUB_M, sizeof(btree_node *));
    if (node->childrens == NULL)
    {
        printf("running time error fail in creating node\n");
        free(node->keys);
        free(node);
        return NULL;
    }
    node->num = 0;

    return node;
}

void btree_destory_node(btree_node *node)
{
    if (node == NULL)
    {
        return;
    }
    for (int i = 0; i <= node->num; i++)
    {
        btree_destory_node(node->childrens[i]);
    }
    free(node->childrens);
    free(node->keys);
    free(node);
}

void btree_split_child(btree *Tree, btree_node *parent, int index)
{
    // we need to split a node to two part;
    btree_node *child = parent->childrens[index];
    btree_node *newnode = btree_creat_node(child->leaf);
    newnode->num = SUB_M - 1;

    int i = 0;
    for (i = 0; i < SUB_M - 1; i++)
    {
        newnode->keys[i] = child->keys[SUB_M + i];
    }

    // if current node is not leaf, it should move node's child-trees
    if (!child->leaf)
    {
        for (i = 0; i < SUB_M; i++)
        {
            newnode->childrens[i] = child->childrens[SUB_M + i];
        }
    }
    child->num = SUB_M;

    // implment insert element by moving element from last one
    for (i = parent->num; i >= index + 1; i--)
    {
        parent->childrens[i + 1] = parent->childrens[i];
    }
    parent->childrens[i + 1] = newnode;

    for (i = parent->num - 1; i >= index; i--)
    {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = child->keys[SUB_M - 1];
    parent->num += 1;
}

void btree_insert_non_full(btree_node *node, int key)
{
    int i = node->num - 1;
    if (node->leaf)
    {
        while (i >= 0 && key < node->keys[i])
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num++;
    }
    else
    {
        while (i >= 0 && key < node->keys[i])
        {
            i--;
        }
        i++;
        if (node->childrens[i]->num == 2 * SUB_M - 1)
        {
            btree_split_child(NULL, node, i);
            if (key > node->keys[i])
            {
                i++;
            }
        }
        btree_insert_non_full(node->childrens[i], key);
    }
}

void btree_insert(btree *Tree, int key)
{
    if (Tree->root == NULL)
    {
        Tree->root = btree_creat_node(1);
    }
    btree_node *curr = Tree->root;
    if (curr->num == 2 * SUB_M - 1)
    {
        btree_node *node = btree_creat_node(0);
        Tree->root = node;
        node->childrens[0] = curr;
        btree_split_child(Tree, node, 0);
        btree_insert_non_full(node, key);
    }
    else
    {
        btree_insert_non_full(curr, key);
    }
}

void btree_merge(btree *Tree, btree_node *parent, int index)
{
    btree_node *left = parent->childrens[index];
    btree_node *right = parent->childrens[index + 1];

    // let parent node mid keys copy to left
    left->keys[left->num] = parent->keys[index];

    int i = 0;
    for (i = 0; i < right->num; i++)
    {
        left->keys[SUB_M + i] = right->keys[i];
    }
    if (!left->leaf)
    {
        for (i = 0; i < SUB_M; i++)
        {
            left->childrens[SUB_M + i] = right->childrens[i];
        }
    }
    left->num += SUB_M;

    btree_destory_node(right);

    for (i = index + 1; i < parent->num; i++)
    {
        parent->keys[i - 1] = parent->keys[i];
        parent->childrens[i] = parent->childrens[i + 1];
    }
    parent->num--;
}

void btree_traverse(btree_node *node)
{
    if (node != NULL)
    {
        for (int i = 0; i < node->num; i++)
        {
            if (!node->leaf)
            {
                btree_traverse(node->childrens[i]);
            }
            printf("%d ", node->keys[i]);
        }
        printf("  ");
        if (!node->leaf)
        {
            btree_traverse(node->childrens[node->num]);
        }
    }
}

void btree_clear(btree *Tree)
{
    if (Tree->root != NULL)
    {
        btree_destory_node(Tree->root);
    }
    // free(Tree);
}
