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

#include "processing/idx_trie_tree.hpp"
namespace cep {

    void TrieMapTree::cleanAll() {
        node_queue nodeQueue;
        nodeQueue.push_back(root_);
        node_queue::const_iterator iter = nodeQueue.begin();
        node_map * lp_node_map;
        node_map::iterator node_map_iter;
        while (iter != nodeQueue.end()) {
            if ((*iter)->child != NULL) {
                lp_node_map = (*iter)->child;
                node_map_iter = lp_node_map->begin();
                for (; node_map_iter != lp_node_map->end(); node_map_iter++) {
                    nodeQueue.push_back(node_map_iter->second);
                }
            }
            if ((*iter)->child) {
                delete (*iter)->child;
                (*iter)->child = NULL;
            }
            if (*iter != root_) delete (*iter);
            // ++iter;
            nodeQueue.pop_front();
            iter = nodeQueue.begin();
        }
    }

    void TrieMapTree::build(const string & info, const index_type index) {
        Node * node;
        char tmpChar;
        Node * newNode;
        node_map::const_iterator itr4m;
        node = root_;
        for (int i = 0; i < info.size(); ++i) {
            tmpChar = info[i];
            if (tmpChar <= 0x20) continue; // 过滤掉不可见字符
            if (node->child == NULL) {
                node->child = new node_map;

                newNode = new Node;
                newNode->child = NULL;
                //                newNode->idx = -1;

                node->child->insert(make_pair(tmpChar, newNode));
                node = newNode;
                // continue;
            } else {
                itr4m = node->child->find(tmpChar);
                if (itr4m == node->child->end()) {

                    newNode = new Node;
                    newNode->child = NULL;
                    //                    newNode->idx = -1;

                    node->child->insert(make_pair(tmpChar, newNode));
                    node = newNode;
                    // continue;
                } else
                    node = itr4m->second;
            }
        }
        //        node->idx = index;
        node->idxs.push_back(index);
    }

    void TrieMapTree::build(const vector<string> & vec) {
        if (root_->child) cleanAll();
        vector<string>::const_iterator itr = vec.begin();
        vector<string>::const_iterator end = vec.end();
        for (vector<string>::size_type count = 0; itr != end; ++itr, ++count) {
            build(*itr, count);
        }
    }

    vector<TrieMapTree::index_type> TrieMapTree::match(const string & info, const char suffix,
            const char extsuffix) const {
        vector<index_type> vec;
        Node * node = root_;
        char tmpChar;
        node_map::const_iterator iter;
        for (int i = 0; i < info.size(); ++i) {
            if (info[i] <= 0x20) continue; //过滤掉不可见字符
            tmpChar = info[i];
            if (node == NULL) return vec;
            if (node->child == NULL) return vec;
            iter = node->child->find(suffix);
            if (iter != node->child->end()) { // find the *
                vector<index_type> tempVec = traverse(iter->second, suffix, 0);
                vec.insert(vec.end(), tempVec.begin(), tempVec.end());
            }
            iter = node->child->find(tmpChar);
            if (iter != node->child->end()) { // find
                node = iter->second;
                if (i == info.size() - 1) { //�
                    vector<index_type> tempVec = traverse(iter->second, suffix, extsuffix);
                    vec.insert(vec.end(), tempVec.begin(), tempVec.end());
                }
            } else if (iter == node->child->end()) {
                return vec;
            }
        }
        return vec;
    }

    vector<TrieMapTree::index_type> TrieMapTree::traverse(const Node * node, const char suffix,
            const char extsuffix) const {
        vector<index_type> vec;
        if (node == NULL) return vec;
        int i = 0;
        //        if (node->idx != -1) vec.push_back(node->idx);
        for (i = 0; i < node->idxs.size(); i++) {
            vec.push_back(node->idxs[i]);
        }
        node_queue nodeQueue;
        if (node->child != NULL) {
            node_map::iterator iter = node->child->find(suffix);
            if (iter != node->child->end()) {
                nodeQueue.push_back(iter->second);
            }
        }
        node_queue::iterator iter;
        Node *lpnode;
        node_map::iterator itr4m;
        while (nodeQueue.size() != 0) {
            iter = nodeQueue.begin(); //
            lpnode = *iter;
            if (lpnode->child != NULL) {
                itr4m = lpnode->child->find(suffix);
                if (itr4m != lpnode->child->end()) {
                    nodeQueue.push_back(itr4m->second);
                }
            }
            for (i = 0; i < lpnode->idxs.size(); i++) {
                vec.push_back(lpnode->idxs[i]);
            }
            //            if (lpnode->idx != -1) vec.push_back(lpnode->idx);
            nodeQueue.erase(iter);
        }
        if (extsuffix && node->child != NULL) {
            /*
            +- '*'
            +- '.'
            |   +- '*'
            |   +- 'c'
            |   |   +- 'n'      -- .cn
            |   |   +- 'o'
            |   |       +- 'm'  -- .com
            |   |
            |   +- 'o'          -- .org
            |   |*/
            MLOG_TRACE << "find extend suffix by[" << extsuffix << "]\n";
            // << *this << endl;
            itr4m = node->child->find(extsuffix);
            if (itr4m != node->child->end()) {
                Node* extNode = itr4m->second;
                if (/*extNode->idx != -1 &&*/extNode->child != NULL) {
                    node_map::iterator iter4c = extNode->child->find(suffix);
                    if (iter4c != extNode->child->end()) {
                        for (i = 0; i < iter4c->second->idxs.size(); i++) {
                            vec.push_back(iter4c->second->idxs[i]);
                        }
                        //                        if (iter4c->second->idx != -1) vec.push_back(iter4c->second->idx);
                    }
                }
            }
        }
        return vec;
    }

    ostream& operator<<(ostream& s, const TrieMapTree& r) {
        s << &r << "->";
        TrieMapTree::node_queue nodeQueue;
        nodeQueue.push_back(r.root_);
        TrieMapTree::node_queue::const_iterator iter = nodeQueue.begin();
        TrieMapTree::node_map * lp_node_map;
        TrieMapTree::node_map::iterator node_map_iter;
        while (iter != nodeQueue.end()) {
            if ((*iter)->child != NULL) {
                lp_node_map = (*iter)->child;
                node_map_iter = lp_node_map->begin();
                for (; node_map_iter != lp_node_map->end(); ++node_map_iter) {
                    nodeQueue.push_back(node_map_iter->second);
                    s << node_map_iter->first << "=["; //<< node_map_iter->second->idx << endl;
                    for (int i = 0; i < node_map_iter->second->idxs.size(); i++) {
                        s << node_map_iter->second->idxs[i];
                        if (i != node_map_iter->second->idxs.size() - 1) {
                            s << ',';
                        }
                    }
                    s << ']' << endl;
                }
            }
            // ++iter;
            nodeQueue.pop_front();
            iter = nodeQueue.begin();
        }
        return s; // << endl;
    }
}
