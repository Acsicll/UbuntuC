#include "Rbtree.c"
#include "btree.c"
#include <stdlib.h>

void test_1()
{
    rbtree myTree;
    rbtree_node nilNode;
    nilNode.color = BLACK;
    myTree.nil = &nilNode;
    myTree.root = &nilNode;

    for (int i = 0; i < 10; i++)
    {
        rbtree_node *temp = create_node(i * 10, NULL);
        if (temp != NULL)
        {
            printf("success in creating node\n");
        }
        rbtree_insert(&myTree, temp);
    }
    printf("Red-Black Tree:\n");
    print_tree(myTree.root, myTree.nil, 0);
}

void test_2(){
    btree myTree = { NULL };

    // 插入测试
    int keys[] = { 10, 20, 5, 6, 12, 30, 25 };
    for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        btree_insert(&myTree, keys[i]);
    }

    printf("B+ Tree in sorted order:\n");
    btree_traverse(myTree.root);
    printf("\n");

    // 释放内存
    btree_clear(&myTree);
}

int main()
{
#if 0
    test_1();
#else if 1
    test_2();
#endif
    return 0;
}