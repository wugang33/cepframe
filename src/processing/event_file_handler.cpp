/* 
 * File:   event_file_handler.cpp
 * Author: WangquN
 *
 * Created on 2012-07-17 PM 09:23
 */

#include "processing/event_file_handler.hpp"

#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <boost/detail/container_fwd.hpp>

#include "event2text.hpp"
#include "file_lock_counter.hpp"
#include "config_metadata.hpp"
#include "buffer_metadata.hpp"
#include "cep_base.hpp"
#include "utils_nbr2str.hpp"
#include "processing/utils.hpp"

#include "communication/baseutils/FSPublic.h"
#include "communication/baseutils/lzowrapper.h"

using namespace std;

namespace cep {

    FilepathScanner::CmdFileReader::CmdFileReader()
    : filepath_(NULL), txtfile_(NULL), lzofile_(NULL)
    , offset4read_(0), closed_(true), cmd_(), filename_()
    //,isInternalCmd_(false)
    {
    }

    FilepathScanner::CmdFileReader::~CmdFileReader() {
        /*if (filepath_ != NULL) {
            delete filepath_;
            filepath_ = NULL;
        }*/
        close();
    }

    bool FilepathScanner::CmdFileReader::init(
            const Config4ReadCmdFilepath& filepath, const string& filename) {
        if (!closed_) return false;
        cep::event2text cmdFile;
        cep::buffer_metadata *metadata = cep::config_metadata::instance().metadata(file_cvs_cmd);
        if (metadata == 0) {
            return false;
        }
        cep::event evnt(metadata, file_cvs_cmd);
        //        evnt.set_metadata(cep::config_metadata::instance().metadata(file_cvs_cmd));
        string full_filename = filepath.src + "/" + filename;
        if (cmdFile.open(full_filename.c_str(), "r") == 0) {
            return false;
        }
        if (cmdFile.read(evnt)) {
            this->filename_ = filename;
            this->filepath_ = &filepath;
            assert(evnt.len() <= sizeof (this->cmd_));
            memcpy(&this->cmd_, evnt.data(), evnt.len());
        } else {
            return false;
        }
        if (filepath.cmdType == file_cvs_cmd) {
            this->txtfile_ = new cep::event2text();
        } else if (filepath.cmdType == file_lzo_cmd) {
            this->lzofile_ = new LZOFileReader();
        } else {
            return false;
        }
        return true;
    }

    bool FilepathScanner::CmdFileReader::open() {
        if (!closed_) return true;
        if (file_cvs_cmd == this->filepath_->cmdType) {
            assert(this->txtfile_ != 0);
            if (this->txtfile_->open(this->cmd_.file_path, "r")) {
                closed_ = false;
                boost::system::error_code ec;
                // maintenance archive file path
                boost::filesystem::path filePath(this->filepath_->archive);
                if (!boost::filesystem::exists(filePath, ec)) {
                    while (!boost::filesystem::create_directories(filePath, ec)) {
                    };
                }
                return true;
            }
            return false;
        } else if (file_lzo_cmd == filepath_->cmdType) {
            assert(this->lzofile_ != 0);
            if (0 == lzofile_->Init(fopen(this->cmd_.file_path, "r"))) {
                closed_ = false;
                boost::system::error_code ec;
                // maintenance archive file path
                boost::filesystem::path filePath(this->filepath_->archive);
                if (!boost::filesystem::exists(filePath, ec)) {
                    while (!boost::filesystem::create_directories(filePath, ec)) {
                    };
                }
                return true;
            }
            return false;
        }
        return false;

    }

    bool FilepathScanner::CmdFileReader::read(event& evt) {
        if (closed_) return false;
        if (file_cvs_cmd == this->filepath_->cmdType) {
            assert(this->txtfile_ != 0);
            if (this->txtfile_->read(evt))
                return true;
            else
                return false;
        } else if (file_lzo_cmd == filepath_->cmdType) {
            assert(this->lzofile_ != 0);
            int size = evt.len();
            char *event_data = const_cast<char*> (evt.data());
            fseek(*lzofile_, offset4read_, 0);
            int flag = fread(event_data, 1, size, *lzofile_);
            this->offset4read_ += flag;
            if (flag == size) {
                return true;
            }
            return false;
        }
        return false;
    }

