// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_type_trees.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

// Original tree code Copyright 2013-6 Ian G Burleigh
// MIT-licence

#include "core_type_trees.h"

namespace core { namespace tree {
//------------------------------------------------------------------------------

TreeBase::TreeBase(): root(nullptr), cnt(0) {
}

TreeBase::~TreeBase() {
  clear();
}

TreeBase::node::node(): left(nullptr), right(nullptr) {
}

int TreeBase::node::compareTo(rcNode) const {
  NEVER return 0;  // "abstract"
}

TreeBase::pNode TreeBase::min(pNode n) const {
  if (!n) return nullptr;
  pNode l; while ((l=n->left)) n = l;
  return n;
}

TreeBase::pNode TreeBase::max(pNode n) const {
  if (!n) return nullptr;
  pNode r; while ((r=n->right)) n = r;
  return n;
}

TreeBase::pNode TreeBase::min() const {
  return min(root);
}

TreeBase::pNode TreeBase::max() const {
  return max(root);
}

TreeBase::pNode TreeBase::curr() const {
  return root;
}

void TreeBase::clear(pNode n) {
  if (!n) return;
  clear(n->left);
  clear(n->right);
  delete n; --cnt;
}

void TreeBase::clear() {
  clear(root); root = nullptr;
}

//------------------------------------------------------------------------------

RBTreeBase::node::node(): red(true) {
}

static inline void flib(bool& b) { b = !b; }

void RBTreeBase::node::flip() {
  flib(red);
  if (left)  flib(N(left)->red);
  if (right) flib(N(right)->red);
}

void RBTreeBase::node::assign(rcNode) {
  NEVER // "abstract"
}

#define isRed(n) (n && N(n)->red)

RBTreeBase::pNode RBTreeBase::rl(pNode n) {
  node *r = N(n->right);
  n->right = r->left; r->left = n;
  N(r)->red = N(n)->red; N(n)->red = true;
  N(n)->ufix(); N(r)->ufix();
  return r;
}

RBTreeBase::pNode RBTreeBase::rr(pNode n) {
  node *l = N(n->left);
  n->left = l->right; l->right = n;
  N(l)->red = N(n)->red; N(n)->red = true;
  N(n)->ufix(); N(l)->ufix();
  return l;
}

RBTreeBase::pNode RBTreeBase::ml(pNode n) {
  N(n)->flip();
  if (n->right && isRed(n->right->left)) {
    n->right = rr(N(n->right));
    n = rl(n);
    N(n)->flip();
  }
  return n;
}

RBTreeBase::pNode RBTreeBase::mr(pNode n) {
  N(n)->flip();
  if (n->left && isRed(n->left->left)) {
    n = rr(n);
    N(n)->flip();
  }
  return n;
}

RBTreeBase::pNode RBTreeBase::fix(pNode n) {
  if (isRed(n->right)) n = rl(n);
  if (isRed(n->left) && isRed(n->left->left)) n = rr(n);
  if (isRed(n->left) && isRed(n->right))      N(n)->flip();
  return n;
}

RBTreeBase::pNode RBTreeBase::find(rcNode that) const {
  pNode n = root;
  while (n) {
    int cmp = that.compareTo(*n);
    if (0 == cmp) break;
    n = (0 > cmp) ? n->left : n->right;
  }
  return n;
}

bool RBTreeBase::insert(rcNode that) {
  if (!insert(root,that)) return false;
  N(root)->red = false;
  return true;
}

bool RBTreeBase::insert(pNode& n,rcNode that) {
  if (!n) {
    n = clone(that); N(n)->ufix(); return true;
  }

  int cmp = that.compareTo(*n);
  if (0 == cmp) return false;

  if (!((0 > cmp) ? insert(n->left,that) : insert(n->right,that))) return false;
  N(n)->ufix();
  if (isRed(n->right) && !isRed(n->left))      n = rl(n);
  if (isRed(n->left)  && isRed(n->left->left)) n = rr(n);
  if (isRed(n->left)  && isRed(n->right))      N(n)->flip();
  return true;
}

bool RBTreeBase::remove(rcNode that) {
  if (!root) return false;
  bool res = remove(root,that);
  if (root) N(root)->red = false;
  return res;
}

bool RBTreeBase::remove(pNode& n, rcNode that) {
  int cmp = that.compareTo(*n);

  bool res = false;
  if (0 > cmp) {
    if (n->left) {
      if (!N(n->left)->red && !isRed(n->left->left)) n = ml(n);
      res = remove(n->left,that);
      N(n)->ufix();
    }
  } else {
    if (isRed(n->left)) {
      n = rr(n);
      cmp = that.compareTo(*n);
    }

    if (0==cmp && !n->right) {
      delete n; n = nullptr; --cnt; return true;
    }

    if (n->right) {
      if (!N(n->right)->red && !isRed(n->right->left)) {
        n = mr(n);
        cmp = that.compareTo(*n);
      }

      if (0 == cmp) {
        N(n)->assign(*min(n->right));
        removeMin(n->right);
        N(n)->ufix();
        res = true;
      } else {
        res = remove(n->right,that);
        N(n)->ufix();
      }
    }
  }

  n = fix(n);
  return res;
}

void RBTreeBase::removeMin(pNode& n) {
  if (!n->left) {
    delete n; n = nullptr; --cnt; return;
  }

  if (!N(n->left)->red && !isRed(n->left->left)) n = ml(n);
  removeMin(n->left); N(n)->ufix();
  n = fix(n);
}

//------------------------------------------------------------------------------
}}
// eof
