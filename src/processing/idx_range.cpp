/* 
 * File:   idx_for_ip_range.cpp
 * Author: Administrator
 * 
 * Created on 2012年2月21日, 上午10:47
 */

#include "processing/idx_range.hpp"
#include "buffer_metadata.hpp"
#include "dynamic_struct.hpp"
#include <algorithm>

namespace cep {

    bool idx_range::rebuild() {
        // 范围索引必须建立在两个以上的列上面
        if (size_ != 2) {
            MLOG_ERROR << "idx_range::rebuild --> range index must be created on two columns..." << endl;
            return false;
        }

        cep::field_cfg_entry::data_t type;
        // 如果两列的类型不匹配，不建立相关索引
        //table_->metadata().get_cfg_entry(*idx_keys_)->type
        if ((type = table_->metadata().get_cfg_entry(idx_keys_[0])->type())
                != table_->metadata().get_cfg_entry(idx_keys_[1])->type()) {
            MLOG_ERROR << "idx_range::rebuild --> range index must be created on the same type columns..." << endl;
            return false;
        }

        cep::type_slice typeSlice, typeSlice2;
        cep::slice defalutSlice;
        typeSlice.type = type;
        typeSlice2.type = type;
        const const_dataset::table::rows_t::size_type tableRows = table_->size();
        // 将表中所有行对应的两列值放入 range_element_set_ 集合，O(n)的时间复杂度
        range_element element;
        set<int> rowIdx;
        for (const_dataset::table::rows_t::size_type i = 0; i < tableRows; ++i) {
            // 将第一、二列插入range_element_set_集合
            /*for (int j = 0; j < size_; ++j) {
                 typeSlice.data = table_->get(i)->value(idx_keys_[j], defalutSlice);
                 element.typeSlice = typeSlice;
                 range_element_set_.insert(element);
             }*/
            typeSlice.data = table_->get(i)->value(idx_keys_[0], defalutSlice);
            typeSlice2.data = table_->get(i)->value(idx_keys_[1], defalutSlice);
            if (typeSlice <= typeSlice2) {
                element.typeSlice = typeSlice;
                range_element_set_.insert(element);
                element.typeSlice = typeSlice2;
                range_element_set_.insert(element);
            } else {
                rowIdx.insert(i);
                MLOG_WARN << "idx_range::rebuild first element:" << typeSlice << " > second element:" << typeSlice2 << "" << endl;
                continue;
            }
        }

        // 对 range_element_set_ 中的每个元素的 self 进行设值 O(nlogn)
        rangeSet::iterator lower, upper;
        for (const_dataset::table::rows_t::size_type i = 0; i < tableRows; ++i) {
            if(rowIdx.count(i) != 0) continue;
            typeSlice.data = table_->get(i)->value(idx_keys_[0], defalutSlice);
            element.typeSlice = typeSlice;
            lower = range_element_set_.lower_bound(element);

            typeSlice.data = table_->get(i)->value(idx_keys_[1], defalutSlice);
            element.typeSlice = typeSlice;
            upper = range_element_set_.upper_bound(element);
            // 开始设置 range_element_set_ 中的每个元素的 self
            if (lower != range_element_set_.end()) {
                while (lower != range_element_set_.end() && lower != upper) {
                    // 由于set里面的元素都被设置成const，所以，如果要修改，必须进行转换，
                    // 这里要注意的是，如果要强行修改，则必须保证修改之后，对set里面的元素的顺序不会造成影响
                    // Effective C++ 条款22：避免原地修改set和multiset的键
                    const_cast<range_element&> (*lower).self.push_back(i);
                    lower++;
                }
            }
        }

        // 求range_element_set_中所有元素的 iset4prev 值
        lower = range_element_set_.begin();
        upper = range_element_set_.end();
        TabIdxVector::iterator resultEnd;
        rangeSet::iterator prev;
        // 第一个元素的iset4prev肯定是空值
        if (lower != upper) {
            const_cast<range_element&> (*lower).iset4prev.clear();
            prev = lower; // 指向第一个元素
            lower++; // 指向第一个元素的后一个元素
            while (lower != upper) {
                // 对lower和lower-1的两个元素的self数组求交集，并将结果存放在lower的iset4prev里面
                const_cast<range_element&> (*lower).iset4prev.resize(lower->self.size() > prev->self.size() ? lower->self.size() : prev->self.size());
                resultEnd = set_intersection(lower->self.begin(), lower->self.end(),
                        prev->self.begin(), prev->self.end(), const_cast<range_element&> (*lower).iset4prev.begin());
                const_cast<range_element&> (*lower).iset4prev.resize(resultEnd - lower->iset4prev.begin());

                prev = lower;
                lower++;
            }
        }

        return true;
    } // end of rebuild

