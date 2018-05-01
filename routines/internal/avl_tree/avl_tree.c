/*
  Copyright (c) 2017, Dmitry D. Chernov
*/

#include "avl_tree.h"

#include <stdlib.h>

#define MIN2(a, b) ( ((a) < (b)) ? (a) : (b) )
#define MAX2(a, b) ( ((a) > (b)) ? (a) : (b) )

#define ASSIGN_NODE_LINK_SAFE(node, link_node, link_bias) \
  if ((node) != NULL) IGENA_AVL_NODE_LINK( node, link_bias ) = (link_node)

#define SELECT_BIAS(parent, child) \
  ( ((parent)->left == (child)) \
    ? IGENA_AVL_BIAS_LEFT \
    : IGENA_AVL_BIAS_RIGHT )

static const igena_avl_node_s default_node_c = {
  NULL,
  NULL,
  NULL,
  IGENA_AVL_BIAS_PARENT
};

/******************************************************************************/

static igena_avl_node_p rotate_node_left( igena_avl_node_p node ) {
  const igena_avl_node_p new_root = node->right;
{
  node->right = new_root->left;
  new_root->left = node;

  ASSIGN_NODE_LINK_SAFE( node->right, node, IGENA_AVL_BIAS_PARENT );
  new_root->parent = node->parent;
  node->parent = new_root;

  node->weight -= 1 + MAX2( 0, new_root->weight );
  new_root->weight -= 1 - MIN2( 0, node->weight );

  return new_root;
}}

static igena_avl_node_p rotate_node_right( igena_avl_node_p node ) {
  const igena_avl_node_p new_root = node->left;
{
  node->left = new_root->right;
  new_root->right = node;

  ASSIGN_NODE_LINK_SAFE( node->left, node, IGENA_AVL_BIAS_PARENT );
  new_root->parent = node->parent;
  node->parent = new_root;

  node->weight += 1 - MIN2( 0, new_root->weight );
  new_root->weight += 1 + MAX2( 0, node->weight );

  return new_root;
}}

static igena_avl_node_p balance_node( igena_avl_node_p node ) {
  const igena_avl_node_p ancestor = node->parent;
  igena_avl_node_p new_root;
{
  switch (node->weight) {
    case -2:
      if (node->left->weight == 1) {
        node->left = rotate_node_left( node->left );
      }
      new_root = rotate_node_right( node );
    break;

    case 2:
      if (node->right->weight == -1) {
        node->right = rotate_node_right( node->right );
      }
      new_root = rotate_node_left( node );
    break;

    default:
      return node;
  }

  ASSIGN_NODE_LINK_SAFE( ancestor, new_root, SELECT_BIAS( ancestor, node ) );
  return new_root;
}}

static igena_avl_node_p rebalance_branch_from_leaf( igena_avl_node_p parent,
  igena_avl_bias link, igena_avl_node_p null_root )
{
  const gena_bool is_insertion = (null_root == NULL);
  igena_avl_node_p new_root = parent;
{
  while (parent != NULL) {
    parent->weight += (is_insertion) ? link : -link;
    new_root = balance_node( parent );
    parent = new_root->parent;
    if (parent != NULL) {
      if ( (new_root->weight == 0) == is_insertion ) { return null_root; }
      link = SELECT_BIAS( parent, new_root );
    }
  }

  return new_root;
}}

/******************************************************************************/

igena_avl_node_p igena_avl_node_create( size_t key_size, size_t value_size ) {
  igena_avl_node_p node;
{
  node = malloc( sizeof(igena_avl_node_s) + key_size + value_size );
  if (node == NULL) { return NULL; }

  *node = default_node_c;
  return node;
}}

igena_avl_node_p igena_avl_node_attach( igena_avl_node_p node,
  igena_avl_node_p parent, igena_avl_bias link )
{
  node->parent = parent;
  if (parent == NULL) { return node; }
  IGENA_AVL_NODE_LINK( parent, link ) = node;
  return rebalance_branch_from_leaf( parent, link, NULL );
}

igena_avl_node_p igena_avl_node_detach( igena_avl_node_p node ) {
  const igena_avl_node_p ancestor = node->parent;
  igena_avl_node_p successor, deputy, orphan, relative, rebalancing;
  igena_avl_bias deputy_bias, ancestor_bias;
{
  if (node->left == NULL) {
    deputy_bias = IGENA_AVL_BIAS_RIGHT;
  } else if (node->right == NULL) {
    deputy_bias = IGENA_AVL_BIAS_LEFT;
  } else {
    /* Choose a longer subtree to try to avoid possible rebalancing. */
    deputy_bias = (node->left->weight != 0)
      ? IGENA_AVL_BIAS_LEFT
      : IGENA_AVL_BIAS_RIGHT;
  }

  successor = IGENA_AVL_NODE_LINK( node, deputy_bias );
  if (successor != NULL) {
    relative = successor;
    do {
      deputy = relative;
    } while ( NULL != (relative = IGENA_AVL_NODE_LINK(deputy, -deputy_bias)) );

    orphan = IGENA_AVL_NODE_LINK( node, -deputy_bias );
    IGENA_AVL_NODE_LINK( deputy, -deputy_bias ) = orphan;
    ASSIGN_NODE_LINK_SAFE( orphan, deputy, IGENA_AVL_BIAS_PARENT );

    if (successor != deputy) {
      relative = deputy->parent;
      orphan = IGENA_AVL_NODE_LINK( deputy, deputy_bias );
      IGENA_AVL_NODE_LINK( relative, -deputy_bias ) = orphan;
      ASSIGN_NODE_LINK_SAFE( orphan, relative, IGENA_AVL_BIAS_PARENT );

      IGENA_AVL_NODE_LINK( deputy, deputy_bias ) = successor;
      IGENA_AVL_NODE_LINK( successor, IGENA_AVL_BIAS_PARENT ) = deputy;

      deputy_bias = -deputy_bias;
      successor = relative;
    }

    deputy->parent = ancestor;
    deputy->weight = node->weight;
  } else {
    deputy = NULL;
    successor = ancestor;
  }

  if (ancestor != NULL) {
    ancestor_bias = SELECT_BIAS( ancestor, node );
    IGENA_AVL_NODE_LINK( ancestor, ancestor_bias ) = deputy;
    if (deputy == NULL) { deputy_bias = ancestor_bias; }
  }

  *node = default_node_c;
  rebalancing = rebalance_branch_from_leaf( successor, deputy_bias, node );
  if (deputy == NULL) { return rebalancing; }
  return (deputy->parent == NULL) ? deputy : rebalancing;
}}

/******************************************************************************/

void igena_avl_subtree_free( igena_avl_node_p root ) {
  igena_avl_node_p next_node;
{
  while (root != NULL) {
    if (root->left != NULL) {
      next_node = root->left;
      root->left = NULL;
    } else if (root->right != NULL) {
      next_node = root->right;
      root->right = NULL;
    } else {
      next_node = root->parent;
      free( root );
    }
    root = next_node;
  }
}}