#include <stdlib.h>
#include "tree.h"
#include "list.h"
#include <stdio.h>
#include <assert.h>

typedef struct node node_t;

/// 
struct node
{
  /// @brief Left subtree
  node_t *left;
  /// @brief Right subtree
  node_t *right;
  /// @brief Unique key for all nodes
  tree_key_t key;
  /// Element 
  elem_t item;
};

/// @brief Describes if 
enum branch {EMPTY, LEAF, LEFT, RIGHT, FULL};

/// Tree structure
struct tree
{
  /// Size of the tree
  int size;
  /// Pointer to the root of the tree
  node_t *root;
  /// Function used to copy one element in a node
  element_copy_fun element_copy;
  /// Function used to free a key in a node
  key_free_fun key_free;
  /// Function to free one element in a node
  element_free_fun element_free;
  /// Function to compare two elements
  element_comp_fun compare;
};

void add_subtree(tree_t **tree, tree_key_t key, elem_t elem);
void tree_insert_aux(tree_t *tree, tree_key_t key, elem_t elem, int direction);
void tree_keys_aux(node_t *node, tree_key_t *keys, int *index);
node_t **find_index(tree_t *tree, node_t **node, tree_key_t key);

// Returnerar vilka branches som finns på trädet, om några alls
enum branch tree_branches(node_t *node)
{
  enum branch type; 
  if (!node)
  {
    type = EMPTY;
  }
  else if (!node->right && !node->left)
  {
    type = LEAF;
  }
  else if (node->right && node->left)
  {
    type = FULL;
  }
  else if (node->right)
  {
    type = RIGHT;
  }
  else
  {
    type = LEFT;
  }
  return type;
}


elem_t element_no_copy(elem_t element)
{
  return element;
}

void key_no_free(elem_t key)
{
  return;
}

void element_no_free(elem_t element)
{
  return;
}

int compare_default(elem_t a, elem_t b)
{
  if (a.u == b.u)
    {
      return 0;
    }
  else if ( a.u <  b.u)
    {
      return -1;
    }
  else
  {
    return 1;
  }
}

tree_t *tree_new(element_copy_fun element_copy, key_free_fun key_free, element_free_fun element_free, element_comp_fun compare)
{
  tree_t *new_tree = calloc(1, sizeof(tree_t));
  *new_tree = (tree_t) { .element_copy = (element_copy) ? element_copy : element_no_copy,
                         .key_free = (key_free) ? key_free : key_no_free,
                         .element_free = (element_free) ? element_free : element_no_free,
                         .compare = (compare) ? compare : compare_default,
                         .size = 0
  };
  return new_tree;
}

node_t *node_new()
{
  node_t *new_node = calloc(1, sizeof(node_t));
  return new_node;
}

void tree_balance_aux(tree_t *tree, elem_t *elements, tree_key_t *keys, int start, int end)
{
  int middle = (start+end)/2;
  if (start == end)
    {
     tree_insert(tree, keys[middle], elements[middle]);
    }
  else if(start<end)
    {
      tree_insert(tree, keys[middle], elements[middle]);
      tree_balance_aux(tree, elements, keys, start , middle -1);
      tree_balance_aux(tree, elements, keys, middle +1, end);
    }
}

void tree_balance(tree_t **tree)
{
  int size = tree_size(*tree);
  elem_t *elements = tree_elements(*tree);
  elem_t *keys = tree_keys(*tree);
  tree_t *tmp = *tree;
  *tree = tree_new((*tree)->element_copy, (*tree)->key_free,\
                         (*tree)->element_free, (*tree)->compare);
  
  tree_balance_aux(*tree, elements, keys, 0, size-1);
  free(elements);
  free(keys);
  tree_delete(tmp ,false, false);
}

void tree_remove_copy(tree_t *tree, node_t **node_dest, node_t **node_source)
{
  tree->element_free((*node_dest)->item);
  elem_t item_copy = tree->element_copy(((*node_source)->item));
  (*node_dest)->item = item_copy; 
  (*node_dest)->key = (*node_source)->key;        

}

node_t **most_left_subtree(node_t **node)
{
  node_t **cursor = node;
  while ((*cursor)->left) {
    cursor = &(*cursor)->left;
  }
  return cursor;
}

