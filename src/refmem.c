#include "refmem.h"
#include "allocate.h"

void retain(obj o)
{
  return retain_inner(o);
}

void release(obj o)
{
  release_inner(o);
}

size_t rc(obj o)
{
  return rc_inner(o);
}

obj allocate(size_t bytes, function1_t destructor)
{
  return allocate_inner(bytes, destructor);
}

obj allocate_array(size_t elements, size_t elem_size, function1_t destructor)
{
  return allocate_array_inner(elements, elem_size, destructor);
}

void deallocate(obj o)
{
  deallocate_inner(o);
}

void set_cascade_limit(size_t size)
{
  set_cascade_limit_inner(size);
}

size_t get_cascade_limit()
{
  return get_cascade_limit_inner();
}

void cleanup()
{
  cleanup_inner();
  // Frigör hela cascade kön. True gör att så vi inte bryr oss om cascade-limit.
}

void shutdown()
{
  // Tar bort listan som sparar pekare till objekt som ska frigöras.
  save_delete();
  
  known_pointers_delete();
 }
