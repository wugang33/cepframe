/* 
 * File:   utils.hpp
 * Author: WangquN
 *
 * Created on 2012-02-14 PM 05:14
 */

#ifndef PROCESSING_UTILS_HPP
#define	PROCESSING_UTILS_HPP

#include <string.h>
#include <vector>
#include <sys/timeb.h>
#include <sys/time.h>

#include "slice.hpp"
#include "dynamic_struct.hpp"
#include "buffer_function.hpp"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "utils_nbr2str.hpp"
using namespace std;
using std::vector;

namespace cep {

    class processing_utils {
    private:
        processing_utils();
    public:

        static int encoding2slice(const type_slice keys[], const size_t keys_count,
                const size_t splitter_length, slice keys_slice[]) {
            // if (keys == NULL) return -1;
            int len = 0; // - splitter_length;
            for (int i = 0; i < keys_count; ++i) {
                if (keys[i].type == buffer_metadata::STR) {
                    // if it is a string, we can save some memory
                    const size_t len4str = strlen(keys[i].data.data());
                    if (len4str < keys[i].data.size())
                        keys_slice[i] = slice(keys[i].data.data());
                    else // it can cause error by virtual STR field
                        keys_slice[i] = keys[i].data;
                } else
                    keys_slice[i] = keys[i].data;
                len += keys_slice[i].size() + splitter_length;
            }
            return len;
        }

        static void encoding(const slice keys[], const size_t keys_count,
                char key[], const size_t key_length, const char splitter) {
            // if (keys == NULL || key == NULL) return;
            memset(key, 0, key_length);
            size_t len;
            for (int i = 0; i < keys_count; ++i) {
                len = keys[i].size();
                memcpy(key, keys[i].data(), len);
                // if (i + 1 < keys_count) {
                key += len;
                *key = splitter;
                ++key;
                // }
            }
        }

        /**
         * 
         * @param keys_idx
         * @param keys_count
         * @param splitter
         * @param data
         * @return slice.data() must delete!
         *
        static slice encoding(const size_t keys_idx[], const size_t keys_count,
                const char splitter, const dynamic_struct& data) {
            // if (keys_idx == NULL) return;
            if (keys_count < 1) return slice(new char[0]);
            // size_t idx;
            slice keys[keys_count];
            slice default_data;
            int len = 0;
            for (int i = 0; i < keys_count; ++i, ++keys_idx) {
                // idx = *(keys_idx + i);
                if (buffer_metadata::STR == data.metadata()->get_cfg_entry(*keys_idx)->type) {
                    // if it is a string, we can save some memory
                    keys[i] = slice(data.value(*keys_idx, default_data).data());
                } else
                    keys[i] = data.value(*keys_idx, default_data);
                len += keys[i].size() + 1;
            }
            --len; // size_ must greater than zero
            char *ckey = new char[len]; // must delete
            processing_utils::encoding(keys, keys_count, ckey, len, splitter);
            return slice(ckey, len);
        }

        static void encoding(vector<slice*>::const_iterator first,
                vector<slice*>::const_iterator last,
                char* key, const size_t size, const char splitter) {
            memset(key, 0, size);
            size_t len;
            for (; first != last; ++first) {
                len = (*first)->size();
                memcpy(key, (*first)->data(), len);
                if (first + 1 != last) {
                    key += len;
         *key = splitter;
                    ++key;
                }
            }
        }*/

        /**
         * /usr/bin/man => /usr/bin, man
         * c:\windows\winhelp.exe => c:\windows\, winhelp.exe
         * 
         * @param fileName
         * @param rst
         */
        static void split_path(const string& path, pair<string, string>& rst) {
            size_t found = path.find_last_of("/\\");
            if (found == string::npos) {
                rst.first = path;
                rst.second = "";
            } else {
                rst.first = path.substr(0, found);
                rst.second = path.substr(found + 1);
            }
        }

        static void split_strs(const string& str, const string& splitter, vector<string>& strs) {
            string s = str, seg;
            size_t found;
            while (s.size() > 0) {
                found = s.find(splitter);
                if (found != string::npos) {
                    seg = s.substr(0, found);
                    s = s.substr(found + 1);
                } else {
                    seg = s;
                    s = "";
                }
                strs.push_back(seg);
            }
        }

