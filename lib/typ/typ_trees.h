// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

// Original tree code Copyright 2013-6 Ian G Burleigh
// MIT-licence

#ifndef TYP_TREES_H
#define TYP_TREES_H

#include "def/def_macros.h"
#include "def/def_compare.h"

namespace typ {
//------------------------------------------------------------------------------

class TreeBase {
protected:
  TreeBase();
  virtual ~TreeBase();

  struct  node;

  typedef node        Node;
  typedef Node const& rcNode;
  typedef Node      * pNode;
  typedef Node const* pcNode;

  struct node {
    node();
    virtual ~node() {}
    virtual int compareTo(rcNode) const; // "abstract"
    node *left, *right;
  };

  virtual pNode clone(rcNode) const = 0;

  mutable node *root;
  mutable uint cnt;

  pNode min(pNode)    const;
  pNode max(pNode)    const;
  pNode min()         const;
  pNode max()         const;
  pNode curr()        const;

  void  clear(pNode);

public:
  bool  isEmpty() const { return !root; }
  uint  count()   const { return cnt;   }
  void  clear();
};

template <typename T>
int comparePlain(T const& t1, T const& t2) {
  COMPARE_VALUE2(t1, t2)
  return 0;
}

//------------------------------------------------------------------------------
// red-black

class RBTreeBase: public TreeBase {
  CLS(RBTreeBase) SUPER(TreeBase)
protected:
  struct node;

  static inline node const& N(rcNode n) { return static_cast<node const&>(n); }
  static inline node const* N(pcNode n) { return static_cast<node const*>(n); }
  static inline node      * N( pNode n) { return static_cast<node      *>(n); }

  struct node: super::node {
    node();
    void flip();
    virtual void assign(rcNode); // "abstract"
    virtual void ufix() const {}
    bool red;
  };

  pNode rl(pNode);
  pNode rr(pNode);
  pNode ml(pNode);
  pNode mr(pNode);
  pNode fix(pNode);

  pNode find(rcNode)  const;

  bool  insert(rcNode); // duplicates fail to insert
  bool  insert(pNode&,rcNode);
  bool  remove(rcNode);
  bool  remove(pNode&,rcNode);
  void  removeMin(pNode&);
};

//------------------------------------------------------------------------------

template <typename Item>
class RBTree: public RBTreeBase {
  CLS(RBTree) SUPER(RBTreeBase)
public:
  typedef Item const* pcItem;
  typedef Item const& rcItem;
  typedef QList<pcItem> item_lst;

protected:
  struct node;
  static inline node const& N(rcNode n) { return static_cast<node const&>(n); }
  static inline node const* N(pcNode n) { return static_cast<node const*>(n); }

  struct node: super::node {

    node(rcItem it): item(&it) {}

    void assign(rcNode that) {
      item = N(that).item;
    }

    int compareTo(rcNode that) const {
      rcItem i1 = *item, i2 = *N(that).item;
      int cmp;
      if ((cmp=i1.compareTo(i2)))
        return cmp;
      return i1.compareToEqual(i2);
    }

    pcItem item;
  };

  pNode clone(rcNode n) const {
    ++cnt; return new node(*N(n).item);
  }

private:
  inline pcItem it(pcNode n) const {
    return n ? N(n)->item : nullptr;
  }

  void add(item_lst& lst,pcNode n) const {
    if (!n)
      return;
    add(lst,n->left); lst.append(it(n)); add(lst,n->right);
  }

public:
  pcItem min() const {
    return it(super::min());
  }

  pcItem max() const {
    return it(super::max());
  }

  pcItem find(rcItem item) const {
    return it(super::find(node(item)));
  }

  pcItem take(rcItem item) {
    pcItem res = find(item);
    if (res) remove(item);
    return res;
  }

  pcItem curr() const {
    return it(super::curr());
  }

  bool insert(rcItem item) {
    return super::insert(node(item));
  }

  bool remove(rcItem item) {
    return super::remove(node(item));
  }

  item_lst allItems() const {	// sorted
    item_lst lst; add(lst,root);
    return lst;
  }
};

//------------------------------------------------------------------------------
// range-tree item

template <typename Key>
struct RangeTreeItem {
  typedef RangeTreeItem const& rcItem;
  typedef Key           const& rcKey;

  RangeTreeItem() {} virtual ~RangeTreeItem() {}

  virtual Key val() const = 0;
  virtual int compareToEqual(rcItem) const = 0;

  virtual int myCompare(rcItem that) const {
    return comparePlain(this,&that);
  }

  int compareTo(rcItem that) const {
    int cmp;
    if ((cmp=comparePlain(val(),that.val())))  return cmp;
    return myCompare(that);
  }

  bool isBetween(rcKey min, rcKey max) const {
    auto v = val();
    return min <= v  && v < max;
  }
};

//------------------------------------------------------------------------------
// range tree

template <typename Item,typename Key>
class RangeTree: public RBTree<Item> {
  CLS(RangeTree) SUPER(RBTree<Item>)
public:
  typedef Key const& rcKey;
  typedef QList<Item const*> item_lst;

protected:
  struct node;
  static inline node const& N(typename super::rcNode n) { return static_cast<node const&>(n); }
  static inline node const* N(typename super::pcNode n) { return static_cast<node const*>(n); }
  static inline node      * N(typename super::pNode n)  { return static_cast<node      *>(n); }

  struct node: super::node {
    node(typename super::rcItem item): super::node(item) {
    }
    mutable Key iMax;
    void ufix() const {
      iMax = this->item->val();
      if (this->left)  iMax = qMax(iMax,N(this->left)->iMax);
      if (this->right) iMax = qMax(iMax,N(this->right)->iMax);
    }
  };

  node* clone(typename super::rcNode n) const {
    ++this->cnt; return new node(*N(n).item);
  }

public:
  Key minKey(rcKey def) const {
    return this->root ? N(TreeBase::min(this->root))->iMax : def;
  }

  Key maxKey(rcKey def) const {
    return this->root ? N(this->root)->iMax : def;
  }

private:
  void findAllBetween(item_lst& lst,rcKey beg,rcKey end,node* n,bool reverse) const {
    if (!n)
      return;
    if (n->iMax>=beg) {
      Item const* &item = n->item;
      if (item->isBetween(beg,end)) {
        if (reverse) lst.prepend(item); else lst.append(item);
      }
      Key ctr = n->item->val();
      findAllBetween(lst,beg,end,N(n->left),reverse);
      if (end>ctr)
        findAllBetween(lst,beg,end,N(n->right),reverse);
    }
  }

public:
  item_lst findAllBetween(rcKey beg, rcKey end,bool reverse = false) const {
    item_lst lst;
    findAllBetween(lst,beg,end,N(this->root),reverse);
    return lst;
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_TREES_H
