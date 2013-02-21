/* 
 * File:   main.cpp
 * Author: CEP
 *
 * Created on 2012-02-26 AM 10:15
 */

#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include "utils_xml.hpp"
#include "config_metadata.hpp"
#include "cep_base.hpp"
#include "utils_cstr2nbr.hpp"
#include "event.hpp"
#include "system_config.h"
#include "processing/pn.hpp"
#include "communication/CEPDispatch.h"

// #ifndef __CORE_STACK_PRINT__
#ifdef __VER4SIGSEGV_PRINT_BACKTRACE__
#include "sigsegv.h"
#endif

#ifdef __DEBUG4NO_NETWORK_UNITEST__
#include <vector>

#include "event2text.hpp"
#include "utils_cstr2nbr.hpp"
#include "buffer_utility.hpp"
#include "processing/utils.hpp"
#include "processing/pe.hpp"
#else
#include"processing/utils.hpp"
#endif // __DEBUG4NO_NETWORK_UNITEST__
using namespace std;

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

#ifdef __DEBUG4NO_NETWORK_UNITEST__

void* logging(void *args) {
    while (true) {
        cep::pn::instance().logging();
        sleep(5);
    }
}
#endif // __DEBUG4NO_NETWORK_UNITEST__

int main(int argc, char** argv) {
    // #ifdef __CORE_STACK_PRINT__
#ifndef __VER4SIGSEGV_PRINT_BACKTRACE__
#ifdef __VER4SIGSEGV_CATCHER__
    signal(SIGSEGV, sigCatcher);
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
#endif // __VER4SIGSEGV_CATCHER__
#endif // __VER4SIGSEGV_PRINT_BACKTRACE__
    // This signal do not generate core dumped file!
    signal(SIGUSR1, sigCatcher); /* 10:User-defined signal 1 (POSIX).  */
    signal(SIGUSR2, sigCatcher); /* 12:User-defined signal 2 (POSIX).  */
    signal(SIGPIPE, sigCatcher); /* SIG_IGN 13:Broken pipe (POSIX).  */
    char * pn_id = argv[1];
    cep::pn::pn_id_t pnId = cep::cstr2nbr(pn_id, (cep::pn::pn_id_t) 0);
    string event_type, cep_config, comm_u;
    //begin test
    if (argc < 3) {
        cout << "Wrong parameters count, it need 2 at least!\n"
                " Usage: ./cepframe 0[PN id] ./conf/[configuration files path]" << endl;
    } else {
#ifndef __DEBUG4NO_NETWORK_UNITEST__
        //        string log_path = "./logs/";
        //        log_path.append(pn_id);
        //        cep::cep_loger loger(log_path);
#endif
        //begin to load xml_file
        string inifile(argv[2]);
        inifile = inifile + "/config.sys";
        cep::Config conf(inifile);
        if (conf.GetFileFlag()) {
            char *p = strstr(argv[2], "http:");
            // http://10.45.48.54:1548/dw/jobquery?type=1001&jobId=
            if (p) { // from server
                event_type = conf.getProperty("http_event_type", "");
                comm_u = conf.getProperty("http_comm_u", "");
                cep_config = conf.getProperty("http_cep_config", "");

                event_type = argv[2] + event_type + pn_id;
                comm_u = argv[2] + comm_u + pn_id;
                cep_config = argv[2] + cep_config + pn_id;
                //begin load xml_file from http://
                cout << "From HTTP and Load ...\nevent_type xml is [" << event_type << "] \ncomm_u xml is [" << comm_u << "] \ncep_config is[" << cep_config << "]" << endl;
                // TODO
            } else { // from file
                event_type = conf.getProperty("event_type", "");
                comm_u = conf.getProperty("comm_u", "");
                cep_config = conf.getProperty("cep_config", "");

                event_type = argv[2] + event_type;
                comm_u = argv[2] + comm_u;
                cep_config = argv[2] + cep_config;

                cout << "Start load event type configuration:" << event_type << endl;
                cep::xml_document *doc = cep::load_xmlfile(event_type.c_str());
                if (!doc) {
                    cout << "Load file [" << event_type.c_str() << "] failure, Abort CEP!" << endl;
                    return 0;
                } else {
                    const cep::xml_element *xml_temp = cep::root_element(*doc);
                    if (xml_temp == NULL) {
                        cout << "Can not find " << event_type << " root, Abort CEP!" << endl;
                        return 0;
                    }
                    xml_temp = cep::find_element(*xml_temp, "eventset");
                    if (xml_temp != NULL) {
                        if (cep::config_metadata::instance().reload_metadata(*xml_temp))
                            cout << "configuration for event meta data is:\n"
                                << cep::config_metadata::instance() << endl;
                        else {
                            cout << "Load file [" << event_type.c_str() << "] failure, Abort CEP!" << endl;
                            return 0;
                        }
                    } else {
                        cout << "Can not find eventset, Abort CEP!" << endl;
                        return 0;
                    }
                }
                delete doc;

                cout << "Start load communication configuration:" << comm_u << endl;
                doc = cep::load_xmlfile(comm_u.c_str());
                if (!doc) {
                    cout << "Load file [" << comm_u << "] failure, Abort CEP!" << endl;
                    return 0;
                } else {
                    const cep::xml_element *xml_temp = cep::root_element(*doc);
                    if (xml_temp == NULL) {
                        cout << "Can not find " << comm_u << " root, Abort CEP!" << endl;
                        return 0;
                    }
                    xml_temp = cep::find_element(*xml_temp, "communication");
                    if (xml_temp != NULL) {
                        CEPDispatch::instance().reload_metadata(*xml_temp);
                    } else {
                        cout << "Can not find communication tag, Abort CEP!" << endl;
                        return 0;
                    }
                }
                delete doc;
                CEPDispatch::instance().initConfig(pnId);
                if (!CEPDispatch::instance().Start()) {
                    cout << "CEPDispatch::Start failure, Abort CEP!" << endl;
                    return 0;
                }

                cout << "Start load PN configuration:" << cep_config << endl;
                doc = cep::load_xmlfile(cep_config.c_str());
                if (!doc) {
                    cout << "Load file [" << cep_config << "] failure, Abort CEP!" << endl;
                    return 0;
                } else {
                    cep::pn::instance().init(pnId, cep_config);
                    const cep::xml_element *xml_temp = cep::root_element(*doc);
                    if (xml_temp != NULL) {
                        if (cep::pn::instance().reload(*xml_temp)) {
                            if (!cep::pn::instance().startup()) {
                                cout << "pn startup config file [" << cep_config << "] failure, Abort CEP!" << endl;
                                return 0;
                            }
                        } else {
                            cout << "pn reload config file [" << cep_config << "] failure, Abort CEP!" << endl;
                            return 0;
                        }
                    } else {
                        cout << "open file and parse [" << cep_config << "] failure file, Abort CEP!" << endl;
                        return 0;
                    }
                }
                delete doc;
            }
#ifdef __DEBUG4NO_NETWORK_UNITEST__
            pthread_t log_thread;
            if (argc > 3) {
                cout << "*** welcome to version for no network ***" << endl;
                cep::event::type_t type = cep::cstr2nbr(argv[3], (cep::event::type_t) 0);
                if (!type) {
                    cout << "Input event_type_id[" << argv[3] << "] failure, Abort CEP!" << endl;
                    return 0;
                }
                cep::buffer_metadata* metadata = cep::config_metadata::instance().metadata(type);
                if (!metadata) {
                    cout << "can not find event_type[" << type << "], Abort CEP!" << endl;
                    return 0;
                }
                cep::event2text file;
                cep::event* evt;
                const char* fileName = argv[4]; // "/root/ztesoft/CEP/mmfiles/test4wq.mm";
                vector<cep::event*> evts;
                time_t begin, end;
                size_t size = 0;
                file.open(fileName, "r");
                if (file.open(fileName, "r")) {
                    cout << "opened file:" << fileName << endl;
                    long counter = 0;
                    begin = cep::processing_utils::systime_msec();
                    while (!file.is_end()) {
                        evt = new cep::event(metadata, type);
                        evt->set_pe_id(cep::pe::INIT_PE_ID);
                        file.read(*evt);
                        /*cout << counter;
                        cep::buff_util::to_hex_string(evt->data(), evt->len(), cout);
                        cout << endl;*/
                        evts.push_back(evt);
                        ++counter;
                        // delete evt;
                        // if (counter > 2) break;
                    }
                    end = cep::processing_utils::systime_msec();
                    size = evts.size();
                    cout << "****\n read and load " << size << " events used:"
                            << (end - begin) << " msec!" << endl;
                } else {
                    cout << "cant open file:" << fileName << endl;
                }
                pthread_create(&log_thread, NULL, logging, NULL);
                begin = cep::processing_utils::systime_msec();
                for (int i = 0; i < size; ++i)
                    cep::pn::instance().push(*(evts[i]));
                end = cep::processing_utils::systime_msec();
                cout << "****\n push " << size << " events to PN used:"
                        << (end - begin) << " msec!" << endl;
            } else
                pthread_create(&log_thread, NULL, logging, NULL);
            while (true) { // death loop for wait to user kill
                sleep(100);
            }
#else // __DEBUG4NO_NETWORK_UNITEST__
            while (true) {
                cep::pn::instance().logging();
                sleep(5);
                // 86400 one day, 3600 for one hour, 60 for minute.
                //                loger.change_log_file_name(86400);
            }
#endif
        } else
            cout << "Can not find SYSTEM-CONFIG file \"" << inifile << "\"" << endl;
    }
    return 0;
}
