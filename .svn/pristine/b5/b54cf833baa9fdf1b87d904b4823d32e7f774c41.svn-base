/* 
 * File:   event2text.hpp
 * Author: WG
 * Created on 2012-01-31 PM 04:45
 * 
 * Modified by luoxiaoyi 2012-04-05
 */

#ifndef EVENT2TEXT_HPP
#define	EVENT2TEXT_HPP
#include <string>
#include<vector>
#include <stdio.h>
using namespace std;

namespace cep {
    class event;

    class event2text {
    public:
        event2text(char csv_escape = 0x22/* " */, char csv_delimiter = 0x2c/* , */);
        /*  parameter mode
       "r" Open a file for reading. The file must exist. 
       "w" Create an empty file for writing. If a file with the same name already exists its content is erased and the file is treated as a new empty file.  
       "a" Append to a file. Writing operations append data at the end of the file. The file is created if it does not exist. 
       "r+" Open a file for update both reading and writing. The file must exist. 
       "w+" Create an empty file for both reading and writing. If a file with the same name already exists its content is erased and the file is treated as a new empty file. 
       "a+" Open a file for reading and appending. All writing operations are performed at the end of the file, protecting the previous content to be overwritten. You can reposition (fseek, rewind) the internal pointer to anywhere in the file for reading, but writing operations will move it back to the end of file. The file is created if it does not exist. 
         */
        int open(const char * filename, const char * mode);
        bool write(const string& event_str);
        bool write(const event &eventmsg);
        void to_string(const event &eventmsg, string& event_str);
        bool read(event &);
        // this method can be invoked in anywhere and for any times.
        void read_header(vector<string> &fields);
        int swap(const char *filename, const char *mode); //swap file  if file is opened close the file  if successful return 1 if open false return 0

        int is_open() {
            return file_ == NULL ? 0 : 1;
        }
        void close();

        int flush() {
            if (file_)
                return fflush(file_);
            return -1;
        }

        int rename(const string & new_filename);

        int is_end() const {
            if (!file_) return 1;
            fpos_t pos;
            fgetpos(file_, &pos);
            int isendof = 0;
            fgetc(file_);
            isendof = feof(file_);
            fsetpos(file_, &pos);
            if (isendof == 1 && start_buf_cur_ >= end_buf_cur_) {
                return 1;
            }
            return 0;
        }
        virtual ~event2text();

        string getFileName() const {
            return this->filename_;
        }

        typedef void (*func4set_value)(event& eventmsg, size_t index, const string& value);
        typedef string(*func4get_value)(const event& eventmsg, size_t index);
        func4set_value* set_values;
        func4get_value* get_values;
    private:
        FILE * file_;
        const static int buffersize_ = 4096;
        int linenum_;
        char csv_escape_; // CSV_ESCAPE: "
        char csv_delimiter_; // CSV_DELIMITER: ,
        char buffer_[buffersize_];
        int start_buf_cur_; // buffer的游标
        int end_buf_cur_;
        string filename_;
        vector<string> header_;
        fpos_t header_pos_;

        void read2buf() {
            end_buf_cur_ = fread(buffer_, 1, buffersize_, file_);
            start_buf_cur_ = 0;
        }
        void readfields_csv(vector<string> &fields, char escape, char delimiter);

        event2text(const event2text&) {
        };

        event2text& operator=(const event2text&) {
        };

        // void replace_escape(string &str, int mode);
    };
}
#endif	/* EVENT2TEXT_HPP */
