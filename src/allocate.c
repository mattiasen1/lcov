#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "allocate.h"

#define object(p) (((head_t*)p) + 1)
#define head(p) (((head_t*)p) - 1)


/////////////////////////  GLOBAL VARIABLES   ////////////////////////////////////////
//                             ↓ ↓ ↓

size_t limit = 2;
size_t count = 0;
size_t size_freed = 0;
obj save = NULL;
obj first_known_pointer = NULL;


/////////////////////////  STRUCT, TYPEDEF, DECLARATIONS  /////////////////////////////
//                              ↓ ↓ ↓


struct head
{
  int rc;
  int size_allocated; 
  function1_t fun;
  obj next_alloc;
};

typedef struct link
{
  obj elem;
  struct link *next;
} link_t;

/// @brief List of links
typedef struct list
{
  /// First element in the list
  link_t *first;
  /// Last element in the list
  link_t *last;
  /// Amount of links in the list
  int size;
} list_t;


void release_inner(obj o);
list_t *list_new();
link_t *link_new();
int list_length();
void list_append(list_t *list, obj elem);
bool list_remove(list_t *list, int index, bool delete);
void list_delete(list_t *list, bool delete);

//////////////////////////  ENDAST FÖR TEST   //////////////////////////////////////////
//                              ↓ ↓ ↓

int save_length()
{
  return list_length(save);
}

obj get_save()
{
  return save;
}

obj get_first_known_pointer()
{
  return first_known_pointer;
}

int get_count()
{
  return count;
}


void test_reset()
{
  count = 0;
  save = NULL;
  first_known_pointer = NULL;

}

/////////////////////        DELETE FUNCTIONS (shutdown)   //////////////////////////////////
//                               ↓ ↓ ↓


void save_delete()
{
  list_delete(save, false);
}

// Frigör alla known_pointers.
void known_pointers_delete()
{
  if (first_known_pointer) {
    head_t *current = first_known_pointer;

    while (current) {
      head_t *tmp = current;
      current = current->next_alloc;
      
      free(tmp);
    }
  }
}

/////////////////      HELPER FUNCTIONS for FREE   ///////////////////////////////////////////
//                             ↓ ↓ ↓
/*
void list_free_fun(elem_t pointer)
{
  release(pointer.p);
}
*/

///////////////////////////    CASCADE    ///////////////////////////////////////////////////
//                             ↓ ↓ ↓

void set_cascade_limit_inner(size_t size)
{
  limit = size;
}

size_t get_cascade_limit_inner()
{
  return limit;
}

///////////////////////////////////////////////////////////////////////////////////////////


void resume_release(size_t total_size_to_alloc)
{
  if (save) {
    
    count = 0;
    size_freed = 0;
    bool list_not_empty = false;
    bool condition = false;
    do {
      
      list_not_empty = list_remove(save, 0, false);
      // tar bort första pekaren i kön och releasar den.

      condition = (count < limit && count != 0) || size_freed < total_size_to_alloc;
      // Vi fortsätter loopen om, 1. antal releases är mindre än limit + count != 0.
      //                   eller, 2. storleken minne frigjort är mindre än allokerat. 
      }
    while (condition && list_not_empty);

  }
  count = 0;
  size_freed = 0;

}

void prepend_known_pointer(head_t *ob)
{
  obj tmp = first_known_pointer;
  first_known_pointer = ob;
  ob->next_alloc = tmp;
}

void remove_known_pointer(head_t *ob)
{
  if (first_known_pointer) {
    
    if (first_known_pointer == ob) {
      first_known_pointer = ((head_t *)first_known_pointer)->next_alloc;
      return;
    }
    
    head_t *current = first_known_pointer;
    while (current && current->next_alloc != ob) {
      current = current->next_alloc;
    }
    assert(current);
    current->next_alloc = ((head_t *)current->next_alloc)->next_alloc;
  }
  return;
}

