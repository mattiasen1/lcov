#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include "refmem.h"
#include "list.h"
#include "tree.h"
#include "allocate.h"
#include <time.h>

struct test
{
  struct test *test;
  //struct test *test2;
};

typedef struct test test_t;

void destruct(obj t)
{
  release(((struct test *) t)->test);
  //release(((struct test *) t)->test2);

}


void test_allocate()
{
  test_reset();

  char *test= allocate(sizeof(char), NULL);
  CU_ASSERT_PTR_NOT_NULL(test);
  retain(test);
  CU_ASSERT_TRUE(rc(test)==1);
  release(test);

  test_t *t = allocate(sizeof(test_t), NULL);
  retain(t);
  CU_ASSERT_PTR_NOT_NULL(t);
  release(t);

  shutdown();

}
void test_allocate_array()
{
  test_reset();

  
  char *test= allocate_array(1, sizeof(char), NULL);
  CU_ASSERT_PTR_NOT_NULL(test);
  retain(test);
  CU_ASSERT_TRUE(rc(test)==1);
  release(test);

  test_t *t = allocate_array(3, sizeof(test_t), NULL);
  retain(t);
  CU_ASSERT_PTR_NOT_NULL(t);
  release(t);

  test_t *t2 = allocate_array(67, sizeof(test_t), NULL);
  retain(t2);
  CU_ASSERT_PTR_NOT_NULL(t2);
  release(t2);

  shutdown();
}
void test_retain_release()
{
  test_reset();

  test_t *t = allocate(sizeof(test_t), NULL);
  CU_ASSERT_TRUE(rc(t)==0);
  retain(t);
  CU_ASSERT_TRUE(rc(t)==1);
  release(t);

  test_t *t2 = allocate(sizeof(test_t), NULL);
  CU_ASSERT_TRUE(rc(t2)==0);
  retain(t2);
  retain(t2);
  retain(t2);
  retain(t2);
  retain(t2);
  retain(t2);
  retain(t2);
  retain(t2);       
  CU_ASSERT_TRUE(rc(t2)==8);
  release(t2);
  release(t2);
  release(t2);
  release(t2);
  release(t2);
  release(t2);
  release(t2);
  release(t2);

  test_t *t3 = allocate(sizeof(test_t), NULL);
  release(t3);

  shutdown();
  
}
void test_rc()
{
  test_reset();

  test_t *t = allocate(sizeof(test_t), NULL);
  CU_ASSERT_TRUE(rc(t)==0);
  retain(t);
  CU_ASSERT_TRUE(rc(t)==1);
  retain(t);
  CU_ASSERT_TRUE(rc(t)==2);
  retain(t);
  CU_ASSERT_TRUE(rc(t)==3);
  retain(t);
  CU_ASSERT_TRUE(rc(t)==4);
  retain(t);
  CU_ASSERT_TRUE(rc(t)==5);
  
  release(t);
  CU_ASSERT_TRUE(rc(t)==4);
  release(t);
  CU_ASSERT_TRUE(rc(t)==3);
  release(t);
  CU_ASSERT_TRUE(rc(t)==2);
  release(t);
  CU_ASSERT_TRUE(rc(t)==1);
  release(t);

  
  shutdown();
}
void test_destructor()
{
  test_reset();

  struct test *t = allocate_array(1, sizeof(struct test), destruct);
  retain(t);
  
  t->test = allocate_array(1, sizeof(struct test), destruct);
  retain(t->test);
  
  t->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(t->test->test);
  
  t->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(t->test->test->test);
  
  t->test->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(t->test->test->test->test);

  t->test->test->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(t->test->test->test->test->test);
  
  t->test->test->test->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(t->test->test->test->test->test->test);
  
  struct test *Q = allocate_array(1, sizeof(struct test), destruct);
  retain(Q);
  
  Q->test = allocate_array(1, sizeof(struct test), destruct);
  retain(Q->test);
  
  Q->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(Q->test->test);
  
  Q->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(Q->test->test->test);
  
  Q->test->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(Q->test->test->test->test);

  Q->test->test->test->test->test = allocate_array(1, sizeof(struct test), destruct);
  retain(Q->test->test->test->test->test);
  
  release(t);
  release(Q);

 
  char *test = allocate(sizeof(char), NULL);
  retain(test);
  release(test);
  
  
  char *test2 = allocate(sizeof(char), NULL);
  retain(test2);
  release(test2);
  
  
  char *test3 = allocate(sizeof(char), NULL);
  retain(test3);
  release(test3);
  
  shutdown();
  
}

