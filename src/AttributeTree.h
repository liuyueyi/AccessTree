/*
 * generateAttributeTree.h
 *
 *  Created on: 2013-4-16
 *      Author: administrator
 */

#ifndef ATTRIBUTE_TREE_H_
#define ATTRIBUTE_TREE_H_

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <stack>
#include <sstream>

using namespace std;

typedef struct node
{
    string value;	// 二叉树节点的值，具体可能为AND, OR, NOT, Attributes等
    struct node * lchild, *rchild;
    /**
     *  value:二叉树节点的值，AND，OR，Attributes等
     *  lchild: 二叉树节点的左孩子
     *  rchild: 二叉树节点的右孩字
     */
    node(const string value1 = "", node * lchild1 = NULL,
         node * rchild1 = NULL)
    {
        value = value1;
        lchild = lchild1;
        rchild = rchild1;
    }
} node, *pnode;

void search(node *root);
// 生成二叉树
node * generate_tree(const string & policy);
// 回收属性树的内存空间
void delete_tree(pnode & root);
// 判断用户属性集是否满足二叉树
int match(node * root, const string & attrs);
// 判断用户是否在黑名单内
bool in_blacklist(const string & id, const string & blacklist);

//bool judge(const string & filename, const string & policy, const string & blacklist,
//           const Credential & credential, map<string, list<int> > & my_map);

void save_tree(const string & file_name, const pnode & root, int expiration_data, map<string, list<long> > & my_map);

bool get_tree(const string & file_name, pnode & root, map<string, list<long> > & my_map);

bool remove_tree(const string & file_name, pnode & root, map<string, list<long> > & my_map);

bool clear_tree(map<string, list<int> > & my_map);

#endif /* GENERATEATTRIBUTETREE_H_ */
