///@file tree.h
///@author Lukas Annsberg, Love Osslund
///@date 16 Oktober 2017
///@bug Inga kända
///
///@brief Create and manipulate a tree
#ifndef __tree_h__
#define __tree_h__

#include <stdbool.h>
#include "common.h"

/// @brief Unique key
///
/// The key is used to distinguish each node so
/// we may find the specifik node we are searching for
typedef elem_t tree_key_t;

/// @brief Used to free the key
typedef element_free_fun key_free_fun;

/// @brief Defined in tree.c
typedef struct tree tree_t;

/// @brief Checks if the tree is balanced.
///
/// @param tree: The tree.
/// @returns: true or false.
bool tree_is_balanced(tree_t *tree);

/// @brief Balanced the tree.
///
/// @param tree: Tree to balance
/// @return A balanced tree.
void tree_balance(tree_t **tree);

/// @brief Creates a new tree.
///
/// @param copy: (may be NULL) A function applied to all elements when stored in the trtree
/// @param key_free: (may be NULL) Used to free keys in tree_delete
/// @param elem_free: (may be NULL) Used to free elements in tree_delete
/// @param compare: (may be NULL) Used to compare keys
/// @return Empty tree
tree_t *tree_new(element_copy_fun element_copy, key_free_fun key_free, element_free_fun element_free, element_comp_fun compare);

/// @brief Remove a tree along with all elem_t elements.
///
/// @param tree: The tree
/// @param delete_keys: If true, run tree's key_free function on all keys
/// @param delete_elements: If true, run tree's elem_free function on all elements
void tree_delete(tree_t *tree, bool delete_keys, bool delete_elements);

/// @brief Get the size of the tree.
///
/// @param tree: Tree to get size of
/// @return The number of nodes in the tree
int tree_size(tree_t *tree);

/// Get the depth of the tree.
///
/// @return The depth of the deepest subtree
int tree_depth(tree_t *tree);

/// @brief Insert element into the tree. Returns false if the key is already used.
///
/// Uses the tree's compare function to compare keys.
///
/// If tree's copy function is non-NULL, it will be applied to
/// elem and its result stored in the tree. Otherwise, elem will
/// be stored in the tree. Note that keys are never copied and are
/// assumed to be immutable. (They may however be freed by the
/// tree.)
///
/// @param tree: Pointer to the tree
/// @param key: The key of element to be appended -- this is assumed to be an immutable value
/// @param elem: The element 
/// @return true if successful, else false
bool tree_insert(tree_t *tree, tree_key_t key, elem_t elem);

/// @brief Checks whether a key is used in a tree.
///
/// Uses the tree's compare function to compare keys.
/// 
/// @param tree: Pointer to the tree
/// @param key: The key to check for inclusion in the tree
/// @return true if key is a key in the tree
bool tree_has_key(tree_t *tree, tree_key_t key);

/// @brief Finds the element for a given key in tree.
/// 
/// @param tree: Pointer to the tree
/// @param key: The key of elem to be removed
/// @param result: A pointer to where result can be stored (only defined when result is true)
/// @return true if key is a key in the tree
bool tree_get(tree_t *tree, tree_key_t key, elem_t *result);

/// @brief Removes the element for a given key in tree.
///
/// @param tree: Pointer to the tree
/// @param key: The key of elem to be removed
/// @param result: A pointer to where result can be stored (only defined when result is true)
/// @param delete: If true run tree_free_key_fun
/// @return true if key is a key in the tree
bool tree_remove(tree_t *tree, tree_key_t key, elem_t *result, bool delete);

/// @brief Swap the element for a given key for another.
///
/// @param t: Pointer to the tree
/// @param k: The key of elem to be changed
/// @param e: The updated element 
/// @param result: A elem_t where result will be stored (only defined when result is true)
/// @return true if both operations succeeded
#define tree_update(t, k, e, tmp) \
  (tree_remove(t, k, &tmp) ? tree_insert(t, k, e) : false)

/// @brief Returns an array holding all the keys in the tree
/// in ascending order.
///
/// @param tree: Pointer to the tree
/// @return Array of tree_size() keys
tree_key_t *tree_keys(tree_t *tree);

/// @brief Get all elements in tree
///
/// Returns an array holding all the elements in the tree
/// in ascending order of their keys (which are not part
/// of the value).
///
/// @param tree: Pointer to the tree
/// @return Array of tree_size() elements
elem_t *tree_elements(tree_t *tree);

/// This function is used in tree_apply() to allow applying a function
/// to all elements in a tree. 
typedef bool(*key_elem_apply_fun)(tree_key_t key, elem_t elem, void *data);

enum tree_order { inorder = 0, preorder = -1, postorder = 1 };

/// @brief Applies a function to all elements in the tree in a specified order.
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
/// @param tree: The tree
/// @param order: The order in which the elements will be visited
/// @param fun: The function to apply to all elements
/// @param data: An extra argument passed to each call to fun (may be NULL)
/// @return The result of all fun calls, combined with OR (||)
bool tree_apply(tree_t *tree, enum tree_order order, key_elem_apply_fun fun, void *data);

#endif