void test_deallocate()
{
  test_reset();
  
  char *test= allocate(sizeof(char), NULL);
  retain(test);

  deallocate(test);
  CU_ASSERT_TRUE(rc(test)==1);
  release(test);

  char *test2= allocate(sizeof(char), NULL);
  
  retain(test2);
  deallocate(test2);
  retain(test2);
  retain(test2);
  deallocate(test2);
  CU_ASSERT_TRUE(rc(test2)==3);
  release(test2);
  CU_ASSERT_TRUE(rc(test2)==2);
  deallocate(test2);
  release(test2);
  
  CU_ASSERT_TRUE(rc(test2)==1);
  release(test2);


  shutdown();  
}

void test_get_and_set_cascade_limit()
{
  test_reset();

  CU_ASSERT_TRUE(get_cascade_limit() >= 0);  //eftersom den är av typen size_t
  
  set_cascade_limit(5);
  CU_ASSERT_TRUE(get_cascade_limit() == 5);
  
  set_cascade_limit(12);
  CU_ASSERT_TRUE(get_cascade_limit() == 12);
  
  shutdown();
}

void test_cascade()
{
  test_reset();

  int rand_int = rand() % 200; // random int mellan 1-200.

  for (int i = 1; i < 200; i++) {
    set_cascade_limit(i);
    struct test *A = allocate_array(1, sizeof(struct test), destruct);
    retain(A);
    struct test *B = A;
    
    for (int a = 0; a < rand_int; a++) {
       B->test = allocate_array(1, sizeof(struct test), destruct);
       B = B->test;
       retain(B);
    }
   
    release(A);
  }
 
  shutdown();  
}

void test_aux(elem_t pointer)
{
  release(pointer.p);
}


void test_size_freed()
{
  test_reset();

  list_t *test_list = list_new(NULL, test_aux, NULL);
  set_cascade_limit(1);

  for (int i = 0; i < 10; i++) {

    struct test *A = allocate(sizeof(struct test), destruct);
    retain(A);

    A->test = allocate(sizeof(struct test), destruct);
    retain(A->test);
    
    
    list_append(test_list, (elem_t) { .p = A });
  }
  
  list_delete(test_list, true);
  
  struct test *C = allocate_array(37, sizeof(struct test), destruct);
  retain(C);

  // Se förklaring i del två varför det borde vara så här.
  CU_ASSERT_TRUE(save_length() == 0);
  
  release(C);
  

  ////////////  DEL TVÅ //////////////////////////////////
  

  test_list = list_new(NULL, test_aux, NULL);
  set_cascade_limit(1);
  
  for (int i = 0; i < 20; i++) {

    struct test *A = allocate(sizeof(struct test), destruct);
    retain(A);
    struct test *B = A;
    
    for (int a = 0; a < 10; a++) {
      B->test = allocate(sizeof(struct test), destruct);
       B = B->test;
       retain(B);
    }
    
    list_append(test_list, (elem_t) { .p = A });
  }
  
  list_delete(test_list, true);
  
  struct test *D = allocate_array(157, sizeof(struct test), destruct);
  retain(D);

  /* Totala mängd allocs i for-loopen: 10*(4+1) = 50
     set_cascade_limit == 1, därmed frigörs 1 per kedja om 5 vid list_delete.
     Allocs kvar efter list_delete 50-(1*10) = 40. Dessa ligger nu i cascade-kön.
     
     sizeof(struct test) == 8.
     sizeof(head_t) == 24.
     Därmed har varje allokering storleken 32.
     40*32 = 1280.

     För att allt detta ska frigöras vid EN ENDA allokering måste vi allokera minst 960.
     
     D har storleken 24(header) + 157 * 8 = 960.

     Alltså bör allokeringen av D frigöra allt i kön och längden av cascade-kön ska vara lika med 0.
   */

  printf("%d" , save_length());
  CU_ASSERT_TRUE(save_length() == 0);
  
  release(D);              
  shutdown();
}


