#pragma once

#include "BinNode.h"
#include "util/release.h"

template <typename T>
class BinTree {
protected:
    int _size;
    BinNodePosi(T) _root;                        // 规模，根节点
    virtual int updateHeight(BinNodePosi(T) x);  // 更新节点x的高度
    void updateHeightAbove(BinNodePosi(T) x);    // 更新节点x及祖先的高度

public:
    BinTree() : _size(0), _root(nullptr) {}  // 构造函数
    ~BinTree() {                             // 析构函数
        if (0 < _size) {
            remove(_root);
        }
    }

    int size() const { return _size; }                            // 规模
    bool empty() const { return !_root; }                         // 判空
    BinNodePosi(T) root() const { return _root; }                 // 树根
    BinNodePosi(T) insertAsRoot(const T& e);                      // 插入根节点
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x, const T& e);      // e作为x的左孩子（原无）插入
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x, const T& e);      // e作为x的右孩子（原无）插入
    BinNodePosi(T) attachAsLC(BinNodePosi(T) x, BinTree<T>*&);  // T作为x左子树接入
    BinNodePosi(T) attachAsRC(BinNodePosi(T) x, BinTree<T>*&);  // T作为x右子树接入

    int remove(BinNodePosi(T) x);          // 删除以x为根的子树，返回该子树原先的规模
    BinTree<T>* secede(BinNodePosi(T) x);  // 将子树x从当前树中摘除，并独立为一个子树

    template <typename VST>
    void travLevel(VST& visit) {  // 层序遍历
        if (_root) {
            _root->travLevel(visit);
        }
    }
    template <typename VST>
    void travPre(VST& visit) {  // 先序遍历
        if (_root) {
            _root->travPre(visit);
        }
    }
    template <typename VST>
    void travIn(VST& visit) {  // 中序遍历
        if (_root) {
            _root->travIn(visit);
        }
    }
    template <typename VST>
    void travPost(VST& visit) {  // 后序遍历
        if (_root) {
            _root->travPost(visit);
        }
    }
    template <typename VST>
    void traverse(VST&);  // 自定义遍历

    bool operator<(BinTree<T> const& t) {  // 比较器（其余自行补充）
        return _root && t._root && lt(_root, t._root);
    }
    bool operator==(BinTree<T> const& t) {  // 判等器
        return _root && t._root && (_root == t._root);
    }
};  // BinTree

// 高度更新
template <typename T>
int BinTree<T>::updateHeight(BinNodePosi(T) x) {
    // 更新节点x的高度，具体规则因树而异
    return x->height = 1 + std::max(stature(x->lc), stature(x->rc));
}

template <typename T>
void BinTree<T>::updateHeightAbove(BinNodePosi(T) x) {
    // 更新高度，从x出发，依次遍历祖先。可优化
    while (x) {
        updateHeight(x);
        x = x->parent;
    }
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot(const T& e) {
    _size = 1;
    return _root = new BinNode<T>(e);  // 将e作为根节点插入空的二叉树
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, const T& e) {
    _size++;
    x->insertAsRC(e);  // e作为x的右孩子
    updateHeightAbove(x);
    return x->rc;
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x, const T& e) {
    _size++;
    x->insertAsLC(e);  // e作为x的左孩子
    updateHeightAbove(x);
    return x->rc;
}

template <typename T>
BinNodePosi(T) BinTree<T>::attachAsRC(BinNodePosi(T) x, BinTree<T>*& S) {
    // x->rc == nullptr
    if (x->rc = S->_root) {  // 注意是赋值，不是等号
        x->rc->parent = x;
    }
    // 更新全树规模与x所有祖先的高度
    _size += S->_size;
    updateHeightAbove(x);
    // 释放原树，返回接入位置
    S->_root = nullptr;
    S->_size = 0;
    release(S);
    S = nullptr;
    return x;
}

template <typename T>
BinNodePosi(T) BinTree<T>::attachAsLC(BinNodePosi(T) x, BinTree<T>*& S) {
    // x->lc == nullptr
    if (x->lc = S->_root) {  // 注意是赋值，不是等号
        x->lc->parent = x;
    }
    // 更新全树规模与x所有祖先的高度
    _size += S->_size;
    updateHeightAbove(x);
    // 释放原树，返回接入位置
    S->_root = nullptr;
    S->_size = 0;
    release(S);
    S = nullptr;
    return x;
}

// 删除二叉树中x节点及其后代，返回被删除节点的数值
template <typename T>
int BinTree<T>::remove(BinNodePosi(T) x) {
    FromParentTo(*x) = nullptr;    // 切断父节点的指针
    updateHeightAbove(x->parent);  // 更新祖先高度
    // 删除子树，更新规模，返回删除节点个数
    int n = removeAt(x);
    _size -= n;
    return n;
}

template <typename T>
static int removeAt(BinNodePosi(T) x) {
    if (!x)
        return 0;                                   // 递归基：空树
    int n = 1 + removeAt(x->lc) + removeAt(x->rc);  // 递归释放左右子树
    // 释放被摘除节点，返回删除节点总数
    release(x->data);
    release(x);
    return n;
}

// 二叉树子树分离算法：将子树x从当前树中摘除，将其封装为一棵独立子树返回
template <typename T>
BinTree<T>* BinTree<T>::secede(BinNodePosi(T) x) {  // assert: x为二叉树中的合法位置
    FromParentTo(*x) = NULL;                        // 切断来自父节点的指针
    updateHeightAbove(x->parent);                   // 更新原树中所有祖先的高度
    // 新树以x为根
    BinTree<T>* S = new BinTree<T>;
    S->_root = x;
    x->parent = NULL;
    // 更新规模，返回分离出来的子树
    S->_size = x->size();
    _size -= S->_size;
    return S;
}