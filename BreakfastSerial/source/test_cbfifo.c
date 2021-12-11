/***********************************************************************************************************************
* File Name    : test_cbfifo.c
* Project      : PES Assignment 6
* Version      : 1.0
* Description  : Contains all the function implementation code along with variable definitions for cbfifo test suite
* Author       : Howdy Pierce and Vishal Raj
* Creation Date: 11.10.21
***********************************************************************************************************************/
#include "test_cbfifo.h"
#include <string.h>
#include "cbfifo.h"
#include <assert.h>
#include "uart.h"
#include <stdint.h>
#include "Log.h"

/***********************************************************************************************
* Name			   : test_cbfifo
* Description 	   : used to run test on cbfifo multiple instances.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void test_cbfifo()
{
  char *str ="To be, or not to be: that is the question:\n"
    "Whether 'tis nobler in the mind to suffer\n"
    "The slings and arrows of outrageous fortune,\n"
    "Or to take arms against a sea of troubles,\n"
    "And by opposing end them? To die, to sleep--\n"
    "No more--and by a sleep to say we end\n"
    "The heart-ache and the thousand natural shocks\n"
    "That flesh is heir to, 'tis a consummation\n"
    "Devoutly to be wish'd. To die, to sleep;\n"
    "To sleep: perchance to dream: ay, there's the rub;\n"
    "For in that sleep of death what dreams may come\n"
    "When we have shuffled off this mortal coil,\n"
    "Must give us pause.";

  char buf[1024];
  cbfifo_t *fp1 = &rx;//creating cbfifo instance for testing
  const int cap = cbfifo_capacity(fp1);

  // asserts in following 2 lines -- this is not testing the student,
  // it's validating that the test is correct
  assert(strlen(str) >= cap*2);
  assert(sizeof(buf) > cap);
  //assert(cap == 128 || cap == 127);
  assert(cap == 256 || cap == 255);

  assert(cbfifo_length(fp1) == 0);
  assert(cbfifo_dequeue(fp1,buf, cap) == 0);
  assert(cbfifo_dequeue(fp1,buf, 1) == 0);

  // enqueue 10 bytes, then dequeue same amt
  assert(cbfifo_enqueue(fp1,str, 10) == 10);
  assert(cbfifo_length(fp1) == 10);
  assert(cbfifo_dequeue(fp1,buf, 10) == 10);
  assert(strncmp(buf, str, 10) == 0);
  assert(cbfifo_length(fp1) == 0);

  // enqueue 20 bytes;  dequeue 5, then another 20
  assert(cbfifo_enqueue(fp1,str, 20) == 20);
  assert(cbfifo_length(fp1) == 20);
  assert(cbfifo_dequeue(fp1,buf, 5) == 5);
  assert(cbfifo_length(fp1) == 15);
  assert(cbfifo_dequeue(fp1,buf+5, 20) == 15);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str, 20) == 0);

  // fill buffer and then read it back out
  assert(cbfifo_enqueue(fp1,str, cap) == cap);
  assert(cbfifo_length(fp1) == cap);
  assert(cbfifo_enqueue(fp1,str, 1) == 0);
  assert(cbfifo_dequeue(fp1,buf, cap) == cap);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str, cap) == 0);

  // Add 20 bytes and pull out 18
  assert(cbfifo_enqueue(fp1,str, 20) == 20);
  assert(cbfifo_length(fp1) == 20);
  assert(cbfifo_dequeue(fp1,buf, 18) == 18);
  assert(cbfifo_length(fp1) == 2);
  assert(strncmp(buf, str, 18) == 0);

  // Now add a bunch of data in 4 chunks
  int chunk_size = (cap-2) / 4;
  for (int i=0; i<4; i++) {
    assert(cbfifo_enqueue(fp1,str+i*chunk_size, chunk_size) == chunk_size);
    assert(cbfifo_length(fp1) == (i+1)*chunk_size + 2);
  }
  assert(cbfifo_length(fp1) == 4*chunk_size + 2);

  // Take out the 2 remaining bytes from above
  assert(cbfifo_dequeue(fp1,buf, 2) == 2);
  assert(strncmp(buf, str+18, 2) == 0);

  // now read those chunks out a byte at a time
  for (int i=0; i<chunk_size*4; i++) {
    assert(cbfifo_dequeue(fp1,buf+i, 1) == 1);
    assert(cbfifo_length(fp1) == chunk_size*4 - i - 1);
  }
  assert(strncmp(buf, str, chunk_size*4) == 0);

  // write more than capacity
  assert(cbfifo_enqueue(fp1,str, 65) == 65);
  assert(cbfifo_enqueue(fp1,str+65, cap) == cap-65);
  assert(cbfifo_length(fp1) == cap);
  assert(cbfifo_dequeue(fp1,buf, cap) == cap);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str, cap) == 0);

  // write zero bytes
  assert(cbfifo_enqueue(fp1,str, 0) == 0);
  assert(cbfifo_length(fp1) == 0);

  // Exercise the following conditions:
  //    enqueue when read < write:
  //        bytes < CAP-write  (1)
  //        bytes exactly CAP-write  (2)
  //        bytes > CAP-write but < space available (3)
  //        bytes exactly the space available (4)
  //        bytes > space available (5)
  assert(cbfifo_enqueue(fp1,str, 32) == 32);  // advance so that read < write
  assert(cbfifo_length(fp1) == 32);
  assert(cbfifo_dequeue(fp1,buf, 16) == 16);
  assert(cbfifo_length(fp1) == 16);
  assert(strncmp(buf, str, 16) == 0);

  assert(cbfifo_enqueue(fp1,str+32, 32) == 32);  // (1)
  assert(cbfifo_length(fp1) == 48);
  assert(cbfifo_enqueue(fp1,str+64, cap-64) == cap-64);  // (2)
  assert(cbfifo_length(fp1) == cap-16);
  assert(cbfifo_dequeue(fp1,buf+16, cap-16) == cap-16);
  assert(strncmp(buf, str, cap) == 0);

  assert(cbfifo_enqueue(fp1,str, 32) == 32);  // advance so that read < write
  assert(cbfifo_length(fp1) == 32);
  assert(cbfifo_dequeue(fp1,buf, 16) == 16);
  assert(cbfifo_length(fp1) == 16);
  assert(strncmp(buf, str, 16) == 0);

  //fifo 2 instance-------------------------------------------------------------------------
  	  char buf_1[1024];
  	  cbfifo_t *fp2 = &tx;//creating cbfifo instance for testing
      const int capi = cbfifo_capacity(fp2);

      // asserts in following 2 lines -- this is not testing the student,
      // it's validating that the test is correct
      assert(strlen(str) >= capi*2);
      assert(sizeof(buf_1) > capi);
      //assert(capi == 128 || capi == 127);
      assert(capi == 256 || capi == 255);

      assert(cbfifo_length(fp2) == 0);
      assert(cbfifo_dequeue(fp2,buf_1, capi) == 0);
      assert(cbfifo_dequeue(fp2,buf_1, 1) == 0);

      // enqueue 10 bytes, then dequeue same amt
      assert(cbfifo_enqueue(fp2,str, 10) == 10);
      assert(cbfifo_length(fp2) == 10);
      assert(cbfifo_dequeue(fp2,buf_1, 10) == 10);
      assert(strncmp(buf_1, str, 10) == 0);
      assert(cbfifo_length(fp2) == 0);

      // enqueue 20 bytes;  dequeue 5, then another 20
      assert(cbfifo_enqueue(fp2,str, 20) == 20);
      assert(cbfifo_length(fp2) == 20);
      assert(cbfifo_dequeue(fp2,buf_1, 5) == 5);
      assert(cbfifo_length(fp2) == 15);
      assert(cbfifo_dequeue(fp2,buf_1+5, 20) == 15);
      assert(cbfifo_length(fp2) == 0);
      assert(strncmp(buf_1, str, 20) == 0);

      // fill buf_1fer and then read it back out
      assert(cbfifo_enqueue(fp2,str, capi) == capi);
      assert(cbfifo_length(fp2) == capi);
      assert(cbfifo_enqueue(fp2,str, 1) == 0);
      assert(cbfifo_dequeue(fp2,buf_1, capi) == capi);
      assert(cbfifo_length(fp2) == 0);
      assert(strncmp(buf_1, str, capi) == 0);

      // Add 20 bytes and pull out 18
      assert(cbfifo_enqueue(fp2,str, 20) == 20);
      assert(cbfifo_length(fp2) == 20);
      assert(cbfifo_dequeue(fp2,buf_1, 18) == 18);
      assert(cbfifo_length(fp2) == 2);
      assert(strncmp(buf_1, str, 18) == 0);

      // Now add a bunch of data in 4 chunks
      int chunk_size_ = (capi-2) / 4;
      for (int i=0; i<4; i++) {
        assert(cbfifo_enqueue(fp2,str+i*chunk_size_, chunk_size_) == chunk_size_);
        assert(cbfifo_length(fp2) == (i+1)*chunk_size_ + 2);
      }
      assert(cbfifo_length(fp2) == 4*chunk_size_ + 2);

      // Take out the 2 remaining bytes from above
      assert(cbfifo_dequeue(fp2,buf_1, 2) == 2);
      assert(strncmp(buf_1, str+18, 2) == 0);

      // now read those chunks out a byte at a time
      for (int i=0; i<chunk_size_*4; i++) {
        assert(cbfifo_dequeue(fp2,buf_1+i, 1) == 1);
        assert(cbfifo_length(fp2) == chunk_size_*4 - i - 1);
      }
      assert(strncmp(buf_1, str, chunk_size_*4) == 0);

      // write more than capiacity
      assert(cbfifo_enqueue(fp2,str, 65) == 65);
      assert(cbfifo_enqueue(fp2,str+65, capi) == capi-65);
      assert(cbfifo_length(fp2) == capi);
      assert(cbfifo_dequeue(fp2,buf_1, capi) == capi);
      assert(cbfifo_length(fp2) == 0);
      assert(strncmp(buf_1, str, capi) == 0);

      // write zero bytes
      assert(cbfifo_enqueue(fp2,str, 0) == 0);
      assert(cbfifo_length(fp2) == 0);

      // Exercise the following conditions:
      //    enqueue when read < write:
      //        bytes < capi-write  (1)
      //        bytes exactly capi-write  (2)
      //        bytes > capi-write but < space available (3)
      //        bytes exactly the space available (4)
      //        bytes > space available (5)
      assert(cbfifo_enqueue(fp2,str, 32) == 32);  // advance so that read < write
      assert(cbfifo_length(fp2) == 32);
      assert(cbfifo_dequeue(fp2,buf_1, 16) == 16);
      assert(cbfifo_length(fp2) == 16);
      assert(strncmp(buf_1, str, 16) == 0);

      assert(cbfifo_enqueue(fp2,str+32, 32) == 32);  // (1)
      assert(cbfifo_length(fp2) == 48);
      assert(cbfifo_enqueue(fp2,str+64, capi-64) == capi-64);  // (2)
      assert(cbfifo_length(fp2) == capi-16);
      assert(cbfifo_dequeue(fp2,buf_1+16, capi-16) == capi-16);
      assert(strncmp(buf_1, str, capi) == 0);

      assert(cbfifo_enqueue(fp2,str, 32) == 32);  // advance so that read < write
      assert(cbfifo_length(fp2) == 32);
      assert(cbfifo_dequeue(fp2,buf_1, 16) == 16);
      assert(cbfifo_length(fp2) == 16);
      assert(strncmp(buf_1, str, 16) == 0);

      assert(cbfifo_enqueue(fp2,str+32, capi-20) == capi-20);  // (3)
      assert(cbfifo_length(fp2) == capi-4);
      assert(cbfifo_dequeue(fp2,buf_1, capi-8) == capi-8);
      assert(strncmp(buf_1, str+16, capi-8) == 0);
      assert(cbfifo_length(fp2) == 4);
      assert(cbfifo_dequeue(fp2,buf_1, 8) == 4);
      assert(strncmp(buf_1, str+16+capi-8, 4) == 0);
      assert(cbfifo_length(fp2) == 0);

      assert(cbfifo_enqueue(fp2,str, 49) == 49);  // advance so that read < write
      assert(cbfifo_length(fp2) == 49);
      assert(cbfifo_dequeue(fp2,buf_1, 16) == 16);
      assert(cbfifo_length(fp2) == 33);
      assert(strncmp(buf_1, str, 16) == 0);

      assert(cbfifo_enqueue(fp2,str+49, capi-33) == capi-33);  // (4)
      assert(cbfifo_length(fp2) == capi);
      assert(cbfifo_dequeue(fp2,buf_1, capi) == capi);
      assert(cbfifo_length(fp2) == 0);
      assert(strncmp(buf_1, str+16, capi) == 0);

      assert(cbfifo_enqueue(fp2,str, 32) == 32);  // advance so that read < write
      assert(cbfifo_length(fp2) == 32);
      assert(cbfifo_dequeue(fp2,buf_1, 16) == 16);
      assert(cbfifo_length(fp2) == 16);
      assert(strncmp(buf_1, str, 16) == 0);

      assert(cbfifo_enqueue(fp2,str+32, capi) == capi-16);  // (5)
      assert(cbfifo_dequeue(fp2,buf_1, 1) == 1);
      assert(cbfifo_length(fp2) == capi-1);
      assert(cbfifo_dequeue(fp2,buf_1+1, capi-1) == capi-1);
      assert(cbfifo_length(fp2) == 0);
      assert(strncmp(buf_1, str+16, capi) == 0);

      //    enqueue when write < read:
      //        bytes < read-write (6)
      //        bytes exactly read-write (= the space available) (7)
      //        bytes > space available (8)

      int wpos_=0, rpos_=0;
      assert(cbfifo_enqueue(fp2,str, capi-4) == capi-4);
      wpos_ += capi-4;
      assert(cbfifo_length(fp2) == capi-4);
      assert(cbfifo_dequeue(fp2,buf_1, 32) == 32);
      rpos_ += 32;
      assert(cbfifo_length(fp2) == capi-36);
      assert(strncmp(buf_1, str, 32) == 0);
      assert(cbfifo_enqueue(fp2,str+wpos_, 12) == 12);
      wpos_ += 12;
      assert(cbfifo_length(fp2) == capi-24);

      assert(cbfifo_enqueue(fp2,str+wpos_, 16) == 16);  // (6)
      assert(cbfifo_length(fp2) == capi-8);
      assert(cbfifo_dequeue(fp2,buf_1, capi) == capi-8);
      assert(cbfifo_length(fp2) == 0);
      assert(strncmp(buf_1, str+rpos_, capi-8) == 0);

      // reset
      wpos_=0;
      rpos_=0;
      assert(cbfifo_enqueue(fp2,str, capi-4) == capi-4);
      wpos_ += capi-4;
      assert(cbfifo_length(fp2) == capi-4);
      assert(cbfifo_dequeue(fp2,buf_1, 32) == 32);
      rpos_ += 32;
      assert(cbfifo_length(fp2) == capi-36);
      assert(strncmp(buf_1, str, 32) == 0);
      assert(cbfifo_enqueue(fp2,str+wpos_, 12) == 12);
      wpos_ += 12;
      assert(cbfifo_length(fp2) == capi-24);



    assert(cbfifo_enqueue(fp1,str+32, cap-20) == cap-20);  // (3)
    assert(cbfifo_length(fp1) == cap-4);
    assert(cbfifo_dequeue(fp1,buf_1, cap-8) == cap-8);
    assert(strncmp(buf_1, str+16, cap-8) == 0);
    assert(cbfifo_length(fp1) == 4);
    assert(cbfifo_dequeue(fp1,buf_1, 8) == 4);
    assert(strncmp(buf_1, str+16+cap-8, 4) == 0);
    assert(cbfifo_length(fp1) == 0);


    assert(cbfifo_enqueue(fp2,str+wpos_, 24) == 24);  // (7)
    assert(cbfifo_length(fp2) == capi);
    assert(cbfifo_dequeue(fp2,buf_1, capi) == capi);
    assert(cbfifo_length(fp2) == 0);
    assert(strncmp(buf_1, str+rpos_, capi) == 0);

  	  // reset
  	  wpos_=0;
  	  rpos_=0;
  	  assert(cbfifo_enqueue(fp2,str, capi-4) == capi-4);
  	  wpos_ += capi-4;
  	  assert(cbfifo_length(fp2) == capi-4);
  	  assert(cbfifo_dequeue(fp2,buf_1, 32) == 32);
  	  rpos_ += 32;
  	  assert(cbfifo_length(fp2) == capi-36);
  	  assert(strncmp(buf_1, str, 32) == 0);
  	  assert(cbfifo_enqueue(fp2,str+wpos_, 12) == 12);
  	  wpos_ += 12;
  	  assert(cbfifo_length(fp2) == capi-24);

  	  assert(cbfifo_enqueue(fp2,str+wpos_, 64) == 24);  // (8)
  	  assert(cbfifo_length(fp2) == capi);
  	  assert(cbfifo_dequeue(fp2,buf_1, capi) == capi);
  	  assert(cbfifo_length(fp2) == 0);
  	  assert(strncmp(buf_1, str+rpos_, capi) == 0);

  	  //uncomment below line as required to display output
  	  //LOG("%s tx: passed all test cases\n\r", __FUNCTION__);

  assert(cbfifo_enqueue(fp1,str, 49) == 49);  // advance so that read < write
  assert(cbfifo_length(fp1) == 49);
  assert(cbfifo_dequeue(fp1,buf, 16) == 16);
  assert(cbfifo_length(fp1) == 33);
  assert(strncmp(buf, str, 16) == 0);

  assert(cbfifo_enqueue(fp1,str+49, cap-33) == cap-33);  // (4)
  assert(cbfifo_length(fp1) == cap);
  assert(cbfifo_dequeue(fp1,buf, cap) == cap);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str+16, cap) == 0);

  assert(cbfifo_enqueue(fp1,str, 32) == 32);  // advance so that read < write
  assert(cbfifo_length(fp1) == 32);
  assert(cbfifo_dequeue(fp1,buf, 16) == 16);
  assert(cbfifo_length(fp1) == 16);
  assert(strncmp(buf, str, 16) == 0);

  assert(cbfifo_enqueue(fp1,str+32, cap) == cap-16);  // (5)
  assert(cbfifo_dequeue(fp1,buf, 1) == 1);
  assert(cbfifo_length(fp1) == cap-1);
  assert(cbfifo_dequeue(fp1,buf+1, cap-1) == cap-1);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str+16, cap) == 0);

  //    enqueue when write < read:
  //        bytes < read-write (6)
  //        bytes exactly read-write (= the space available) (7)
  //        bytes > space available (8)

  int wpos=0, rpos=0;
  assert(cbfifo_enqueue(fp1,str, cap-4) == cap-4);
  wpos += cap-4;
  assert(cbfifo_length(fp1) == cap-4);
  assert(cbfifo_dequeue(fp1,buf, 32) == 32);
  rpos += 32;
  assert(cbfifo_length(fp1) == cap-36);
  assert(strncmp(buf, str, 32) == 0);
  assert(cbfifo_enqueue(fp1,str+wpos, 12) == 12);
  wpos += 12;
  assert(cbfifo_length(fp1) == cap-24);

  assert(cbfifo_enqueue(fp1,str+wpos, 16) == 16);  // (6)
  assert(cbfifo_length(fp1) == cap-8);
  assert(cbfifo_dequeue(fp1,buf, cap) == cap-8);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str+rpos, cap-8) == 0);

  // reset
  wpos=0;
  rpos=0;
  assert(cbfifo_enqueue(fp1,str, cap-4) == cap-4);
  wpos += cap-4;
  assert(cbfifo_length(fp1) == cap-4);
  assert(cbfifo_dequeue(fp1,buf, 32) == 32);
  rpos += 32;
  assert(cbfifo_length(fp1) == cap-36);
  assert(strncmp(buf, str, 32) == 0);
  assert(cbfifo_enqueue(fp1,str+wpos, 12) == 12);
  wpos += 12;
  assert(cbfifo_length(fp1) == cap-24);

  assert(cbfifo_enqueue(fp1,str+wpos, 24) == 24);  // (7)
  assert(cbfifo_length(fp1) == cap);
  assert(cbfifo_dequeue(fp1,buf, cap) == cap);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str+rpos, cap) == 0);

  // reset
  wpos=0;
  rpos=0;
  assert(cbfifo_enqueue(fp1,str, cap-4) == cap-4);
  wpos += cap-4;
  assert(cbfifo_length(fp1) == cap-4);
  assert(cbfifo_dequeue(fp1,buf, 32) == 32);
  rpos += 32;
  assert(cbfifo_length(fp1) == cap-36);
  assert(strncmp(buf, str, 32) == 0);
  assert(cbfifo_enqueue(fp1,str+wpos, 12) == 12);
  wpos += 12;
  assert(cbfifo_length(fp1) == cap-24);

  assert(cbfifo_enqueue(fp1,str+wpos, 64) == 24);  // (8)
  assert(cbfifo_length(fp1) == cap);
  assert(cbfifo_dequeue(fp1,buf, cap) == cap);
  assert(cbfifo_length(fp1) == 0);
  assert(strncmp(buf, str+rpos, cap) == 0);

  //uncomment below line as required to display output
  //LOG("%s rx: passed all test cases\n\r", __FUNCTION__);
}
/*[EOF]*/