    void idx_range::select(const cep::type_slice where[], iterator& func) const {
        // 如果传入的查询参数类型与索引的数据类型不匹配
        if (table_->metadata().get_cfg_entry(idx_keys_[0])->type() != where[0].type) {
            MLOG_ERROR << "idx_range::select ERROR, Maybe you have affernted wrong parameters in ROW!!!" << endl;
            return;
        }

        range_element element;
        element.typeSlice = where[0];

        rangeSet::const_iterator result = range_element_set_.lower_bound(element);

        if (result != range_element_set_.end()) { // 说明已经找到
            const_dataset::table::rows_t::size_type rownum = 0;
            if (result->typeSlice.data == element.typeSlice.data) { // result->typeSlice.self
                TabIdxVector::const_iterator begin = result->self.begin(), end = result->self.end();
                for (; begin != end; begin++, ++rownum) {
                    if (!func(*(table_->get(*begin)), rownum)) // rownum 给调用者作为参考来返回 true or false
                        break;
                }
            } else { // result->typeSlice.iset4prev
                TabIdxVector::const_iterator begin = result->iset4prev.begin(), end = result->iset4prev.end();
                for (; begin != end; begin++, ++rownum) {
                    if (!func(*(table_->get(*begin)), rownum)) // rownum 给调用者作为参考来返回 true or false
                        break;
                }
            }
        }
    }

    /*
    void idx_range::select(const dynamic_struct& row, const_dataset::index::iterator& func) const {
        // 默认从idx_keys_的第一个下标中取得对应的值
        size_t index = idx_keys_[0];
        const cep::field_cfg_entry* entry = row.metadata()->get_cfg_entry(idx_keys_[0]);
        if (!entry) {
            entry = row.metadata()->get_cfg_entry(idx_keys_[1]);
            index = idx_keys_[1];
        }

        if (!entry) { // 可能是因为row与索引所在的表对应的列没有设置相应的值所造成的
            MLOG_ERROR << "idx_range::select ERROR, Maybe you do not set the corresponding value in ROW!!!" << endl;
            return;
        }

        // 如果传入的查询参数类型与索引的数据类型不匹配
        if (table_->metadata().get_cfg_entry(index)->type != entry->type) {
            MLOG_ERROR << "idx_range::select ERROR, Maybe you have affernted wrong parameters in ROW!!!" << endl;
            return;
        }

        // 准备在range_element_set_里面查找需要的表的行
        cep::type_slice typeSlice;
        cep::slice defalutSlice;
        range_element element;

        typeSlice.data = row.value(index, defalutSlice);
        typeSlice.type = entry->type;
        element.typeSlice = typeSlice;

        rangeSet::const_iterator result = range_element_set_.lower_bound(element);

        if (result != range_element_set_.end()) { // 说明已经找到
            const_dataset::table::rows_t::size_type rownum = 0;
            if (result->typeSlice.data == element.typeSlice.data) { // result->typeSlice.self
                TabIdxVector::const_iterator begin = result->self.begin(), end = result->self.end();
                for (; begin != end; begin++, ++rownum) {
                    if (!func(*(table_->get(*begin)), rownum)) // rownum 给调用者作为参考来返回 true or false
                        break;
                }
            } else { // result->typeSlice.iset4prev
                TabIdxVector::const_iterator begin = result->iset4prev.begin(), end = result->iset4prev.end();
                for (; begin != end; begin++, ++rownum) {
                    if (!func(*(table_->get(*begin)), rownum)) // rownum 给调用者作为参考来返回 true or false
                        break;
                }
            }
        }

    }*/
}
