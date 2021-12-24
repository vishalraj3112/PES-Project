/***********************************************************************************************************************
* File Name    : test_huffman.c
* Project      : PES Project
* Version      : 1.0
* Description  : Contains the unit test code for testing huffman encode, decode functions using Cmocka unit test
*                framework.
* Author       : Vishal Raj & referred from https://cmocka.org/.
* Creation Date: 12.23.21
***********************************************************************************************************************/
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "huff.h"

/***********************************************************************************************
* Name             : test_huffman_encode_decode_1
* Description      : Used to run automated test-1 on huffman encode, decode.
* Parameters       : None
* RETURN           : None
***********************************************************************************************/
static void test_huffman_encode_decode_1()
{
    uint8_t decodedstring[200] = {0};

    uint8_t data_to_encode[] = "Dec 11 04:05:45 vishal-Lenovo-ideapad-520S-14IKB rsyslogd:";
    uint8_t data_to_encode2[] = "Dec 11 04:09:45 vishal-Lenovo-ideapad-520S-14IKB org.gnome.Shell.desktop[2145]:\
                                 #0 0x7ffed15c99d0 I   resource:///org/gnome/gjs/modules/";
    uint8_t data_to_encode3[] = "Entering a random string";
    uint8_t data_to_encode4[] = "test string 12345 RaNDommmmm cAsinggg";
    uint8_t data_to_encode5[] = "-Lenovo-ideapad-520S-14IKB org.gnome.Shell.desktop[2145]: == Stack trace for \
                                 context 0x55eeb291c340 ==";
    uint8_t data_to_encode6[] = "I'll never stoop so low again, I guess I don't know my own strength";

    uint8_t buff[100];

    //test for input - 1
    uint16_t indexes = encode_string(data_to_encode, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 2
    indexes = encode_string(data_to_encode2, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode2,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 3
    indexes = encode_string(data_to_encode3, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode3,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 4
    indexes = encode_string(data_to_encode4, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode4,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 5
    indexes = encode_string(data_to_encode5, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode5,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 6
    indexes = encode_string(data_to_encode6, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode6,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

}

/***********************************************************************************************
* Name             : test_huffman_encode_decode_2
* Description      : Used to run automated test-2 on huffman encode, decode.
* Parameters       : None
* RETURN           : None
***********************************************************************************************/
static void test_huffman_encode_decode_2()
{
    uint8_t decodedstring[200] = {0};

    uint8_t data_to_encode1[] = "ubuntu-appindicators@ubuntu.com/indicatorStatus\
                                Icon.js:93 (0x7f0bb47cab38 @ 58)\
                                Dec 11 04:09:45 vishal-Lenovo-ideapad-520S-14IKB\
                                org.gnome.Shell.desktop[2145]:";

    uint8_t data_to_encode2[] = "you swore you've never hit 'em, never do nothing to hurt 'em\
                                 Now you're in each other's face\
                                 Spewing venom in your words when you spit 'em\
                                 you push, pull each other's hair, scratch, claw, bit 'em";

    uint8_t data_to_encode3[] = "Code: 89 d0 48 c1 e0 02 48 01 d0 48 8d 14 85 00 00 00 00 48 \
                                 01 d0 48 c1 e0 02 48 01 c8 48 8d 14 85 00 00 00 00 48 8d";

    uint8_t data_to_encode4[] = "ideapad-520S-14IKB whoopsie[1430]: [04:50:49] Not a paid data\
                                 plan: /org/freedesktop/NetworkManager/ActiveConnection/1";

    uint8_t data_to_encode5[] = "unit='dbus-org.freedesktop.nm-dispatcher.service' requested by\
                                 com':1.14' (uid=0 pid=872 comm='/usr/sbin/NetworkManager --no-\
                                 daemon ' label='unconfined')";

    uint8_t data_to_encode6[] = "'org.freedesktop.nm_dispatcher' unit='dbus-org.freedes\
                                  ktop.nm-dispatcher.service' requested by ':1.14' (uid=0 pid=872\
                                  comm='/usr/sbin/NetworkManager --no-daemon ' label='unconfined')";

    uint8_t data_to_encode7[] = "Dec 11 04:46:15 vishal-Lenovo-ideapad-520S-14IKB NetworkManager[872]\
                                : <info>  [1639178175.2755] manager: NetworkManager state is now CONNECTED_SITE";

    uint8_t data_to_encode8[] = "apad-520S-14IKB kernel: [ 2794.285885] huffman[3566]: segfault at 5600c5bf00cc\
                                 ip 00005600c59ec151 sp 00007ffc59c18bf0 error 6 in huffman[5600c59eb000+3000]";
    
    uint8_t data_to_encode9[] = "vishal-Lenovo-ideapad-520S-14IKB org.gnome.Shell.desktop[2145]: #0 0x7ffed15c9\
                                 9d0 I   resource:///org/gnome/gjs/modules/_legacy.js:83 (0x7f0bf80b5de0 @87)";
    
    uint8_t data_to_encode10[] = "Well, that's alright, because I love the way you lie\
                                  I love the way you lie I love the way you lie";

    uint8_t buff[200];

    //test for input - 1
    uint16_t indexes = encode_string(data_to_encode1, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode1,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 2
    indexes = encode_string(data_to_encode2, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode2,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 3
    indexes = encode_string(data_to_encode3, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode3,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 4
    indexes = encode_string(data_to_encode4, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode4,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 5
    indexes = encode_string(data_to_encode5, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode5,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));


    //test for input - 6
    indexes = encode_string(data_to_encode6, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode6,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 7
    indexes = encode_string(data_to_encode7, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode7,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 8
    indexes = encode_string(data_to_encode8, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode8,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 9
    indexes = encode_string(data_to_encode9, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode9,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

    //test for input - 10
    indexes = encode_string(data_to_encode10, buff, sizeof(buff));//string encoded here
    decode_string(buff, indexes, decodedstring);

    assert_string_equal(data_to_encode10,decodedstring);

    memset(buff, 0, sizeof(buff));
    memset(decodedstring,0,sizeof(decodedstring));

}


/***********************************************************************************************
* Name             : setup
* Description      : This function is used to setup resources after each test.
* Parameters       : test state
* RETURN           : None
***********************************************************************************************/
int setup(void **state)
{
    return 0;
}


/***********************************************************************************************
* Name             : teardown
* Description      : This function is used to clean resources after each test.
* Parameters       : test state
* RETURN           : None
***********************************************************************************************/
int teardown(void ** state)
{
    return 0;
}

/***********************************************************************************************
* Name             : main
* Description      : this is the entry point to the program, it initilizes the test functions
*                    and then runs the automated tests.
* Parameters       : None
* RETURN           : None
***********************************************************************************************/
int main()
{

    huffman_exe();

    const struct CMUnitTest tests[] =
    {
            unit_test(test_huffman_encode_decode_1),
            unit_test(test_huffman_encode_decode_2),
    };

    return cmocka_run_group_tests(tests,setup,teardown);

}
//[EOF] 

//test