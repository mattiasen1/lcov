#include <stdlib.h>
#include "list.h"
#include "refmem.h"

bool not_empty_list(list_t *list);
void initiate_list(list_t *list, elem_t  elem);


/// @brief Node in the list
typedef struct link
{
  elem_t elem;
  struct link *next;
} link_t;

/// @brief List of links
struct list
{
  /// First element in the list
  link_t *first;
  /// Last element in the list
  link_t *last;
  /// Amount of links in the list
  int size;
  /// Copy a link
  element_copy_fun copy;
  /// Free a link
  element_free_fun free;
  /// Compares two links
  element_comp_fun compare;
};

elem_t list_no_copy(elem_t elem)
{
  return elem;
}
void list_no_free(elem_t elem)
{
  return;
}

////////////////////////  DESTRUCTOR  ////////////////////////////////

void list_destructor(obj o)
{
  release(((list_t *)o)->first);
}

void link_destructor(obj o)
{
  link_t *link = o;
  release(link->next);
  //release(link->elem.p);
}


//////////////////////////////////////////////////////////////////

list_t *list_new(element_copy_fun copy, element_free_fun free, element_comp_fun compare)
{
  list_t *new = allocate_array(1, sizeof(list_t), list_destructor);
  retain(new);
  *new = (list_t) {
    .copy = copy ? copy : list_no_copy,
    .free = free ? free: list_no_free,
    .compare = compare,
  };
  return new;
}

link_t *link_new()
{
  link_t *link = allocate_array(1, sizeof(list_t), link_destructor);
  retain(link);
  return link;
}

int list_length(list_t *list)
{
  return list->size;
}

int convert_to_valid_index(list_t *list, int index)
{
  int size = list_length(list);
  if (index > size) {
    return -1;
  }
  if (index < 0) {
    if (abs(index) >= size) {
      return 0;
    }
    else {
      return (size + index);
    }
  }
  else {
    return index;
  }
}

link_t **step_to_index(list_t *list, int index)
{ 
  link_t **cursor = &(list->first);
  int i = 0;
  while(*cursor && i < index) {
    cursor = &((*cursor)->next);
    ++i;
  }
  return cursor;
}

void list_insert(list_t *list, int index, elem_t elem)
{

  if (list->first) {
    if (index == -1) {
      list_append(list, elem);
    }
    else {
      int new_index = convert_to_valid_index(list, index);
      link_t **cursor = step_to_index(list, new_index);
      link_t *new_link = link_new();
      new_link->elem = list->copy(elem);
      new_link->next = *cursor;
      *cursor = new_link;
      ++list->size;
      
    }
  }
  else {
    initiate_list(list, elem);
    ++list->size;
  } 
}

void initiate_list(list_t *list, elem_t elem)
{
  link_t *new = link_new();
  new->elem = elem;
  list->first = new;
  list->last = new;
}

void list_append(list_t *list, elem_t elem)
{
  if (list->first)
  {
    link_t *new_last = link_new();
    new_last->elem = elem;
    list->last->next = new_last;
    list->last = new_last;
  }
  else {
    initiate_list(list, elem);
  }
  ++list->size;
}

void list_prepend(list_t *list, elem_t elem)
{
  list_insert(list, 0, elem);
}

bool list_get(list_t *list, int index, elem_t *result)
{
  int size = list_length(list);
  if (index < size && abs(index) <= size) {
  int new_index = convert_to_valid_index(list, index);
  link_t **cursor = step_to_index(list, new_index);
    if (*cursor) {
      (*result) = ((*cursor)->elem);
    }
    return true;
  }
  else {
    return false;
  }
}

/// A convenience for list_get(list, 0, result)
bool list_first(list_t *list, elem_t *result)
{
  return list_get(list, 0, result);
}

/// A convenience for list_get(list, -1, result)
bool list_last(list_t *list, elem_t *result)
{
  return list_get(list, -1, result);
}


bool list_remove(list_t *list, int index, bool delete)
{ int new_index = convert_to_valid_index(list, index);
  link_t **cursor = step_to_index(list, new_index);
  if (!*cursor) {
    return false;
  }
  else {
    link_t *tmp = *cursor;
    *cursor = (*cursor)->next;
    if (*cursor) retain(*cursor);
    if (delete) release(tmp->elem.p);
    release(tmp);
    --list->size;
    return true;
  }
}

void list_delete(list_t *list, bool delete)
{
  if(list != NULL)
    {
     
      bool a;
      do {
        a = list_remove(list, 0, delete);
      }
      while(a);

      release(list);
    }
}


bool list_apply(list_t *list, elem_apply_fun func, void *data)
{
  if (!list)
  {
    return false;
  }
  bool result = false;
  link_t *cursor = list->first;
  while(cursor != NULL)
    {
      bool tmp = func(cursor->elem, data);
      result = result || tmp;
      cursor = cursor->next;
    }
  return result;
}



int list_contains(list_t *list, elem_t elem)
{
  int index = 0;
  link_t *cursor = list->first;
  if (list->compare) {
    while (!list->compare(cursor->elem, elem));
     {
       cursor = cursor->next;
       ++index;
     }
     return index;
      }
  else {
    while (((elem_t*)cursor->elem.p != elem.p));
     {
       cursor = cursor->next;
       ++index;
     }
     return index;
   } 
}