        /***********************************************************************
        1秒   = 1000 毫秒(ms)
        1毫秒 = 1／1,000 秒(s)
        1秒   = 1,000,000 微秒(μs)
        1微秒 = 1／1,000,000 秒(s)
        1秒   = 1,000,000,000 纳秒(ns)
        1纳秒 = 1／1,000,000,000 秒(s)
        1秒   = 1,000,000,000,000 皮秒(ps)
        1皮秒 = 1／1,000,000,000,000 秒(s)
         **********************************************************************/

        /**
         * #include <time.h>
         * 
         * @return The number of seconds since January 1, 1970
         */
        static unsigned long systime_sec() {
            return time(NULL);
        }

        /**
         * 1秒   = 1000 毫秒(ms)
         * 1毫秒 = 1／1,000 秒(s)
         * #include <sys/timeb.h>
         * 
         * @return 
         */
        static unsigned long systime_msec() {
            struct timeb timebuf;
            ftime(&timebuf);
            return timebuf.time * 1000 + timebuf.millitm;
        }

        /**
         * 1秒   = 1,000,000 微秒(μs)
         * 1微秒 = 1／1,000,000 秒(s)
         * #include <sys/time.h>
         * 
         * @return 
         */
        static unsigned long systime_usec() {
            struct timeval tv;
            // struct timezone tz;

            if (gettimeofday(&tv, NULL) == 0)
                return tv.tv_sec * 1000000 + tv.tv_usec;
            else
                return 0;
        }

        /**
         * 1秒   = 1,000,000,000 纳秒(ns)
         * 1纳秒 = 1／1,000,000,000 秒(s)
         * #include <time.h>
         * 
         * compile: -lrt
         * clock_gettime function implement in librt
         * 
         * 
         * @return 
         */
        static unsigned long systime_nsec() {
            struct timespec ts;
#ifdef __CYGWIN__
            // #define CLOCK_THREAD_CPUTIME (clockid_t)3
            clock_gettime(2, &ts);
#else
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
#endif /*__CYGWIN__*/
            return ts.tv_sec * 1000000000 + ts.tv_nsec;
        }

        /**
         * 1秒   = 1,000,000,000,000 皮秒(ps)
         * 1皮秒 = 1／1,000,000,000,000 秒(s)
         * 
         * @return 
         */
        static unsigned long systime_psec() {
            return 0;
        }
    };

    class cep_loger {
    public:

        cep_loger(const string & log_dir = "./logs") {
            // time zone set
            tzset();
            // GMT and Local time has time sone diff.
            milsecondDiff_ = -(timezone * 1000);

            if (log_dir.size() != 0) {
                log_dir_ = log_dir;
            } else {
                log_dir_ = "./logs";
            }

            boost::system::error_code ec;
            boost::filesystem::path filePath(log_dir_);
            if (!boost::filesystem::exists(filePath, ec)) {
                while (!boost::filesystem::create_directories(filePath, ec)) {
                };
            }

            pre_time_ = getLocalMilsecond(); // millisecd 
            log_file_name_ = log_dir_ + "/" + "normal_" + cep::milsecond2str(pre_time_, "%4d%02d%02d%02d%02d%02d") + ".log";

            file_.open(log_file_name_.c_str());
            outbuf_ = cout.rdbuf(file_.rdbuf());
        }

        void change_log_file_name(unsigned long interval_time_sec) {
            unsigned long now = getLocalMilsecond();
            if (now > pre_time_ + interval_time_sec * 1000) {
                pre_time_ = now;
                file_.close();
                log_file_name_ = log_dir_ + "/" + "normal_" + cep::milsecond2str(pre_time_, "%4d%02d%02d%02d%02d%02d") + ".log";
                file_.open(log_file_name_.c_str());
                outbuf_ = cout.rdbuf(file_.rdbuf());
            }
        }

        ~cep_loger() {
            if (!outbuf_) {
                cout.rdbuf(outbuf_);
                delete outbuf_;
            }
        }
    private:

        cep_loger(const cep_loger& loger) {
        };

        unsigned long getLocalMilsecond() {
            return cep::processing_utils::systime_msec() + milsecondDiff_;
        }
        ofstream file_;
        streambuf * outbuf_;
        unsigned long pre_time_;
        unsigned long milsecondDiff_;
        string log_file_name_;
        string log_dir_;
    };

}

#endif	/* PROCESSING_UTILS_HPP */
