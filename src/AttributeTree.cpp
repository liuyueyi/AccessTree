/*
 * generateAttributeTree.cpp
 *
 *  Created on: 2013-4-16
 *      Author: administrator
 */
#include "AttributeTree.h"
#include <sys/time.h>
#include <fstream>
/**
 * 迭代先序遍历二叉树，主要是来判断属性树是否构建成功
 */
void search(node *root)
{
    cout << root->value << endl;
    if (root->lchild != NULL)
        search(root->lchild);
    if (root->rchild != NULL)
        search(root->rchild);
}

/**
 * 回收构造树的内存空间
 */
void delete_tree(pnode & root)
{
    if(root != NULL)
    {
        delete_tree(root->lchild);
        delete_tree(root->rchild);
        cout << "free the node " << root -> value << endl;
        delete root;
        root = NULL;
    }
}


/**
 *  获得访问控制策略中的下一个属性
 */
string next_attribute(const string & policy, int & index)
{
	int start = index;
	int length = policy.length();
	while( index < length)
	{
		char ch = policy.at(++index);
		if(ch == ' ' || ch == ')' || ch == '(')
			break;
	}
	
	if(policy.at(index) == ' ')
	{
		while(index < length && policy.at(++ index) == ' ');
		return policy.substr(start, index - start - 1);
	}
	
	return policy.substr(start, index - start);
}


/**
 * Description: 利用访问控制策略生成二叉树
 * Input:
 *		policy: 访问控制策略，形如： "((A OR BC)AND(E OR FG))AND((H AND IJ)OR(K OR MN))"
 *				属性与AND/OR之间利用空格隔开，利用小括号将一个逻辑表达式括起来。为简约存储开销，括弧与括弧、属性、AND/OR之间不用空格
 * Output:
 * 		返回建立的属性二叉树的根节点
 */
node * generate_tree(const string & policy)
{
    // 声明两个堆栈，分别用来存储 (and or not），及由AND OR ATTRIBUTE构建的二叉树根节点
    stack<string> string_stack;
    stack<node *> node_stack;

    int i = 0;
    // 遍历访问策略，首先将属性，AND，OR分离，然后在不同的情况下做出不同的处理
    while (i < policy.length())
    {
        string word = "";
       // while (1)
      //  {
            if (policy.at(i) == '(')
            {
                word = "(";
                string_stack.push(word);

                i++;
               // break;
            }
            else if (policy.at(i) == ')')
            {
                // 右括弧，利用string_stack的栈顶元素构建树节点，左右孩子分别为node_stack的栈顶两个元素
                // 然后将构建的节点压入node_stack栈，string_stack退出栈顶的左括弧
                word = ")";

                node *r;
                if(string_stack.top() == "not")
                    r = NULL;
                else
                {
                    r = node_stack.top();
                    node_stack.pop();
                }
                node *l = node_stack.top();
                node_stack.pop();

                node *n = new node(string_stack.top(), l, r);
                node_stack.push(n);

                string_stack.pop();
                string_stack.pop();

                i++;
               // break;
            }
            else if (policy.at(i) == ' ')
            {
                i++;
               // break;
            }
            else
            {
                    word = next_attribute(policy, i);

                    if (word == "and" || word == "or" || word == "not" )
                    {
                        // AND OR 压入string_stack栈内
                        string_stack.push(word);
                    }
                    else
                    {
                        // 属性值构建叶子节点，并入栈
                        node *n = new node(word);
                        node_stack.push(n);
                    }
               // break;
            }
       // }
        //cout << word << "---" << endl;
    }

    node * r;
    if(string_stack.top() == "not")
    {
        r = NULL;
    }
    else
    {
        r = node_stack.top();
        node_stack.pop();
    }

    node *l = node_stack.top();
    node_stack.pop();

    node * root = new node(string_stack.top(), l, r);
    string_stack.pop();

    //search(root);

    return root;
}

/*
 * Description: match函数用来判断某用户的属性集合是否满足访问控制树
 * Input:
 * 		root: 属性树的根节点
 *		attrs： 用户的属性集，形式类似 "User IT_Department Manager"(属性与属性之间利用空格隔开/为了区别，也可以使用$特殊符号來隔开)
 * Output:
 *		if 满足属性树， 则返回 1
 *		else 返回 0
 */
