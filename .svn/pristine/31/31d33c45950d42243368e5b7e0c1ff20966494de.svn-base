/* 
 * File:   event_file_handler.hpp
 * Author: WangquN
 *
 * Created on 2012-07-17 PM 09:23
 */

#ifndef EVENT_FILE_HANDLER_HPP
#define	EVENT_FILE_HANDLER_HPP

#include <vector>
#include <map>
#include <set>

#include "event.hpp"

using namespace std;

class LZOFileReader;
class LZOFileWriter;
namespace cep {
    class event2text;

    enum cmd_type {
        dim_swap_cmd = 998,
        file_lzo_cmd = 999, file_leveldb_cmd = 1000, file_cvs_cmd = 1001
    };

    /*<!--
    STR = 0; CHAR = 1; UCHAR = 2; SHORT = 3; USHORT = 4; INT = 5; UINT = 6;
    FLOAT = 7; LONG = 8; ULONG = 9; DOUBLE = 10; DATETIME = 11; IP = 12;
    -->
    <event id="1001,999" name="file.cmd">
            <attr type="4" pos="0">pe_id</attr>
            <attr type="6" pos="10">event_type</attr>
            <attr type="0" pos="20" len="128">file_path</attr>
            <attr type="11" pos="30">time_window</attr>
            <attr type="11" pos="40">timestamp</attr>
            <attr type="4" pos="50">cycle</attr>
    </event>*/
    struct file_cmd {
        ushort pe_id;
        uint event_type;
        char file_path[128];
        ulong time_window, timestamp;
        ushort cycle;
    };

    class FilepathScanner {
    public:

        struct Config4ReadCmdFilepath {
            cep::event::type_t cmdType;
            string src;
            string archive;
        };

        class CmdFileReader {
        public:
            CmdFileReader();
            ~CmdFileReader();
        private: // Forbidden copy behave
            CmdFileReader(const CmdFileReader&);
            CmdFileReader& operator=(const CmdFileReader&);
        public:

            const Config4ReadCmdFilepath* filepath() const {
                return filepath_;
            }

            bool closed() const {
                return closed_;
            }

            const file_cmd& cmd() const {
                return cmd_;
            }

            //            bool isInternalCmd() {
            //                return isInternalCmd_;
            //            }
            //
            //            void setInternalCmdFlag(bool flag) {
            //                isInternalCmd_ = flag;
            //            }

            const string& filename() const {
                return filename_;
            }
        public:
            bool init(const Config4ReadCmdFilepath& filepath, const string& filename);
            bool open();
            bool read(event& evt);

            void archive();
            void close();
        public:
            event2text* txtfile();
        private:
            const Config4ReadCmdFilepath* filepath_;
            event2text* txtfile_;
            LZOFileReader* lzofile_;
            ulong offset4read_;

            bool closed_;
            file_cmd cmd_;
            string filename_;

            //            Mark internal resutl cmd
            //            bool isInternalCmd_;
        };
    public:
        FilepathScanner();
        ~FilepathScanner();
    private: // Forbidden copy behave
        FilepathScanner(const FilepathScanner&);
        FilepathScanner& operator=(const FilepathScanner&);
    public:

        event::type_t scanCmdEventType() const {
            return scanCmdEventType_;
        }

        void setScanCmdEventType(event::type_t scanCmdEventType) {
            scanCmdEventType_ = scanCmdEventType;
        }

        ushort scanCmdCycle() const {
            return scanCmdCycle_;
        }

        void setScanCmdCycle(ushort scanCmdCycle) {
            scanCmdCycle_ = scanCmdCycle;
        }

        long msTimeOut() const {
            return msTimeOut_;
        }

        void setMSTimeOut(long msTimeOut) {
            msTimeOut_ = msTimeOut;
        }

        long diffms4timezone() const {
            return diffms4timezone_;
        }

        void set_diffms4timezone(long diffms4timezone) {
            diffms4timezone_ = diffms4timezone;
        }

