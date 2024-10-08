#include "tcp_buffer.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

void test_buffer_reset() {
  buffer_reset();
  CU_ASSERT_TRUE(buffer_is_empty()); // Buffer should be empty after reset
  CU_ASSERT_FALSE(buffer_is_full()); // Buffer should not be full after reset
}

void test_buffer_read_write_seq() {
    uint16_t* got_address;
    buffer_reset();
    uint16_t* expected_addresses[depth] = {};
    int i = 0;
    for (i=0; i<depth; i++) {
        expected_addresses[i]=buffer_reserve_wr();
        buffer_done_wr();
        got_address = buffer_read();
        CU_ASSERT_PTR_EQUAL_FATAL(expected_addresses[i], got_address);
        buffer_release();
    }

    buffer_reserve_wr();
    buffer_done_wr();
    got_address = buffer_read();
    CU_ASSERT_PTR_EQUAL_FATAL(expected_addresses[0], got_address); 
    buffer_release();

    CU_ASSERT_PTR_NULL(buffer_read());

}
void test_buffer_release_multiple_buffers() {
    buffer_reset();

    // Reserve two buffers and mark them as written
    uint16_t* buf1 = buffer_reserve_wr();
    uint16_t* buf2 = buffer_reserve_wr();
    CU_ASSERT_PTR_NOT_NULL(buf1);
    CU_ASSERT_PTR_NOT_NULL(buf2);

    buffer_done_wr(); 
    buffer_done_wr();
    buffer_done_wr();
    buffer_read();
    buffer_read();
    buffer_read();
    //blocked 3 bytes
    CU_ASSERT_EQUAL(buffer_get_blocked_bytes(), width * 3); 

    // read small peace 
    buffer_release_bytes(width / 4 );
    CU_ASSERT_EQUAL(buffer_get_blocked_bytes(), (width * 3) - (width / 4)); 

    buffer_release_bytes((width / 4 ) * 3 );    
    CU_ASSERT_EQUAL(buffer_get_blocked_bytes(), (width * 2) ); 

    buffer_release_bytes(width * 2);    
    CU_ASSERT_EQUAL(buffer_get_blocked_bytes(), 0 ); 

    CU_ASSERT_PTR_NULL( buffer_read());

}
int main() {
  CU_initialize_registry();
  CU_pSuite suite = CU_add_suite("Buffer Tests", 0, 0);

  CU_add_test(suite, "Test buffer reset", test_buffer_reset);
  CU_add_test(suite, "Full test", test_buffer_read_write_seq);
  CU_add_test(suite, "bytes release", test_buffer_release_multiple_buffers);

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  unsigned int number_of_failures = CU_get_number_of_failures();
  CU_cleanup_registry();

  return (number_of_failures == 0) ? 0 : 1;
}