void startup_first_known_pointer(head_t *ob)
{
  if (!first_known_pointer)  // skapar ett träd en gång, endast första gången denna if-sats körs.
    {
      first_known_pointer = ob;
      return;
    }
  
  prepend_known_pointer(ob);
  return;
}

void cleanup_inner()
{
  if(save != NULL)
    {
      bool a;
      do {
        a = list_remove(save, 0, true);   
      }
      while(a);         // Tar bort var objekt i listan och releasar det(true).
    }                   // remove returnerar true när det lyckas remova ngt.
}

// Mallocates ob, initializez rc and fun, returns pointer to data.
obj allocate_inner(size_t bytes, function1_t destructor)
{
  size_t total_size_to_alloc = bytes + sizeof(head_t);
  head_t *ob = malloc(total_size_to_alloc);
  
  if (ob)
    {
      *ob = (head_t) {
        .rc = 0,
        .size_allocated = total_size_to_alloc,
        .fun = destructor
      };
    
      startup_first_known_pointer(ob);
    
      resume_release(total_size_to_alloc);  // Frigör saker som köar pga Cascade-limit.
    }
    return object(ob);
}

// Callocates ob, initializez rc and fun, returns pointer to data.
obj allocate_array_inner(size_t elements, size_t elem_size, function1_t destructor)
{
  size_t total_size_to_alloc = elements * elem_size +  sizeof(struct head);
  head_t *ob = calloc(1, total_size_to_alloc);
    if (ob)

    {
      *ob = (head_t) {
        .rc = 0,
        .size_allocated = total_size_to_alloc,
        .fun = destructor
      };

      startup_first_known_pointer(ob);
  
      resume_release(total_size_to_alloc);  // Frigör saker som köar pga Cascade-limit.
    }
  return object(ob);
}
void deallocate_inner(obj o)
{
  if (head(o)->rc <= 0 && o)
    {
      if (head(o)->fun)
        {
          (head(o)->fun(o));
        }
     
      remove_known_pointer(head(o));

      size_freed += head(o)->size_allocated;
      free(head(o));
    }
}


// moves to the head of the object and increases rc if rc >= 0.
void retain_inner(obj o)
{
  if (head(o)->rc >= 0)
    {
      int rc = (head(o)->rc);
      rc = rc + 1;
      (head(o)->rc) = rc;     
    }
}

void decrement_rc(obj o)
{
  int rc = head(o)->rc;
  rc = rc - 1;
  (head(o)->rc) = rc;

  if (head(o) -> fun)
    {
      count++;
    }
  
  deallocate_inner(o);
  return;  
}

void save_pointer_to_release(obj o)
{
  count = 0;
  if (save == NULL) {
    save = list_new(NULL, NULL, NULL);
  }
  list_append(save, o);
  
}

// moves to the head of the object and decreses rc if rc < 1, and if o != NULL. Then deallocates it.
void release_inner(obj o)
{
  if (o)
    {
       if (count < limit)
         {
           decrement_rc(o);
         }
       else
         {
           save_pointer_to_release(o);
         }
    }
}
//Returns refrerence counter of the object.
size_t rc_inner(obj o)
{
  int rc = head(o)->rc;
  return rc;
}


//////////////////////        Linked List        ////////////////////////////////


list_t *list_new()
{
  list_t *new = calloc(1, sizeof(list_t));
  return new;
}

link_t *link_new()
{
  link_t *link = calloc(1, sizeof(link_t));
  return link;
}

void initiate_list(list_t *list, obj elem)
{
  link_t *new = link_new();
  new->elem = elem;
  list->first = new;
  list->last = new;
}

void list_append(list_t *list, obj elem)
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

bool list_remove(list_t *list, int index, bool delete)
{ int new_index = convert_to_valid_index(list, index);
  link_t **cursor = step_to_index(list, new_index);
  if (!*cursor) {
    return false;
  }
  else {
    link_t *tmp = *cursor;
    *cursor = (*cursor)->next;
    if (delete) release(tmp->elem);
    free(tmp);
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

      free(list);
    }
}
