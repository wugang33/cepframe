/* 
 * File:   idx_for_ip_range.hpp
 * Author: Administrator
 *
 * Created on 2012年2月21日, 上午10:47
 */

#ifndef IDX_FOR_IP_RANGE_HPP
#define	IDX_FOR_IP_RANGE_HPP

#include "const_dataset.hpp"
#include "macro_log.hpp"
#include "buffer_function.hpp"
#include <set>

namespace cep {
    class dynamic_struct;

    class idx_range : public const_dataset::index {
    public:

        /**
         * 构造索引
         * @param id 索引的唯一标识
         * @param keys 需要建立索引的列名
         * @param tab 需要建立的表
         * @param  
         */
        idx_range(const string& id, const std::vector<string>& keys, const const_dataset::table& tab) : index(id, keys, tab),range_element_set_() {
            MLOG_TRACE << "cep::idx_range constructor invoked..." << endl;
        }

        virtual ~idx_range() {
            MLOG_TRACE << "cep::idx_range de-constructor invoked..." << endl;
        }

        /**
         * 索引的创建
         * @return 创建成功返回true，否则返回false
         */
        virtual bool rebuild();
        
        /**
         * 
         * @param row 这里的row必须是和索引所在的表的row保持一样的结构
         * @param func 用于回调和处理找到的表的行
         */
      //  virtual void select(const dynamic_struct& row, const_dataset::index::iterator& func) const;
       /**
        * 用于从索引中查询表中的有关信息
        * @param where 建立索引时的使用的type_slice
        * @param func 用于回调和处理找到的表的行
        */
        virtual void select(const type_slice where[], iterator& func) const;
    protected:
        // 用于存储 table 下标的数组
        typedef std::vector<const_dataset::table::rows_t::size_type> TabIdxVector;

        struct range_element {
            cep::type_slice typeSlice; // 数据字段
            TabIdxVector self, iset4prev; // 存放索引，也就是table中row的下标
        };

        // 用于在set里面进行比较

        struct less_seg : public std::less<range_element> {

            bool operator()(const range_element& ls, const range_element & rs) const{
                return cep::logic_function::less(ls.typeSlice, rs.typeSlice);
            }
        };

        typedef  std::set<range_element, less_seg> rangeSet;
    private:
        // 用于保存所有索引字段的值，同时排重，并按照 less_seg 定义的方法进行排序
        rangeSet range_element_set_;
    };
}
#endif	/* IDX_FOR_IP_RANGE_HPP */
