/* 
 * File:   unitest_main.cpp
 * Author: CEP
 *
 * Created on 2012-01-04 PM 04:13
 */

#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <queue>
#include <vector>
#include <limits>

#include "macro_log.hpp"
#include "cep_base.hpp"
#include "utils_time.h"
#include "circular_queue.hpp"
#include "buffer_metadata.hpp"
#include "buffer_utility.hpp"
#include "dynamic_struct.hpp"
#include "slice.hpp"

#include "event.hpp"

#include "utils_cstr2nbr.hpp"
// for pure tinyxml test
#include "tinyxml.h"
#include "utils_xml.hpp"

#include "singleton.hpp"
#include "config_metadata.hpp"

#include "utils_nbr2str.hpp"
#include "event2text.hpp"

#include "const_dataset.hpp"
#include "buffer_function.hpp"
#include "processing/idx_rbtree.hpp"

#include "processing/pn.hpp"
#include "processing/pe.hpp"
#include "processing/router.hpp"

#include "processing/plugin_symbol.hpp"
#include "dlhandler.hpp"
#include "processing/pe4initest.hpp"
#include "processing/utils.hpp"

#include "processing/idx_range.hpp"
#include "processing/idx_trie_tree.hpp"

#include "file_lock_counter.hpp"

#include "communication/baseutils/FSPublic.h"
#include "communication/baseutils/lzowrapper.h"
#include "processing/event_file_handler.hpp"

using namespace std;
using namespace cep;

typedef cep::circular_bounded_queue<int> cep_queue;
typedef cep::circular_bounded_queue<int, cep::true_tag> cep_mt_queue;

void sigCatcher(int sig) {
    printf("\n ### signal=[%d] ### \n", sig);

    if (sig == SIGPIPE) {
        printf("\n >>> Ingore: Broken pipe (POSIX) <<< \n");
        signal(SIGPIPE, sigCatcher);
    } else if (sig == SIGSEGV) {
        printf("\n >>> SIGSEGV 11 C 无效的内存引用 系统正在退出！ <<< \n");
        // CSystemManager::instance()->getWriteLogFile()->info("\n SIGSEGV 11 C 无效的内存引用 系统正在退出！\n");
        exit(1);
    } else if (sig == SIGILL) {
        printf("\n >>> SIGILL 4 C 非法指令 系统正在退出！ <<< \n");
        exit(1);
    } else if (sig == SIGABRT) {
        printf("\n >>> SIGABRT 6 C 由abort(3)发出的退出指令 系统正在退出！ <<< \n");
        exit(1);
    } else if (sig == SIGHUP) {
        printf("\n >>> Ingore: Hangup (POSIX) SIGHUP 1 A 终端挂起或者控制进程终止！ <<< \n");
        signal(SIGHUP, sigCatcher);
    } else if (sig == SIGINT) {
        exit(0);
    } else if (sig == SIGUSR1) {
        printf("\n ### OK, system will shutdown by your command! ### \n");
        // ProcessingNode::instance().shutdown();
        exit(0);
    }
}

inline unsigned long long int getCPUTick() {
    __asm__ __volatile__("rdtsc");
}

template<typename T>
void __queue_push(T& queue, const bool autoStopFlag) {
    cout << boost::this_thread::get_id() << "::Push4Queue autoStopFlag is:"
            << autoStopFlag << endl;
    srand(time(0));
    // srand(static_cast<unsigned int> (time(0)));
    int nbr;
    unsigned long counter = 0;
    while (true) {
        nbr = rand();
        queue.push(nbr);
        // cout << "push:" << nbr << endl;
        if (autoStopFlag) {
            ++counter;
            if (counter > 99999999) break;
        }
    }
}

template<typename T>
void __queue_pop(T& queue) {
    int nbr;
    while (true) {
        queue.pop(nbr);
        // cout << "pop:" << nbr << endl;
    }
}

template<typename T>
void __queue_try_push(T& queue, const bool autoStopFlag) {
    cout << boost::this_thread::get_id() << "::TryPush4Queue autoStopFlag is:"
            << autoStopFlag << endl;
    srand(time(0));
    // srand(static_cast<unsigned int> (time(0)));
    int nbr;
    unsigned long counter = 0;
    while (true) {
        nbr = rand();
        while (!queue.try_push(nbr))
            usleep(cep_queue::DEFAULT_WAIT_UNTIL_MS * 1000);
        // cout << "push:" << nbr << endl;
        if (autoStopFlag) {
            ++counter;
            if (counter > 99999999) break;
        }
    }
}

template<typename T>
void __queue_try_pop(T& queue) {
    int nbr;
    while (true) {
        while (!queue.try_pop(nbr))
            usleep(cep_queue::DEFAULT_WAIT_UNTIL_MS * 1000);
        // cout << "pop:" << nbr << endl;
    }
}

template<typename T>
void __queue_timed_push(T& queue, const bool autoStopFlag) {
    cout << boost::this_thread::get_id() << "::TimedPush4Queue autoStopFlag is "
            << (autoStopFlag ? "true" : "false") << endl;
    srand(time(0));
    // srand(static_cast<unsigned int> (time(0)));
    int nbr;
    unsigned long counter = 0;
    while (true) {
        nbr = rand();
        while (!queue.timed_push(nbr));
        // cout << "push:" << nbr << endl;
        if (autoStopFlag) {
            ++counter;
            // cout << boost::this_thread::get_id() << "::TimedPush4Queue::counter=" << counter << endl;
            if (counter > 99999999) break;
        }
    }
}

template<typename T>
void __queue_timed_pop(T& queue) {
    int nbr;
    while (true) {
        while (!queue.timed_pop(nbr));
        // cout << "pop:" << nbr << endl;
    }
}

void Pop4Queue(cep_queue& queue) {
    __queue_pop(queue);
}

void Push4Queue(cep_queue& queue, const bool autoStopFlag) {
    __queue_push(queue, autoStopFlag);
}

void TryPop4Queue(cep_queue& queue) {
    __queue_try_pop(queue);
}

void TryPush4Queue(cep_queue& queue, const bool autoStopFlag) {
    __queue_try_push(queue, autoStopFlag);
}

void TimedPop4Queue(cep_queue& queue) {
    __queue_timed_pop(queue);
}

void TimedPush4Queue(cep_queue& queue, const bool autoStopFlag) {
    __queue_timed_push(queue, autoStopFlag);
}

void TimedMtPush4Queue(cep_queue& queue, const bool autoStopFlag) {
    cout << boost::this_thread::get_id() << "::TimedPush4Queue autoStopFlag is "
            << (autoStopFlag ? "true" : "false") << endl;
    srand(time(0));
    // srand(static_cast<unsigned int> (time(0)));
    int nbr;
    unsigned long counter = 0;
    while (true) {
        nbr = rand();
        // while (!queue.timed_push(nbr));
        while (!queue.mt_timed_push(nbr, 500));
        // cout << "push:" << nbr << endl;
        if (autoStopFlag) {
            ++counter;
            // cout << boost::this_thread::get_id() << "::TimedPush4Queue::counter=" << counter << endl;
            if (counter > 999999) break;
        }
    }
}

void Pop4MtQueue(cep_mt_queue& queue) {
    __queue_pop(queue);
}

void Push4MtQueue(cep_mt_queue& queue, const bool autoStopFlag) {
    __queue_push(queue, autoStopFlag);
}

void TryPop4MtQueue(cep_mt_queue& queue) {
    __queue_try_pop(queue);
}

void TryPush4MtQueue(cep_mt_queue& queue, const bool autoStopFlag) {
    __queue_try_push(queue, autoStopFlag);
}

void TimedPop4MtQueue(cep_mt_queue& queue) {
    __queue_timed_pop(queue);
}

void TimedPush4MtQueue(cep_mt_queue& queue, const bool autoStopFlag) {
    __queue_timed_push(queue, autoStopFlag);
}

class object4test {
public:

    object4test() {
        cout << this << "->object4test ctor executed!" << endl;
    }

    ~object4test() {
        cout << this << "->object4test dtor executed!" << endl;
    }

    object4test(const object4test& obj) {
        cout << this << "->object4test copy from[" << &obj << "] ctor execute!" << endl;
    }

    object4test& operator=(const object4test& obj) {
        cout << this << "->object4test assignment operator(" << &obj << ") execute!" << endl;
        if (this == &obj) // effective c++ 16
            return *this;
        return *this;
    }
};

void TestQueue(char *arg) { // 1
    cout << "*** begin of TestQueue[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long queueSize = 1000;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            queueSize = atoi(arg + 2);
    } else
        runMode = 'x';
    MLOG_INFO << "test run mode is [" << runMode << "] and queue size is:" << queueSize << endl;
    if (runMode == 'X') {
        cep_queue queue(queueSize);
        queue.reset_counter();
        cout << "TestQueue[" << arg << "] queue:" << queue << endl;
        boost::thread popThread(boost::bind(TimedPop4Queue, boost::ref(queue)));
        boost::thread pushThread(boost::bind(TimedMtPush4Queue, boost::ref(queue), true));

        while (true) {
            sleep(5); // 1 second = 1000 milliseconds
            // usleep(5000000); // 1 millisecond = 1000 microseconds
            cout << "Queue:" << queue << endl;
            queue.reset_counter();
        }
    } else if (runMode == 'x') {
        cep_queue queue(queueSize);
        queue.reset_counter();
        for (int i = 0; i < queueSize; ++i)
            queue.push(i);
        int nbr;
        while (queue.try_pop(nbr))
            cout << "pop:" << nbr << endl;
        cout << "Pop speed is:" << queue.pop_speed() << endl;
        cout << "Queue:" << queue << endl;
        nbr = -1;

        time_t begin = time(0);
        bool gotIt = queue.timed_pop(nbr, 11000);
        time_t end = time(0);
        if (gotIt) cout << "I'm got [" << nbr << "] and used:" << (end - begin) << endl;
        else cout << "I cant get [" << nbr << "] and used:" << (end - begin) << endl;

        begin = time(0);
        gotIt = queue.try_pop(nbr);
        end = time(0);
        if (gotIt) cout << "I'm try to got [" << nbr << "] and used:" << (end - begin) << endl;
        else cout << "I cant try to get [" << nbr << "] and used:" << (end - begin) << endl;

        queue.pop(nbr);
        cout << "Finally, I got:" << nbr << endl;
    } else if (runMode == 'a' || runMode == 'A'
            || runMode == 'b' || runMode == 'B'
            || runMode == 'c' || runMode == 'C') {
        cep_queue queue(queueSize);
        queue.reset_counter();
        if (runMode == 'a' || runMode == 'A') {
            boost::thread popThread(boost::bind(Pop4Queue, boost::ref(queue)));
            boost::thread pushThread(boost::bind(Push4Queue, boost::ref(queue), runMode == 'a'));
        } else if (runMode == 'b' || runMode == 'B') {
            boost::thread popThread(boost::bind(TryPop4Queue, boost::ref(queue)));
            boost::thread pushThread(boost::bind(TryPush4Queue, boost::ref(queue), runMode == 'b'));
        } else {
            boost::thread popThread(boost::bind(TimedPop4Queue, boost::ref(queue)));
            boost::thread pushThread(boost::bind(TimedPush4Queue, boost::ref(queue), runMode == 'c'));
        }

        while (true) {
            sleep(5); // 1 second = 1000 milliseconds
            // usleep(5000000); // 1 millisecond = 1000 microseconds
            cout << "Queue:" << queue << endl;
            queue.reset_counter();
        }
    } else if (runMode == 'd' || runMode == 'D'
            || runMode == 'e' || runMode == 'E'
            || runMode == 'f' || runMode == 'F') {
        cep_mt_queue queue(queueSize);

#if __VER4TEST_TEMPLATE_SPECIALIZATION__
        // queue.template_test(); // error: no matching function to call
        // queue.template_test<>(); // error: no matching function to call
        // queue.template_test<int>(); // call template<T1>
        // queue.template_test<char>(); // call template<T1>
        queue.template_test((int) 0); // call template<T1>
        queue.template_test((char) 0); // call template<T1>

        queue.template_impl_test();
#endif

        queue.reset_counter();
        if (runMode == 'd' || runMode == 'D') {
            boost::thread popThread(boost::bind(Pop4MtQueue, boost::ref(queue)));
            boost::thread popThread2(boost::bind(Pop4MtQueue, boost::ref(queue)));
            boost::thread pushThread(boost::bind(Push4MtQueue, boost::ref(queue), runMode == 'd'));
            boost::thread pushThread2(boost::bind(Push4MtQueue, boost::ref(queue), runMode == 'd'));
            boost::thread pushThread3(boost::bind(Push4MtQueue, boost::ref(queue), runMode == 'd'));
        } else if (runMode == 'e' || runMode == 'E') {
            boost::thread popThread(boost::bind(TryPop4MtQueue, boost::ref(queue)));
            boost::thread popThread2(boost::bind(TryPop4MtQueue, boost::ref(queue)));
            boost::thread pushThread(boost::bind(TryPush4MtQueue, boost::ref(queue), runMode == 'e'));
            boost::thread pushThread2(boost::bind(TryPush4MtQueue, boost::ref(queue), runMode == 'e'));
            boost::thread pushThread3(boost::bind(TryPush4MtQueue, boost::ref(queue), runMode == 'e'));
        } else {
            boost::thread popThread(boost::bind(TimedPop4MtQueue, boost::ref(queue)));
            boost::thread popThread2(boost::bind(TimedPop4MtQueue, boost::ref(queue)));
            boost::thread pushThread(boost::bind(TimedPush4MtQueue, boost::ref(queue), runMode == 'f'));
            boost::thread pushThread2(boost::bind(TimedPush4MtQueue, boost::ref(queue), runMode == 'f'));
            boost::thread pushThread3(boost::bind(TimedPush4MtQueue, boost::ref(queue), runMode == 'f'));
        }

        while (true) {
            sleep(5); // 1 second = 1000 milliseconds
            // usleep(5000000); // 1 millisecond = 1000 microseconds
            cout << "Queue:" << queue << endl;
            queue.reset_counter();
        }
    } else if (runMode = 'o') {
        typedef cep::circular_bounded_queue<const object4test *> objptr_queue;
        objptr_queue queue(queueSize);
        object4test* obj = new object4test();
        queue.push(obj);
        cout << "queue push:" << obj << endl;
        obj = new object4test();
        queue.push(obj);
        cout << "queue push 2:" << obj << endl;

        const object4test* ptr_obj = NULL;
        queue.pop(ptr_obj);
        cout << "queue pop:" << ptr_obj << endl;

        while (true) {
            ptr_obj = NULL;
            while (!queue.timed_pop(ptr_obj))
                cout << "timed_pop time out!" << endl;
            cout << "queue timed_pop:" << ptr_obj << endl;
        }
    }
    cout << "*** end of TestQueue[" << arg << "] ***" << endl;
}

void TestDynamicStruct(char *arg) { // 2
    cout << "*** begin of TestDynamicStruct[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long queueSize = 1000;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            queueSize = atoi(arg + 2);
    } else
        runMode = 'x';
    MLOG_INFO << "test run mode is [" << runMode << "] and size is " << queueSize << endl;

    cep::buffer_metadata bm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("test", 0, cep::buffer_metadata::IP, 12, 0);
    //    entry->length = 12;
    //    entry->type = cep::buffer_metadata::IP;
    //    entry->flag = 0;
    //    entry->idx = 0;
    //    entry->code = "test";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test2", 100, cep::buffer_metadata::DATETIME, 13, 0);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::DATETIME;
    //    entry->flag = 0;
    //    entry->idx = 100;
    //    entry->code = "test2";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test3", 0, cep::buffer_metadata::STR, 14, 17, 32);
    //    entry->offset = 32;
    //    entry->length = 14;
    //    entry->type = cep::buffer_metadata::STR;
    //    entry->flag = 17;
    //    entry->code = "test3";
    bm.add_cfg_entry(entry);

    cout << "buffer_metadata::to_string=\n" << bm.to_string() << endl;
    cout << "buffer_metadata:" << bm << endl;
    bm.rebuild();
    cout << "after rebuild buffer_metadata:" << bm << endl;
    string code = "test3";
    const cep::field_cfg_entry *entry2 = bm.get_cfg_entry(code);
    cout << "cfg_entry[" << code << "]=" << entry << ':' << endl;
    if (entry) cout << *entry << endl;
    else cout << "NULL" << endl;

    char buffer0[500] = {0};
    cout << "buffer0=";
    cep::buff_util::to_hex_string(buffer0, 500, cout);
    cout << endl;
    // memset(buffer0, 0, sizeof (buffer0));
    // cout << "after memset buffer0=";
    cep::buff_util::set_value<int>(buffer0, 16, 999);
    cout << "after set buffer0=";
    cep::buff_util::to_hex_string(buffer0, 160, cout);
    cout << endl;
    int* data;
    cep::buff_util::reference<int>(buffer0, 64, &data);
    (*data) = 17;
    cout << "after reference buffer0=";
    cep::buff_util::to_hex_string(buffer0, 160, cout);
    cout << endl;
    cep::buff_util::to_hex_string(buffer0, 161, cout);
    cout << endl;
    cout << "to_string buffer0=" << cep::buff_util::to_hex_string(buffer0, 500) << endl;

    cep::dynamic_struct ds(&bm);
    cout << "dynamic_struct:" << ds << endl;
    long* data2;
    ds.reference(2, &data2);
    cout << *data2 << '=' << ds.value("test2", (long) 0) << ":add reference dynamic_struct:" << ds << endl;
    *(data2) = 256;
    cout << *data2 << '=' << ds.value("test2", (long) 0) << ":after reference dynamic_struct:" << ds << endl;

    const unsigned int buff_len = 10;
    const unsigned int buff_ext_len = 2;
    const unsigned int buff_idx = 2;
    char buff[buff_len]; // = {0}; // equals memset
    int nbr = 12;

    // cout << "buff=" << cep::buff_util::to_hex_string(buff, buff_len + buff_ext_len) << endl;
    // buff = 0;
    cout << "buff=" << cep::buff_util::to_hex_string(buff, buff_len + buff_ext_len) << endl;
    memset(buff, 0, sizeof (buff));
    cout << "after memset buff=" << cep::buff_util::to_hex_string(buff, buff_len + buff_ext_len) << endl;
    cep::buff_util::set_value<int>(buff, buff_idx, nbr);
    //cep::buff_util::set_value(buff, buff_idx, cep::slice(0, 0));
    cout << "after set buff=" << cep::buff_util::to_hex_string(buff, buff_len + buff_ext_len) << endl;
    cout << "cep::buff_util::get_int=" << cep::buff_util::get_value<unsigned long> (buff, buff_idx) << endl;
    cout << "cep::buff_util::get_short=" << cep::buff_util::get_value<short>(buff, buff_idx) << endl;
    cout << "cep::buff_util::get_long=" << cep::buff_util::get_value<long>(buff, buff_idx) << endl;
    cout << "cep::buff_util::get_float=" << cep::buff_util::get_value<float>(buff, buff_idx) << endl;
    cout << "cep::buff_util::get_double=" << cep::buff_util::get_value<double>(buff, buff_idx) << endl;

    char* nullPtr = NULL;
    cout << "Null pointer is[" << (nullPtr ? nullPtr : "null") << ']' << endl;
    // cout << "Null pointer is[" << nullPtr << ']' << endl;
    // cout << "Null pointer is[" << (nullPtr ? nullPtr : "null") << ']' << endl;

    cout << "*** end of TestDynamicStruct[" << arg << "] ***" << endl;
}

struct normal_struct {
    unsigned int test;
    char test3[14];
    unsigned long test2;
};