void free_subnode(tree_t *tree, node_t **tmp, bool delete)
{
  if (delete) {
    tree->element_free((*tmp)->item);
    tree->key_free((*tmp)->key);
  }
  free(*tmp);
}

void tree_remove_reorder(tree_t *tree, node_t **cursor, bool delete)
{
  enum branch type = tree_branches(*cursor);
  switch (type) {
  case LEAF: {
    free_subnode(tree, cursor, delete);
    *cursor = NULL;
    break;
  }
  case LEFT: {
    node_t *tmp = *cursor;
    *cursor = (*cursor)->left;
    free_subnode(tree, &tmp, delete);
    break;
  }
  case RIGHT: {
    node_t *tmp = *cursor;
    *cursor = (*cursor)->right;
    free_subnode(tree, &tmp, delete);
    break;
  }     // if full tree.
  default:  {
    node_t **new_place_to_remove = most_left_subtree(&(*cursor)->right);
    tree_remove_copy(tree, cursor, new_place_to_remove);
    tree_remove_reorder(tree, new_place_to_remove, delete); 
    break;
  }
 }
}
bool tree_remove(tree_t *tree, tree_key_t key, elem_t *result, bool delete)
{
  assert(tree);

  node_t **cursor = &tree->root;
  cursor = find_index(tree, cursor, key);
  if (*cursor) {
    *result = (*cursor)->item;  //OBS skickar inte ut en kopia
    tree_remove_reorder(tree, cursor, delete);
    --tree->size;
    return true;
  }
  else {
   return false;
  }
}

void tree_delete_free(tree_t *tree, tree_key_t key, elem_t elem, bool delete_keys, bool delete_elements)
{
  if (delete_keys) {
   tree->key_free(key); 
  }
  if (delete_elements) {
   tree->element_free(elem);
  }
}

void tree_delete_apply(tree_t *tree, node_t *node, bool delete_keys, bool delete_elements)
{ 
  if (!node) {
    return;
  }
  else 
  { 
    tree_delete_apply(tree, node->left, delete_keys, delete_elements);
    tree_delete_apply(tree, node->right, delete_keys, delete_elements);
    tree_delete_free(tree, node->key, node->item, delete_keys, delete_elements);
    free(node);
  }
 }

void tree_delete(tree_t *tree, bool delete_keys, bool delete_elements)
{
  assert(tree);
  
  tree_delete_apply(tree, tree->root, delete_keys, delete_elements);
  free(tree);
}

int tree_size(tree_t *tree)
{
  assert(tree);
  
  return tree->size;
}

int biggest(int a, int b, bool *balance)
{
  if (abs(a-b) > 1) {
    *balance = false;
  }
  return (a > b) ? a : b;
}

int tree_depth_aux(node_t *node, int depth, bool *balance)
{
  if (node == NULL) {
    return depth;
  }
  else {
    return ( biggest((tree_depth_aux(node->right, depth+1, balance)),\
                     (tree_depth_aux(node->left, depth+1, balance)), balance));

  
  }
    }

int tree_depth(tree_t *tree)
{
  assert(tree);
  
  bool balance = true;
  return tree_depth_aux(tree->root, -1, &balance);
}

bool tree_is_balanced(tree_t *tree)
{
  assert(tree);
  
  bool balance = true;
  tree_depth_aux(tree->root, -1, &balance);
  return balance;
}

node_t **find_index(tree_t *tree, node_t **node, tree_key_t key)
{
  node_t **cursor = node;
  if (*cursor == NULL) {
    return cursor;
  }
  int compare = tree->compare(key, (*cursor)->key);
  if (compare == 0) {
    return cursor;
  }
  else if (compare < 0) {
    return find_index(tree, &(*cursor)->left, key);
  }
  else {
    return find_index(tree, &(*cursor)->right, key);
  }
}


bool tree_has_key(tree_t *tree, tree_key_t key)
{
  assert(tree);
  
  node_t **cursor = &tree->root;
  cursor = find_index(tree, cursor, key);
  if (*cursor) {
    return true;
  }
  else {
    return false;
  }
}

bool tree_get(tree_t *tree, tree_key_t key, elem_t *result)
{
  assert(tree);
  
  node_t **cursor = &tree->root;
  cursor = find_index(tree, cursor,  key);
  if (*cursor) {
    *result = (*cursor)->item;
    return true;
  }
  else {
    return false;
  }
}

