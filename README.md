AccessTree
==========

the source code of access tree

根据owner制定的访问策略来生成属性树，并用此属性树来判断用户的属性集是否满足定义规则。
AccessTree中实现了将属性树常驻内存的功能，能有效的减少频繁访问时生成属性树的开销。

使用规则：
  引入头文件AccessTree.h即可调用各功能函数
