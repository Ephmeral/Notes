
```cpp
inline _Rb_tree_node_base*
_Rb_tree_rebalance_for_erase(_Rb_tree_node_base* z,
                             _Rb_tree_node_base*& root,
                             _Rb_tree_node_base*& leftmost,
                             _Rb_tree_node_base*& rightmost)
{
  _Rb_tree_node_base* y = z; // y是要删除的节点
  _Rb_tree_node_base* x = 0;
  _Rb_tree_node_base* x_parent = 0;
  // z没有左孩子，令x为z的右孩子
  if (y->_M_left == 0)     // z has at most one non-null child. y == z.
    x = y->_M_right;     // x might be null.
  else // z有左孩子
    // 如果z没有右孩子，令x为z的左孩子
    if (y->_M_right == 0)  // z has exactly one non-null child. y == z.
      x = y->_M_left;    // x is not null.
    else { // 这里说明z左右孩子都有                  // z has two non-null children.  Set y to
      y = y->_M_right; // 令y为z的右孩子  //   z's successor.  x might be null.
      // y往左边递归，直到y没有左孩子
      while (y->_M_left != 0)
        y = y->_M_left;
      // 令x为y的右孩子
      x = y->_M_right;
    }
  // y不等于z，说明上面遇见的分支是第3个，即z左右孩子都有，此时y是z的直接后继
  if (y != z) {          // relink y in place of z.  y is z's successor
    // 将z左孩子的父亲设为y
    z->_M_left->_M_parent = y;
    // y的左孩子设置为z的左孩子，相当于将z的左孩子过继到y
    y->_M_left = z->_M_left;
    // y不是z的右孩子，说明y与z之间还有其他节点
    if (y != z->_M_right) {
      // 令x_parent为y的父节点
      x_parent = y->_M_parent;
      // 如果x不为空，x的父节点设置为y的父节点
      if (x) x->_M_parent = y->_M_parent;
      // 将y父节点的左孩子设置为x
      y->_M_parent->_M_left = x;      // y must be a child of _M_left
      // 将z的右孩子变成y的右孩子
      y->_M_right = z->_M_right;
      z->_M_right->_M_parent = y; // 设置z右孩子的父节点为y
    } // 到这里为止相当于将y替换z的位置
    else // 分支，y == z->_M_right，此时x的父节点还是y
      x_parent = y;  
    // 如果z是根节点
    if (root == z)
      root = y; // 令根节点为y，即修正根节点
    else if (z->_M_parent->_M_left == z) // 如果z是父节点的左孩子
      z->_M_parent->_M_left = y; // 将y设置为z父节点的左孩子
    else // 如果z是父节点的右孩子
      z->_M_parent->_M_right = y; // 将y设置为z父节点的右孩子
    y->_M_parent = z->_M_parent; // 更新y父节点
    __STD::swap(y->_M_color, z->_M_color); // 交换y和z的颜色
    y = z; // 令y指向z，此时y指向的节点是实际被删除的z节点
    // y now points to node to be actually deleted
  } 
  else { // y == z，说明z只有一个孩子，此时x指向这个孩子
  // 这个时候只需要将这个孩子连接到z的父节点即可
    x_parent = y->_M_parent; // x_parent 为y的父节点
    // x不为空的话，更新x的父节点
    if (x) x->_M_parent = y->_M_parent;    
    if (root == z) // 如果z是根节点
      root = x; // 说明此时删除的是根节点，将x设置为根节点
    else // z不是根节点
      if (z->_M_parent->_M_left == z)  // 如果z是父节点的左孩子
        z->_M_parent->_M_left = x; // 令x为z父节点的左孩子
      else // 否则，如果z是父节点的右孩子
        z->_M_parent->_M_right = x; // 令x为z父节点的右孩子
      
    /* 下面是更新leftmost和rightmost */
    if (leftmost == z) 
      if (z->_M_right == 0)        // z->_M_left must be null also
        leftmost = z->_M_parent;
    // makes leftmost == _M_header if z == root
      else
        leftmost = _Rb_tree_node_base::_S_minimum(x);
    if (rightmost == z)  
      if (z->_M_left == 0)         // z->_M_right must be null also
        rightmost = z->_M_parent;  
    // makes rightmost == _M_header if z == root
      else                      // x == z->_M_left
        rightmost = _Rb_tree_node_base::_S_maximum(x);
  }

  // 此时的y的颜色指的是x父节点的颜色，即实际删除的节点，如果是黑节点，需要进行修正
  if (y->_M_color != _S_rb_tree_red) { 
    // x 不是根节点且x为黑色（空节点也是黑色）
    while (x != root && (x == 0 || x->_M_color == _S_rb_tree_black))
      // x是父节点的左孩子
      if (x == x_parent->_M_left) {
        // 令w为x的兄弟节点
        _Rb_tree_node_base* w = x_parent->_M_right;
        // w是红节点
        if (w->_M_color == _S_rb_tree_red) {
          // 将w改为黑色
          w->_M_color = _S_rb_tree_black; 
          // x_parent改成红色
          x_parent->_M_color = _S_rb_tree_red;
          // 以x_parent节点进行左旋
          _Rb_tree_rotate_left(x_parent, root);
          w = x_parent->_M_right;
        }
        if ((w->_M_left == 0 || 
             w->_M_left->_M_color == _S_rb_tree_black) &&
            (w->_M_right == 0 || 
             w->_M_right->_M_color == _S_rb_tree_black)) {
          w->_M_color = _S_rb_tree_red;
          x = x_parent;
          x_parent = x_parent->_M_parent;
        } else {
          if (w->_M_right == 0 || 
              w->_M_right->_M_color == _S_rb_tree_black) {
            if (w->_M_left) w->_M_left->_M_color = _S_rb_tree_black;
            w->_M_color = _S_rb_tree_red;
            _Rb_tree_rotate_right(w, root);
            w = x_parent->_M_right;
          }
          w->_M_color = x_parent->_M_color;
          x_parent->_M_color = _S_rb_tree_black;
          if (w->_M_right) w->_M_right->_M_color = _S_rb_tree_black;
          _Rb_tree_rotate_left(x_parent, root);
          break;
        }
      } else {                  // same as above, with _M_right <-> _M_left.
        _Rb_tree_node_base* w = x_parent->_M_left;
        if (w->_M_color == _S_rb_tree_red) {
          w->_M_color = _S_rb_tree_black;
          x_parent->_M_color = _S_rb_tree_red;
          _Rb_tree_rotate_right(x_parent, root);
          w = x_parent->_M_left;
        }
        if ((w->_M_right == 0 || 
             w->_M_right->_M_color == _S_rb_tree_black) &&
            (w->_M_left == 0 || 
             w->_M_left->_M_color == _S_rb_tree_black)) {
          w->_M_color = _S_rb_tree_red;
          x = x_parent;
          x_parent = x_parent->_M_parent;
        } else {
          if (w->_M_left == 0 || 
              w->_M_left->_M_color == _S_rb_tree_black) {
            if (w->_M_right) w->_M_right->_M_color = _S_rb_tree_black;
            w->_M_color = _S_rb_tree_red;
            _Rb_tree_rotate_left(w, root);
            w = x_parent->_M_left;
          }
          w->_M_color = x_parent->_M_color;
          x_parent->_M_color = _S_rb_tree_black;
          if (w->_M_left) w->_M_left->_M_color = _S_rb_tree_black;
          _Rb_tree_rotate_right(x_parent, root);
          break;
        }
      }
    if (x) x->_M_color = _S_rb_tree_black;
  }
  return y;
}
```