#include "refmem.h"

typedef struct head head_t;

/// @brief Allocates memory for an object including space for a header holding a reference counter and a destructor function (may be NULL). 
/// 
/// Resumes release of objects if cascade limit has been reached.
///
/// @param bytes: size in bytes of object to be allocated
/// @param destructor: (may be NULL) used to free complex data structures
/// @returns: pointer to allocated memory starting at object data.
obj allocate_inner(size_t bytes, function1_t destructor);

/// @brief Allocates and sets memory to 0 for an object including space for a header holding a reference counter and a destructor function (may be NULL). 
///  
/// Resumes release of objects if cascade limit has been reached.
///
/// @param elements: number of elements to be allocated
/// @param elem_size: size in bytes of an element 
/// @param destructor: (may be NULL) used to free complex data structures
/// @returns: pointer to allocated memory starting at object data.
obj allocate_array_inner(size_t elements, size_t elem_size, function1_t destructor);

/// @brief Calls the destructor function on an object, if any, and frees the object if its reference counter == 0.
///
/// @param o: pointer to the object
void deallocate_inner(obj o);

/// @brief Increments reference counter of an object.
///
/// @param o: pointer to the object
void retain_inner(obj o);

/// @brief Stores pointers to objects so that process of release can be resumed if cascade limit is reached. 
///
/// @param o: pointer to the object to be stored
void save_pointer_to_release(obj o);

/// @brief Decrements reference counter of an object if cascade limit has not been reached.
///
/// @param o: pointer to the object to be released
void release_inner(obj o);

/// @brief Returns reference counter of an object.
///
/// @param o: pointer to the object
/// @returns: reference counter of the object
size_t rc_inner(obj o);

/// @brief Decrements reference counter of an object. If reference counter == 1, the object is freed. 
///
/// @param o: pointer to the object
void decrement_rc(obj o);

/// @brief
///
/// @param
/// @return
void set_cascade_limit_inner(size_t size);

/// @brief
///
/// @param
/// @return
size_t get_cascade_limit_inner();

void save_delete();

/// @brief Stores pointers to objects that are allocated.
///
/// @param ob: pointer to header of the object allocated
/// @param size_of_allocation: size in bytes of the object allocated
void startup_known_pointers(head_t *ob, size_t size_of_allocation);

/// @brief Resumes process of releasing objects if cascade limit has been reached.
///
/// Process continues until limit is once again reached. However, if amount of bytes freed is less 
/// than needed for allocation, cascade limit is ignored.
///
/// @param total_size_to_alloc: size in bytes of object to be allocated including header
void resume_release(size_t total_size_to_alloc);

/// @brief Releases objects regardless of cascade limit.
void cleanup_inner();

void known_pointers_delete();

int save_length();

obj get_save();

obj get_first_known_pointers();

int get_count();

void test_reset();
