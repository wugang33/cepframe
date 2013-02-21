/* 
 * File:   trie_map_tree.hpp
 * forest_map.h
 * Author: wugang
 *
 * Created on 2011-08-14 AM 10:58
 * 
 * WangquN
 * Modified on 2011-08-18 PM 12:10
 */

#ifndef TRIE_MAP_TREE_HPP
#define	TRIE_MAP_TREE_HPP

#include <map>
#include <vector>
#include <list>

#include "macro_log.hpp"

using namespace std;
namespace cep {

    class TrieMapTree {
    public:
        // typedef vector<string>::size_type index_type;
        typedef int index_type;
        struct Node;
        typedef map<char, Node*> node_map;
        typedef list<Node*> node_queue;

        struct Node {
            node_map * child;
            vector<index_type> idxs;
            //            index_type idx;
        };
    private: // Forbidden copy behave

        TrieMapTree(const TrieMapTree & trieMapTree) {
            MLOG_TRACE << this << "->TrieMapTree copy from[" << &trieMapTree << "] ctor execute!" << endl;
        }

        TrieMapTree & operator=(const TrieMapTree & trieMapTree) {
            if (this == &trieMapTree) // effective c++ 16
                return *this;
            MLOG_TRACE << this << "->TrieMapTree assignment operator(" << &trieMapTree << ") execute!" << endl;
            return *this;
        }
    public:

        TrieMapTree() {
            root_ = new Node;
            root_->child = NULL;
            //            root_->idx = -1;
        }

        virtual ~TrieMapTree() {
            cleanAll();
            delete root_;
        }
    public:
        void cleanAll();

        
        /**
         * 对单个字符串进行索引的创建
         * @param str 字符串
         * @param idx 索引的位置
         */
        void build(const string & str, const index_type idx);

        /**
         * 对集合进行索引创建
         * @param 
         */
        void build(const vector<string> &);

        /**
         * 在字典树种查找和匹配字符串
         * @param str 被查找的字符串
         * @param suffix 所使用的后缀或前缀，如“*”
         * @param extsuffix 扩展的字符，如“.”
         * @return table中所有符合条件的下标值
         */
        vector<index_type> match(const string & str, const char suffix, const char extsuffix) const;

        /**
         * 对某个节点进行遍历
         * @param node 被遍历的节点
         * @param suffix 所使用的后缀或前缀，如“*”
         * @param extsuffix 扩展的字符，如“.”
         * @return table中所有符合条件的下标值
         */
        vector<index_type> traverse(const Node * node, const char suffix, const char extsuffix) const;

    private:
        friend ostream& operator<<(ostream&, const TrieMapTree&);

        Node * root_;
    };
}
#endif	/* TRIE_MAP_TREE_HPP */