    void FilepathScanner::CmdFileReader::archive() {
        if (closed_) return;
        close();
        string old_path = this->filepath_->src + "/" + this->filename_;
        string new_path = this->filepath_->archive + "/" + this->filename_;
        int ret = ::rename(old_path.c_str(), new_path.c_str());
        //        MLOG_DEBUG << "FilepathScanner::CmdFileReader::archive rename[" << old_path <<
        //                "] to [" << new_path << "]" << endl;
        if (0 != ret) {
            MLOG_ERROR << "FilepathScanner::CmdFileReader::archive rename[" << old_path <<
                    "] to [" << new_path << "] failure retval[" << ret << "] errno[" << errno << "]" << endl;
        }
        file_lock_counter<int> flc;
        int delta = -1;
        string lockFileName = string(this->cmd_.file_path) + ".lock";

        if (!flc.fileIsExist(lockFileName)) {
            MLOG_WARN << "counting lockFileName: " << lockFileName << " is not exist" << endl;
            return;
        }

        if (!flc.counting(lockFileName, delta)) {
            MLOG_ERROR << "FilepathScanner::CmdFileReader::archive read lock file "
                    << lockFileName << " error!" << endl;
        }
        flc.fclose();
        // no one need the cep file, remove the cep and .lock file.
        if (delta < 1) { // delta < 0 is error need clean cmd
            boost::system::error_code ec;
            boost::filesystem::remove(boost::filesystem::path(this->cmd_.file_path), ec);
            if (ec) {
                MLOG_ERROR << "FilepathScanner::CmdFileReader::archive delete file["
                        << this->cmd_.file_path << "] error :" << boost::system::system_error(ec).what() << endl;
            }
            //            MLOG_DEBUG << "delete file[" << this->cmd_.file_path << "]" << endl;
            boost::filesystem::remove(boost::filesystem::path(lockFileName), ec);
            if (ec) {
                MLOG_ERROR << "FilepathScanner::CmdFileReader::archive delete file["
                        << lockFileName << "] error :" << boost::system::system_error(ec).what() << endl;
            }
            //            MLOG_DEBUG << "delete file[" << lockFileName << "]" << endl;
        }
    }

    void FilepathScanner::CmdFileReader::close() {
        // if (closed_) return;
        if (txtfile_ != NULL) {
            delete txtfile_;
            txtfile_ = NULL;
        }
        if (lzofile_ != NULL) {
            delete lzofile_;
            lzofile_ = NULL;
        }
        closed_ = true;
    }

    event2text* FilepathScanner::CmdFileReader::txtfile() {
        return txtfile_;
    }
    ////////////////////////////////////////////////////////////////////////////

    FilepathScanner::FilepathScanner()
    : scanCmdEventType_(0), scanCmdCycle_(60), msTimeOut_(60 * 60000), diffms4timezone_(-28800 * 1000)
    , secScanCycle_(10), cmdFilePath_(), loadedCmds_(), statisticFrequency_(-1), isScanForMonth_(false) {

        //        MLOG_DEBUG << this << "->FilepathScanner ctor execute!" << endl;
    }

    FilepathScanner::~FilepathScanner() {
        FilepathScanner::cmd_map_t::iterator input_map_iter = this->scanedCmdMap_.begin();
        for (; input_map_iter != scanedCmdMap_.end(); input_map_iter++) {
            FilepathScanner::cmds_t::iterator input_vec_iter = input_map_iter->second.begin();
            for (; input_vec_iter != input_map_iter->second.end(); input_vec_iter++) {
                delete (*input_vec_iter);
                //                MLOG_DEBUG << "delete" << endl;
            }
        }
        //        MLOG_DEBUG << this << "->FilepathScanner dtor execute!" << endl;
    }

    /**
     * 毫秒数的时间格式转换成月粒度的月，如：2012-08-04 00：00：00.000 的毫秒数，转换之后就返回月的时间片：20120801
     * @param milscds 毫秒数的时间
     * @return 20120801日期所表示的数字类型
     */
    unsigned long dateTime2MonthTime(const unsigned long milscds) {
        string tt = milsecond2str(milscds, "%4d%02d%02d");
        unsigned long tt_num = (unsigned long) atol(tt.c_str());
        tt_num /= 100;
        tt_num = tt_num * 100 + 1;
        return tt_num;
    }

    /**
     * 用于月粒度统计，如统计3个月的，那么需要把20120101，20120201，20120301计算到一个时间片上
     * @param month_t 如：20120101
     * @param cycle 月统计粒度，如3，表示将3个月做一次统计结果输出
     * @return 返回月统计粒度的key
     */
    int getMonthKey(const unsigned long month_t, const int cycle) {
        int month_time = month_t / 100;
        int year = month_time / 100;
        int month = month_time % 100;
        return ((year - 1970)*12 + month - 1) / cycle;
    }

    /**
     * 得到月粒度统计的时间片对应的超时时间字符串，如201201，201202，201203这三个月的超时时间的下个窗口应该是20120401
     * @param key 月的时间片，如201201
     * @param cycle 几个月作为一次统计结果输出，如3
     * @return 这几个月统计结果的输出的超时时间的字符串
     */
    string getMonthTimeoutTime(const int key, const int cycle) {
        int month_count = (key + 1) * cycle;
        int year = month_count / 12 + 1970;
        int month = month_count % 12 + 1;
        int day = 1;
        char tt[25];
        sprintf(tt, "%4d-%02d-%02d 00:00:00.000", year, month, day);
        return string(tt);
    }

    /**
     * 用于月粒度统计，如统计3个月的，那么需要把20120101，20120201，20120301计算到一个时间片上，函数返回20120101的毫秒数
     * @param month_t 如：20120101
     * @param cycle 月统计粒度，如3，表示将3个月做一次统计结果输出
     * @return 返回月统计粒度的key,函数返回20120101的毫秒数
     */
    unsigned long getMonthMilscdKey(const unsigned long month_t, const int cycle) {
        int key = getMonthKey(month_t, cycle);
        string time_str = getMonthTimeoutTime(key - 1, cycle);
        return cep::cstr2milsecond(time_str.c_str(), "%4d-%02d-%02d %02d:%02d:%02d");
    }