void test_cleanup()
{
  test_reset();
  
  int rand_int = rand() % 100;

  list_t *test_list = list_new(NULL, test_aux, NULL);
  set_cascade_limit(5);
  
  for (int i = 1; i < 100; i++) {
    
    struct test *A = allocate_array(1, sizeof(struct test), destruct);
    retain(A);
    struct test *B = A;
    
    for (int a = 0; a < rand_int; a++) {
       B->test = allocate_array(1, sizeof(struct test), destruct);
       B = B->test;
       retain(B);
    }
    list_append(test_list, (elem_t) { .p = A });
  }
  
  list_delete(test_list, true); // releasar alla objekt i listan.
  // tanken att lägga de i en lista är att det det ska kunna releasas utan allokeringar imellan
  // så att vi kan testa om cleanup gör det den ska, alltså frigör allt i "save" listan,
  // utan minesläckage.

 CU_ASSERT_TRUE(save_length() > 0);
 cleanup();
 CU_ASSERT_TRUE(save_length() == 0);
 
  //////////////////////   DEL TVÅ (samma sak, nya värden)  ///////////////////////////

  test_list = list_new(NULL, test_aux, NULL);
  set_cascade_limit(3);
  
  for (int i = 1; i < 100; i++) {
    
    struct test *A = allocate_array(1, sizeof(struct test), destruct);
    retain(A);
    struct test *B = A;
    
    for (int a = 0; a < rand_int; a++) {
       B->test = allocate_array(1, sizeof(struct test), destruct);
       B = B->test;
       retain(B);
    }
    list_append(test_list, (elem_t) { .p = B });
  }
  
  list_delete(test_list, true);

 CU_ASSERT_TRUE(save_length() > 0);
 cleanup();
 CU_ASSERT_TRUE(save_length() == 0);

  shutdown();  
}



void test_shutdown()

{
  test_reset();

  set_cascade_limit(100);
  
  for (int i = 1; i < 100; i++) {
    
    struct test *A = allocate_array(1, sizeof(struct test), destruct);
    int rand_int = rand() % 20;
    
    for (int a = 0; a < rand_int; a++) {
      // Vi får en blandad mängd retains på allokeringar, men inga sparas eller releasas.
      // det ska shutdown ta hand om utan minesläckage.
      retain(A); 
    }
  }
  shutdown();
}


int main(int argc, char *argv[argc])
{
  // För att få korrekt random.
  srand(time(NULL));
  
  // Initialise
  CU_initialize_registry();

  // Set up suites and tests
  
  CU_pSuite allocate = CU_add_suite("Test malloc", NULL, NULL);
  CU_add_test(allocate, "Malloc", test_allocate);
  CU_pSuite deallocate = CU_add_suite("Test free", NULL, NULL);
  CU_add_test(deallocate, "Free", test_deallocate);
  
  CU_pSuite allocate_array = CU_add_suite("Test calloc", NULL, NULL);
  CU_add_test(allocate_array, "Calloc", test_allocate_array);
  
  CU_pSuite destructor = CU_add_suite("Test destructor", NULL, NULL);
  CU_add_test(destructor, "Destructor function", test_destructor);
   
  CU_pSuite retain_release = CU_add_suite("Test retain and release", NULL, NULL);
  CU_add_test(retain_release, "Retain_release", test_retain_release);
  CU_pSuite rc = CU_add_suite("Test reference counter", NULL, NULL);
  CU_add_test(rc, "Reference count", test_rc);

  
  CU_pSuite set_get_cascade_limit = CU_add_suite("Test set and get cascade limit", NULL, NULL);
  CU_add_test(set_get_cascade_limit, "Set and Get cascade-limit", test_get_and_set_cascade_limit);
  CU_pSuite cascade_limit = CU_add_suite("Test cascade limit", NULL, NULL);
  CU_add_test(cascade_limit, "Cascade-limit", test_cascade);
  
  CU_pSuite size_freed = CU_add_suite("Test size freed", NULL, NULL);
  CU_add_test(size_freed, "Test size freed", test_size_freed);
  
  
  CU_pSuite cleanup = CU_add_suite("Test Cleanup", NULL, NULL);
  CU_add_test(cleanup, "Test Cleanup", test_cleanup);
  
  CU_pSuite shutdown = CU_add_suite("Test Shutdown", NULL, NULL);
  CU_add_test(shutdown, "Test Shutdown", test_shutdown);
  

  // Actually run tests
  CU_basic_run_tests();
  
  // Tear down
  CU_cleanup_registry();
  return CU_get_error();
}