/// Initiera ett tomt, dock !NULL, träd med en nod.
///
/// \param tree-pekare till ett träd
/// \param key nyckel till den nya noden
/// \param elem element till den nya noden
/// \void

void add_subnode(tree_t *tree, node_t **node, tree_key_t key, elem_t elem)
{
  *node = node_new();
  (*node)->key = key;
  (*node)->item = tree->element_copy(elem);
}


bool tree_insert(tree_t *tree, tree_key_t key, elem_t elem)
{
  assert(tree);

  if (tree->root == NULL) {
    tree->root = node_new();
    tree->root->key = key;
    tree->root->item = elem;
    ++tree->size;
    return true;
  }
  node_t **cursor = &tree->root;
  cursor = find_index(tree, cursor, key);
  if (*cursor) {
    return false;
  }
  else {
    *cursor = node_new();
    (*cursor)->key = key;
    (*cursor)->item = elem;
    ++tree->size;
    return true;
  }
}


/// Returns an array holding all the keys in the tree
/// in ascending order.///
/// \param tree pointer to the tree
/// \returns: array of tree_size() keys

tree_key_t *tree_keys(tree_t *tree)
{
  assert(tree);
  
  int size = tree_size(tree);
  tree_key_t *keys = calloc(size, sizeof(tree_key_t));
  int *index = calloc(1, sizeof(int));
  *index = 0;
  tree_keys_aux(tree->root, keys, index);
  free(index);
  return keys;
}

void tree_keys_aux(node_t *node, tree_key_t *keys, int *index)
{
  if (!node)
  {
    return;
  }
  else
  {
    tree_keys_aux(node->left, keys, index);
    keys[*index] = node->key;
    ++(*index);
    tree_keys_aux(node->right, keys, index);
  }
}

void tree_elements_aux(node_t *node, elem_t *elems, int *index)
{
  if (!node)
  {
    return;
  }
  else
  {
    tree_elements_aux(node->left, elems, index);
    elems[*index] = node->item;
    ++(*index);
    tree_elements_aux(node->right, elems, index);
  }
}


/// Returns an array holding all the elements in the tree
/// in ascending order of their keys (which are not part
/// of the value).
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() elements
elem_t *tree_elements(tree_t *tree)
{
  assert(tree);
  
  int size = tree_size(tree);
  elem_t *elems = calloc(size, sizeof(elem_t));
  int *index = calloc(1, sizeof(int));
  *index = 0;
  tree_elements_aux(tree->root, elems, index);
  free(index);
  return elems;
}

/// Applies a function to all elements in the tree in a specified order.
/// Example (using shelf as key):
///
///     tree_t *t = tree_new();
///     tree_insert(t, "A25", some_item);
///     int number = 0;
///     tree_apply(t, inorder, print_item, &number);
///
/// where print_item is a function that prints the number and increments it,
/// and prints the item passed to it.
///
/// \param tree the tree
/// \param order the order in which the elements will be visited
/// \param fun the function to apply to all elements
/// \param data an extra argument passed to each call to fun (may be NULL)



bool tree_apply_aux(node_t *node, enum tree_order order, key_elem_apply_fun fun, void *data)
{
    if (!node)
    {
      return false;
    }
    else
    { 
      switch (order)
      {
        case inorder:
        {
          bool a = tree_apply_aux(node->left, order, fun, data);
          bool b = fun(node->key, node->item, data);
          bool c = tree_apply_aux(node->right, order, fun, data);
          return a || b || c;
          break;
        }
        case preorder:
        {
          bool a = fun(node->key, node->item, data);
          bool b = tree_apply_aux(node->left, order, fun, data);
          bool c = tree_apply_aux(node->right, order, fun, data);
          return a || b || c;
          break;
        }
        default:  {
          bool a = tree_apply_aux(node->left, order, fun, data);
          bool b = tree_apply_aux(node->right, order, fun, data);
          bool c = fun(node->key, node->item, data);
         return a || b || c;
          break;
        }
      }
    }  
 
}

bool tree_apply(tree_t *tree, enum tree_order order, key_elem_apply_fun fun, void *data)
{
  assert(tree);
  
  return tree_apply_aux(tree->root, order, fun, data);
}