    /**
     * 根据天的时间毫秒数，得到这这一天在统计频率上的时间片key，如,统计频率为3天，则20120803，20120802，20120801 都应该得到20120801这个时间片
     * @param milscds 时间毫秒数
     * @param cycle 统计频率，如3天统计一次
     * @return 返回日的时间片
     */
    int getDateKey(const unsigned long milscds, const int cycle) {
        string tt = milsecond2str(milscds, "%4d%02d%02d");
        unsigned long tt_num = atoi(tt.c_str());
        int ym = tt_num / 100;
        int day = tt_num % 100;
        day = ((day - 1) / cycle) * cycle + 1;
        return ym * 100 + day;
    }

    /**
     * 根据天的时间毫秒数，得到这这一天在统计频率上的时间片key，如,统计频率为3天，则20120803，20120802，20120801 都应该得到20120801这个时间片
     * @param milscds
     * @param cycle
     * @return 返回20120801的时间毫秒数
     */
    ulong getDateMilscdKey(const unsigned long milscds, const int cycle) {
        int key = getDateKey(milscds, cycle);
        int year = key / 10000;
        int month = (key / 100) % 100;
        int day = key % 100;
        char tt[25] = {0};
        sprintf(tt, "%4d-%02d-%02d 00:00:00.000", year, month, day);
        return cep::cstr2milsecond(tt, "%4d-%02d-%02d %02d:%02d:%02d");
    }

    /**
     * 判断某年是否是闰年
     * @param year 年份，如2012
     * @return 是返回1，否返回0
     */
    int isLeapYear(int year) {
        // 闰年——能被4整除而不能被100整除，或者能被400整除
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
            return 1;
        else return 0;
    }

