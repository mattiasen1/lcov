///@file common.h

#ifndef __common_h__
#define __common_h__

#include <stdbool.h>

/// @brief Element
typedef union element elem_t;

/// @brief Element wrapper
///
/// Elements in the list are stored inside an elem_t which is
/// passed in by copy. 
union element
{
  int           i;
  unsigned int  u;
  long unsigned int lu;
  void         *p; 
  float         f;
};

/// @brief Copy an element
typedef elem_t(*element_copy_fun)(elem_t);

/// @brief Free an element
typedef void(*element_free_fun)(elem_t);

/// @brief Compare two elements
typedef int(*element_comp_fun)(elem_t, elem_t);

/// @brief Apply a function to all elements
typedef bool(*elem_apply_fun)(elem_t elem, void *data);

#endif
