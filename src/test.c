#include "refmem.h"
#include <assert.h>
#include <stdio.h>

struct cell
{
  struct cell *cell;
  int i;
  char *string;
};

void cell_destructor(obj c)
{
  release(((struct cell *) c)->cell);
}

int main(void)
{

  struct cell *c = allocate_array(1, sizeof(struct cell), cell_destructor);

  assert(rc(c) == 0);
  retain(c);
  assert(rc(c) == 1);


  c->cell = allocate_array(1, sizeof(struct cell), cell_destructor);

  assert(rc(c->cell) == 0);
  retain(c->cell);
  assert(rc(c->cell) == 1);

  c->cell->cell = NULL;

  release(c);

  return 0;
}