    /**
     * 分开统计频率的时候，需要得到某个范围之内的超时时间
     * @param key 如20120803，20120802，20120801 的key应该是 20120801
     * @param cycle 统计频率，如3天统计一次
     * @return 超时时间的字符串，该例返回：2012-08-04 00:00:00.000
     */
    string getDayFrequencyTimeout(const int key, const int cycle) {
        int day = key % 100;
        int month = (key / 100) % 100;
        int year = key / 10000;

        static int daysForMonths[2][12] = {
            {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
            {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
        };

        // 如果大于某月的最后一天，那么，直接取下个月的第一天
        if (day + cycle > daysForMonths[isLeapYear(year)][month - 1]) {
            month += 1;
            day = 1;
            // 如果month超过12了，则年+1，mouth=1，day =1
            if (month > 12) {
                month = 1;
                year += 1;
            }
        } else {
            day += cycle;
        }
        char tt[25] = {0};
        sprintf(tt, "%4d-%02d-%02d 00:00:00.000", year, month, day);
        return string(tt);
    }

    void insert2map(FilepathScanner::cmd_map_t& readyCmds,
            ulong timeWindow, FilepathScanner::CmdFileReader* cmd) {
        FilepathScanner::cmd_map_t::iterator found = readyCmds.find(timeWindow);
        if (found == readyCmds.end()) {
            FilepathScanner::cmds_t cmds;
            cmds.push_back(cmd);
            readyCmds.insert(FilepathScanner::cmd_map_t::value_type(timeWindow, cmds));
        } else {
            found->second.push_back(cmd);
        }
    }

    /**
     * 
     * @param scannedcmds 根据类型和cycle扫描好的vector<CmdFileReader*>集合
     * @param msTimeOut 设置的超时（毫秒数）
     * @param diffms4timezone 时区的差别（毫秒数）
     * @param readyCmds 返回值：满足了超时规则的vector<CmdFileReader*>集合
     * @param cycle
     */
    void GetReadyCmds(FilepathScanner::cmd_map_t& scannedcmds,
            const long msTimeOut, const long diffms4timezone,
            FilepathScanner::cmd_map_t& readyCmds, const ushort cycle) {
        readyCmds.clear();
        const ulong mscycle = cycle * 60000;
        const ulong sysms = processing_utils::systime_msec();
        FilepathScanner::cmd_map_t::iterator input_map_iter = scannedcmds.begin();
        for (; input_map_iter != scannedcmds.end();) {
            ulong timewindow4cmd = input_map_iter->first;
            FilepathScanner::cmds_t::iterator input_vec_iter = input_map_iter->second.begin();
            /*
             * -1 is max ulong
             */
            ulong mintimestamp4cmd = -1;
            // 得到这一批文件最先生成的timestamp
            for (; input_vec_iter != input_map_iter->second.end(); input_vec_iter++) {
                ulong timestamp4cmd = (*input_vec_iter)->cmd().timestamp;
                if (timestamp4cmd < mintimestamp4cmd) {
                    mintimestamp4cmd = timestamp4cmd;
                }
            }
            /*
             * utc to local 
             * ms4sys 得到的是系统的本地时间直接转换成utc时间 比如现在是2012-07-19 11：42 那么得到的是标准时间2012-07-19 11：42的毫秒
             * ms4startwork 是cmd的下一个时间窗口的开始时间的毫秒数 +超时的毫秒数
             */
            ulong ms4sys = (sysms - diffms4timezone);
            ulong ms4startwork = (timewindow4cmd + 1) * mscycle + msTimeOut;
            //            MLOG_DEBUG << "end timewindow[" << cep::milsecond2str(ms4startwork - msTimeOut) << "]" << endl;
            //            MLOG_DEBUG << "start work time[" << cep::milsecond2str(ms4startwork) << "] system time[" << cep::milsecond2str(ms4sys) << "]" << endl;
            if (ms4startwork < ms4sys) {
                /*
                 * 最先生成的cmd的时间加上超时的时间如果小于系统时间 那么超时
                 */
                //                MLOG_DEBUG << "mintimestamp4cmd [" << cep::milsecond2str(mintimestamp4cmd - diffms4timezone) << "]" << endl;
                //                MLOG_DEBUG << "timestamp+timeout[" << cep::milsecond2str(mintimestamp4cmd + msTimeOut - diffms4timezone) << "] system time[" << cep::milsecond2str(sysms - diffms4timezone) << "]" << endl;
                if (mintimestamp4cmd + msTimeOut < sysms) {
                    std::pair < FilepathScanner::cmd_map_t::iterator, bool> insertresult = readyCmds.insert(FilepathScanner::cmd_map_t::value_type(timewindow4cmd, input_map_iter->second));
                    if (!insertresult.second) {
                        MLOG_ERROR << "GetReadyCmds insert  failure" << endl;
                    } else {
                        //                        MLOG_DEBUG << "readyCmds insert  succ" << endl;
                    }
                    scannedcmds.erase(input_map_iter++);
                    continue;
                }
            }
            input_map_iter++;
        }
    }

    void ScanCmdPath(cep::FilepathScanner::Config4ReadCmdFilepath&config4Read,
            cep::FilepathScanner::cmd_map_t &scannedcmds, set<string>&loadedCmds_,
            const cep::event::type_t eventType, const ushort cycle) {
        boost::system::error_code ec;
        // boost::filesystem::directory_iterator begin(config4Read.src, ec);
        boost::filesystem::recursive_directory_iterator begin(config4Read.src, ec);
        if (ec) {
            MLOG_WARN << "FilepathScanner::scanning not found ScanCmdPath file path["
                    << config4Read.src << "]:" << boost::system::system_error(ec).what() << endl;
            return;
        }
        // boost::filesystem::directory_iterator end;
        boost::filesystem::recursive_directory_iterator end;
        const ulong mscycle = cycle * 60000;
        for (; begin != end; ++begin) {
            if (boost::filesystem::is_directory(*begin)) continue;
            string filename = begin->path().filename().native();
            // string fullpath = config4Read.src + "/" + filename;
            string fullpath = begin->path().string();
            // cache the no timeout cmd event and it cant modify correct cmd!
            if (boost::filesystem::extension(begin->path()) == ".cmd" && !loadedCmds_.count(fullpath)) {
                // cmdType has been check when reload, constructor a cmd event.
                cep::FilepathScanner::CmdFileReader *cmdFileReader = new cep::FilepathScanner::CmdFileReader();
                if (cmdFileReader->init(config4Read, filename)) {
                    if (cycle == cmdFileReader->cmd().cycle && eventType == cmdFileReader->cmd().event_type
                            && cmdFileReader->open()/* add by wangqun */) {
                        ulong win4time = cmdFileReader->cmd().time_window / mscycle;
                        //                        MLOG_DEBUG << "cycle[" << cycle << "] type[" << eventType << "] timewindow[" << win4time << "]" << endl;
                        insert2map(scannedcmds, win4time, cmdFileReader);
                        std::pair < set<string>::iterator, bool> insertresult = loadedCmds_.insert(fullpath);
                        if (!insertresult.second) {
                            MLOG_ERROR << "FilepathScanner::scanning ScanCmdPath loadedCmds insert [" << fullpath << "] failure!" << endl;
                        }
                    } else {
                        //                        MLOG_DEBUG << "cycle cmd[" << cmdFileReader->cmd().cycle <<
                        //                                "] or cmd type[" << cmdFileReader->cmd().event_type <<
                        //                                "] not equal cycle config[" << cycle << "] type config[" << eventType << "]" << endl;
                        delete cmdFileReader;
                    }
                } else {
                    MLOG_ERROR << "FilepathScanner::CmdFileReader::init failure filepath[" << config4Read.src << "] filename[" << filename << "]" << endl;
                    delete cmdFileReader;
                }
            }
        }
    }

    /**
     * used to load month statistic cmds.
     * @param config4Read desription for input cmd, such cmd's cmd_type,src,archive
     * @param scannedcmds used to store the cmds that loaded to memory.      
     * @param loadedCmds_ used to store the cmds that loaded to memory, for unique.   
     * @param eventType the cmd type will be loaded.
     * @param cycle statistic cycle, 1 for one month, 2 for 2 months.
     * @statisticFrequency statistic frequency, unit is day.
     */
    void ScanCmdPathForMonth(cep::FilepathScanner::Config4ReadCmdFilepath & config4Read,
            cep::FilepathScanner::cmd_map2map_t & scannedcmds, set<string> & loadedCmds,
            const cep::event::type_t eventType, const ushort cycle, const int statisticFrequency) {
        boost::system::error_code ec;
        // boost::filesystem::directory_iterator begin(config4Read.src, ec);
        boost::filesystem::recursive_directory_iterator begin(config4Read.src, ec);
        if (ec) {
            MLOG_WARN << "FilepathScanner::scanning not found ScanCmdPathForMonth file path["
                    << config4Read.src << "]:" << boost::system::system_error(ec).what() << endl;
            return;
        }
        // boost::filesystem::directory_iterator end;
        boost::filesystem::recursive_directory_iterator end;
        for (; begin != end; begin++) {
            if (boost::filesystem::is_directory(*begin)) continue;
            string filename = begin->path().filename().native();
            // string fullpath = config4Read.src + "/" + filename;
            string fullpath = begin->path().string();
            // cache the no timeout cmd event and it cant modify correct cmd!
            if (boost::filesystem::extension(begin->path()) == ".cmd" && !loadedCmds.count(fullpath)) {
                // cmdType has been check when reload, constructor a cmd event.
                cep::FilepathScanner::CmdFileReader *cmdFileReader = new cep::FilepathScanner::CmdFileReader();
                if (cmdFileReader->init(config4Read, filename)) {
                    if (cycle * 10000 == cmdFileReader->cmd().cycle && eventType == cmdFileReader->cmd().event_type
                            && cmdFileReader->open()) {
                        // get month statistic time slice. month_time_slice will be like: 20120815
                        ulong month_time_slice = dateTime2MonthTime(cmdFileReader->cmd().time_window);
                        //   MLOG_DEBUG << "month_time_slice: " << month_time_slice << endl;
                        ulong month_milscd_key = getMonthMilscdKey(month_time_slice, cycle);
                        //    MLOG_DEBUG << "month_milscd_key: " << month_milscd_key << endl;
                        // int month_key = getMonthKey(month_time_slice, cycle);
                        // string month_timeout_str = getMonthTimeoutTime(month_key, cycle);
                        // ulong month_timeout_key = cep::cstr2milsecond(month_timeout_str.c_str(), "%4d-%02d-%02d %02d:%02d:%02d.%03d");

                        ulong date_key = 0;
                        // If config statisticFrequency
                        if (statisticFrequency > 0) {
                            date_key = getDateMilscdKey(cmdFileReader->cmd().time_window, statisticFrequency);
                            // date_timeout will be the string like: 2012-08-15 00:00:00.000, this is the next time window of the currrent one.
                            // string date_timeout_str = getDayFrequencyTimeout(dateKey, statisticFrequency);
                            // date_timeout_key = cep::cstr2milsecond(date_timeout_str.c_str(), "%4d-%02d-%02d %02d:%02d:%02d.%03d");
                        } else {
                            date_key = month_milscd_key;
                        }

                        //     MLOG_DEBUG << "date_key: " << date_key << endl;
                        cep::FilepathScanner::cmd_map2map_t::iterator found = scannedcmds.find(month_milscd_key);
                        // if this key has a value, then get it,
                        if (found != scannedcmds.end()) {
                            insert2map(found->second, date_key, cmdFileReader);
                        } else {
                            cep::FilepathScanner::cmd_map_t cmd_map;
                            insert2map(cmd_map, date_key, cmdFileReader);
                            scannedcmds.insert(cep::FilepathScanner::cmd_map2map_t::value_type(month_milscd_key, cmd_map));
                        }

                        std::pair < set<string>::iterator, bool> insertresult = loadedCmds.insert(fullpath);
                        if (!insertresult.second) {
                            MLOG_ERROR << "FilepathScanner::scanning ScanCmdPath loadedCmds insert [" << fullpath << "] failure!" << endl;
                        }
                    } else {
                        delete cmdFileReader;
                    }
                } else {
                    MLOG_ERROR << "FilepathScanner::CmdFileReader::init failure filepath[" << config4Read.src << "] filename[" << filename << "]" << endl;
                    delete cmdFileReader;
                }
            }
        }
    }

    /**
     * 
     * @param scannedcmds 根据类型和cycle扫描好的vector<CmdFileReader*>集合
     * @param msTimeOut 设置的超时（毫秒数）
     * @param diffms4timezone 时区的差别（毫秒数）
     * @param readyCmds 返回值：满足了超时规则的vector<CmdFileReader*>集合
     */
    void GetReadyCmdsForMonth(FilepathScanner::cmd_map2map_t & scannedcmds,
            const long msTimeOut, const long diffms4timezone,
            FilepathScanner::cmd_map_t& readyCmds, const ushort cycle, const int statisticFrequency) {
        readyCmds.clear();
        FilepathScanner::cmd_map2map_t::iterator outside_begin = scannedcmds.begin();
        const ulong sysms = processing_utils::systime_msec();
        while (outside_begin != scannedcmds.end()) {
            string time_out_str = getMonthTimeoutTime(getMonthKey(dateTime2MonthTime(outside_begin->first), cycle), cycle);
            ulong out_timeout = cep::cstr2milsecond(time_out_str.c_str(), "%4d-%02d-%02d %02d:%02d:%02d") + msTimeOut;
            // The whole map is out of time, then get all cmds for the current key.
            if (out_timeout < sysms) {
                readyCmds.insert(outside_begin->second.begin(), outside_begin->second.end());
                scannedcmds.erase(outside_begin++);
            } else {
                FilepathScanner::cmd_map_t::iterator inside_begin = outside_begin->second.begin();

                while (inside_begin != outside_begin->second.end()) {

                    ulong mintimestamp4cmd = -1;
                    FilepathScanner::cmds_t::iterator input_vec_iter = inside_begin->second.begin();
                    // 得到这一批文件最先生成的timestamp
                    for (; input_vec_iter != inside_begin->second.end(); input_vec_iter++) {
                        ulong timestamp4cmd = (*input_vec_iter)->cmd().timestamp;
                        if (timestamp4cmd < mintimestamp4cmd) {
                            mintimestamp4cmd = timestamp4cmd;
                        }
                    }

                    time_out_str = getDayFrequencyTimeout(getDateKey(inside_begin->first, statisticFrequency), statisticFrequency);
                    ulong int_timeout = cep::cstr2milsecond(time_out_str .c_str(), "%4d-%02d-%02d %02d:%02d:%02d") + msTimeOut;
                    if (int_timeout < sysms) {
                        if (mintimestamp4cmd + msTimeOut < sysms) {
                            // 设置当前cmd为中间结果的cmd，这样，输出的cmd将会在输入的cmd目录下
                            // for (int i = 0; i < inside_begin->second.size(); ++i) {
                            //   inside_begin->second[i]->setInternalCmdFlag(true);
                            // }

                            std::pair < FilepathScanner::cmd_map_t::iterator, bool> insertresult
                                    = readyCmds.insert(FilepathScanner::cmd_map_t::value_type(inside_begin->first, inside_begin->second));
                            if (!insertresult.second) {
                                MLOG_ERROR << "GetReadyCmds insert  failure" << endl;
                            }
                            outside_begin->second.erase(inside_begin++);
                        }
                    } else {
                        inside_begin++;
                    }
                }

                outside_begin++;
            }
        }
    }

    void FilepathScanner::scanning(cmd_map_t & timeWindowMapCmdFileReaders) {
        if (!isScanForMonth_) {
            for (int i = 0; i<this->cmdFilePath_.size(); i++) {
                ScanCmdPath(cmdFilePath_.at(i), scanedCmdMap_, loadedCmds_, scanCmdEventType_, scanCmdCycle_);
            }
            GetReadyCmds(scanedCmdMap_, msTimeOut_, diffms4timezone_, timeWindowMapCmdFileReaders, scanCmdCycle_);
        } else {
            for (int i = 0; i<this->cmdFilePath_.size(); i++) {
                ScanCmdPathForMonth(cmdFilePath_.at(i), scanedCmdMap4Month_, loadedCmds_,
                        scanCmdEventType_, scanCmdCycle_, statisticFrequency_);
            }
            GetReadyCmdsForMonth(scanedCmdMap4Month_, msTimeOut_, diffms4timezone_,
                    timeWindowMapCmdFileReaders, scanCmdCycle_, statisticFrequency_);
        }
    }

    void FilepathScanner::archive(cmds_t & cmdFileReaders) {
        const size_t size = cmdFileReaders.size();
        CmdFileReader* cmd;
        for (int i = 0; i < size; ++i) {

            cmd = cmdFileReaders[i];
            cmd->archive();
            loadedCmds_.erase(cmd->filepath()->src + "/" + cmd->filename());
            delete cmd;
        }
        cmdFileReaders.clear();
    }

    void FilepathScanner::close(cmds_t & cmdFileReaders) {
        const size_t size = cmdFileReaders.size();
        CmdFileReader* cmd;
        for (int i = 0; i < size; ++i) {
            cmd = cmdFileReaders[i];
            loadedCmds_.erase(cmd->filepath()->src + "/" + cmd->filename());
            delete cmd;
        }
        cmdFileReaders.clear();
    }

    ostream& operator<<(ostream& s, const FilepathScanner & r) {
        s << "FilepathScanner@" << &r << " scanCmdEventType["
                << r.scanCmdEventType_ << "] scanCmdCycle["
                << r.scanCmdCycle_ << "] msTimeOut["
                << r.msTimeOut_ << "] diffms4timezone["
                << r.diffms4timezone_ << "] secScanCycle["
                << r.secScanCycle_ << "] and cmdFilePath has:";
        const size_t size = r.cmdFilePath_.size();
        for (int i = 0; i < size; ++i)
            s << r.cmdFilePath_[i].cmdType << ':' << r.cmdFilePath_[i].src << "->"
                << r.cmdFilePath_[i].archive << "!";
        return s;
    }
    ////////////////////////////////////////////////////////////////////////////

    FileOutputter::FileOutputter()
    : closed_(true), txtfile_(NULL), lzofile_(NULL), cmdMetadata_(NULL), cmd_(NULL)
    , timestamp4speed_(0), counter4speed_(0), total4speed_(0), cmdPaths_(), diffms4timezone_(-28800 * 1000)//, isInternalCmd_(false)
    {
        //        MLOG_DEBUG << this << "->FileOutputter ctor execute!" << endl;
        // Set the write file mod 777
        umask(0);
        filenames_[0] = 0;
    }

    FileOutputter::~FileOutputter() {
        close();
        //        MLOG_DEBUG << this << "->FileOutputter dtor execute!" << endl;
    }

    bool FileOutputter::open(event::type_t cmdType, size_t maxSize,
            const string& filepath, const string& suffix4filename,
            pn_id_t pnId, pe_id_t peId, event::type_t eventType, time_t win4time//, const bool isInternalCmd
            ) {
        if (!closed_) return true;

        if (config_metadata::instance().metadata(eventType) == NULL) {
            MLOG_ERROR << "FileOutputter@" << this << " open error event type:" << eventType << endl;
            return false;
        }
        cmdMetadata_ = config_metadata::instance().metadata(cmdType);
        if (cmdMetadata_ == NULL) {
            MLOG_ERROR << "FileOutputter@" << this << " open error cmd type:" << cmdType << endl;
            return false;
        }
        if (cmdType == file_lzo_cmd)
            lzofile_ = new LZOFileWriter;
        else if (cmdType == file_cvs_cmd)
            txtfile_ = new event2text;
        else {
            MLOG_ERROR << "FileOutputter@" << this << " open unsupported cmd type:" << cmdType << endl;
            return false;
        }
        // Then, it must be OK!
        cmdType_ = cmdType;
        cmd_ = new event(cmdMetadata_, cmdType_);

        maxSize_ = maxSize;
        memset(filenames_, 0, sizeof (filenames_));
        // filename_;
        suffix4filename_ = suffix4filename;
        eventType_ = eventType;

        time_t ms = processing_utils::systime_msec();
        sprintf(filenames_, "%hu_%hu_%hu_%s_%lu_%%d", pnId, peId, eventType_,
                cep::milsecond2str(win4time, "%4d%02d%02d%02d%02d").c_str(), ms);

        // add eventType_/pnId/system to filepath_, modified by lxy 2012/09/10
        filepath_ = filepath;
        char temp[128] = {0};
        sprintf(temp, "/%hu/%hu/%s", eventType_, pnId, cep::milsecond2str(ms - diffms4timezone_, "%4d%02d%02d").c_str());
        filepath_.append(temp);

        file_cmd* structCmd = (file_cmd*) cmd_->data();
        structCmd->pe_id = peId;
        structCmd->event_type = eventType_;
        structCmd->time_window = win4time;
        structCmd->timestamp = ms;

        boost::system::error_code ec;
        // maintenance file path
        boost::filesystem::path filePath(filepath_);
        if (!boost::filesystem::exists(filePath, ec)) {
            while (!boost::filesystem::create_directories(filePath, ec)) {
            };
        }
        // maintenance cmd path
        for (int i = cmdPaths_.size() - 1; i >= 0; --i) {
            boost::filesystem::path filePath2(cmdPaths_[i].path);
            if (!boost::filesystem::exists(filePath2, ec)) {
                while (!boost::filesystem::create_directories(filePath2, ec)) {
                };
            }
        }

        counter_ = 0;
        cnt4file_ = 0;
        fileCounter_ = 0;
        counter4speed_ = 0;
        //  isInternalCmd_ = isInternalCmd;
        closed_ = false;
        return true;
    }

    void PostOutput2File(const string& filename, const string& filepath,
            const string& suffix4filename, event& cmd,
            const vector<FileOutputter::CmdPath>& cmdPaths) {
        string strFilePathWithoutSuffix = filepath + "/" + filename;
        string strFilePathWithSuffix = strFilePathWithoutSuffix + "." + suffix4filename;
        // success return 0, else a nonzero value is returned
        if (::rename(strFilePathWithoutSuffix.c_str(), strFilePathWithSuffix.c_str()) != 0) {
            MLOG_ERROR << "FileOutputter finish to write[" << strFilePathWithoutSuffix
                    << "] and failure rename to:" << strFilePathWithSuffix
                    << ". errno[" << errno << "]" << endl;
        } else {
            const int cnt4filelock = cmdPaths.size();
            int fileLockCounter = 0;
            for (int i = 0; i < cnt4filelock; ++i) {
                string strCmdFilePath = cmdPaths[i].path + "/" + filename;
                cmd.set_value("file_path", slice(strFilePathWithSuffix));
                cmd.set_value("cycle", cmdPaths[i].cycle);
                event2text cmdFile;
                cmdFile.open(strCmdFilePath.c_str(), "w");
                if (!cmdFile.write(cmd)) {
                    MLOG_ERROR << "FileOutputter write cmd to:" << strCmdFilePath
                            << " failure!" << endl;
                } else {
                    if (cmdFile.rename(strCmdFilePath + ".cmd") != 0) {
                        MLOG_ERROR << "FileOutputter finish to write " << strCmdFilePath
                                << ".cmd failure!" << endl;
                    } else
                        ++fileLockCounter;
                }
            }
            if (fileLockCounter > 0) {
                file_lock_counter<int> flc;
                string strLockFilePath = strFilePathWithSuffix + ".lock";
                if (!flc.reset(strLockFilePath, fileLockCounter)) {
                    MLOG_ERROR << "FileOutputter write " << strLockFilePath
                            << " and reset counter to " << fileLockCounter
                            << " failure!" << endl;
                }
            }
        }
    }

    void FileOutputter::write(event & evt) {
        if (closed_ || evt.type() != eventType_) return;

        if (cmdType_ == file_cvs_cmd) {
            if (counter_ == 0 || cnt4file_ == maxSize_) {
                if (counter_ != 0) {
                    txtfile_->close();
                    // if (!isInternalCmd_) { // this is the last result cmds
                    PostOutput2File(filename_, filepath_, suffix4filename_, *cmd_, cmdPaths_);
                    //  } else {
                    //      vector<CmdPath> inputPaths;
                    //      inputPaths.push_back(oneInputCmdPath_);
                    //      PostOutput2File(filename_, filepath_, suffix4filename_, *cmd_, inputPaths);
                    //  }
                }
                char filenames[128] = {0};
                sprintf(filenames, filenames_, fileCounter_);
                filename_ = filenames;
                if (!txtfile_->open((filepath_ + "/" + filename_).c_str(), "w")) {
                    MLOG_WARN << "FileOutputter@" << this << " write cvs file[" << (filepath_ + "/" + filename_)
                            << "] failure." << endl;
                    return;
                }

                cnt4file_ = 0;
                ++fileCounter_;
            }
            if (!txtfile_->write(evt)) {
                MLOG_ERROR << "FileOutputter@" << this << " write " << counter_ << " events to file["
                        << filepath_ << '/' << filename_ << '.' << suffix4filename_
                        << "]:" << fileCounter_ << " error!" << endl;
                return;
            }
            ++counter_;
            ++cnt4file_;
            ++counter4speed_;
            ++total4speed_;
        } else if (cmdType_ == file_lzo_cmd) {
            if (counter_ == 0 || cnt4file_ == maxSize_) {
                if (counter_ != 0) {
                    lzofile_->Close();
                    //  if (!isInternalCmd_) {
                    PostOutput2File(filename_, filepath_, suffix4filename_, *cmd_, cmdPaths_);
                    //  } else {
                    //      vector<CmdPath> inputPaths;
                    //      inputPaths.push_back(oneInputCmdPath_);
                    //      PostOutput2File(filename_, filepath_, suffix4filename_, *cmd_, inputPaths);
                    //  }
                }
                char filenames[128] = {0};
                sprintf(filenames, filenames_, fileCounter_);
                filename_ = filenames;
                // txtfile_->open((filepath_ + "/" + filename_).c_str(), "w");
                if (lzofile_->Init(fopen((filepath_ + "/" + filename_).c_str(), "w")) < 0) {
                    MLOG_WARN << "FileOutputter@" << this << " write lzo file[" << (filepath_ + "/" + filename_)
                            << "] failure." << endl;
                    return;
                };

                cnt4file_ = 0;
                ++fileCounter_;
            }
            // if (!txtfile_->write(evt)) {
            // if (lzofile_->Write(evt.data(), evt.len(), 1) == -1) { // assert(cnt != -1);
            if (fwrite(evt.data(), evt.len(), 1, *lzofile_) == -1) { // assert(cnt != -1);
                MLOG_ERROR << "FileOutputter@" << this << " write " << counter_ << " events to file["
                        << filepath_ << '/' << filename_ << '.' << suffix4filename_
                        << "]:" << fileCounter_ << " error!" << endl;
                return;
            }
            ++counter_;
            ++cnt4file_;
            ++counter4speed_;
            ++total4speed_;
        } else {
            MLOG_ERROR << "FileOutputter@" << this << " write " << counter_ << " events to file["
                    << filepath_ << '/' << filenames_ << '.' << suffix4filename_
                    << "] format[" << cmdType_ << "] unsupported!" << endl;
        }
    }

    void FileOutputter::close() {
        if (closed_) return;
        if (cmdType_ == file_cvs_cmd) {
            txtfile_->close();
            if (counter_ != 0)
                PostOutput2File(filename_, filepath_, suffix4filename_, *cmd_, cmdPaths_);
            delete txtfile_;
            txtfile_ = NULL;
        } else if (cmdType_ == file_lzo_cmd) {
            lzofile_->Close();
            if (counter_ != 0)
                PostOutput2File(filename_, filepath_, suffix4filename_, *cmd_, cmdPaths_);
            delete lzofile_;
            lzofile_ = NULL;
        }

        delete cmd_;
        cmd_ = NULL;
        filename_ = "";
        closed_ = true;
    }

    void FileOutputter::logging() const {
        MLOG_INFO << "FileOutputter@" << this << " to [" << filenames_ << "->" << filename_ << "] speed is:"
                << ((double) counter4speed_ * 1000 / (processing_utils::systime_msec() - timestamp4speed_))
                << '=' << counter4speed_ << '/' << total4speed_ << endl;
        timestamp4speed_ = processing_utils::systime_msec();
        counter4speed_ = 0;
    }

    event2text * FileOutputter::txtfile() {
        return txtfile_;
    }
}