        uint secScanCycle() const {
            return secScanCycle_;
        }

        void setSecScanCycle(uint secScanCycle) {
            secScanCycle_ = secScanCycle;
        }

        vector<Config4ReadCmdFilepath>& cmdFilePath() {
            return cmdFilePath_;
        }

        const set<string>& loadedCmds() const {
            return loadedCmds_;
        }

        bool isScanForMonth() {
            return isScanForMonth_;
        }

        void setScanForMonth(bool isScanForMonth) {
            isScanForMonth_ = isScanForMonth;
        }

        int statisticFrequency() const {
            return statisticFrequency_;
        }

        void setStatisticFrequency(int statisticFrequency) {
            statisticFrequency_ = statisticFrequency;
        }
    public:
        typedef vector<CmdFileReader*> cmds_t;
        typedef map<ulong, cmds_t> cmd_map_t;
        typedef map<ulong, cmd_map_t> cmd_map2map_t;
        void scanning(cmd_map_t&);
        void archive(cmds_t&);
        void close(cmds_t&);
    private:
        event::type_t scanCmdEventType_;
        ushort scanCmdCycle_;
        long msTimeOut_;
        /**
         * Set time conversion information from the TZ environment variable.
         * If TZ is not defined, a locale-dependent default is used.
         * 
         * #include <time.h>
         * tzset();
         * ? = timezone * 1000; // sec to ms
         */
        long diffms4timezone_;

        /**
         * 已经扫描过了的文件就列入这个里面
         */
        cmd_map_t scanedCmdMap_;
        cmd_map2map_t scanedCmdMap4Month_;
        uint secScanCycle_;
        vector<Config4ReadCmdFilepath> cmdFilePath_;
        set<string> loadedCmds_;

        // isScanForMonth_ = true statistic for month, and every statisticFrequency_[1,30] stastistic one time.
        bool isScanForMonth_;
        int statisticFrequency_;

        friend ostream& operator<<(ostream& s, const FilepathScanner& r);
    };

    class FileOutputter {
    public:
        typedef unsigned short pn_id_t;
        typedef unsigned short int pe_id_t;

        struct CmdPath {
            ushort cycle;
            string path;
        };
    public:
        FileOutputter();
        ~FileOutputter();
    private: // Forbidden copy behave
        FileOutputter(const FileOutputter&);
        FileOutputter& operator=(const FileOutputter&);
    public:

        vector<CmdPath>& cmdPaths() {
            return cmdPaths_;
        }

        //        CmdPath& inputCmdPath() {
        //            return oneInputCmdPath_;
        //        }
    public:
        bool open(event::type_t cmdType, size_t maxSize,
                const string& filepath, const string& suffix4filename,
                pn_id_t pnId, pe_id_t peId, event::type_t eventType, time_t win4time
                // const bool isInternalCmd = false
                );
        void write(event& evt);
        void close();

        void logging() const;

        void set_diffms4timezone(long diffms4timezone) {
            diffms4timezone_ = diffms4timezone;
        };

        long diffms4timezone() const {
            return diffms4timezone_;
        }
    public:
        event2text* txtfile();
    private:
        bool closed_;
        event2text* txtfile_;
        LZOFileWriter* lzofile_;

        ulong counter_, cnt4file_;
        int fileCounter_;
        mutable time_t timestamp4speed_;
        mutable ulong counter4speed_, total4speed_;

        event::type_t cmdType_, eventType_;
        const buffer_metadata *cmdMetadata_;
        event *cmd_;

        size_t maxSize_;
        char filenames_[128];
        string filename_;
        string filepath_;
        string suffix4filename_;
        vector<CmdPath> cmdPaths_;
        long diffms4timezone_;

        // Mark internal resutl cmd
        //   bool isInternalCmd_;
        // used to keep internal result cmds.
        // CmdPath oneInputCmdPath_;
    };
}

#endif	/* EVENT_FILE_HANDLER_HPP */
