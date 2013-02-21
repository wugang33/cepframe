/* 
 * File:   idx_trie_tree.cpp
 * Author: luoxiaoyi
 * 
 * Created on 2012年2月27日, 下午1:01
 */

#include "processing/idx_trie_tree.hpp"
#include "buffer_metadata.hpp"
#include "dynamic_struct.hpp"
#include <algorithm>

namespace cep {

    bool idx_trie_tree::rebuild() {
        if (size_ < 1) {
            MLOG_ERROR << "idx_trie_tree::rebuild ERROR; Index Key words can not be empty! " << endl;
            return false;
        }

        int size = table_->size();
        cep::slice defalutSlice, tempSlice;
        if (this->isAllLike_) {
            forestMap_.cleanAll();
            // 便利数据表的所有元素，并建立相关索引,支持*xx*xx*匹配
            for (int i = 0; i < size; ++i) {
                tempSlice = table_->get(i)->value(idx_keys_[0], defalutSlice);
                string data = string(tempSlice.to_string().c_str());
                if (data.size() < 1) {
                    // maybe error or
                    // empty string is not as a index value! add by waq@2012-06-15
                    continue;
                }
                forestMap_.buildForest(data, i);
            }
        } else {
            trieMapTree_.cleanAll();
            if (isNeedReverse_) {
                for (int i = 0; i < size; ++i) {
                    tempSlice = table_->get(i)->value(idx_keys_[0], defalutSlice);
                    string data = string(tempSlice.to_string().c_str());
                    if (data.size() < 1) {
                        // maybe error or
                        // empty string is not as a index value! add by waq@2012-06-15
                        continue;
                    }
                    // 因为之前局方给的数据是类似：*.qq.com,*.sina.com.cn之类的维表数据，所以，进行reverse操作，把*替换到字符串最后
                    reverse(data.begin(), data.end());
                    trieMapTree_.build(data, i);
                }
            } else {
                for (int i = 0; i < size; ++i) {
                    tempSlice = table_->get(i)->value(idx_keys_[0], defalutSlice);
                    string data = string(tempSlice.to_string().c_str());
                    if (data.size() < 1) {
                        // maybe error or
                        // empty string is not as a index value! add by waq@2012-06-15
                        continue;
                    }
                    trieMapTree_.build(data, i);
                }
            }
        }

        return true;
    }

    void idx_trie_tree::select(const cep::type_slice where[], iterator& func) const {
        string data = string(where[0].data.to_string().c_str());

        vector<int> rst;
        if (isAllLike_) { // 全匹配
            forestMap_.match(data, rst);
        } else {
            // 因为在创建索引的时候对字符串进行了反转操作，所以，这个地方进行查询时，进行同样的操作
            if (isNeedReverse_)
                reverse(data.begin(), data.end());
            // 匹配所有表中可能匹配的记录，并得到记录在表中的下标
            rst = trieMapTree_.match(data, ALL_MATCH_CHAR, EXT_SUFFIX_CHAR);
        }

        int size = rst.size();
        for (int i = 0; i < size; ++i) {
            if (!func(*(table_->get(rst.at(i))), i))
                break;
        }
    }
}