int match(node * root, const string & attrs)
{

    if(root -> value == "not")
    {
        return match(root->lchild, attrs) ? 0 : 1;
    }

    else if( root -> value == "and")
    {
        return (match(root->lchild, attrs) + match(root->rchild, attrs)) == 2 ? 1 : 0;
    }

    else if( root -> value == "or")
    {
        if(match(root->lchild, attrs) || match(root->rchild, attrs))
            return 1;
        else
            return 0;
    }
    else // leaf node
    {
        //judge if the leaf node attribute is in the user's attributes set
        int first = 0;
        int index = attrs.find(root->value, first);
        do
        {
            int length = root->value.length();
            if (index != attrs.npos)
            {
                // 叶子节点表示的属性在用户属性集内，进一步进行判断，以确定是否为完全匹配
                // 即：root->value 在用户属性集合中，若前面有元素则必然是空格；若后面有元素也必然是空格
                // 简单来说，就是该属性在属性集中独立存在，而不是属性集中某个属性的部分片段（如叶子属性为 HEHE， 属性集和中有一属性为 HEHEHE， 这显然是不满足要求的
                if (index + length == attrs.length()
                        || ((index + length != attrs.length())
                            && attrs.at(index + length) == ' '))
                {
                    if (index == 0
                            || (index != 0 && attrs.at(index - 1) == ' '))
                    {
                        return 1;
                    }
                    else
                    {
                        first = index + 1;
//						return 0;
                    }
                }
                else
                {
                    first = index + 1;
//					return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        while((index = attrs.find(root->value, first)) != string::npos);
        return 0;
    }
}


/*
 * Description: 判断用户是否在黑名单内，是则返回true; 否则返回false
 * Input:
 *		id: 用户的id
 *		blacklist: 黑名单
 * Output：
 *		id在黑名单内，返回ture，否则返回false
 */
bool in_blacklist(const string & id, const string & blacklist)
{
    int first = 0, id_length = id.length();
    int index = blacklist.find(id, first);
    while(index !=  string::npos)
    {
        if(index + id_length >= blacklist.length() || blacklist.at(index + id_length) == ' ')
        {
            if(first == 0 || blacklist.at(index - 1) == ' ') // 在黑名单内
            {
                return true;
            }
        }
        first = index + id_length;
        index = blacklist.find(id, first);
    }
    return false;
}

//bool judge(const string & filename, const string & policy, const string & blacklist,
//           const Credential & credential, map<string, list<int> > & my_map )
//{
//    node * root;
//    if(!get_tree(filename, root, my_map))
//    {
//        ofstream gen_tree_cost("result/gen_tree_result", ios::app);
//        // 若内存中没有该树，则创建并保存到内存中
//        struct timeval start, finish;
//        gettimeofday(&start, NULL);
//        root = generate_tree(policy);
//        gettimeofday(&finish, NULL);
//        double duration = 1000000 * (finish.tv_sec - start.tv_sec) + (finish.tv_usec - start.tv_usec);
//        cout << "Generate tree cost is : " << duration << endl;
//        gen_tree_cost << filename << "---> Generate tree cost is : " << duration << endl;
//        gen_tree_cost.close();
//        save_tree(filename, root, 10, my_map);
//        cout << "generate the tree" << endl;
//    }
//
//    if (match(root, credential.attributes) == 1)
//    {
//        //然后判断用户是否在黑名单中
//        ostringstream oss;
//        oss << credential.UID;
//        string id = oss.str();
////        delete_tree(root);
//        return in_blacklist(id, blacklist) ? false : true ;
//    }
//    else
//        return false;
//}
//

/*
 * Description: 将属性树保存到map中（即保存在内存中），避免每次访问时都需要构建二叉树的过程
 * Input:
 *		file_name: 属性树对应的文件唯一标识
 *		root：属性树的根节点
 *		expiration_data: 属性树的截止日期
 * Output:
 *		NULL
 */
void save_tree(const string & file_name, const pnode & root, int expiration_data, map<string, list<int> > & my_map)
{
    list<int> temp;
    temp.push_back((int) root);
    temp.push_back(expiration_data);

    my_map.insert(make_pair(file_name, temp));
}

/*
 * Description: 根据文件的标识在map(内存)中获取其属性树的根节点
 * Input：
 *		file_name: 访问的文件我i义标识
 *		root:	作为输出参数使用，属性树的根节点
 * Output:
 *		如果内存中存在属性树，则返回true； 否则返回false
 */
bool get_tree(const string & file_name, pnode & root, map<string, list<int> > & my_map)
{
    map<string, list<int> >::iterator it = my_map.find(file_name);
    if(it != my_map.end())
    {
        list<int> list_temp = it->second;
        list<int>::iterator list_it = list_temp.begin();
        root = (pnode) * list_it ++;
        return true;
    }
    else
        return false;
}

bool remove_tree(const string & file_name, pnode & root, map<string, list<int> > & my_map)
{
    delete_tree(root);
    my_map.erase(file_name);
    return true;
}


bool clear_tree(map<string, list<int> > & my_map)
{
    map<string, list<int> >::iterator it = my_map.begin();
    while(it != my_map.end())
    {
        node * root = (pnode)((it -> second).front());
        delete_tree(root);
        my_map.erase(it);
        it ++;
//        cout << endl;
    }
    return true;
}


int main()
{
	string policy = "((boy or girl) and student) and ( not man ) ";
	node * root = generate_tree(policy);
	search(root);
	delete_tree(root);
	return 0;		
}
