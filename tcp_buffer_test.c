#include "tcp_buffer.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

void test_buffer_reset() {
  buffer_reset();
  CU_ASSERT_TRUE(buffer_is_empty()); // Buffer should be empty after reset
  CU_ASSERT_FALSE(buffer_is_full()); // Buffer should not be full after reset
}

void test_buffer_consume() {
  buffer_reset();

  uint16_t *buf = buffer_consume();
  CU_ASSERT_PTR_NOT_NULL(buf); // First consumption should be valid
  CU_ASSERT_EQUAL(buffer_get_write_index(), 1);

  for (int i = 0; i < depth; i++) {
    buffer_consume();
  }
  CU_ASSERT_TRUE(
      buffer_is_full()); // Buffer should be full after depth consumption
  CU_ASSERT_PTR_NULL(
      buffer_consume()); // Consuming from full buffer should return NULL
}

void test_buffer_release() {
  buffer_reset();
  buffer_consume();
  buffer_read();
  CU_ASSERT_EQUAL(buffer_get_read_index(),
                  1); // Read index should advance after release

  // buffer_reset();
  for (int i = 0; i < depth - 1; i++) {
    buffer_consume();
    buffer_read();
  }
  CU_ASSERT_EQUAL(buffer_get_read_index(), 0);
  buffer_consume();
  buffer_read(); // Should wrap around to 0
  CU_ASSERT_EQUAL(buffer_get_read_index(), 1);
}
void test_buffer_full_cycle() {
  buffer_reset();
  uint16_t *buffer_addresses[depth];
  for (int i = 0; i < depth; i++) {
    buffer_addresses[i] = buffer_consume();
    buffer_read();
  }
  buffer_reset();
  buffer_consume();
  buffer_read();
  buffer_consume();
  buffer_read();
  int i;
  for (i = 2; i < depth; i++) // 8
  {
    CU_ASSERT_EQUAL(buffer_addresses[i], buffer_consume());
  }
  buffer_consume();
  buffer_read();
  CU_ASSERT_EQUAL(buffer_addresses[1], buffer_consume());
}
int main() {
  CU_initialize_registry();
  CU_pSuite suite = CU_add_suite("Buffer Tests", 0, 0);

  CU_add_test(suite, "Test buffer reset", test_buffer_reset);
  CU_add_test(suite, "Test buffer consume", test_buffer_consume);
  CU_add_test(suite, "Test buffer release", test_buffer_release);
  CU_add_test(suite, "Full test", test_buffer_full_cycle);

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  unsigned int number_of_failures = CU_get_number_of_failures();
  CU_cleanup_registry();

  return (number_of_failures == 0) ? 0 : 1;
}