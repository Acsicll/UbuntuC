#include <stdlib.h>
#include <stdio.h>

typedef int KEY_TYPE;

enum color
{
    BLACK = 0,
    RED,
};
#define RBTREE_ENTRY(name, type) \
    struct name                  \
    {                            \
        struct type *right;      \
        struct type *left;       \
        struct type *parent;     \
        unsigned char color;     \
    }

typedef struct _rbtree_node
{
    KEY_TYPE key;
    void *value;
#if 1
    struct _rbtree_node *right;
    struct _rbtree_node *left;
    struct _rbtree_node *parent;
    unsigned char color;
#else
    RBTREE_ENTRY(, rbtree_node)
    node;
#endif

} rbtree_node;

typedef struct _rbtree
{
    struct _rbtree_node *root;
    struct _rbtree_node *nil;
} rbtree;

// left-rotate and right-rotate would happen when the current tree
// does not conform to the definition about red-black binary tree

// left-rotate : current node will be which right-tree's left node
void rbtree_left_rotate(rbtree *Tree, rbtree_node *curr)
{
    rbtree_node *right = curr->right;

    curr->right = right->left;
    if (right->left != Tree->nil)
    {
        right->left->parent = curr;
    }

    right->parent = curr->parent;
    if (curr->parent == Tree->nil)
    {
        Tree->root = right;
    }
    else if (curr == curr->left)
    {
        curr->parent->left = right;
    }
    else
    {
        curr->parent->right = right;
    }

    right->left = curr;
    curr->parent = right;
}

// right-rotate : current node will be which left-treee's right node
void rbtree_right_rotate(rbtree *Tree, rbtree_node *curr)
{
    rbtree_node *left = curr->left;

    curr->left = left->right;
    if (left->right != Tree->nil)
    {
        left->right->parent = curr;
    }

    left->parent = curr->parent;
    if (curr->parent == Tree->nil)
    {
        Tree->root = left;
    }
    else if (curr == curr->parent->left)
    {
        curr->parent->left = left;
    }
    else
    {
        curr->parent->right = left;
    }

    left->right = curr;
    curr->parent = left;
}

void rbtree_insert_fixup(rbtree *Tree, rbtree_node *curr)
{
    // aussm current parent node's color is red
    while (curr->parent->color == RED)
    {
        if (curr->parent == curr->parent->parent->left)
        {
            // L : parent is grandparent's left
            rbtree_node *unc = curr->parent->parent->right;
            if (unc->color == RED)
            {
                // if uncle is red, it need to change color instead of rotate
                curr->parent->color = BLACK;
                unc->color = BLACK;
                curr->parent->parent->color = RED;
                // recursion,continue to fix parent
                curr = curr->parent->parent;
            }
            else
            {
                // uncle is black
                if (curr == curr->parent->right)
                {
                    // R : after fixup or insert, right weight large than left
                    curr = curr->parent;
                    rbtree_left_rotate(Tree, curr);
                    // after rotate,curr still is leaf
                }
                // L : after fixup or insert, left weight large than right
                curr->parent->color = BLACK;
                curr->parent->parent->color = RED;
                rbtree_right_rotate(Tree, curr->parent->parent);
            }
        }
        else
        {
            // R
            rbtree_node *unc = curr->parent->parent->left;
            if (unc->color == RED)
            {
                curr->parent->color = BLACK;
                unc->color = BLACK;
                curr->parent->parent->color = RED;
                curr = curr->parent->parent;
            }
            else
            {
                if (curr == curr->parent->left)
                {
                    curr = curr->parent;
                    rbtree_right_rotate(Tree, curr);
                }
                curr->parent->color = BLACK;
                curr->parent->parent->color = RED;
                rbtree_left_rotate(Tree, curr->parent->parent);
            }
        }
    }
    // if parent node's color is black,it does not need to do anymore
    Tree->root->color = BLACK;
    //printf("fixup tree done\n");
}

void rbtree_insert(rbtree *Tree, rbtree_node *node)
{

    rbtree_node *curr = Tree->root;
    rbtree_node *prev = Tree->nil;

    while (curr != Tree->nil)
    {
        prev = curr;
        if (node->key < curr->key)
        {
            curr = curr->left;
        }
        else if (node->key > curr->key)
        {
            curr = curr->right;
        }
        else
        {
            //printf("error key\n");
            return;
        }
    }

    // root's color must be black
    node->parent = prev;
    if (prev == Tree->nil)
    {
        Tree->root = node;
        // node->color = BLACK;
    }
    else
    {
        if (node->key < prev->key)
        {
            prev->left = node;
        }
        else /*if (node->key < prev->key)*/
        {
            prev->right = node;
        }
    }
    node->color = RED;
    node->left = Tree->nil;
    node->right = Tree->nil;
    // To confirm red-black binary tree's definition, the node which
    // be inserted that color should be red

    rbtree_insert_fixup(Tree, node);
    //printf("success in inserting a node\n");
}

rbtree_node *create_node(KEY_TYPE key, void *value)
{
    rbtree_node *node = (rbtree_node *)malloc(sizeof(rbtree_node));
    if (node == NULL)
    {
        printf("running time error, malloc faild in creating node");
        return NULL;
    }
    node->key = key;
    node->value = value;
    node->color = RED;
    return node;
}

void print_tree(rbtree_node *node, rbtree_node *nil, int depth)
{
    if (node != nil)
    {
        print_tree(node->right, nil, depth + 1);
        for (int i = 0; i < depth; i++)
        {
            printf(" ");
        }
        printf("%d (%s)\n", node->key, node->color == RED ? "RED" : "BLACK");
        print_tree(node->left, nil, depth+1);
    }
}