#include<sys/time.h>

long getCurTimeMs(int delay) {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000 - delay * 1000;
}

void TestMetaProgram(int mode) { // 3
    cep::buffer_metadata bm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("test", 0, cep::buffer_metadata::IP, 12, 0);
    //    entry->length = 12;
    //    entry->type = cep::buffer_metadata::IP;
    //    entry->flag = 0;
    //    entry->idx = 0;
    //    entry->code = "test";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test2", 100, cep::buffer_metadata::DATETIME, 13, 0);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::DATETIME;
    //    entry->flag = 0;
    //    entry->idx = 100;
    //    entry->code = "test2";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test3", 10, cep::buffer_metadata::STR, 14, 0);
    //    entry->offset = 32;
    //    entry->length = 14;
    //    entry->type = cep::buffer_metadata::STR;
    //    entry->flag = 0; // 1
    //    entry->idx = 10;
    //    entry->code = "test3";
    bm.add_cfg_entry(entry);
    cout << "buffer_metadata::to_string=\n" << bm.to_string() << endl;
    cout << "buffer_metadata:" << bm << endl;
    bm.rebuild();
    cout << "after rebuild buffer_metadata:" << bm << endl;
    string code = "test3";
    const cep::field_cfg_entry *entry2 = bm.get_cfg_entry(code);
    cout << "cfg_entry[" << code << "]=" << entry << ':' << endl;
    if (entry) cout << *entry << endl;
    else cout << "NULL" << endl;

    cep::dynamic_struct ds(&bm);
    cout << "before setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    ds.set_value("test", (float) 12);
    cout << "after setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    cout << ds.value("test", 1) << endl;
    char buffer[10] = {'h', 'e', 'l', 'l', 'o', '\0'};
    cep::slice def_slice(0, 0);
    ds.set_value(0, cep::slice(buffer, sizeof (buffer)));
#ifdef __CYGWIN32__
    ds.set_value("test2", (unsigned long) 12313123);
#else
    ds.set_value("test2", (unsigned long) 121321312313123);
#endif
    cout << ds.value("test", def_slice).to_string() << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    int * a = NULL;
    ds.reference("test", &a);
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
#ifdef __CYGWIN32__
    *a = 0xFFFF;
#else
    *a = 0xFFFFFFFFFFF;
#endif
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    cout << ds.value("test", 1) << endl;
    cout << "*********************************************" << endl;
    ds.set_value("test", (char) 12);
    printf("%d\n", ds.value("test", (char) 0));
    ds.set_value("test", (unsigned char) 12);
    printf("%d\n", ds.value("test", (unsigned char) 0));
    ds.set_value("test", (unsigned short) 12);
    cout << ds.value("test", (unsigned short) 0) << endl;
    ds.set_value("test", (short) 12);
    cout << ds.value("test", (short) 0) << endl;
    ds.set_value("test", (int) 12);
    cout << ds.value("test", (int) 0) << endl;
    ds.set_value("test", (unsigned int) 12);
    cout << ds.value("test", (unsigned int) 0) << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    ds.set_value("test", (float) 12.0);
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    float f = ds.value("test", (float) 12);
    cout << f << endl;
    float *ft = NULL;
    ds.reference("test", &ft);
    cout << *ft << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    ds.set_value("test", (double) 12.0f);
    cout << ds.value("test", (double) 0.0f) << endl;
    ds.set_value("test", (long) 12);
    cout << ds.value("test", (long) 0) << endl;
    ds.set_value("test", (unsigned long) 12);
    cout << ds.value("test", (unsigned long) 0) << endl;
    vector<cep::dynamic_struct*> ds_vec;
    vector<normal_struct*> ns_vec;
#ifdef __CYGWIN32__
    const int ds_nums = 5000000; // 50000 2000000 4000000
#else
    const int ds_nums = 5000000;
#endif
    for (int i = 0; i < ds_nums; i++) {
        cep::dynamic_struct *ds_ = new cep::dynamic_struct(&bm);
        ds_->set_value(0, (int) i);
        ds_->set_value(2, i * i);
        ds_vec.push_back(ds_);

        normal_struct *ns_ = new normal_struct;
        ns_->test = 0;
        ns_->test2 = i * i;
        ns_vec.push_back(ns_);
    }
    int j = 0;
    long start = getCurTimeMs(0);
    string t1("test"); // IP (unsigned int)
    string t2("test2"); // DATETIME (unsigned long)
    printf("*********************TEST START******************************\n");
    switch (mode) {
        case 0:
        {
            for (int i = 0; i < ds_nums; i++) {
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    dss->set_value(t1, (unsigned int) i);
                    dss->set_value(t2, (unsigned long) (i * i));
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    j += dss->value(t1, (unsigned int) 1);
                    j += dss->value(t2, (unsigned long) 1);
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 1:
        {
            for (int i = 0; i < ds_nums; i++) {
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    dss->set_value(0, (unsigned int) i);
                    dss->set_value(2, (unsigned long) (i * i));
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    j += dss->value(0, (unsigned int) 1);
                    j += dss->value(2, (unsigned long) 1);
                }

            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 2:
        {
            for (int i = 0; i < ds_nums; i++) {
                unsigned int *test_ptr1 = NULL;
                unsigned long *test_ptr2 = NULL;
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    dss->reference(t1, &test_ptr1);
                    dss->reference(t2, &test_ptr2);
                    (*test_ptr1) = i;
                    (*test_ptr2) = i*i;
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 3:
        {
            for (int i = 0; i < ds_nums; i++) {
                unsigned int *test_ptr1 = NULL;
                unsigned long *test_ptr2 = NULL;
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    dss->reference(0, &test_ptr1);
                    dss->reference(2, &test_ptr2);
                    (*test_ptr1) = i;
                    (*test_ptr2) = i*i;
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 4: // 若一般结构体字段存取消耗为1，则动态结构提供下标存取消耗为1.2
        {
            for (int i = 0; i < ds_nums; i++) {
                normal_struct *nss = ns_vec[i];
                if (nss) {
                    nss->test = i;
                    nss->test2 = i * i;
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                normal_struct *nss = ns_vec[i];
                if (nss) {
                    j += nss->test;
                    j += nss->test2;
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 5: // 若一般结构体字段存取消耗为1，则使用offset直接截位存取消耗为1.05
        {
            for (int i = 0; i < ds_nums; i++) {
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    dss->set_value(0, cep::buffer_metadata::SIZEOF_IP, (unsigned int) i);
                    dss->set_value(18, cep::buffer_metadata::SIZEOF_DATETIME, (unsigned long) (i * i));
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                cep::dynamic_struct *dss = ds_vec[i];
                if (dss) {
                    j += dss->value(0, cep::buffer_metadata::SIZEOF_IP, (unsigned int) 1);
                    j += dss->value(18, cep::buffer_metadata::SIZEOF_DATETIME, (unsigned long) 1);
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        default:
            break;
    }
}

void TestEvent(int mode) { // 4
    cep::buffer_metadata bm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("test", 0, cep::buffer_metadata::IP);
    //    entry->length = 12;
    //    entry->type = cep::buffer_metadata::IP;
    //    entry->flag = 0;
    //    entry->idx = 0;
    //    entry->code = "test";
    bm.add_cfg_entry(entry);
    // code, idx, type, length, flag
    entry = new cep::field_cfg_entry("test2", 100, cep::buffer_metadata::DATETIME);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::DATETIME;
    //    entry->flag = 0;
    //    entry->idx = 100;
    //    entry->code = "test2";
    bm.add_cfg_entry(entry);
    // code, idx, type, length, flag
    entry = new cep::field_cfg_entry("test3", 0, cep::buffer_metadata::STR, 14, 1, 32);
    //    entry->offset = 32;
    //    entry->length = 14;
    //    entry->type = cep::buffer_metadata::STR;
    //    entry->flag = 1;
    //    entry->code = "test3";
    bm.add_cfg_entry(entry);
    cout << "buffer_metadata::to_string=\n" << bm.to_string() << endl;
    cout << "buffer_metadata:" << bm << endl;
    bm.rebuild();
    cout << "after rebuild buffer_metadata:" << bm << endl;
    string code = "test3";
    const cep::field_cfg_entry *entry2 = bm.get_cfg_entry(code);
    cout << "cfg_entry[" << code << "]=" << entry << ':' << endl;
    if (entry) cout << *entry << endl;
    else cout << "NULL" << endl;

    cep::event ds(&bm);
    cout << "before setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    ds.set_value("test", (float) 12);
    cout << "after setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    cout << ds.value("test", 1) << endl;
    char buffer[10] = {'h', 'e', 'l', 'l', 'o', '\0'};
    cep::slice def_slice(0, 0);
    ds.set_value(0, cep::slice(buffer, sizeof (buffer)));
#ifdef __CYGWIN32__
    ds.set_value("test2", (unsigned long) 12313123);
#else
    ds.set_value("test2", (unsigned long) 121321312313123);
#endif
    cout << ds.value("test", def_slice).to_string() << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    int * a = NULL;
    ds.reference("test", &a);
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
#ifdef __CYGWIN32__
    *a = 0xFFFF;
#else
    *a = 0xFFFFFFFFFFF;
#endif
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    cout << ds.value("test", 1) << endl;
    cout << "*********************************************" << endl;
    ds.set_value("test", (char) 12);
    printf("%d\n", ds.value("test", (char) 0));
    ds.set_value("test", (unsigned char) 12);
    printf("%d\n", ds.value("test", (unsigned char) 0));
    ds.set_value("test", (unsigned short) 12);
    cout << ds.value("test", (unsigned short) 0) << endl;
    ds.set_value("test", (short) 12);
    cout << ds.value("test", (short) 0) << endl;
    ds.set_value("test", (int) 12);
    cout << ds.value("test", (int) 0) << endl;
    ds.set_value("test", (unsigned int) 12);
    cout << ds.value("test", (unsigned int) 0) << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    ds.set_value("test", (float) 12.0);
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    float f = ds.value("test", (float) 12);
    cout << f << endl;
    float *ft = NULL;
    ds.reference("test", &ft);
    cout << *ft << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    ds.set_value("test", (double) 12.0f);
    cout << ds.value("test", (double) 0.0f) << endl;
    ds.set_value("test", (long) 12);
    cout << ds.value("test", (long) 0) << endl;
    ds.set_value("test", (unsigned long) 12);
    cout << ds.value("test", (unsigned long) 0) << endl;
    vector<cep::event*> ds_vec;
    int ds_nums = 3000000;
    for (int i = 0; i < ds_nums; i++) {
        cep::event *ds_ = new cep::event(&bm);
        ds_->set_value(0, (int) i);
        ds_->set_value(2, i * i);
        ds_vec.push_back(ds_);
    }
    int j = 0;
    long start = getCurTimeMs(0);
    string t1("test");
    string t2("test2");
    printf("*********************TEST START******************************\n");
    switch (mode) {
        case 0://set value get_value name
        {
            for (int i = 0; i < ds_nums; i++) {
                cep::event *dss = ds_vec[i];
                if (dss) {
                    dss->set_value(t1, i);
                    dss->set_value(t2, i * i);
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                cep::event *dss = ds_vec[i];
                if (dss) {
                    j += dss->value(t1, 1);
                    j += dss->value(t2, 1);
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 1://set vale get_value index
        {
            for (int i = 0; i < ds_nums; i++) {
                cep::event *dss = ds_vec[i];
                if (dss) {
                    dss->set_value(0, i);
                    dss->set_value(2, i * i);
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                cep::event *dss = ds_vec[i];
                if (dss) {
                    j += dss->value(0, 1);
                    j += dss->value(2, 1);
                }

            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 2://reference name
        {
            for (int i = 0; i < ds_nums; i++) {
                int *test_ptr1 = NULL;
                int *test_ptr2 = NULL;
                cep::event *dss = ds_vec[i];
                if (dss) {
                    dss->reference(t1, &test_ptr1);
                    dss->reference(t2, &test_ptr2);
                    (*test_ptr1) = i;
                    (*test_ptr2) = i*i;
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 3:// reference index
        {
            for (int i = 0; i < ds_nums; i++) {
                int *test_ptr1 = NULL;
                int *test_ptr2 = NULL;
                cep::event *dss = ds_vec[i];
                if (dss) {
                    dss->reference(0, &test_ptr1);
                    dss->reference(2, &test_ptr2);
                    (*test_ptr1) = i;
                    (*test_ptr2) = i*i;
                }
            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        case 4://offset len setvale get vale
        {
            for (int i = 0; i < ds_nums; i++) {
                cep::event *dss = ds_vec[i];
                if (dss) {
                    dss->set_value(0, 4, i);
                    dss->set_value(4, 8, i * i);
                }
            }
            for (int i = 0; i < ds_nums; i++) {
                cep::event *dss = ds_vec[i];
                if (dss) {
                    j += dss->value(0, 4, 1);
                    j += dss->value(4, 8, 1);
                }

            }
            long end = getCurTimeMs(0);
            *a = j;
            fprintf(stdout, "timeuse[%ld]\n", end - start);
        }
            break;
        default:
            break;
    }
}

void TestOffsetLen(int mode) { // 5
    char *body = new char[16];
    memset(body, 0, 16);
    cep::event ds(body, 16);
    cout << "before setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;

    cout << "after setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;

    char buffer[10] = {'h', 'e', 'l', 'l', 'o', '\0'};
    cep::slice def_slice(0, 0);
    ds.set_value(0, 10, cep::slice(buffer, 10));
    //    ds.set_value(0, cep::slice(buffer, sizeof (buffer)));
    //    ds.set_value("test2", (unsigned long) 121321312313123);
    //    cout << ds.value("test", def_slice).to_string() << endl;
    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
}

void DepthFirstSearch4XML(const cep::xml_element *ele, int depth = 0) {
    if (ele != NULL) {
        slice slc;
        for (ele = cep::try2get_first_child(ele); ele != NULL;
                ele = cep::try2get_next_sibling(ele)) {
            if (cep::try2get_name(ele, slc)) {
                cout << depth << ">name is:" << slc.data() << '|'
                        << cep::buff_util::to_hex_string(slc.data(), slc.size()) << '!' << endl;
            } else cout << depth << ">name is not found!" << endl;
            if (cep::try2get_value(ele, slc)) {
                cout << depth << ">value is:" << slc.data() << '|'
                        << cep::buff_util::to_hex_string(slc.data(), slc.size()) << '!' << endl;
            } else cout << depth << ">value is not found!" << endl;

            if (cep::try2get_attribute(ele, "type", slc)) {
                cout << depth << ">attribute \"type\" is:" << slc.data() << '|'
                        << cep::buff_util::to_hex_string(slc.data(), slc.size()) << '!' << endl;
            } else cout << depth << ">attribute \"type\" is not found!" << endl;
            if (cep::try2get_attribute(ele, NULL, slc)) {
                cout << depth << ">attribute NULL is:" << slc.data() << '|'
                        << cep::buff_util::to_hex_string(slc.data(), slc.size()) << '!' << endl;
            } else cout << depth << ">attribute NULL is not found!" << endl;

            DepthFirstSearch4XML(ele, depth + 1);
        }
    } else
        cout << "cep::xml_element point to NULL!" << endl;
}

void TestXML(char *arg) { // 6
    cout << "*** begin of TestXML[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long size = 1000;
    const char * arg2 = "cep_config.xml";
    if (runMode != '\0') {
        if (*(arg + 2) != '\0') {
            size = atoi(arg + 2);
            arg2 = arg + 2;
        }
    } else
        runMode = 'x';
    MLOG_INFO << "test run mode is [" << runMode << "] and size is " << size << endl;

    // TiXmlDocument * myDocument = new TiXmlDocument();
    // myDocument->SetCondenseWhiteSpace(false); // 保留xml原始字符
    // myDocument->Parse("");
    if (runMode == 's') {
        TiXmlDocument myDocument;
        myDocument.SetCondenseWhiteSpace(false); // 保留xml原始字符
        myDocument.Parse("<cep><tag/></cep>");
        TiXmlElement * node = myDocument.RootElement();
        if (!node) {
            cout << "Load char* [" << myDocument.Value() << "] failure for not found root element!" << endl;
        } else {
            cout << "Load char* [" << myDocument.Value() << "] success!" << endl;
        }
    } else if (runMode == 'f') {
        TiXmlDocument myDocument("cep_config.xml");
        myDocument.SetCondenseWhiteSpace(false); // 保留xml原始字符
        myDocument.LoadFile();
        TiXmlElement * node = myDocument.RootElement();
        if (!node) {
            cout << "Load file [" << myDocument.Value() << "] failure for not found root element!" << endl;
        } else {
            cout << "Load file [" << myDocument.Value() << "] success!" << endl;
        }
    } else {
        cep::xml_document *doc = cep::load_xmlfile(arg2);
        if (!doc) {
            cout << "Load file [" << arg2 << "] failure open xml file!" << endl;
        } else {
            cout << "Load file [" << arg2 << "] success open xml file!" << endl;
            auto_ptr<cep::xml_document> gc4xml(doc);
            const cep::xml_element *root = cep::root_element(*doc);
            if (root) {
                cout << "root is:" << cep::name(*root).data() << '!' << endl;
                slice slc;
                if (cep::try2get_name(root, slc))
                    cout << "root is:" << slc.data() << '>'
                    << cep::buff_util::to_hex_string(slc.data(), slc.size())
                    << '!' << endl;
                else cout << "root is not found!" << endl;
                if (runMode == 'F') {
                    DepthFirstSearch4XML(root);
                } else {
                    const cep::xml_element *ele = NULL;
                    cep::slice value, default_value("xxx");
                    for (ele = cep::first_child(*root); ele != NULL;
                            ele = cep::next_sibling(*ele)) {
                        cout << "first children is:" << cep::name(*ele).data() << '!' << endl;
                        if ("JOB" == cep::name(*ele).to_string()) {
                            cout << "I want to try to get int attribute ID" << endl;
                            value = cep::attribute(*ele, "ID", default_value);
                            if (value != default_value) {
                                int id = cep::cstr2nbr(value.data(), (int) - 1);
                                cout << "JOB attribute ID[" << value.data() << "] convert to int is:" << id << endl;
                            } else cout << "JOB attribute ID is not found!" << endl;
                        } else if (strcmp("EVENTSET", cep::name(*ele).data()) == 0) {
                            const cep::xml_element *event = NULL;
                            for (event = cep::first_child(*ele); event != NULL;
                                    event = cep::next_sibling(*event)) {
                                value = cep::value(*event, default_value);
                                if (value > default_value)
                                    cout << "event value > default:" << value.data() << '!' << endl;
                                else
                                    cout << "event value <= default:" << value.data() << '!' << endl;
                            }
                        }
                    }
                    ele = cep::find_element(*root, "ATTR_NAME");
                    if (ele)
                        cout << "found first element[" << cep::name(*ele).data()
                        << "] is:" << cep::value(*ele, default_value).data() << '!' << endl;
                    else
                        cout << "not found element ATTR_NAME!" << endl;
                    ele = cep::find_element(*root, "haha");
                    if (ele)
                        cout << "found first element[" << cep::name(*ele).data()
                        << "] is:" << cep::value(*ele, default_value).data() << '!' << endl;
                    else
                        cout << "not found element haha!" << endl;
                }
            } else
                cout << "Load file [" << arg2 << "] is no root element!" << endl;
        }
    }
    cout << "*** end of TestXML[" << arg << "] ***" << endl;
}

void TestCstr2nbr(char *arg) { // 7
    cout << "*** begin of TestCstr2nbr[" << arg << "] ***" << endl;
    cout << "sizeof (void *)=" << sizeof (void *)
            << ", sizeof (int *)=" << sizeof (int *)
            << ", sizeof (long)=" << sizeof (long)
            << ", sizeof (unsigned long)=" << sizeof (unsigned long)
            << ", sizeof (long long)=" << sizeof (long long) << endl;
    cout << "CHAR_MIN=" << CHAR_MIN << endl;
    cout << "CHAR_MAX=" << CHAR_MAX << endl;
    cout << "UCHAR_MAX=" << UCHAR_MAX << endl;
    cout << "SHRT_MIN=" << SHRT_MIN << endl;
    cout << "SHRT_MAX=" << SHRT_MAX << endl;
    cout << "USHRT_MAX=" << USHRT_MAX << endl;
    cout << "INT_MIN=" << INT_MIN << endl;
    cout << "INT_MAX=" << INT_MAX << endl;
    cout << "UINT_MAX=" << UINT_MAX << endl;
    cout << "LONG_MIN=" << LONG_MIN << endl;
    cout << "LONG_MAX=" << LONG_MAX << endl;
    cout << "ULONG_MAX=" << ULONG_MAX << endl;

    cout << "numeric_limits<float>::min()=" << numeric_limits<float>::min() << endl;
    cout << "numeric_limits<float>::max()=" << numeric_limits<float>::max() << endl;
    cout << "numeric_limits<double>::min()=" << numeric_limits<double>::min() << endl;
    cout << "numeric_limits<double>::max()=" << numeric_limits<double>::max() << endl;
    cout << "char=1|a, uchar=2|b, short=3|c, ushort=4|d, int=5|e, uint=6|f,"
            " long=7|g, ulong=8|h, float=9|i, double=0|j" << endl;
    char runMode = *(arg + 1);
    unsigned long size = 1000;
    const char * arg2 = "123";
    if (runMode != '\0') {
        if (*(arg + 2) != '\0') {
            size = atoi(arg + 2);
            arg2 = arg + 2;
        }
    } else
        runMode = 'x';
    MLOG_INFO << "test run mode is [" << runMode << "] and size is " << size << endl;

    if (runMode == '1') {
        char nbr = cep::cstr2nbr(arg2, (char) - 1);
        cout << "arg2[" << arg2 << "] convert to char is:" << (int) nbr << endl;
    } else if (runMode == '2') {
        unsigned char nbr = cep::cstr2nbr(arg2, (unsigned char) 0);
        cout << "arg2[" << arg2 << "] convert to unsigned char is:" << (int) nbr << endl;
    } else if (runMode == '3') {
        short nbr = cep::cstr2nbr(arg2, (short) - 1);
        cout << "arg2[" << arg2 << "] convert to short is:" << nbr << endl;
    } else if (runMode == '4') {
        unsigned short nbr = cep::cstr2nbr(arg2, (unsigned short) 0);
        cout << "arg2[" << arg2 << "] convert to unsigned short is:" << nbr << endl;
    } else if (runMode == '5') {
        int nbr = cep::cstr2nbr(arg2, (int) - 1);
        cout << "arg2[" << arg2 << "] convert to int is:" << nbr << endl;
    } else if (runMode == '6') {
        unsigned int nbr = cep::cstr2nbr(arg2, (unsigned int) 0);
        cout << "arg2[" << arg2 << "] convert to unsigned int is:" << nbr << endl;
    } else if (runMode == '7') {
        long nbr = cep::cstr2nbr(arg2, (long) - 1);
        cout << "arg2[" << arg2 << "] convert to long is:" << nbr << endl;
    } else if (runMode == '8') {
        unsigned long nbr = cep::cstr2nbr(arg2, (unsigned long) 0);
        cout << "arg2[" << arg2 << "] convert to unsigned long is:" << nbr << endl;
    } else if (runMode == '9') {
        float nbr = cep::cstr2nbr(arg2, (float) - 1);
        cout << "arg2[" << arg2 << "] convert to float is:" << nbr << endl;
    } else if (runMode == '0') {
        double nbr = cep::cstr2nbr(arg2, (double) - 1);
        cout << "arg2[" << arg2 << "] convert to double is:" << nbr << endl;
    } else if (runMode == 'a') {
        char nbr;
        cout << "arg2[" << arg2 << "] convert to char is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << (int) nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'b') {
        unsigned char nbr;
        cout << "arg2[" << arg2 << "] convert to unsigned char is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << (int) nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'c') {
        short nbr;
        cout << "arg2[" << arg2 << "] convert to short is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'd') {
        unsigned short nbr;
        cout << "arg2[" << arg2 << "] convert to unsigned short is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'e') {
        int nbr;
        cout << "arg2[" << arg2 << "] convert to int is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'f') {
        unsigned int nbr;
        cout << "arg2[" << arg2 << "] convert to unsigned int is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'g') {
        long nbr;
        cout << "arg2[" << arg2 << "] convert to long is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'h') {
        unsigned long nbr;
        cout << "arg2[" << arg2 << "] convert to unsigned long is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'i') {
        float nbr;
        cout << "arg2[" << arg2 << "] convert to float is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else if (runMode == 'j') {
        double nbr;
        cout << "arg2[" << arg2 << "] convert to double is:";
        if (cep::cstrtonbr(arg2, nbr)) cout << nbr << endl;
        else cout << "failure!" << endl;
    } else cout << "Unsupported run mode[" << runMode << "] for:" << arg2 << endl;
    cout << "*** end of TestCstr2nbr[" << arg << "] ***" << endl;
}

void Testnbr2str(int mode) { // a
    cout << cep::nbr2str((double) (1 / 3.0)) << endl;
    cout << cep::nbr2str((float) (1 / 3.0)) << endl;
    cout << cep::nbr2str((int) 0xFFFFFFFF) << endl;
#ifdef __CYGWIN32__
    cout << cep::nbr2str((unsigned int) 0xFFFF) << endl;
    cout << cep::nbr2str((short) 0xFF) << endl;
#else
    cout << cep::nbr2str((unsigned int) 0xFFFFFFFF) << endl;
    cout << cep::nbr2str((short) 0xFFFF) << endl;
#endif
    cout << cep::nbr2str((unsigned short) 0xFFFF) << endl;
    cout << cep::nbr2str((unsigned char) 0xFF) << endl;
#ifdef __CYGWIN32__
    cout << cep::nbr2str((long) 0xFFFFFFFF) << endl;
    cout << cep::nbr2str((unsigned long) 0xFFFFFFFF) << endl;
#else
    cout << cep::nbr2str((long) 0xFFFFFFFFFFFFFFFF) << endl;
    cout << cep::nbr2str((unsigned long) 0xFFFFFFFFFFFFFFFF) << endl;
#endif
    cout << "TIME:" << cep::milsecond2str(cep::cstr2milsecond("2011-12-32 23:58:49.1001")) << endl;
    cout << "IP:" << cep::ip2str(cep::str2ip("255.255.255.0", 0)) << endl;
}

void Testevent2text(int mode) { // b
    cep::buffer_metadata *bmm = new cep::buffer_metadata();
    cep::buffer_metadata &bm = *bmm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("test0", 0, cep::buffer_metadata::STR, 12);
    //    entry->length = 12;
    //    entry->type = cep::buffer_metadata::STR;
    //    entry->flag = 0;
    //    entry->idx = 0;
    //    entry->code = "test0";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test1", 1, cep::buffer_metadata::CHAR, 12, 0);
    //    entry->length = 12;
    //    entry->type = cep::buffer_metadata::CHAR;
    //    entry->flag = 0;
    //    entry->idx = 1;
    //    entry->code = "test1";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test2", 2, cep::buffer_metadata::UCHAR, 13);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::UCHAR;
    //    entry->flag = 0;
    //    entry->idx = 2;
    //    entry->code = "test2";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test3", 3, cep::buffer_metadata::SHORT, 13);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::SHORT;
    //    entry->flag = 0;
    //    entry->idx = 3;
    //    entry->code = "test3";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test4", 4, cep::buffer_metadata::USHORT, 14);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::USHORT;
    //    entry->flag = 0;
    //    entry->idx = 4;
    //    entry->code = "test4";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test5", 5, cep::buffer_metadata::INT, 15);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::INT;
    //    entry->flag = 0;
    //    entry->idx = 5;
    //    entry->code = "test5";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test6", 6, cep::buffer_metadata::UINT, 16);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::UINT;
    //    entry->flag = 0;
    //    entry->idx = 6;
    //    entry->code = "test6";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test7", 7, cep::buffer_metadata::FLOAT, 17);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::FLOAT;
    //    entry->flag = 0;
    //    entry->idx = 7;
    //    entry->code = "test7";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test8", 8, cep::buffer_metadata::LONG, 18);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::LONG;
    //    entry->flag = 0;
    //    entry->idx = 8;
    //    entry->code = "test8";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test9", 9, cep::buffer_metadata::ULONG, 19);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::ULONG;
    //    entry->flag = 0;
    //    entry->idx = 9;
    //    entry->code = "test9";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test10", 10, cep::buffer_metadata::DOUBLE, 10);
    //    entry->length = 13;
    //    entry->type = cep::buffer_metadata::DOUBLE;
    //    entry->flag = 0;
    //    entry->idx = 10;
    //    entry->code = "test10";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test11", 11, cep::buffer_metadata::DATETIME, 11);
    //    entry->offset = 32;
    //    entry->length = 14;
    //    entry->type = cep::buffer_metadata::DATETIME;
    //    entry->flag = 0;
    //    entry->idx = 11;
    //    entry->code = "test11";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("test12", 12, cep::buffer_metadata::IP, 12);
    //    entry->offset = 32;
    //    entry->length = 14;
    //    entry->type = cep::buffer_metadata::IP;
    //    entry->flag = 0;
    //    entry->idx = 12;
    //    entry->code = "test12";
    bm.add_cfg_entry(entry);
    cout << "buffer_metadata::to_string=\n" << bm.to_string() << endl;
    cout << "buffer_metadata:" << bm << endl;
    bm.rebuild();
    cout << "after rebuild buffer_metadata:" << bm << endl;
    //    string code = "test3";
    //    const cep::field_cfg_entry *entry2 = bm.get_cfg_entry(code);
    //    cout << "cfg_entry[" << code << "]=" << entry << ':' << endl;
    //    if (entry) cout << *entry << endl;
    //    else cout << "NULL" << endl;

    cep::event ds(&bm);
    cout << "before setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    ds.set_value("test", (float) 12);
    //    cout << "after setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    cout << ds.value("test", 1) << endl;

    //    ds.set_value("test2", (unsigned long) 121321312313123);
    //    cout << ds.value("test", def_slice).to_string() << endl;
    //    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    int * a = NULL;
    //    ds.reference("test", &a);
    //    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    *a = 0xFFFFFFFFFFF;
    //    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    cout << ds.value("test", 1) << endl;
    cout << "*********************************************" << endl;
    char buffer[10] = {'h', 'e', 'l', 'l', 'o', '\0'};
    cep::slice def_slice(0, 0);
    ds.set_value("test0", cep::slice(buffer, sizeof (buffer)));
    ds.set_value("test1", (char) 'a');
    ds.set_value("test2", (unsigned char) 12);
    ds.set_value("test3", (short) - 1);
    ds.set_value("test4", (unsigned short) - 1);
    ds.set_value("test5", (int) - 1);
    ds.set_value("test6", (unsigned int) - 1);
    ds.set_value("test7", (float) (1 / 3.0));
    ds.set_value("test8", (long) - 1);
    ds.set_value("test9", (unsigned long) - 1);
    ds.set_value("test10", (double) (1 / 3.0));
    ds.set_value("test11", (unsigned long) (cep::cstr2milsecond("2011-12-31 23:59:59.011")));
    ds.set_value("test12", (unsigned int) cep::str2ip("192.168.1.1", 0));
    cout << "after setvalue" << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    ds.set_value("test", (char) 12);
    //    printf("%d\n", ds.value("test", (char) 0));
    //    ds.set_value("test", (unsigned char) 12);
    //    printf("%d\n", ds.value("test", (unsigned char) 0));
    //    ds.set_value("test", (unsigned short) 12);
    //    cout << ds.value("test", (unsigned short) 0) << endl;
    //    ds.set_value("test", (short) 12);
    //    cout << ds.value("test", (short) 0) << endl;
    //    ds.set_value("test", (int) 12);
    //    cout << ds.value("test", (int) 0) << endl;
    //    ds.set_value("test", (unsigned int) 12);
    //    cout << ds.value("test", (unsigned int) 0) << endl;
    //    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    //    ds.set_value("test", (float) 12.0);
    //    cout << cep::buff_util::to_hex_string(ds.data(), ds.len()) << endl;
    config_metadata::instance().insert_metadata(0x1404, "0x1404", &bm);
    event *eventmsg = new event(&bm, 0x1404);
    ds.set_type(0x1404);
    event2text eventsaver(0, ';');
    //cep::config_metadata::instance().
    if (mode) {
        eventsaver.open("hello", "w");
        eventsaver.write(ds);
    } else {
        eventsaver.open("hello", "r");
        eventsaver.read(*eventmsg);
        cout << endl;
        cout << eventmsg->value("test0", slice(0, 0)).to_string() << endl;
        cout << eventmsg->value("test1", (char) 0) << endl;
        cout << (int) eventmsg->value("test2", (unsigned char) 0) << endl;
        cout << eventmsg->value("test3", (short) 0) << endl;
        cout << eventmsg->value("test4", (unsigned short) 0) << endl;
        cout << eventmsg->value("test5", (int) 0) << endl;
        cout << eventmsg->value("test6", (unsigned int) 0) << endl;
        cout << eventmsg->value("test7", (float) 0) << endl;
        cout << eventmsg->value("test8", (long) 0) << endl;
        cout << eventmsg->value("test9", (unsigned long) 0) << endl;
        cout << eventmsg->value("test10", (double) 0) << endl;
        cout << cep::milsecond2str(eventmsg->value("test11", (unsigned long) 0)) << endl;
        cout << cep::ip2str(eventmsg->value("test12", (unsigned int) 0)) << endl;
    }
    eventsaver.close();
}

class PN : public cep::singleton<PN> {
    friend class cep::singleton<PN>;

private: // singleton pattern // protected

    PN() { // must have a default constructor
        cout << this << "-> PN default ctor execute!" << endl;
    }

    PN(PN& pn) { // Forbidden copy behave
        cout << this << "-> PN copy from[" << &pn << "] ctor execute!" << endl;
    }

    PN & operator=(const PN& pn) { // Forbidden copy behave
        if (this == &pn) // effective c++ 16
            return *this;
        cout << this << "->PN assignment operator(" << &pn << ") execute!" << endl;
        return *this;
    }
public:

    ~PN() {
        cout << this << "->PN dtor execute!" << endl;
    }
public:

    void IAmHere() {
        cout << this << "->I am here!" << endl;
    }
};

void TestSingleton(char *arg) { // s // '~'
    cout << "*** begin of TestSingleton[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long typeId = 0;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            typeId = atoi(arg + 2);
    } else
        runMode = 'x';
    MLOG_INFO << "test run mode is [" << runMode << "] and type id is " << typeId << endl;

    PN::instance().IAmHere();
    cout << "call member function again!" << endl;
    PN *pn = &(PN::instance());
    pn->IAmHere();
    if (runMode == 'd') {
        cout << "try to delete singleton!" << endl;
        delete pn; // runtime error: free(): invalid pointer
    }

    const char *xmlfile = "event.xml";
    cep::xml_document *doc = cep::load_xmlfile(xmlfile);
    if (!doc) {
        cout << "Load file [" << xmlfile << "] failure open xml file!" << endl;
        const cep::xml_element xml(xmlfile);
        cep::config_metadata::instance().reload_metadata(xml);
        // test for reload
        cep::config_metadata::instance().reload_metadata(xml);
    } else {
        cout << "Load file [" << xmlfile << "] success open xml file!" << endl;
        auto_ptr<cep::xml_document> gc4xml(doc);
        const cep::xml_element *root = cep::root_element(*doc);
        cep::config_metadata::instance().reload_metadata(*root);
        // test for reload
        cep::config_metadata::instance().reload_metadata(*root);
    }
    cout << "*** *** ***\nconfiguration for meta data is:" << cep::config_metadata::instance() << endl;
    cep::buffer_metadata* metadata = cep::config_metadata::instance().metadata(typeId);
    if (metadata)
        cout << "*** *** ***\nfound type id[" << typeId << "] meta data:" << *metadata << endl;
    else
        cout << "*** *** ***\nnot found meta data by type id:" << typeId << endl;
    cout << "*** *** ***\nreview the configuration for meta data is:" << cep::config_metadata::instance() << endl;

    cout << "*** test cep::pn ***" << endl;
    pn::instance().startup();
    pn::instance().shutdown();
    cout << "*** end of TestSingleton[" << arg << "] ***" << endl;
}

void TestLoadEvent(char *arg) { // J
    cout << "*** begin of TestLoadEvent[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    //    unsigned long typeId = 0;
    //两种模式： 本地读取、网络读取
    if (runMode == 'a') {
        MLOG_INFO << "test run mode is [" << runMode << "] and type id is " << endl;
        //网络读取
    } else {
        runMode = 'b';
        MLOG_INFO << "test run mode is [" << runMode << "] and type id is " << endl;
        const char *xmlfile = "cep_config.xml";
        cep::xml_document *doc = cep::load_xmlfile(xmlfile);
        if (!doc) {
            cout << "Load file [" << xmlfile << "] failure open xml file!" << endl;
            //todo xml from net
        } else {
            const cep::xml_element *xml_temp = cep::root_element(*doc);
            xml_temp = cep::find_element(*xml_temp, "eventset");
            if (xml_temp != NULL)
                cep::config_metadata::instance().reload_metadata(*xml_temp);
            else
                return;
        }
        cout << "*** *** ***\nconfiguration for meta data is:\n"
                << cep::config_metadata::instance() << endl;
        // cout << cep::config_metadata::instance() << endl;
        cout << "*** end of TestLoadEvent[" << arg << "] ***" << endl;
    }
}

void TestConstDataset(char *arg) { // t
    cout << "*** begin of TestConstDataset[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long nbr = 0;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            nbr = atoi(arg + 2);
    } else
        runMode = '0';
    MLOG_INFO << "test run mode is [" << runMode << "] and number is " << nbr << endl;

    if (runMode >= '0' && runMode <= '9') {
        cout << "sizeof (cep::buffer_metadata::enum_t)=" << sizeof (cep::buffer_metadata::enum_t) << endl;
        cout << "sizeof (cep::slice)=" << sizeof (cep::slice) << endl;
        cout << "sizeof (cep::type_slice)=" << sizeof (cep::type_slice) << endl;
        // cep::type_slice x, y;
        char data4x[128] = {0};
        char data4y[64] = {0};
        struct cep::type_slice x = {cep::buffer_metadata::INT, cep::slice(data4x, sizeof (data4x))};
        struct cep::type_slice y = {y.type = 0, y.data = cep::slice(data4y, sizeof (data4y))};
        // const char* data4c = "123";
        // x.data = cep::slice(data4c);
        //        static const enum_t STR = 0;
        //        static const enum_t CHAR = 1;
        //        static const enum_t UCHAR = 2;
        //        static const enum_t SHORT = 3;
        //        static const enum_t USHORT = 4;
        //        static const enum_t INT = 5;
        //        static const enum_t UINT = 6;
        //        static const enum_t FLOAT = 7;
        //        static const enum_t LONG = 8;
        //        static const enum_t ULONG = 9;
        //        static const enum_t DOUBLE = 10;
        //        static const enum_t DATETIME = 11;
        //        static const enum_t IP = 12;
        switch (nbr) {
            case cep::buffer_metadata::CHAR:
                x.type = cep::buffer_metadata::CHAR;
                y.type = cep::buffer_metadata::CHAR;
                cep::buff_util::set_value<char>(data4x, 0, 'B');
                cep::buff_util::set_value<char>(data4y, 0, 'B');
                break;
            case cep::buffer_metadata::INT:
                x.type = cep::buffer_metadata::INT;
                y.type = cep::buffer_metadata::INT;
                cep::buff_util::set_value<int>(data4x, 0, 16);
                cep::buff_util::set_value<int>(data4y, 0, 15);
                break;
            case cep::buffer_metadata::LONG:
                x.type = cep::buffer_metadata::LONG;
                y.type = cep::buffer_metadata::LONG;
                cep::buff_util::set_value<long>(data4x, 0, 15);
                cep::buff_util::set_value<long>(data4y, 0, 256 + 15);
                break;
            default: // cep::buffer_metadata::STR:
                x.type = cep::buffer_metadata::STR;
                y.type = cep::buffer_metadata::STR;
                cep::slice a("ABC"), b("ABC");
                cep::buff_util::set_value(data4x, 0, a.size(), a);
                cep::buff_util::set_value(data4y, 0, b.size(), b);
        }
        cout << "data4x=" << cep::buff_util::to_hex_string(data4x, sizeof (data4x))
                << "\ndata4y=" << cep::buff_util::to_hex_string(data4y, sizeof (data4y)) << endl;
        bool b = cep::logic_function::call(runMode - '0', x, y);
        cout << runMode - '0' << "=\n" << cep::buff_util::to_hex_string(x.data.data(), x.data.size())
                << '\n' << cep::buff_util::to_hex_string(y.data.data(), y.data.size()) << endl;
        if (b) cout << " return true!" << endl;
        else cout << " return false!" << endl;

        cout << "x == y is:" << (x == y) << endl;
        cout << "x != y is:" << (x != y) << endl;
        cout << "x < y is:" << (x < y) << endl;
        cout << "x <= y is:" << (x <= y) << endl;
        cout << "x > y is:" << (x > y) << endl;
        cout << "x >= y is:" << (x >= y) << endl;

        const char* strNbr = "-123";
        char data4slice[10] = {0};
        size_t sizeOfNbr = utils_function::cstr2nbr(buffer_metadata::INT, strNbr, data4slice);
        cout << cep::buff_util::to_hex_string(strNbr, sizeof (strNbr)) << " convert to int("
                << sizeOfNbr << ") is "
                << cep::buff_util::to_hex_string(data4slice, sizeof (data4slice))
                << " value is " << *((int*) data4slice) << endl;
        memset(data4slice, 0, sizeof (data4slice));
        const bool flag = utils_function::cstrtonbr(buffer_metadata::INT, strNbr, data4slice, sizeOfNbr);
        cout << cep::buff_util::to_hex_string(strNbr, sizeof (strNbr)) << " convert to int("
                << sizeOfNbr << ") is "
                << cep::buff_util::to_hex_string(data4slice, sizeof (data4slice))
                << " value is " << *((int*) data4slice) << (flag ? " success!" : " failure!") << endl;
        buff_util::set_value<int> (data4slice, 5, -123);
        cout << "set -123 at 5 is " << cep::buff_util::to_hex_string(data4slice, sizeof (data4slice)) << endl;

        // typedef map < cep::type_slice, string, cep::less> slice_map;
        typedef map < cep::type_slice, string> slice_map;
        slice_map map;
        std::pair < slice_map::iterator, bool> rst;
        rst = map.insert(slice_map::value_type(x, "1"));
        if (!rst.second) cout << "insert error!" << endl;
        rst = map.insert(slice_map::value_type(y, "2"));
        if (!rst.second) cout << "insert error!" << endl;
        slice_map::const_iterator itr = map.begin(), end = map.end();
        for (; itr != end; ++itr) {
            const slice* data = &(itr->first.data);
            cout << itr->first.type << ':' << cep::buff_util::to_hex_string(data->data(), data->size())
                    << "\n=" << itr->second << endl;
        }

        cout << "*** unit test for idx_rbtree::slice_map_t ***" << endl;
        //    char* k = new char[8];
        //    memset(k, 0, 16);
        //    memcpy(k, "wangqun", 7);
        struct cep::type_slice keyA = {cep::buffer_metadata::STR, cep::slice("wangqun")}; // k)};
        //    k = new char[2];
        //    memset(k, 0, 2);
        //    memcpy(k, "x", 1);
        struct cep::type_slice keyB = {cep::buffer_metadata::STR, cep::slice("x")}; // k)};
        //    k = new char[2];
        //    memset(k, 0, 2);
        //    memcpy(k, "x", 1);
        struct cep::type_slice keyC = {y.type = 0, y.data = cep::slice("x")}; // k)};
        idx_rbtree::slice_map_t smap;
        smap.insert(idx_rbtree::slice_map_t::value_type(keyA, 0));
        smap.insert(idx_rbtree::slice_map_t::value_type(keyB, 1));
        smap.insert(idx_rbtree::slice_map_t::value_type(keyC, 2));
        //    k = new char[2];
        //    memset(k, 0, 2);
        //    memcpy(k, "x", 1);
        struct cep::type_slice findKey = {y.type = 0, y.data = cep::slice("x")}; // k)};
        pair<idx_rbtree::slice_map_t::const_iterator, idx_rbtree::slice_map_t::const_iterator>
                ret = smap.equal_range(findKey);
        cout << "equal_range by:" << findKey << endl;
        if (ret.first == ret.second) cout << "multimap not found" << endl;
        else {
            int rownum = 0;
            for (idx_rbtree::slice_map_t::const_iterator itr = ret.first;
                    itr != ret.second; ++itr, ++rownum) {
                cout << rownum << '>' << itr->first << '=' << itr->second << endl;
            }
        }
        cout << "show all multimap:" << endl;
        idx_rbtree::slice_map_t::const_iterator itr4smap = smap.begin(), end4smap = smap.end();
        for (; itr4smap != end4smap; ++itr4smap) {
            const slice* data = &(itr4smap->first.data);
            cout << itr4smap->first << '=' << itr4smap->second << endl;
        }
        return;
    } else if (runMode == 'c') {
        const size_t len = 16;
        // char *a = new char[len], *b = new char[len];
        // memset(a, 0, len);
        // memset(b, 0, len);
        char a[len] = {0}, b[len] = {0};
        cout << "a=" << cep::buff_util::to_hex_string(a, len)
                << "\nb=" << cep::buff_util::to_hex_string(b, len) << endl;
        *((int*) a) = 2;
        *((int*) b) = (16 * 16) - 1;
        // buff_util::set_value<int>(a, 0, 2);
        // buff_util::set_value<int>(b, 0, 16 * 16);
        cout << "a=" << cep::buff_util::to_hex_string(a, len)
                << "\nb=" << cep::buff_util::to_hex_string(b, len) << endl;
        cout << memcmp(a, b, len) << endl;
        *((int*) b) = 16 * 16;
        cout << "a=" << cep::buff_util::to_hex_string(a, len)
                << "\nb=" << cep::buff_util::to_hex_string(b, len) << endl;
        cout << memcmp(a, b, len) << endl;
        return;
    }
    const_dataset dataset;

    cep::buffer_metadata bm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("id", 0, cep::buffer_metadata::INT);
    //    entry->type = cep::buffer_metadata::INT;
    //    entry->idx = 0;
    //    entry->code = "id";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("name", 10, cep::buffer_metadata::STR, 128);
    //    entry->offset = -1;
    //    entry->length = 128;
    //    entry->type = cep::buffer_metadata::STR;
    //    entry->flag = 0;
    //    entry->idx = 10;
    //    entry->code = "name";
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("gender", 20, cep::buffer_metadata::CHAR);
    //    entry->type = cep::buffer_metadata::CHAR;
    //    entry->idx = 20;
    //    entry->code = "gender"; // sex
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("birthday", 30, cep::buffer_metadata::DATETIME, -1, 1, 112);
    //    entry->offset = 112;
    //    entry->type = cep::buffer_metadata::DATETIME;
    //    entry->flag = 1; // __CEP_VIRTUAL_MASK__
    //    entry->idx = 30;
    //    entry->code = "birthday";
    bm.add_cfg_entry(entry);
    bm.rebuild();

    const_dataset::table tbl("stack_person", bm);
    tbl.set_idx(100);
    tbl.reserve(4);
    dynamic_struct* data = new dynamic_struct(&bm);
    slice name("wq");
    data->set_value("name", name);
    data->set_value(0, (int) 16);
    data->set_value(2, (char) 'm');
#include <time.h>
    unsigned long now = time(NULL);
    now *= 1000;
    cout << "now is:" << milsecond2str(now) << endl;
    data->set_value(3, now);
    cout << "insert data:" << *data << endl;
    if (tbl.insert(data)) cout << "OK!" << endl;
    else cout << "ERROR!" << endl;
    data = new dynamic_struct(&bm);
    slice name2("x");
    data->set_value("name", name2);
    data->set_value(0, (int) 258);
    data->set_value(2, (char) 'f');
    tbl.insert(data);

    data = new dynamic_struct(&bm); // memory leak!
    slice name3("x");
    data->set_value("name", name3);
    data->set_value(0, (int) 259);
    data->set_value(2, (char) 'f');
    cout << "exists data:" << *data << endl;
    if (tbl.exists(*data)) cout << "successful!" << endl;
    else cout << "failure!" << endl;

    data->set_value(0, (int) 258);
    cout << "exists data:" << *data << endl;
    if (tbl.exists(*data)) cout << "successful!" << endl;
    else cout << "failure!" << endl;

    const_dataset::table::rows_t::size_type size = tbl.size() + 1;
    const dynamic_struct* row;
    for (int i = 0; i < size; ++i) {
        row = tbl.get(i);
        if (row) cout << i << "> found:" << *row << endl;
        else cout << i << "> not found!" << endl;
    }

    cout << "table is:" << tbl << endl;
    const string tableName = "person";
    const string indexName = "idx_person_name", indexName2 = "idx_person_name_gender",
            indexName3 = "idx_person_birthday";
    const_dataset::table* p_tbl = dataset.insert(tableName, bm);
    cout << "add table person:" << p_tbl << endl;
    p_tbl = dataset.get_table(tableName);
    if (p_tbl == NULL) cout << "cant found table by name:" << tableName << endl;
    else {
        cout << "found table[" << tableName << "]:" << *p_tbl << endl;
        p_tbl = dataset.get_table(p_tbl->idx());
        cout << "found table using index:" << p_tbl << endl;
    }

    data = new dynamic_struct(&bm);
    data->set_value("name", slice("haha"));
    data->set_value("id", (int) 258);
    data->set_value("gender", (char) 'm');
    data->set_value("birthday", now);
    p_tbl->insert(data);
    data = new dynamic_struct(&bm);
    data->set_value("name", slice("hehe"));
    data->set_value("id", (int) 16);
    data->set_value("gender", (char) 'f');
    data->set_value("birthday", now);
    p_tbl->insert(data);
    data = new dynamic_struct(&bm);
    // data->set_value("name", slice("x ")); 78 20
    data->set_value("name", slice("x"));
    data->set_value("id", (int) 17);
    data->set_value("gender", (char) 'f');
    data->set_value("birthday", (unsigned long) 0);
    p_tbl->insert(data);
    data = new dynamic_struct(&bm);
    data->set_value("name", slice("x"));
    data->set_value("id", (int) 1025);
    data->set_value("gender", (char) 'f');
    data->set_value("birthday", (unsigned long) 0); // 129
    p_tbl->insert(data);

    vector<string> keys(1);
    keys[0] = "name";
    const_dataset::index* p_idx = new idx_rbtree(indexName, keys, tbl); // *p_tbl);
    cout << "new idx_rbtree index:" << *((idx_rbtree*) p_idx) << endl;
    cout << "index's table is:" << p_idx->get_table() << endl;
    if (dataset.insert(p_idx)) {
        cout << "insert index successful:" << *p_idx << endl;
    } else {
        cout << "insert index failure:" << *p_idx << endl;
        delete p_idx;
    }
    p_idx = dataset.get_index(indexName);
    if (p_idx == NULL) cout << "cant found index by name:" << indexName << endl;
    else {
        cout << "found index[" << indexName << "]:" << *p_idx << endl;
        p_idx = dataset.get_index(p_idx->idx());
        cout << "found index using index:" << p_idx << endl;
    }
    p_idx->rebuild();
    // dynamic_struct selectArg(&bm);
    // selectArg.set_value("name", slice("x"));
    type_slice where[p_idx->key_size()];
    where[0].data = slice("x");
    where[0].type = buffer_metadata::STR;

    struct DatasetIterator : public const_dataset::index::iterator {
        int counter;

        bool operator()(const dynamic_struct& data, const size_t & rownum) {
            cout << rownum << '>' << data << endl;
            ++counter;
            return true;
        }
    };

    // error: braces around initializer for non-aggregate type ‘DatasetIterator’
    // struct DatasetIterator di = {di.counter = 0};
    DatasetIterator di;
    di.counter = 0;
    // cout << "*** start select index by:" << selectArg << endl;
    cout << "*** start select index by:" << where[0] << endl;
    // p_idx->select(selectArg, di);
    p_idx->select(where, di);

    // selectArg.set_value("gender", (char) 'f');
    cout << "search table is:" << *p_tbl << '!' << endl;
    keys.resize(2);
    keys[1] = "gender";
    p_idx = new idx_rbtree(indexName2, keys, *p_tbl);
    if (dataset.insert(p_idx)) {
        cout << "insert index successful:" << *p_idx << endl;
    } else {
        cout << "insert index failure:" << *p_idx << endl;
        delete p_idx;
    }
    cout << "new idx_rbtree index:" << *((idx_rbtree*) p_idx) << endl;
    p_idx->rebuild();
    type_slice where2[p_idx->key_size()];
    where2[0].data = slice("x"); // name
    where2[0].type = buffer_metadata::STR;
    where2[1].data = slice("f"); // gender
    where2[1].type = buffer_metadata::CHAR;
    // cout << "*** start select index by:" << selectArg << endl;
    cout << "*** start select index by:" << where2[0] << " and " << where2[1] << endl;
    // p_idx->select(selectArg, di);
    p_idx->select(where2, di);

    // selectArg.set_value("birthday", now);
    char birthday[buffer_metadata::SIZEOF_DATETIME];
    buff_util::set_value<unsigned long>(birthday, 0, now);
    where[0].data = slice(birthday, sizeof (birthday));
    where[0].type = buffer_metadata::DATETIME;
    // cout << "*** start select index by:" << selectArg << endl;
    cout << "*** start select index by:" << where[0] << endl;
    keys.resize(1);
    keys[0] = "birthday";
    p_idx = new idx_rbtree(indexName3, keys, *p_tbl);
    if (dataset.insert(p_idx)) {
        cout << "insert index successful:" << *p_idx << endl;
    } else {
        cout << "insert index failure:" << *p_idx << endl;
        delete p_idx;
    }
    cout << "new idx_rbtree index:" << *((idx_rbtree*) p_idx) << endl;
    p_idx->rebuild();
    // p_idx->select(selectArg, di);
    p_idx->select(where, di);

    if (runMode == 'r') {
        cout << "*** const_dataset is:" << dataset << endl;

        ref_dataset::metadata ref_md;
        const string recordsetName = "test1", recordsetName2 = "test2";
        ref_dataset::metadata::impl_t* p_refmd = new ref_dataset::metadata::impl_t();
        // p_refmd->insert(ref_dataset::metadata::impl_t::value_type("col", 0));
        // p_refmd->insert(ref_dataset::metadata::impl_t::value_type("col2", 1));
        ref_dataset::metadata::impl_t::field_t* field = new ref_dataset::metadata::impl_t::field_t;
        field->code = "col";
        field->idx = 0;
        field->type = buffer_metadata::STR;
        p_refmd->add_entry(field);
        field = new ref_dataset::metadata::impl_t::field_t;
        field->code = "col2";
        field->type = buffer_metadata::CHAR;
        p_refmd->add_entry(field);
        if (ref_md.insert(recordsetName, p_refmd)) {
            cout << "insert ref_dataset::metadata[" << recordsetName << "] successful:" << p_refmd << endl;
            cout << "ref_dataset::metadata::impl_t is:" << *p_refmd << endl;
            const ref_dataset::metadata::impl_t::field_t* ptr_field = p_refmd->entry("col2");
            cout << "p_refmd->entry(\"col2\") is:" << ptr_field << endl;
            ptr_field = p_refmd->entry(1);
            cout << "p_refmd->entry(1) is:" << ptr_field << endl;
        } else {
            cout << "insert ref_dataset::metadata[" << recordsetName << "] failure:" << p_refmd << endl;
            delete p_refmd;
        }
        p_refmd = new ref_dataset::metadata::impl_t();
        // p_refmd->insert(ref_dataset::metadata::impl_t::value_type("field1", 0));
        field = new ref_dataset::metadata::impl_t::field_t;
        field->code = "field1";
        p_refmd->add_entry(field);
        if (ref_md.insert(recordsetName2, p_refmd))
            cout << "insert ref_dataset::metadata[" << recordsetName2 << "] successful:" << p_refmd << endl;
        else {
            cout << "insert ref_dataset::metadata[" << recordsetName2 << "] failure:" << p_refmd << endl;
            delete p_refmd;
        }
        int idx = ref_md.get(recordsetName2);
        if (idx < 0) cout << "not found ref_dataset::metadata[" << recordsetName2 << "] index!" << endl;
        else {
            p_refmd = ref_md.get(idx);
            if (p_refmd == NULL)
                cout << "not found ref_dataset::metadata by index:" << idx << endl;
            else
                cout << "found ref_dataset::metadata by index["
                    << idx << "]:" << p_refmd << endl;
        }
        ref_dataset ref_ds(ref_md);
        ref_dataset::recordset* ref_rs = ref_ds.get(recordsetName);
        if (ref_rs == NULL)
            cout << "not found ref_dataset::recordset by name:" << recordsetName << endl;
        else {
            cout << "found ref_dataset::recordset by name[" << recordsetName << "]:"
                    << *ref_rs << endl;
            idx = ref_rs->idx();
            ref_rs = ref_ds.get(idx);
            cout << "found ref_dataset::recordset by index[" << idx << "]:"
                    << *ref_rs << endl;

            p_tbl = dataset.get_table(tableName);
            cout << "ref table is:" << *p_tbl << '!' << endl;
            ref_dataset::record* ref_r;
            slice defaultSlice;
            for (int i = p_tbl->size() - 1; i >= 0; --i) {
                row = p_tbl->get(i);
                if (row) cout << i << "### found:" << *row << endl;
                else cout << i << "### not found!" << endl;
                ref_r = new ref_dataset::record(*ref_rs);
                // ref_r->set("col", row->value("name", defaultSlice));
                // ref_r->set("col2", row->value("gender", defaultSlice));
                type_slice data;
                data.type = buffer_metadata::STR;
                data.data = row->value("name", defaultSlice);
                ref_r->set("col", data);
                data.type = buffer_metadata::CHAR;
                data.data = row->value("gender", defaultSlice);
                ref_r->set("col2", data);
                if (ref_rs->exists(*ref_r))
                    cout << "<<< exists >>>" << *ref_r << " in " << *ref_rs << endl;
                if (ref_rs->insert(ref_r)) {
                    cout << "success insert " << *ref_r << "\n to " << *ref_rs << endl;
                } else {
                    cout << "failure insert " << *ref_r << "\n to " << *ref_rs << endl;
                    delete ref_r;
                }
            }
        }
        cout << "*** ref_dataset is:" << ref_ds << endl;
        ref_dataset* ptr_refds = new ref_dataset(ref_ds); // new ref_dataset(ref_md);
        event* newEvt = NULL;
        {
            const event evt((char *) 0, 0);
            evt.set_dataset(ptr_refds);
            cout << "set ptr_refds[" << ptr_refds << "] to event[" << &evt << "]->" << *evt.dataset() << endl;
            newEvt = new event(evt);
        }
        if (newEvt != NULL) {
            cout << "newEvt[" << newEvt << "]->" << *(newEvt->dataset()) << endl;
            delete newEvt;
        }
        cout << "after delete newEvt:" << newEvt << '!' << endl;
        /*if (nbr == 12)
            while (true) {
                newEvt = new event(evt);
                delete newEvt;
            }*/
    }
    cout << "*** end of TestConstDataset[" << arg << "] ***" << endl;
}

void TestRouter(int argc, char** argv) { // u
    cout << "*** begin of TestRouter ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }/*
    string pn_id, url;
    if (argc > 2) pn_id = argv[2];
    if (argc > 3) url = argv[3];
    cout << "before init:" << pn::instance() << endl;
    pn::instance().init(pn_id);
    cout << "after no url init:" << pn::instance() << endl;
    pn::instance().init(pn_id, url);
    cout << "after init:" << pn::instance() << endl;

    string config4cep = "<cep>\n"
            "\t<peset>\n"
            "\t\t<pe so_path=\"./libpe4time_series_grouping.so\" config_url=\"http://localhost/cep/config4pe?pe_id=1\" pn_id=\"pn_5000\">4098</pe>\n"
            "\t\t<pe so_path=\"./libpe4time_series_grouping.so\" config_url=\"http://localhost/cep/config4pe?pe_id=2\" pn_id=\"reomte_pn_001\">20381</pe>\n"
            "\t</peset>\n"
            "\t<pnset>\n"
            "\t\t<pn pn_id=\"pn_5000\" cache_len=\"10000\" worker_cnt=\"4\">\n"
            "\t\t\t<prolineset type=\"process\">\n"
            "\t\t\t\t<line begin=\"0\" end=\"4098\">556</line>\n"
            "\t\t\t</prolineset>\n"
            "\t\t\t<reclineset>\n"
            "\t\t\t\t<line begin=\"4098\" end=\"20381\">557</line>\n"
            "\t\t\t</reclineset>\n"
            "\t\t</pn>\n"
            "\t\t<pn pn_id=\"pn_5001\" cache_len=\"10000\" worker_cnt=\"4\">\n"
            "\t\t\t<prolineset type=\"process\">\n"
            "\t\t\t\t<line begin=\"0\" end=\"16384\">5554</line>\n"
            "\t\t\t</prolineset>\n"
            "\t\t\t<reclineset>\n"
            "\t\t\t\t<line begin=\"16384\" end=\"20381\">4</line>\n"
            "\t\t\t</reclineset>\n"
            "\t\t</pn>\n"
            "\t</pnset>\n"
            "</cep>";
    xml_document* doc = cep::parse_xmlstr(config4cep.c_str()); // "<hello><world/></hello>");
    cout << "doc address is:" << doc << endl;
    const xml_element* root = cep::root_element(*doc);
    cout << "root address is:" << root << endl;
    const xml_element* elem = cep::first_child(*root);
    cout << "elem address is:" << elem << endl;

    if (argv[1][1] == 'n') {
        pn::instance().reload(*root);
        cout << "*** start the second reload ***" << endl;
        pn::instance().reload(*root);

        cout << "*** start the 1st startup ***" << endl;
        pn::instance().startup();
        cout << "*** start the 2nd startup ***" << endl;
        pn::instance().startup();
        cout << "*** start the 1st shutdown ***" << endl;
        pn::instance().shutdown();
        cout << "*** start the 2nd shutdown ***" << endl;
        pn::instance().shutdown();

        pn* ptr_pn = &pn::instance();
        cout << "*** start the final shutdown ***" << endl;
        ptr_pn->shutdown();
        cout << "pn:" << *ptr_pn << endl;
    } else if (argv[1][1] == 'u') {
        router rt;
        rt.rebuild(1, 0);
        cout << "initialization router:" << rt << endl;
        pe4initest pe1(*elem, *root);
        pe1.set_id(1);
        rt.set_routing4processed(NULL, pe1);

        cout << "router:" << rt << endl;
        pe * local_pes[rt.local_pe_count()];
        for (int i = 0; i < rt.local_pe_count(); ++i)
            cout << i << "> pe@" << local_pes[i] << endl;

        event evt(NULL, 0, 12);
        evt.set_pe_id(0);
        cout << "event pe id is " << evt.pe_id() << endl;

        int pe_count = rt.routing4processed(evt, local_pes);
        for (int i = 0; i < pe_count; ++i) {
            cout << i << "# pe@" << local_pes[i] << endl;
            evt.router_list().push_back(local_pes[i]->pn_id());
        }
        int size = evt.router_list().size();
        for (int i = 0; i < size; ++i)
            cout << i << '>' << evt.router_list()[i] << '!' << endl;
        evt.router_list().clear();
        cout << "*** remote routing ***" << endl;
        pe_count = rt.routing4processed(evt, local_pes, 'r');
        for (int i = 0; i < pe_count; ++i) {
            cout << i << "# pe@" << local_pes[i] << endl;
            evt.router_list().push_back(local_pes[i]->pn_id());
        }
        size = evt.router_list().size();
        for (int i = 0; i < size; ++i)
            cout << i << '>' << evt.router_list()[i] << '!' << endl;
    } else {
        router rt;
        cout << "initialization router:" << rt << endl;
        const int local_pe_count = 4, reomte_pe_count = 2;
        rt.rebuild(local_pe_count, reomte_pe_count);
        pe4initest pe1(*elem, *root), pe2(*elem, *root), pe3(*elem, *root), pe4(*elem, *root),
                pe_r1(*elem, *root), pe_r2(*elem, *root);
        pe1.set_id(1);
        pe2.set_id(2);
        pe3.set_id(3);
        pe4.set_id(4);
        pe_r1.set_id(5);
        pe_r2.set_id(6);
        pe1.set_pn_id(pn::instance().id());
        pe2.set_pn_id(pn::instance().id());
        pe3.set_pn_id(pn::instance().id());
        pe4.set_pn_id(pn::instance().id());
        pe_r1.set_pn_id("remote_pn_01");
        pe_r2.set_pn_id("remote_pn_02");

        rt.set_routing4processed(NULL, pe1);
        rt.set_routing4processed(NULL, pe2);
        rt.set_routing4processed(&pe2, pe1);
        rt.set_routing4processed(&pe2, pe3);
        rt.set_routing4processed(&pe2, pe_r1);
        rt.set_routing4processed(&pe4, pe_r2);
        rt.set_routing4processed(&pe_r2, pe1);
        rt.set_routing4processed(&pe_r2, pe_r2);

        rt.set_routing4received(NULL, pe_r2);
        rt.set_routing4received(&pe_r2, pe1);

        cout << "router:" << rt << endl;
        pe * local_pes[rt.local_pe_count()];
        for (int i = 0; i < local_pe_count; ++i)
            cout << i << "> pe@" << local_pes[i] << endl;

        event evt(NULL, 0, 12);
        evt.set_pe_id(4);
        cout << "event pe id is " << evt.pe_id() << endl;

        int pe_count = rt.routing4processed(evt, local_pes);
        for (int i = 0; i < pe_count; ++i) {
            cout << i << "# pe@" << local_pes[i] << endl;
            evt.router_list().push_back(local_pes[i]->pn_id());
        }
        int size = evt.router_list().size();
        for (int i = 0; i < size; ++i)
            cout << i << '>' << evt.router_list()[i] << '!' << endl;
        evt.router_list().clear();
        cout << "*** remote routing ***" << endl;
        pe_count = rt.routing4processed(evt, local_pes, 'r');
        for (int i = 0; i < pe_count; ++i) {
            cout << i << "# pe@" << local_pes[i] << endl;
            evt.router_list().push_back(local_pes[i]->pn_id());
        }
        size = evt.router_list().size();
        for (int i = 0; i < size; ++i)
            cout << i << '>' << evt.router_list()[i] << '!' << endl;

        cout << "systime_sec:" << processing_utils::systime_sec() << endl;
        cout << "systime_msec:" << processing_utils::systime_msec() << endl;
        cout << "systime_usec:" << processing_utils::systime_usec() << endl;
        cout << "systime_nsec:" << processing_utils::systime_nsec() << endl;
        cout << "systime_nsec2:" << processing_utils::systime_nsec() << endl;
        cout << "systime_psec:" << processing_utils::systime_psec() << endl;
    }*/
    cout << "*** end of TestRouter ***" << endl;
}

CEP_LOAD_PE(config, main_config) {
    cout << "execute " << CEP_MACRO2CSTR(CEP_LOAD_PE_FUNCTION)
            << '(' << &config << "->" << cep::name(config).data()
            << ',' << &main_config << "->" << cep::name(main_config).data() << ')' << endl;
    return new pe4initest(config, main_config);
}

CEP_UNLOAD_PE(ptr_pe);

CEP_PLUGIN_FUNC4INIT_EXEC(source, source_count, destination, fields_idx, fields_count) {
    cout << "execute " << CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION)
            << '(' << source << ',' << source_count << ',' << &destination << ',' << fields_idx << ',' << fields_count << ')' << endl;
}

void TestPlugin(char *arg) { // v
    cout << "*** begin of TestPlugin[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long typeId = 0;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            typeId = atoi(arg + 2);
    } else
        runMode = 'x';
    MLOG_INFO << "test run mode is [" << runMode << "] and type id is " << typeId << endl;

    // xml_document* doc = cep::parse_xmlstr("<hello>\r\n<world/>\r\n</hello>");
    xml_document* doc = cep::parse_xmlstr("<hello><world/></hello>");
    cout << "doc address is:" << doc << endl;
    const xml_element* root = cep::root_element(*doc);
    cout << "root address is:" << root << endl;
    const xml_element* elem = cep::first_child(*root);
    cout << "elem address is:" << elem << endl;

    cout << "load pe from function:" << CEP_MACRO2CSTR(CEP_LOAD_PE_FUNCTION) << endl;
    cout << "unload pe from function:" << CEP_MACRO2CSTR(CEP_UNLOAD_PE_FUNCTION) << endl;
    cep::load_pe_func_ptr_t pe_creator = &CEP_LOAD_PE_FUNCTION;
    cep::unload_pe_func_ptr_t pe_destroyer = &CEP_UNLOAD_PE_FUNCTION; // terminator

    pe* plugin4pe = pe_creator(*elem, *root);

    plugin_func_ptr_t plugin_func = &CEP_PROCESSING_INIT_EXEC_FUNCTION;
    size_t source_count = 0;
    type_slice source[source_count];
    char *buff = new char[0];
    event destination(buff, 0);
    size_t fields_idx[0], fields_count;
    cout << "execute plug-in function:" << source << ',' << source_count << ',' << &destination << ',' << fields_idx << ',' << fields_count << ')' << endl;
    plugin_func(source, source_count, destination, fields_idx, fields_count);

    dlhandler dlh;
    dlh.open("xxx");
    dlh.open("abc");
    cout << "dlhandler:" << dlh << endl;
    dlhandler::factory dlfactory;
    dlfactory.add_dlhandler("hehe", 0);
    const dlhandler* ptr_dlh = dlfactory.get_dlhandler("haha");
    cout << "*** start test dlsym ***" << (ptr_dlh == NULL ? "null" : "not null") << endl;
    // error: passing ‘const cep::dlhandler’ as ‘this’ argument of ‘bool cep::dlhandler::open(const std::string&, int)’ discards qualifiers
    // ptr_dlh->open("wuwu");

    typedef int* ptr_int_t;
    ptr_int_t *pptr_i;
    cout << "address of pointer to pointer:" << pptr_i << endl;
    ptr_int_t ptr_i = NULL;
    pptr_i = &ptr_i;
    cout << "address of pointer to pointer:" << pptr_i << endl;
    cout << "pointer to pointer address:" << *pptr_i << endl;

    typedef void (*func_ptr_print_t)(const string&);
    func_ptr_print_t* pptr_func;
    cout << "address of pointer to func_ptr:" << pptr_func << endl;
    func_ptr_print_t ptr_func = NULL;
    pptr_func = &ptr_func;
    cout << "address of pointer to func_ptr:" << pptr_func << endl;
    cout << "pointer to func_ptr address:" << *pptr_func << endl;
    if (ptr_dlh != NULL) ptr_dlh->dlsymbol("print", pptr_func);
    if (pptr_func != NULL && *pptr_func != NULL) {
        cout << "dlsymbol:" << *pptr_func << endl;
        (*pptr_func)("hehe");
    }
    const string sofile =
            // "/root/.netbeans/remote/10.45.7.77/waq-Windows-x86/D/data4daily/cc/ztemm/"
            // "cepframe/dist/Release/GNU-Linux-x86"
            "./libplugin_func4natural_min_diff.so";
    const string plugin_func_id = "first_plugin_func_id";
    int plugin_func_idx;
    dlfactory.add_dlhandler(sofile);
    ptr_dlh = dlfactory.get_dlhandler(sofile);
    if (ptr_dlh != NULL) {
        plugin_func_ptr_t* pptr_plugin_func;
        plugin_func_ptr_t ptr_plugin_func = NULL;
        pptr_plugin_func = &ptr_plugin_func;
        // ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION), pptr_plugin_func);
        ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION), ptr_plugin_func);
        //        if (*pptr_plugin_func != NULL) {
        //            cout << "dlsymbol:" << *pptr_plugin_func << endl;
        //            (*pptr_plugin_func)(source, source_count, destination, fields_idx, fields_count);
        //            plugin_func_idx = pn::instance().plugin_funcs().add_plugin_func(plugin_func_id, *pptr_plugin_func);
        //            cout << "pn::instance().plugin_funcs().add_plugin_func(" << plugin_func_id
        //                    << ", " << *pptr_plugin_func << ") return " << plugin_func_idx << endl;
        //        } else {
        if (ptr_plugin_func != NULL) {
            cout << "dlsymbol:" << ptr_plugin_func << endl;
            ptr_plugin_func(source, source_count, destination, fields_idx, fields_count);
            plugin_func_idx = pn::instance().plugin_funcs().add_plugin_func(plugin_func_id, ptr_plugin_func);
            cout << "pn::instance().plugin_funcs().add_plugin_func(" << plugin_func_id
                    << ", " << ptr_plugin_func << ") return " << plugin_func_idx << endl;
        } else {
            cout << "dlsymbol(" << CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION)
                    << ", " << pptr_plugin_func << "->" << ptr_plugin_func << ") return NULL!" << endl;
        }
    } else
        cout << "dlfactory.get_dlhandler(" << sofile << ") return NULL!" << endl;
    cout << "dlhandler::factory:" << dlfactory << endl;
    cout << "pn::instance().plugin_funcs:" << pn::instance().plugin_funcs() << endl;

    plugin_func_idx = pn::instance().plugin_funcs().plugin_func_idx(plugin_func_id);
    cout << "pn::instance().plugin_funcs().plugin_func_idx(" << plugin_func_id
            << ") return " << plugin_func_idx << endl;
    if (plugin_func_idx >= 0) {
        destination.set_pe_id(2000);
        pn::instance().plugin_funcs().execute(plugin_func_idx,
                source, source_count, destination, fields_idx, fields_count);
    }
    cout << "pn::instance().plugin_funcs 2:" << pn::instance().plugin_funcs() << endl;

    const string sofile4pe =
            // "/root/.netbeans/remote/10.45.7.77/waq-Windows-x86/D/data4daily/cc/ztemm/"
            // "cepframe/dist/Release/GNU-Linux-x86"
            "./libpe4enrich.so";
    const string pe_id = "first_pe_id";
    dlfactory.add_dlhandler(sofile4pe);
    ptr_dlh = dlfactory.get_dlhandler(sofile4pe);
    if (ptr_dlh != NULL) {
        // load_pe_func_ptr_t* pptr_load_pe;
        load_pe_func_ptr_t ptr_load_pe; // = NULL;
        // pptr_load_pe = &ptr_load_pe;
        // ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_LOAD_PE_FUNCTION), pptr_load_pe);
        if (ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_LOAD_PE_FUNCTION), ptr_load_pe)) {
            // if (ptr_load_pe != NULL) {
            cout << "dlsymbol:" << ptr_load_pe << endl;
            pe* pe = ptr_load_pe(*elem, *root);
            pe->init();
            pe->startup();
            pe->shutdown();
            delete pe;
        } else {
            cout << "dlsymbol(" << CEP_MACRO2CSTR(CEP_LOAD_PE_FUNCTION)
                    << ", " << ptr_load_pe << ") return NULL!" << endl;
        }
    } else
        cout << "dlfactory.get_dlhandler(" << sofile4pe << ") return NULL!" << endl;
    cout << "dlhandler::factory:" << dlfactory << endl;

    cout << "*** end of TestPlugin[" << arg << "] ***" << endl;
}

void TestEveryPlugin(char *arg) { // K
    cout << "*** begin of TestEveryPlugin[" << arg << "] ***" << endl;
    //假设目的事件2010，则2010的结构必须事先知道，方便测试

    const char *xmlfile = "cep_config.xml";
    cep::xml_document *doc = cep::load_xmlfile(xmlfile);
    if (!doc) {
        cout << "Load file [" << xmlfile << "] failure open xml file!" << endl;
        //todo xml from net
    } else {
        cout << "Load file [" << xmlfile << "] ***" << endl;
        const cep::xml_element *xml_temp = cep::root_element(*doc);
        xml_temp = cep::find_element(*xml_temp, "eventset");
        if (xml_temp != NULL)
            cep::config_metadata::instance().reload_metadata(*xml_temp);
        else
            return;
    }
    cout << "*** *** ***\nconfiguration for meta data is:\n"
            << cep::config_metadata::instance() << endl;
    cout << "*** end of TestLoadEvent[" << arg << "] ***" << endl;
    const buffer_metadata* bufmet = cep::config_metadata::instance().metadata(2000);
    if (bufmet != NULL) {
        event destination(bufmet);
        cout << "ok continue the line is [" << __LINE__ << "]" << endl;
        //for A test over!
        //        size_t source_count = 1;
        size_t source_count = 2;
        type_slice source[source_count];
        slice sli("20");
        source[0].data = sli;
        source[0].type = cep::buffer_metadata::DOUBLE;
        slice sli2("21");
        source[1].data = sli2;
        source[1].type = cep::buffer_metadata::DOUBLE;
        size_t fields_idx[1], fields_count;
        fields_idx[0] = 4;
        fields_count = 1;
        cout << "execute plug-in function:(" << source << ',' << source_count << ',' << &destination << ',' << fields_idx << ',' << fields_count << ')' << endl;
        dlhandler::factory dlfactory;
        typedef int* ptr_int_t;
        ptr_int_t *pptr_i;

        ptr_int_t ptr_i = NULL;
        pptr_i = &ptr_i;

        typedef void (*func_ptr_print_t)(const string&);
        func_ptr_print_t* pptr_func;
        func_ptr_print_t ptr_func = NULL;
        pptr_func = &ptr_func;
        //for A
        //        const string sofile = "./libplugin_func4_min_A.so";
        //        const string sofile = "./libplugin_func4_max_A.so";
        const string sofile = "./libplugin_func4_count.so";
        //        const string sofile = "./libplugin_func4_sum_A.so";
        //for AB
        //        const string sofile = "./libplugin_func4_sum_AB.so";
        //        const string sofile = "./libplugin_func4natural_diff_AB.so";
        //        const string sofile = "./libplugin_func4natural_max_diff_AB.so";
        //        const string sofile = "./libplugin_func4natural_min_diff_AB.so";
        dlfactory.add_dlhandler(sofile);
        const dlhandler* ptr_dlh = dlfactory.get_dlhandler(sofile);
        if (ptr_dlh != NULL) {
            plugin_func_ptr_t* pptr_plugin_func;
            plugin_func_ptr_t ptr_plugin_func = NULL;
            pptr_plugin_func = &ptr_plugin_func;
            ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION), ptr_plugin_func);
            if (ptr_plugin_func != NULL) {
                cout << "dlsymbol:" << ptr_plugin_func << endl;
                ptr_plugin_func(source, source_count, destination, fields_idx, fields_count);
            } else {
                cout << "dlsymbol(" << CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION)
                        << ", " << pptr_plugin_func << "->" << ptr_plugin_func << ") return NULL!" << endl;
            }
            slice sli_("19.8888");
            source[0].data = sli_;
            source[0].type = cep::buffer_metadata::DOUBLE;
            slice sli2_("18.8888");
            source[1].data = sli2_;
            source[1].type = cep::buffer_metadata::DOUBLE;
            cout << "execute plug-in function:(" << source << ',' << source_count << ',' << &destination << ',' << fields_idx << ',' << fields_count << ')' << endl;
            ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_PROCESSING_EXEC_FUNCTION), ptr_plugin_func);
            if (ptr_plugin_func != NULL) {
                cout << "dlsymbol:" << ptr_plugin_func << endl;
                ptr_plugin_func(source, source_count, destination, fields_idx, fields_count);
            } else {
                cout << "dlsymbol(" << CEP_MACRO2CSTR(CEP_PROCESSING_EXEC_FUNCTION)
                        << ", " << pptr_plugin_func << "->" << ptr_plugin_func << ") return NULL!" << endl;
            }
        } else
            cout << "dlfactory.get_dlhandler(" << sofile << ") return NULL!" << endl;
    } else
        cout << "bufmet is [" << bufmet << "]" << __LINE__ << endl;
}

void TestCyclePlugin(char *arg) { // L
    cout << "*** begin of TestEveryPlugin[" << arg << "] ***" << endl;
    //假设目的事件2010，则2010的结构必须事先知道，方便测试

    const char *xmlfile = "cep_config.xml";
    cep::xml_document *doc = cep::load_xmlfile(xmlfile);
    if (!doc) {
        cout << "Load file [" << xmlfile << "] failure open xml file!" << endl;
        //todo xml from net
    } else {
        cout << "Load file [" << xmlfile << "] ***" << endl;
        const cep::xml_element *xml_temp = cep::root_element(*doc);
        xml_temp = cep::find_element(*xml_temp, "eventset");
        if (xml_temp != NULL)
            cep::config_metadata::instance().reload_metadata(*xml_temp);
        else
            return;
    }
    cout << "*** *** ***\nconfiguration for meta data is:\n"
            << cep::config_metadata::instance() << endl;
    cout << "*** end of TestLoadEvent[" << arg << "] ***" << endl;
    const buffer_metadata* bufmet = cep::config_metadata::instance().metadata(2000);
    if (bufmet != NULL) {
        event destination(bufmet);
        cout << "ok continue the line is [" << __LINE__ << "]" << endl;
        time_t sec = 20000000 / 1000;
        struct tm ptm;
        ptm.tm_year = 0;
        ptm.tm_mon = 0;
        ptm.tm_mday = 0;
        ptm.tm_hour = 0;
        ptm.tm_min = 0;
        ptm.tm_sec = 0;
        utils_time::localtime_r(&sec, &ptm);
        cout << "year is[" << ptm.tm_year << "]hour is[" << ptm.tm_hour << "]min is[" << ptm.tm_min << "]" << endl;
        size_t source_count = 1;
        type_slice source[source_count];
        slice sli("20000000");
        source[0].data = sli;
        source[0].type = cep::buffer_metadata::DATETIME;
        size_t fields_idx[3], fields_count;
        fields_idx[0] = 6;
        fields_idx[1] = 7;
        fields_idx[2] = 8;
        fields_count = 3;
        cout << "execute plug-in function:(" << source << ',' << source_count << ',' << &destination << ',' << fields_idx << ',' << fields_count << ')' << endl;
        dlhandler::factory dlfactory;
        typedef int* ptr_int_t;
        ptr_int_t *pptr_i;

        ptr_int_t ptr_i = NULL;
        pptr_i = &ptr_i;

        typedef void (*func_ptr_print_t)(const string&);
        func_ptr_print_t* pptr_func;
        func_ptr_print_t ptr_func = NULL;
        pptr_func = &ptr_func;
        const string sofile = "./libplugin_func4_cycle_A.so";
        dlfactory.add_dlhandler(sofile);
        const dlhandler* ptr_dlh = dlfactory.get_dlhandler(sofile);
        if (ptr_dlh != NULL) {
            plugin_func_ptr_t* pptr_plugin_func;
            plugin_func_ptr_t ptr_plugin_func = NULL;
            pptr_plugin_func = &ptr_plugin_func;
            ptr_dlh->dlsymbol(CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION), ptr_plugin_func);
            if (ptr_plugin_func != NULL) {
                cout << "dlsymbol:" << ptr_plugin_func << endl;
                ptr_plugin_func(source, source_count, destination, fields_idx, fields_count);
            } else {
                cout << "dlsymbol(" << CEP_MACRO2CSTR(CEP_PROCESSING_INIT_EXEC_FUNCTION)
                        << ", " << pptr_plugin_func << "->" << ptr_plugin_func << ") return NULL!" << endl;
            }
        } else
            cout << "dlfactory.get_dlhandler(" << sofile << ") return NULL!" << endl;
    } else
        cout << "bufmet is [" << bufmet << "]" << __LINE__ << endl;
}

void TestRangeIndex(char *arg) { // w
    cout << "*** begin of TestRangeIndex[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long nbr = 0;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            nbr = atoi(arg + 2);
    } else
        runMode = '0';

    const_dataset dataset;
    cep::buffer_metadata bm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("id", 0, cep::buffer_metadata::INT);
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("bip", 10, cep::buffer_metadata::IP);
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("eip", 20, cep::buffer_metadata::IP);
    bm.add_cfg_entry(entry);
    bm.rebuild();

    const_dataset::table *ip_table = dataset.insert("ip", bm);
    if (ip_table) {
        ip_table->set_idx(10);
        // 表至少能装3条记录
        ip_table->reserve(20);

        dynamic_struct * ds = NULL;
        //                                      0   1   2   3   4   5  6   7   8   9   10   11 
        cep::buffer_metadata::enum_t bip[12] = {10, 15, 40, 46, 2, 60, 35, 40, 68, 15, 100, 78};
        cep::buffer_metadata::enum_t dip[12] = {20, 46, 50, 80, 10, 70, 50, 45, 90, 80, 125, 89};

        for (int i = 0; i < 12; ++i) {
            ds = new dynamic_struct(&bm);
            ds->set_value(0, i * 10);
            ds->set_value(1, bip[i]);
            ds->set_value(2, dip[i]);

            // 连续插入12条表记录
            if (!ip_table->insert(ds)) {
                delete ds;
                ds = NULL;
            }
        }
    }

    // idx_range(const string& id, const std::vector<string>& keys, const const_dataset::table& tab)
    vector<string> keys;
    keys.push_back("bip");
    keys.push_back("eip");

    const_dataset::index * ip_table_index = new cep::idx_range("ip_table_index", keys, *ip_table);
    MLOG_INFO << "Begin to build ip_table_index..." << endl;
    if (!ip_table_index->rebuild()) {
        delete ip_table_index;
        return;
    }
    dataset.insert(ip_table_index);

    ip_table_index = dataset.get_index("ip_table_index");

    struct DatasetIterator : public const_dataset::index::iterator {
        int counter;

        bool operator()(const dynamic_struct& data, const size_t & rownum) {
            cout << "rownum: " << rownum << '>' << data.value("bip", 0) << "--->" << data.value("eip", 0) << endl;
            // cout << "rownum: "<< rownum << '>' << data << endl;
            ++counter;
            return true;
        }
    };

    DatasetIterator di;
    // virtual void select(const dynamic_struct& row, const_dataset::index::iterator& func) const;
    dynamic_struct* row = new dynamic_struct(&bm);

    /*
//                                      0   1   2   3   4   5  6   7   8   9   10   11 
cep::buffer_metadata::enum_t bip[12] = {10, 15, 40, 46, 2, 60, 35, 40, 68, 15, 100, 78};
cep::buffer_metadata::enum_t dip[12] = {20, 46, 50, 80, 10, 70, 50, 45, 90, 80, 125, 89};
     */
#include <stdlib.h>
    int count = 0, bb = 0;
    cep::type_slice typeSlice[1];
    char bchar[buffer_metadata::SIZEOF_IP];
    while (count++ < 10) {
        bb = rand() % 200;
        cout << "begin to find bip: " << bb << endl;
        buff_util::set_value<unsigned int>(bchar, 0, bb);
        typeSlice[0].data = cep::slice(bchar, sizeof (bchar));
        typeSlice[0].type = cep::buffer_metadata::IP;
        // row->set_value("bip", bb);
        ip_table_index->select(typeSlice, di);
    }
    cout << "*** end of TestRangeIndex[" << arg << "] ***" << endl;
}

void TestTrieIndex(char *arg) { // x
    cout << "*** begin of TestTrieIndex[" << arg << "] ***" << endl;
    char runMode = *(arg + 1);
    unsigned long nbr = 0;
    if (runMode != '\0') {
        if (*(arg + 2) != '\0')
            nbr = atoi(arg + 2);
    } else
        runMode = '0';

    // 创建数据集
    const_dataset dataset;
    cep::buffer_metadata bm;
    // code, idx, type, length, flag
    cep::field_cfg_entry *entry = new cep::field_cfg_entry("id", 0, cep::buffer_metadata::INT);
    bm.add_cfg_entry(entry);
    entry = new cep::field_cfg_entry("url", 10, cep::buffer_metadata::STR, 20);
    bm.add_cfg_entry(entry);
    bm.rebuild();

    const_dataset::table *url_table = dataset.insert("urltest", bm);
    if (url_table) {
        url_table->set_idx(10);
        // 表至少能装3条记录
        url_table->reserve(20);

        dynamic_struct * ds = NULL;
        string urls[9] = {"*.qq.com", "*.baidu.com", "*.sina.com", "*.sina.com.cn", "*sina.com", "*.google.com.hk", "*.google.com", "*.csdn.com"
            "*.wikipedia.org"};

        for (int i = 0; i < 9; ++i) {
            ds = new dynamic_struct(&bm);
            ds->set_value(0, i * 10);
            cep::slice slc(urls[i]);
            ds->set_value(1, slc);
            // 连续插入12条表记录
            if (!url_table->insert(ds)) {
                delete ds;
                ds = NULL;
            }
        }
    }
    cout << *url_table << endl;

    // 开始创建索引  idx_trie_tree(const string& id, const std::vector<string>& keys, const const_dataset::table& tab)
    std::vector<string> keys;
    keys.push_back("url");
    idx_trie_tree itt("testTrieidx", keys, *url_table, true);
    if (!itt.rebuild()) {
        MLOG_INFO << "can not build the index testTrieidx " << endl;
        return;
    }

    struct DatasetIterator : public const_dataset::index::iterator {
        int counter;
        cep::slice slc;

        bool operator()(const dynamic_struct& data, const size_t & rownum) {
            cout << "rownum: " << rownum << '>' << data.value("id", 0) << "--->" << data.value("url", slc).to_string() << endl;
            // cout << "rownum: "<< rownum << '>' << data << endl;
            ++counter;
            return true;
        }
    };

    DatasetIterator di;

    cep::type_slice typeSlice[1];
    //    typeSlice[0].data = cep::slice(" www.sina.com.cn");
    typeSlice[0].data = cep::slice("news.sina.com");
    typeSlice[0].type = cep::buffer_metadata::STR;
    itt.select(typeSlice, di);
    cout << "*** end of TestTrieIndex[" << arg << "] ***" << endl;
}

unsigned long sys_cstr2milsecond(const char *date, const char *format) {
    assert(date != 0 && format != 0);
    char point[4];
    const char * p = NULL;
    memset(point, '0', sizeof (point));
    point[3] = '\0';
    struct tm stt;
    memset(&stt, 0, sizeof (stt));
    unsigned int milsecond = 0;
    sscanf(date, format, &stt.tm_year, &stt.tm_mon, &stt.tm_mday,
            &stt.tm_hour, &stt.tm_min, &stt.tm_sec);
    stt.tm_year -= 1900;
    stt.tm_mon -= 1;
    // long rt = utils_time::mktime(&stt)*1000;
    long rt = mktime(&stt)*1000;
    p = strstr(format, ".");
    if (!p)
        return rt;
    p = strstr(date, ".");
    if (p)
        ++p;
    else
        return rt;
    if (p) {
        for (int i = 0; i < 3; ++i) {
            if (*p != '\0') {
                point[i] = *p;
            } else {
                break;
            }
            ++p;
        }
    }
    milsecond = atoi(point);
    return rt + milsecond;
}

string sys_milsecond2str(long milsecond, const char *format) {
    unsigned int mils = milsecond % 1000;
    long second = milsecond / 1000;
    struct tm ptm;
    ptm.tm_year = 0;
    ptm.tm_mon = 0;
    ptm.tm_mday = 0;
    ptm.tm_hour = 0;
    ptm.tm_min = 0;
    ptm.tm_sec = 0;
    // utils_time::localtime_r(&second, &ptm);
    localtime_r(&second, &ptm);
    char buffer[32] = {0};
    sprintf(buffer, format, ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec, mils);
    return string(buffer);
}

void TestSystime(int argc, char** argv) { // y
    cout << "*** begin of TestSystime ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }
    cout << "test _CEPTIMEZONE[" << _CEPTIMEZONE
            << "] usage: [0]cepframe [1]y [2]mode [3]%4d-%02d-%02d_%02d:%02d:%02d"
            << " [4]cycle(minute)" << endl;
    if (argc < 5) {
        if (argc > 3) {
            char runMode = *(argv + 2)[0];
            const char* strtime = *(argv + 3); // "1970-01-01 23:00:00";

            int cycle = 24 * 60 * 60000;
            int delay = 8 * 60;
            time_t currdelay;

            time_t sysms = cep::cstr2milsecond(strtime, "%4d-%02d-%02d_%02d:%02d:%02d");
            time_t win4sys = sysms / cycle;
            // if (win4time > win4sys) {
            currdelay = sysms / 1000 + delay;
            cout << "if (win4time > win4sys):" << milsecond2str(currdelay * 1000, "%4d-%02d-%02d %02d:%02d:%02d") << endl;
            // } else {
            currdelay = (win4sys + 1) * cycle / 1000 + delay;
            cout << "else:" << milsecond2str(currdelay * 1000, "%4d-%02d-%02d %02d:%02d:%02d") << endl;

            cout << "timezone:" << timezone << endl;
            tzset();
            // printf("current_timet: %d, timezone info:[%d / %d]\n",
            //         current_timet, timezone, daylight);
            cout << "after tzset() call, timezone(second):" << timezone << endl;
            long diff4timezone = timezone;

            sysms = sysms - diff4timezone * 1000; // UTC to local time
            win4sys = sysms / cycle;
            // if (win4time > win4sys) {
            // currdelay = sysms / 1000 + delay;
            // cout << "if (win4time > win4sys):" << milsecond2str(currdelay * 1000, "%4d-%02d-%02d %02d:%02d:%02d") << endl;
            // } else {
            currdelay = (win4sys + 1) * cycle / 1000 + delay + diff4timezone; // local time to UTC
            cout << "with time zone difference:" << milsecond2str(currdelay * 1000, "%4d-%02d-%02d %02d:%02d:%02d") << endl;
            // }
        } else {
            const long now = cep::processing_utils::systime_msec();
            string ceptime = milsecond2str(now, "%4d-%02d-%02d %02d:%02d:%02d");

            unsigned int mils = now % 1000;
            long second = now / 1000;
            struct tm ptm;
            ptm.tm_year = 0;
            ptm.tm_mon = 0;
            ptm.tm_mday = 0;
            ptm.tm_hour = 0;
            ptm.tm_min = 0;
            ptm.tm_sec = 0;
            localtime_r(&second, &ptm);
            char buffer[32] = {0};
            sprintf(buffer, "%4d-%02d-%02d %02d:%02d:%02d", ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec, mils);

            cout << now << "->ceptime[" << ceptime << "]" << buffer << '!' << endl;
        }
    } else {
        char runMode = *(argv + 2)[0];
        const char* strtime = *(argv + 3); // "1970-01-01 23:00:00";
        int cycle = atoi(*(argv + 4)); // 1440
        if (runMode == 's') {
            time_t sec = sys_cstr2milsecond(strtime, "%4d-%02d-%02d_%02d:%02d:%02d") / 1000;
            cout << "second is:" << sec << endl;
            sec = sec / (cycle * 60);
            sec = sec * cycle * 60;
            cout << "after cycle second is:" << sec << endl;
            struct tm ptm;
            ptm.tm_year = 0;
            ptm.tm_mon = 0;
            ptm.tm_mday = 0;
            ptm.tm_hour = 0;
            ptm.tm_min = 0;
            ptm.tm_sec = 0;
            localtime_r(&sec, &ptm);
            string strtime2 = sys_milsecond2str(sec * 1000, "%4d-%02d-%02d_%02d:%02d:%02d");
            cout << strtime << "->" << sec << "->" << strtime2 << endl;
            cout << ptm.tm_year << '-' << ptm.tm_mon << '-' << ptm.tm_mday << ' '
                    << ptm.tm_hour << ':' << ptm.tm_min << ':' << ptm.tm_sec << endl;
        } else {
            time_t sec = cep::cstr2milsecond(strtime, "%4d-%02d-%02d_%02d:%02d:%02d") / 1000;
            cout << "second is:" << sec << endl;
            sec = sec / (cycle * 60);
            sec = sec * cycle * 60;
            cout << "after cycle second is:" << sec << endl;
            struct tm ptm;
            ptm.tm_year = 0;
            ptm.tm_mon = 0;
            ptm.tm_mday = 0;
            ptm.tm_hour = 0;
            ptm.tm_min = 0;
            ptm.tm_sec = 0;
            utils_time::localtime_r(&sec, &ptm);
            string strtime2 = milsecond2str(sec * 1000, "%4d-%02d-%02d_%02d:%02d:%02d");
            cout << strtime << "->" << sec << "->" << strtime2 << endl;
            cout << ptm.tm_year << '-' << ptm.tm_mon << '-' << ptm.tm_mday << ' '
                    << ptm.tm_hour << ':' << ptm.tm_min << ':' << ptm.tm_sec << endl;
        }
    }
    cout << "*** end of TestSystime ***" << endl;
}

#ifdef __VER4ON_INSTALL_LEVELDB__
#include <snappy.h>
#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include "leveldb/write_batch.h"

class LongComparator : public leveldb::Comparator {
public:
    // Three-way comparison function:
    //   if a < b: negative result
    //   if a > b: positive result
    //   else: zero result

    int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
        return *((long*) a.data()) - *((long*) b.data());
    }

    // Ignore the following methods for now:

    const char* Name() const {
        return "LongComparator";
    }

    void FindShortestSeparator(std::string*, const leveldb::Slice&) const {
    }

    void FindShortSuccessor(std::string*) const {
    }
};

void TestSnappy(int argc, char** argv) { // z
    cout << "*** begin of TestSnappy ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }
    if (argc < 7) {
        cout << "usage: [0]cepframe [1]z [2]mode [3]event_conf [4]event_type"
                " [5]source_file [6]target_file" << endl;
        return;
    }

    char runMode = *(argv + 2)[0];
    const char* event_conf = *(argv + 3);
    int event_type = atoi(*(argv + 4));
    const char* source_file = *(argv + 5);
    const char* target_file = *(argv + 6);

    cout << "run:" << runMode << ',' << event_conf << ',' << event_type << ','
            << source_file << ',' << target_file << '!' << endl;

    cout << "Start load event type configuration:" << event_conf << endl;
    cep::xml_document *doc = cep::load_xmlfile(event_conf);
    if (!doc) {
        cout << "Load event configuration file[" << event_conf << "] failure!" << endl;
        return;
    } else {
        const cep::xml_element *xml_temp = cep::root_element(*doc);
        if (xml_temp == NULL) {
            cout << "Can not find " << event_conf << " root!" << endl;
            return;
        }
        xml_temp = cep::find_element(*xml_temp, "eventset");
        if (xml_temp != NULL) {
            if (cep::config_metadata::instance().reload_metadata(*xml_temp))
                cout << "configuration for event meta data is:\n"
                    << cep::config_metadata::instance() << endl;
            else {
                cout << "Load event configuration file[" << event_conf << "] failure!" << endl;
                return;
            }
        } else {
            cout << "Can not find eventset!" << endl;
            return;
        }
    }
    delete doc;

    buffer_metadata* metadata = config_metadata::instance().metadata(event_type);
    event eventmsg(metadata, event_type);
    event2text eventread;
    string alltxt;
    FILE* cpsfile;
    if (runMode == '0' || runMode == '1' || runMode == '2') {
        cpsfile = fopen(target_file, "w");
        if (cpsfile == NULL) {
            cout << "open file failure:" << target_file << endl;
            return;
        }
    }

    leveldb::DB* db = NULL;
    leveldb::Status s;
    leveldb::WriteOptions wo;
    leveldb::ReadOptions ro;
    LongComparator cmp;

    if (runMode == '3') {
        leveldb::Options options;
        options.create_if_missing = true;
        options.compression = leveldb::kNoCompression;
        options.comparator = &cmp;
        s = leveldb::DB::Open(options, target_file, &db);
        assert(s.ok());
    } else if (runMode == '4') {
        leveldb::Options options;
        options.create_if_missing = true;
        options.compression = leveldb::kSnappyCompression;
        options.comparator = &cmp;
        s = leveldb::DB::Open(options, target_file, &db);
        assert(s.ok());
    } else if (runMode == '5' || runMode == '6') {
        leveldb::Options options;
        // options.create_if_missing = true;
        if (runMode == '5') options.compression = leveldb::kNoCompression;
        else options.compression = leveldb::kSnappyCompression;
        options.comparator = &cmp;
        s = leveldb::DB::Open(options, source_file, &db);
        assert(s.ok());

        if (!eventread.open(target_file, "w")) {
            cout << "cant open output file:" << target_file << endl;
            return;
        }

        leveldb::Iterator* iter = db->NewIterator(ro);
        long counter = 0;
        time_t begin = cep::processing_utils::systime_msec();
        for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
            ++counter;
            // cout << "> " << *((long *) iter->key().data()) << endl;

            // leveldb::Slice slc = iter->value();
            // char* dat = new char[slc.size()];
            // memcpy(dat, slc.data(), slc.size());
            // eventmsg2.set_metadata(metadata);
            slice slc(iter->value().data(), iter->value().size());
            eventmsg.set_value(0, slc.size(), slc);
            eventread.write(eventmsg);
        }
        time_t end = cep::processing_utils::systime_msec();
        cout << "****\n iterator and write " << counter << " events used:"
                << (end - begin) << " msec!" << endl;
        assert(iter->status().ok()); // Check for any errors found during the scan
        delete iter;
        return;
    }

    if (eventread.open(source_file, "r")) {
        cout << "opened file:" << source_file << endl;
        long counter = 0;
        time_t begin = cep::processing_utils::systime_msec();
        while (!eventread.is_end()) {
            eventread.read(eventmsg);
            ++counter;
            if (runMode == '0') {
                string txt;
                eventread.to_string(eventmsg, txt);
                string cpstxt;
                snappy::Compress(txt.c_str(), txt.size(), &cpstxt);
                string uncpstxt;
                snappy::Uncompress(cpstxt.c_str(), cpstxt.size(), &uncpstxt);
                cout << counter << "\t<" << txt << endl;
                cout << counter << "\t<" << uncpstxt << endl;
            } else if (runMode == '1') {
                eventread.to_string(eventmsg, alltxt);
            } else if (runMode == '2') {
                string txt;
                eventread.to_string(eventmsg, txt);
                string cpstxt;
                snappy::Compress(txt.c_str(), txt.size(), &cpstxt);
                fwrite(cpstxt.c_str(), 1, cpstxt.size(), cpsfile);
            } else if (runMode == '3' || runMode == '4') {
                leveldb::Slice slc(eventmsg.data(), eventmsg.len());
                s = db->Put(wo, leveldb::Slice((char*) &counter, sizeof (long)), slc);
            }
        }
        time_t end = cep::processing_utils::systime_msec();
        cout << "****\n read and load " << counter << " events used:"
                << (end - begin) << " msec!" << endl;
    } else {
        cout << "cant open input file:" << source_file << endl;
        return;
    }

    if (runMode == '1') {
        cout << "alltxt size is:" << alltxt.size() << endl;
        string cpstxt;
        size_t cpsize = snappy::Compress(alltxt.c_str(), alltxt.size(), &cpstxt);
        int retval = fwrite(cpstxt.c_str(), 1, cpstxt.size(), cpsfile);
        cout << "cpstxt size is:" << cpsize << "->" << cpstxt.size() << " fwrite:" << retval << endl;
        fflush(cpsfile);
        fclose(cpsfile);
    } else if (runMode == '2') {
        fflush(cpsfile);
        fclose(cpsfile);
    }

    if (db != NULL) delete db; // shutdown the database
    cout << "*** end of TestSnappy ***" << endl;
}
#endif // __VER4ON_INSTALL_LEVELDB__

#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/asio.hpp>

class TestFileLockThread {
public:
    const char* filename;
    int count, interval;

    void working() {
        for (int i = 0; i < count; ++i) {
            file_lock_counter<int> flc;
            int cnt = interval;
            bool rf = flc.counting(filename, cnt);
            if (!rf) cout << "failure count is:" << cnt << endl;
        }
    }
};

void TestFileLock(int argc, char** argv) { // A
    cout << "*** begin of TestFileLock ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }
    if (argc < 5) {
        cout << "usage: [0]cepframe [1]A [2]mode [3]lock_file_name [4]sleep seconds/counter" << endl;
        return;
    }

    char runMode = *(argv + 2)[0];
    const char* fileName = *(argv + 3);
    int wait = atoi(*(argv + 4));

    using namespace boost::interprocess;
    if (runMode == 's') {
        file_lock_counter<int> flc;
        cout << "reset return:" << flc.reset(fileName, wait) << endl;
    } else if (runMode == 'c') {
        file_lock_counter<int> flc;
        int count = -1;
        bool rf = flc.counting(fileName, count);
        cout << (rf ? "success" : "failure") << " count = " << count << endl;
    } else if (runMode == 'T') {
        if (argc < 7) {
            cout << "usage: [0]cepframe [1]A [2]T [3]lock_file_name"
                    " [4]thread count [5]loop count [6]interval" << endl;
            return;
        }
        int loopCount = atoi(*(argv + 5));
        int interval = atoi(*(argv + 6));

        TestFileLockThread tflt;
        tflt.filename = fileName;
        tflt.count = loopCount;
        tflt.interval = interval;

        boost::thread_group tg;
        for (int i = 0; i < wait; ++i)
            tg.create_thread(boost::bind(&TestFileLockThread::working, &tflt));
        tg.join_all();
    } else {
        // Open a file for update both reading and writing. The file must exist.
        FILE* pFile = fopen(fileName, "r+");
        if (pFile == NULL) {
            cout << "create new file:" << fileName << endl;
            // Create an empty file for both reading and writing.
            // If a file with the same name already exists its content is erased
            // and the file is treated as a new empty file.
            pFile = fopen(fileName, "w+");
            if (pFile == NULL) {
                perror("Error opening file");
                return;
            }
        }
        if (runMode == 'p') {
            try {
                file_lock flock(fileName);
                scoped_lock<file_lock> e_lock(flock);
                if (wait > 0) sleep(wait);
                // char buffer[sizeof (int) ] = {0};
                // *((int*) buffer) = 12;
                int count = 0;
                // fread(&count, 1, sizeof (int), pFile);
                char buffer[16] = {0};
                fread(buffer, 1, sizeof (buffer), pFile);
                count = cep::cstr2nbr(buffer, count);

                cout << "count = " << count << endl;
                // If successful, the function returns a zero value.
                // Otherwise, it returns nonzero value.
                // fseek(pFile, 0, SEEK_SET);
                rewind(pFile);
                ++count;
                // fwrite(&count, 1, sizeof (int), pFile);
                memset(buffer, 0, sizeof (buffer));
                sprintf(buffer, "%016d", count);
                fwrite(buffer, 1, sizeof (buffer), pFile);

                fclose(pFile);
            } catch (interprocess_exception& e) {
                cout << e.what() << endl;
            }
        } else {
            try {
                file_lock flock(fileName);
                scoped_lock<file_lock> e_lock(flock);
                if (wait > 0) sleep(wait);
                // char buffer[sizeof (int) ] = {0};
                // *((int*) buffer) = 12;
                int count = 0;
                fread(&count, 1, sizeof (int), pFile);
                count = boost::asio::detail::socket_ops::host_to_network_long(count);
                cout << "count = " << count << endl;
                // If successful, the function returns a zero value.
                // Otherwise, it returns nonzero value.
                // fseek(pFile, 0, SEEK_SET);
                rewind(pFile);
                ++count;
                count = boost::asio::detail::socket_ops::network_to_host_long(count);
                fwrite(&count, 1, sizeof (int), pFile);
                fclose(pFile);
            } catch (interprocess_exception& e) {
                cout << e.what() << endl;
            }
        }
    }
    cout << "*** end of TestFileLock ***" << endl;
}

void set_value_datatime4hw(event& eventmsg, size_t index, const string& value) {
    // unsigned long temp = cstr2milsecond(value.c_str());
    ulong temp = 0;
    temp = cep::cstr2nbr(value.c_str(), temp) * 1000;
    eventmsg.set_value(index, temp);
}

string get_value_datatime4hw(const event &eventmsg, size_t index) {
    unsigned long temp = eventmsg.value(index, (unsigned long) 0);
    // return milsecond2str(temp);
    return cep::nbr2str(temp / 1000);
}

#ifdef __VER4ON_INSTALL_LEVELDB__

void TestLevelDB(int argc, char** argv) { // B
    cout << "*** begin of TestLevelDB ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }
    if (argc < 3) {
        cout << "usage: [0]cepframe [1]B [2]runMode is must exists!" << endl;
        return;
    }
    char runMode = *(argv + 2)[0];

    leveldb::DB* db = NULL;
    leveldb::Status s;
    leveldb::WriteOptions wo;
    leveldb::ReadOptions ro;
    LongComparator cmp;

    if (runMode == '0') {
        if (argc < 4) {
            cout << "usage: [0]cepframe [1]B [2]0 [3]target_file" << endl;
            return;
        }
        const char* target_file = *(argv + 3);

        leveldb::Options options;
        options.create_if_missing = true;
        options.compression = leveldb::kSnappyCompression;
        options.comparator = &cmp;
        s = leveldb::DB::Open(options, target_file, &db);
        assert(s.ok());
        long key = 0, counter = 0;
        s = db->Put(wo, leveldb::Slice((char*) &key, sizeof (long)), leveldb::Slice((char*) &counter, sizeof (long)));
        if (s.ok()) cout << "Put [" << key << "]=" << counter << std::endl;
        else cout << "Pet[" << key << "]=" << counter << " failure!" << std::endl;
        ++counter;
        s = db->Put(wo, leveldb::Slice((char*) &key, sizeof (long)), leveldb::Slice((char*) &counter, sizeof (long)));
        if (s.ok()) cout << "Put [" << key << "]=" << counter << std::endl;
        else cout << "Pet[" << key << "]=" << counter << " failure!" << std::endl;
    } else if (runMode == 'L') {
        if (argc < 4) {
            cout << "usage: [0]cepframe [1]B [2]L [3]target_file" << endl;
            return;
        }
        const char* target_file = *(argv + 3);

        leveldb::Options options;
        options.create_if_missing = true;
        options.compression = leveldb::kSnappyCompression;
        options.comparator = &cmp;
        s = leveldb::DB::Open(options, target_file, &db);
        assert(s.ok());

        leveldb::Iterator* iter = db->NewIterator(ro);
        long counter = 0;
        time_t begin = cep::processing_utils::systime_msec();
        for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
            ++counter;
            /*slice slc(iter->value().data(), iter->value().size());
            eventmsg.set_value(0, slc.size(), slc);
            eventread.write(eventmsg);*/
            cout << cep::buff_util::to_hex_string(iter->key().data(), iter->key().size()) << '='
                    << cep::buff_util::to_hex_string(iter->value().data(), iter->value().size()) << endl;
        }
        time_t end = cep::processing_utils::systime_msec();
        cout << "****\n iterator and write " << counter << " events used:"
                << (end - begin) << " msec!" << endl;
        assert(iter->status().ok()); // Check for any errors found during the scan
        delete iter;
    } else if ((runMode >= '1' && runMode <= '9') || (runMode >= 'a' && runMode <= 'z')) {
        if (argc < 8) {
            cout << "usage: [0]cepframe [1]B [2]1-9|a-z [3]event_conf [4]event_type"
                    " [5]source_file [6]target_file [7]batch_size" << endl;
            return;
        }
        const char* event_conf = *(argv + 3);
        const int event_type = atoi(*(argv + 4));
        const char* source_file = *(argv + 5);
        const char* target_file = *(argv + 6);
        const int batch_size = atoi(*(argv + 7));

        cout << "Start load event type configuration:" << event_conf << endl;
        cep::xml_document *doc = cep::load_xmlfile(event_conf);
        if (!doc) {
            cout << "Load event configuration file[" << event_conf << "] failure!" << endl;
            return;
        } else {
            const cep::xml_element *xml_temp = cep::root_element(*doc);
            if (xml_temp == NULL) {
                cout << "Can not find " << event_conf << " root!" << endl;
                return;
            }
            xml_temp = cep::find_element(*xml_temp, "eventset");
            if (xml_temp != NULL) {
                if (cep::config_metadata::instance().reload_metadata(*xml_temp))
                    cout << "configuration for event meta data is:\n"
                        << cep::config_metadata::instance() << endl;
                else {
                    cout << "Load event configuration file[" << event_conf << "] failure!" << endl;
                    return;
                }
            } else {
                cout << "Can not find eventset!" << endl;
                return;
            }
        }
        delete doc;

        buffer_metadata* metadata = config_metadata::instance().metadata(event_type);
        event eventmsg(metadata, event_type);
        vector<event*> events;
        event2text eventread;

        if ((runMode >= '1' && runMode <= '8') || runMode == 'b') {
            if (eventread.open(source_file, "r")) {
                cout << "opened file:" << source_file << endl;
                long counter = 0;
                time_t begin = cep::processing_utils::systime_msec();
                while (!eventread.is_end()) {
                    event* newEvent = new event(metadata, event_type);
                    events.push_back(newEvent);
                    eventread.read(*newEvent);
                    ++counter;
                }
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** read and load " << counter << " events used:"
                        << (end - begin) << " msec!" << endl;
            } else {
                cout << "cant open input file:" << source_file << endl;
                return;
            }
            const size_t size = events.size();
            if (runMode == '1' || runMode == '2') {
                leveldb::Options options;
                options.create_if_missing = true;
                options.compression = leveldb::kSnappyCompression;
                options.comparator = &cmp;
                s = leveldb::DB::Open(options, target_file, &db);
                assert(s.ok());
                if (runMode == 1) {
                    time_t begin = cep::processing_utils::systime_msec();
                    for (int i = 0; i < size; ++i) {
                        leveldb::Slice slc(events[i]->data(), events[i]->len());
                        s = db->Put(wo, leveldb::Slice((char*) &i, sizeof (int)), slc);
                    }
                    delete db;
                    time_t end = cep::processing_utils::systime_msec();
                    cout << "**** Put " << size << " events used:"
                            << (end - begin) << " msec!" << endl;
                } else {
                    leveldb::WriteBatch* batch = new leveldb::WriteBatch;
                    time_t begin = cep::processing_utils::systime_msec();
                    int i = 0;
                    for (; i < size; ++i) {
                        leveldb::Slice slc(events[i]->data(), events[i]->len());
                        batch->Put(leveldb::Slice((char*) &i, sizeof (int)), slc);
                        if (i != 0 && (i % batch_size) == 0) {
                            s = db->Write(wo, batch);
                            delete batch;
                            batch = new leveldb::WriteBatch;
                        }
                    }
                    if ((i % batch_size) != 0) s = db->Write(wo, batch);
                    delete batch;
                    delete db;
                    time_t end = cep::processing_utils::systime_msec();
                    cout << "**** Batch Put " << size << " events used:"
                            << (end - begin) << " msec!" << endl;
                }
            } else if (runMode == '3') {
                eventread.open(target_file, "w");
                time_t begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < size; ++i) {
                    eventread.write(*events[i]);
                }
                eventread.close();
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** event2text write " << size << " events used:"
                        << (end - begin) << " msec!" << endl;
            } else if (runMode == '4') {
                LZOFileWriter lzofw;
                lzofw.Init(fopen(target_file, "w"));
                time_t begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < size; ++i) {
                    int cnt = fwrite(events[i]->data(), events[i]->len(), 1, lzofw);
                    assert(cnt != -1);
                }
                lzofw.Close();
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** LZOFileWriter fwrite " << size << " events used:"
                        << (end - begin) << " msec!" << endl;
            } else if (runMode == '5') {
                LZOFileWriter lzofw;
                lzofw.Init(fopen(target_file, "w"), BLOCKSIZE, COMPRESS_LEVEL_MAX);
                time_t begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < size; ++i) {
                    int cnt = fwrite(events[i]->data(), events[i]->len(), 1, lzofw);
                    assert(cnt != -1);
                }
                lzofw.Close();
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** LZOFileWriter COMPRESS_LEVEL_MAX fwrite " << size << " events used:"
                        << (end - begin) << " msec!" << endl;
            } else if (runMode == '6') {
                LZOFileWriter lzofw;
                lzofw.Init(fopen(target_file, "w"), BLOCKSIZE, COMPRESS_LEVEL_NO_COMPRESS);
                time_t begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < size; ++i) {
                    int cnt = fwrite(events[i]->data(), events[i]->len(), 1, lzofw);
                    assert(cnt != -1);
                }
                lzofw.Close();
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** LZOFileWriter COMPRESS_LEVEL_NO_COMPRESS fwrite " << size
                        << " events used:" << (end - begin) << " msec!" << endl;
            } else if (runMode == '7') {
                eventread.open(target_file, "w");
                time_t begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < batch_size; ++i) {
                    for (int j = 0; j < size; ++j)
                        eventread.write(*events[j]);
                }
                eventread.close();
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** event2text write " << size << " events "
                        << batch_size << " times used:"
                        << (end - begin) << " msec!" << endl;
            } else if (runMode == '8') {
                time_t begin = cep::processing_utils::systime_msec();
                for (int j = 0; j < batch_size; ++j) {
                    char file_name[128] = {0};
                    sprintf(file_name, "%s_%d", target_file, j);
                    eventread.open(file_name, "w");
                    for (int i = 0; i < size; ++i) {
                        eventread.write(*events[i]);
                    }
                    eventread.close();
                }
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** event2text write " << size << " events to "
                        << batch_size << " files used:"
                        << (end - begin) << " msec!" << endl;
            } else if (runMode == 'b') {
                event2text eventread4hw(0);
                eventread4hw.get_values[buffer_metadata::DATETIME] = &get_value_datatime4hw;
                eventread4hw.open(target_file, "w");
                time_t begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < size; ++i) {
                    eventread4hw.write(*events[i]);
                }
                eventread4hw.close();
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** event2text write " << size << " events to HW format used:"
                        << (end - begin) << " msec!" << endl;
            } else {
                cout << "Unsupported test mode[" << runMode << "] run 0-9" << endl;
            }
        } else if (runMode == '9') {
            LZOFileReader lzofr;
            lzofr.Init(fopen(source_file, "r"));
            time_t begin = cep::processing_utils::systime_msec();
            char buff[metadata->size_of()]; // = {0}; // equals memset
            long counter = 0;
            long offset = 0;
            while (!fEof(lzofr)) {
                memset(buff, 0, sizeof (buff));
                int flag = fread(buff, sizeof (buff), 1, lzofr);
                assert(flag != -1);
                event* newEvent = new event(metadata, event_type);
                events.push_back(newEvent);
                slice slc(buff, sizeof (buff));
                newEvent->set_value(0, slc.size(), slc);
                offset += sizeof (buff);
                if (batch_size == 0)
                    fseek(lzofr, offset, 0);
                else if (batch_size == 1) // error!
                    fseek(lzofr, 0, offset);
                // else // content error!
                ++counter;
            }
            lzofr.Close();
            time_t end = cep::processing_utils::systime_msec();
            cout << "**** LZOFileWriter fread " << counter << " events used:"
                    << (end - begin) << " msec!" << endl;

            const size_t size = events.size();
            eventread.open(target_file, "w");
            begin = cep::processing_utils::systime_msec();
            for (int i = 0; i < size; ++i) {
                eventread.write(*events[i]);
            }
            eventread.close();
            end = cep::processing_utils::systime_msec();
            cout << "**** event2text write " << size << " events used:"
                    << (end - begin) << " msec!" << endl;
        } else if (runMode == 'a') {
            event2text eventread4hw(0);
            eventread4hw.set_values[buffer_metadata::DATETIME] = &set_value_datatime4hw;
            if (eventread4hw.open(source_file, "r")) {
                cout << "opened file:" << source_file << endl;
                long counter = 0;
                time_t begin = cep::processing_utils::systime_msec();
                while (!eventread4hw.is_end()) {
                    event* newEvent = new event(metadata, event_type);
                    events.push_back(newEvent);
                    eventread4hw.read(*newEvent);
                    ++counter;
                }
                time_t end = cep::processing_utils::systime_msec();
                cout << "**** read and load " << counter << " events from HW format used:"
                        << (end - begin) << " msec!" << endl;
            } else {
                cout << "cant open input file:" << source_file << endl;
                return;
            }

            const size_t size = events.size();
            eventread.open(target_file, "w");
            time_t begin = cep::processing_utils::systime_msec();
            for (int i = 0; i < size; ++i) {
                eventread.write(*events[i]);
            }
            eventread.close();
            time_t end = cep::processing_utils::systime_msec();
            cout << "**** event2text write " << size << " events used:"
                    << (end - begin) << " msec!" << endl;
        } else
            cout << "Unsupported test inner mode[" << runMode << ']' << endl;
    } else {
        cout << "Unsupported test mode[" << runMode << ']' << endl;
    }
    cout << "*** end of TestLevelDB ***" << endl;
}
#endif // __VER4ON_INSTALL_LEVELDB__

void TestEventFileHandler(int argc, char**argv) { // W
    cep::xml_document *doc = cep::load_xmlfile("event.xml");
    if (!doc) {
        cout << "Load file [event.xml] failure, Abort CEP!" << endl;
        return;
    } else {
        const cep::xml_element *xml_temp = cep::root_element(*doc);
        if (xml_temp == NULL) {
            cout << "Can not find event.xml root, Abort CEP!" << endl;
            return;
        }
        xml_temp = cep::find_element(*xml_temp, "eventset");
        if (xml_temp != NULL) {
            if (cep::config_metadata::instance().reload_metadata(*xml_temp))
                cout << "configuration for event meta data is:\n"
                    << cep::config_metadata::instance() << endl;
            else {
                cout << "Load file [ event.xml] failure, Abort CEP!" << endl;
                return;
            }
        } else {
            cout << "Can not find eventset, Abort CEP!" << endl;
            return;
        }
    }
    delete doc;
    FilepathScanner pathScanner;
    pathScanner.setScanCmdCycle(60);
    pathScanner.setScanCmdEventType(2000);
    FilepathScanner::Config4ReadCmdFilepath configentry;
    configentry.cmdType = 999;
    /*
     * 超时60s
     */
    pathScanner.setMSTimeOut(60 * 60 * 1000);
    configentry.src = "/ztesoft/cmd_wg/1000";
    configentry.archive = "/ztesoft/archive/";
    pathScanner.cmdFilePath().push_back(configentry);

    FilepathScanner::cmd_map_t map;
    pathScanner.scanning(map);
    cout << "loadedCmd.size:" << pathScanner.loadedCmds().size() << endl;
    FilepathScanner::cmd_map_t::iterator iter = map.begin();
    for (; iter != map.end(); iter++) {
        FilepathScanner::cmds_t t = iter->second;
        for (int i = 0; i < t.size(); i++) {
            cep::event *evnt = new cep::event(cep::config_metadata::instance().metadata(2000), 2000);
            int count = 0;
            if (t[i]->open()) {
                MLOG_DEBUG << "open succful" << endl;
            }
            MLOG_DEBUG << "start read[" << t[i]->cmd().file_path << "]" << endl;
            while (t[i]->read(*evnt)) {
                count++;
            }
            MLOG_DEBUG << "read event[" << count << "]" << endl;
        }
        pathScanner.close(t);
        //        pathScanner.archive(t);
    }
    cout << "loadedCmd.size:" << pathScanner.loadedCmds().size() << endl;
    pathScanner.scanning(map);
}

class NormalClass {
public:

    explicit NormalClass(const int info) : info_(info) {
    }

    int info() const {
        return info_;
    }
private:
    int info_;
};

class NoncopyableClass : boost::noncopyable {
public:

    explicit NoncopyableClass(const int info) : info_(info) {
    }

    int info() const {
        return info_;
    }
private:
    int info_;
};

class PrivateNoncopyableClass : private boost::noncopyable {
public:

    explicit PrivateNoncopyableClass(const int info) : info_(info) {
    }

    int info() const {
        return info_;
    }
private:
    int info_;
};

void Test4recursive_directory_iterator(int argc, char**argv) { // C
    cout << "*** begin of Test4recursive_directory_iterator ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }
    if (argc < 3) {
        cout << "usage: [0]cepframe [1]C [2]runMode is must exists!" << endl;
        return;
    }
    char runMode = *(argv + 2)[0];
    if (runMode == 'l') {
        if (argc < 4) {
            cout << "usage: [0]cepframe [1]C [2]l [3]dir" << endl;
            return;
        }
        boost::filesystem::path fsPath(*(argv + 3));
        boost::system::error_code ec;
        boost::filesystem::recursive_directory_iterator beg_iter(fsPath, ec);
        if (ec) {
            cout << "Test4recursive_directory_iterator not found dir["
                    << fsPath << "]:" << boost::system::system_error(ec).what() << endl;
            return;
        }
        boost::filesystem::recursive_directory_iterator end_iter;

        for (; beg_iter != end_iter; ++beg_iter) {
            if (boost::filesystem::is_directory(*beg_iter)) {
                continue;
            } else {
                string filename = beg_iter->path().filename().native();
                string fullpath = beg_iter->path().string();
                cout << '>' << filename << '@' << fullpath << endl;
            }
        }
    } else if (runMode == 'x') {
        /*string str("abc");
        if (str != NULL) cout << "str is null" << endl;
        else cout << "str is not null" << endl;*/
        NormalClass nc(1);
        NoncopyableClass ncc(2);
        PrivateNoncopyableClass pnc(3);
        // PrivateNoncopyableClass pnc2 = pnc;
        cout << "NormalClass@" << &nc << "->info()=" << nc.info()
                << "! and sizeof is:" << sizeof (NormalClass) << endl;
        cout << "NoncopyableClass@" << &ncc << "->info()=" << ncc.info()
                << "! and sizeof is:" << sizeof (NoncopyableClass) << endl;
        cout << "PrivateNoncopyableClass@" << &pnc << "->info()=" << pnc.info()
                << "! and sizeof is:" << sizeof (PrivateNoncopyableClass) << endl;

    }
    cout << "*** end of Test4recursive_directory_iterator[" << runMode << "] ***" << endl;
}

void Test4processing_utils(int argc, char**argv) { // D
    cout << "*** begin of Test4processing_utils ***" << endl;
    // Prints each argument on the command line.
    for (int i = 0; i < argc; ++i) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    size_t size = 1;
    if (argc > 2) size = atoi(*(argv + 2));
    type_slice keys[size];
    for (int i = 0; i < size; ++i) {
        keys[i].type = buffer_metadata::STR;
        keys[i].data = slice("");
        MLOG_TRACE << i << "> dim:" << keys[i] << endl;
    }

    slice keys4slice[size];
    const int len = processing_utils::encoding2slice(keys, size, 1, keys4slice);
    if (len < 1) {
        cout << "encoding2slice return:" << len << endl;
        return;
    }
    char key4buf[len]; // = {0};
    processing_utils::encoding(keys4slice, size, key4buf, len, ':');
    slice key(key4buf, len);

    cout << "key:" << buff_util::to_hex_string(key.data(), key.size()) << endl;

    cout << "*** end of Test4processing_utils[" << size << "] ***" << endl;
}

void test_event_readheader(int argc, char** argv) {
    FILE* file = fopen(argv[1], "r");

    char buff[100] = {0};
    fpos_t pos;
    if (file) {
        fgetpos(file, &pos);
        cout << "pos1: " << pos.__pos << ":" << pos.__state.__count << endl;

        fread(buff, 1, 20, file);
        fgetpos(file, &pos);
        cout << "pos1: " << pos.__pos << ":" << pos.__state.__count << endl;

        fread(buff, 1, 20, file);
        fgetpos(file, &pos);
        cout << "pos1: " << pos.__pos << ":" << pos.__state.__count << endl;

        int red_c = fread(buff, 1, 20, file);
        fgetpos(file, &pos);
        cout << "pos1: " << pos.__pos << ":" << pos.__state.__count << endl;
        cout << "read count: " << red_c << endl;

        pos.__pos -= red_c;
        fsetpos(file, &pos);
        red_c = fread(buff, 1, 20, file);
        fgetpos(file, &pos);
        cout << "pos1: " << pos.__pos << ":" << pos.__state.__count << endl;
        cout << "read count: " << red_c << endl;
    }


    cep::event2text reader;

    if (reader.open(argv[1], "r")) {
        vector<string> headers;
        reader.read_header(headers);
        for (int i = 0; i < headers.size(); ++i) {
            cout << headers.at(i) << ":";
        }
        cout << endl;
        headers.clear();

        reader.readfields_csv(headers, '"', ','); //let the readfields_csv becomes public method for test.
        for (int i = 0; i < headers.size(); ++i) {
            cout << headers.at(i) << ":";
        }
        cout << endl;
        headers.clear();
        reader.readfields_csv(headers, '"', ',');
        for (int i = 0; i < headers.size(); ++i) {
            cout << headers.at(i) << ":";
        }
        cout << endl;

        reader.read_header(headers);
        reader.read_header(headers);
        reader.read_header(headers);
        reader.read_header(headers);
        reader.read_header(headers);
        reader.read_header(headers);

        for (int i = 0; i < headers.size(); ++i) {
            cout << headers.at(i) << ":";
        }
        cout << endl;

        if (!reader.is_end()) {
            headers.clear();
            reader.readfields_csv(headers, '"', ',');
            for (int i = 0; i < headers.size(); ++i) {
                cout << headers.at(i) << ":";
            }
            cout << endl;
        }
    }
}

int main(int argc, char** argv) {
    signal(SIGHUP, sigCatcher); /* SIG_IGN 1:Hangup (POSIX).  */
    signal(SIGINT, sigCatcher); /* 2:Interrupt (ANSI).  */

    signal(SIGUSR1, sigCatcher); /* 10:User-defined signal 1 (POSIX).  */

    signal(SIGUSR2, sigCatcher); /* 12:User-defined signal 2 (POSIX).  */
    signal(SIGPIPE, sigCatcher); /* SIG_IGN 13:Broken pipe (POSIX).  */

    signal(SIGTERM, sigCatcher); /* 15:Termination (ANSI).  */
#ifndef __CYGWIN32__
    signal(SIGSTKFLT, sigCatcher); /* 16:Stack fault.  */
#endif
    signal(SIGCHLD, sigCatcher); /* 17:Child status has changed (POSIX).  */
    signal(SIGCONT, sigCatcher); /* 18:Continue (POSIX).  */
    signal(SIGSTOP, sigCatcher); /* 19:Stop, unblockable (POSIX).  */
    signal(SIGTSTP, sigCatcher); /* 20:Keyboard stop (POSIX).  */
    signal(SIGTTIN, sigCatcher); /* 21:Background read from tty (POSIX).  */
    signal(SIGTTOU, sigCatcher); /* 22:Background write to tty (POSIX).  */
    signal(SIGURG, sigCatcher); /* 23:Urgent condition on socket (4.2 BSD).  */
    signal(SIGXCPU, sigCatcher); /* 24:CPU limit exceeded (4.2 BSD).  */
    signal(SIGXFSZ, sigCatcher); /* 25:File size limit exceeded (4.2 BSD).  */
    signal(SIGVTALRM, sigCatcher); /* 26:Virtual alarm clock (4.2 BSD).  */
    signal(SIGPROF, sigCatcher); /* 27:Profiling alarm clock (4.2 BSD).  */
    signal(SIGWINCH, sigCatcher); /* 28:Window size change (4.3 BSD, Sun).  */
    signal(SIGPOLL, sigCatcher); /* 29:Pollable event occurred (System V). I/O now possible (4.2 BSD).  */
    signal(SIGPWR, sigCatcher); /* 30:Power failure restart (System V).  */
#ifndef __CYGWIN32__
    signal(_NSIG, sigCatcher); /* 65:Biggest signal number + 1 (including real-time signals).  */
#endif

    cout << "hello world!" << endl;
    MLOG_INFO << "*** Hello world [" << time(0) << "]***" << endl;
    unsigned long long int tick = getCPUTick();
    int cpu = 0;
    unsigned long long int tick2 = getCPUTick();
    cout << "Used " << tick2 << '-' << tick << '=' << (tick2 - tick) << " CPU tick" << endl;
    MLOG_TRACE << "can u feel my world?" << endl;
    MLOG_DEBUG << "can u feel my world?" << endl;
    MLOG_INFO << "can u feel my world?" << endl;
    MLOG_WARN << "can u feel my world?" << endl;
    MLOG_ERROR << "can u feel my world?" << endl;
    MLOG_FATAL << "can u feel my world?" << endl;

    cout << "> current thread id:" << boost::this_thread::get_id() << endl;
    cout << "> current hardware physical parallelism(!concurrency):" << boost::thread::hardware_concurrency() << endl;

    if (argc > 1) {
        char *arg1 = *(argv + 1);
        cout << "First user's arg is:" << arg1 << endl;
        switch (*arg1) {
            case '1':
                TestQueue(arg1);
                break;
            case '2':
                TestDynamicStruct(arg1);
                break;
            case '3':
            {
                if (argc != 3) {
                    fprintf(stdout, "usage:%s 3 3\n", argv[0]);
                    exit(-1);
                }
                int mode = atoi(argv[2]);
                fprintf(stdout, "mode[%d]\n", mode);
                TestMetaProgram(mode);
            }
                break;
            case '4':
            {
                if (argc != 3) {
                    fprintf(stdout, "usage:%s 3 3\n", argv[0]);
                    exit(-1);
                }
                int mode = atoi(argv[2]);
                fprintf(stdout, "mode[%d]\n", mode);
                TestEvent(mode);
            }
                break;
            case '5':
            {
                if (argc != 3) {
                    fprintf(stdout, "usage:%s 3 3\n", argv[0]);
                    exit(-1);
                }
                int mode = atoi(argv[2]);
                fprintf(stdout, "mode[%d]\n", mode);
                TestOffsetLen(mode);
            }
                break;
            case '6':
                TestXML(arg1);
                break;
            case '7':
                TestCstr2nbr(arg1);
                break;
            case 'a':
            {
                if (argc != 3) {
                    fprintf(stdout, "usage:%s a (3)\n", argv[0]);
                    exit(-1);
                }
                int mode = atoi(argv[2]);
                fprintf(stdout, "mode[%d]\n", mode);
                Testnbr2str(mode);
            }
                break;
            case 'b':
            {
                if (argc != 3) {
                    fprintf(stdout, "usage:%s b [0|1]\n", argv[0]);
                    exit(-1);
                }
                int mode = atoi(argv[2]);
                fprintf(stdout, "mode[%d]\n", mode);
                Testevent2text(mode);
            }
            case 's':
                TestSingleton(arg1);
                break;
            case 't':
                TestConstDataset(arg1);
                break;
            case 'u':
                TestRouter(argc, argv);
                break;
            case 'v':
                TestPlugin(arg1);
                break;
            case 'w':
                TestRangeIndex(arg1);
                break;
            case 'x':
                TestTrieIndex(arg1);
                break;
            case 'y':
                TestSystime(argc, argv);
                break;
#ifdef __VER4ON_INSTALL_LEVELDB__
            case 'z':
                TestSnappy(argc, argv);
                break;
#endif // __VER4ON_INSTALL_LEVELDB__
            case 'A':
                TestFileLock(argc, argv);
                break;
#ifdef __VER4ON_INSTALL_LEVELDB__
            case 'B':
                TestLevelDB(argc, argv);
                break;
#endif // __VER4ON_INSTALL_LEVELDB__
            case 'C':
                Test4recursive_directory_iterator(argc, argv);
                break;
            case 'D':
                Test4processing_utils(argc, argv);
                break;
            case 'J':
                TestLoadEvent(arg1);
                break;
            case 'K':
                TestEveryPlugin(arg1);
                break;
            case 'L':
                TestCyclePlugin(arg1);
                break;
            case 'W':
                TestEventFileHandler(argc, argv);
            default:
                cout << "Do not have such test case for:" << *arg1 << endl;
        }
    } else {
        cout << "No user's arg:" << *argv << endl;
    }
    return 0;
}
