/* 
 * File:   idx_trie_tree.hpp
 * Author: luoxiaoyi
 *
 * Created on 2012年2月27日, 下午1:01
 */

#ifndef IDX_TRIE_TREE_HPP
#define	IDX_TRIE_TREE_HPP

#include "const_dataset.hpp"
#include "macro_log.hpp"
#include "buffer_function.hpp"
#include "trie_map_tree.hpp"
#include "forest_map.h"

namespace cep {

    class idx_trie_tree : public const_dataset::index {
    public:
        static const char ALL_MATCH_CHAR = '*';
        static const char EXT_SUFFIX_CHAR = '.';

        /**
         * 构造索引
         * @param id 索引的唯一标识
         * @param keys 需要建立索引的列名
         * @param tab 需要建立的表
         * @param isNeedReverse 对维表数据建立trie树的时候，是否需要首先将所有数据首位颠倒，如：*.baidu.com 变成 moc.udiab.*
         */
        idx_trie_tree(const string& id, const std::vector<string>& keys, const const_dataset::table& tab, const bool isNeedReverse)
        : index(id, keys, tab), isNeedReverse_(isNeedReverse), isAllLike_(false) {
            MLOG_DEBUG << "idx_trie_tree constructor invoked..." << endl;
        }

        idx_trie_tree(const string& id, const std::vector<string>& keys, const const_dataset::table& tab)
        : index(id, keys, tab), isNeedReverse_(false), isAllLike_(true) {
            MLOG_DEBUG << "idx_trie_tree constructor invoked..." << endl;
        }

        virtual ~idx_trie_tree() {
            MLOG_DEBUG << "idx_trie_tree de-constructor invoked..." << endl;
        }

        /**
         * 索引的创建
         * @return 创建成功返回true，否则返回false
         */
        virtual bool rebuild();

        /**
         * 用于从索引中查询表中的有关信息
         * @param where 建立索引时的使用的type_slice
         * @param func 用于回调和处理找到的表的行
         */
        virtual void select(const type_slice where[], iterator& func) const;
    private:
        TrieMapTree trieMapTree_;
        ForestMap forestMap_;
        // 标识对维表数据建立trie树的时候，是否需要首先将所有数据首位颠倒，如：*.baidu.com 变成 moc.udiab.*
        // 若为true，则需要，否则不需要
        bool isNeedReverse_;
        bool isAllLike_;
    };
}

#endif	/* IDX_TRIE_TREE_HPP */
