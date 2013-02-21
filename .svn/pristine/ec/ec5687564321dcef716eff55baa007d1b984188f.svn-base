/* 
 * File:   ForestMap.cpp
 * Author: wugang
 * 
 * Created on 2011年8月14日, 上午10:58
 */

#include "processing/forest_map.h"
#include<list>

using namespace std;
namespace cep {

    void ForestMap::cleanAll() {
        if (!root->child) {
            return;
        }
        node_queue nodeQueue;
        nodeQueue.push_back(root);
        node_queue::iterator iter = nodeQueue.begin();
        while (iter != nodeQueue.end()) {
            if ((*iter)->child != NULL) {
                node_map * lp_node_map = (*iter)->child;
                node_map::iterator node_map_iter = lp_node_map->begin();
                for (; node_map_iter != lp_node_map->end(); node_map_iter++) {
                    nodeQueue.push_back(node_map_iter->second);
                }
            }
            delete (*iter)->child;
            delete (*iter);
            iter++;
        }
    }

    int ForestMap::buildForest(const string& data, int index) {
        if (!root) root = new forestNode;
        forestNode* nodePtr = root;
        for (int j = 0; j < data.size(); j++) {
            if (data[j] <= 0x20)continue;
            /*如果是不可见字符 那么不建立节点
              也就是说即使是*也会建立一个结点 那么需要建立一个失败跳转
              如果*前面的匹配失败 那么会跳转到*这个结点上面来 如果没有* 那么这个结点就是root*/
            char temp_node_value = data[j];
            // 处理连续2个或2个以上的“*”的问题
            if (temp_node_value == '*' && j > 0 && data[j - 1] == '*') {
                continue;
            }
            if (nodePtr->child == NULL) {
                nodePtr->child = new node_map;
                forestNode *temp_node = new forestNode;
                if (!temp_node) return 0;
                temp_node->ch = temp_node_value;
                nodePtr->child->insert(node_map::value_type(temp_node_value, temp_node));
                nodePtr = temp_node;
                continue;
            }
            node_map::iterator iter = nodePtr->child->find(temp_node_value);
            if (iter == nodePtr->child->end())//兄弟容器中没有这个节点
            {
                forestNode *temp_node = new forestNode;
                if (!temp_node) return 0;
                temp_node->ch = temp_node_value;
                nodePtr->child->insert(node_map::value_type(temp_node_value, temp_node));
                nodePtr = temp_node;
                continue;
            } else {
                nodePtr = iter->second;
            }
        }
        //nodePtr->index = index;
        nodePtr->indexs.push_back(index);

        return 1;
    }

    int ForestMap::buildForest(const vector<string>* vec) {
        cleanAll(); //会清理掉root自己
        if (!root)
            root = new forestNode;

        for (int i = 0; i < vec->size(); i++) {
            if (!buildForest((*vec)[i], i)) {
                return 0;
            }
        }
    }

    bool ForestMap::push_back_diff(node_queue&queue, forestNode*node) const {
        node_queue::const_iterator iter = queue.begin();
        bool alreadhas = false;
        while (iter != queue.end()) {
            if ((*iter) == node) {
                alreadhas = true;
                break;
            }
            iter++;
        }
        if (!alreadhas) queue.push_back(node);
        return !alreadhas;
    }

    void ForestMap::match(const string&str_bematch, vector<int> &vec) const {
        node_queue matchQueue; //用来下一步需要匹配的结点  如果是* 那么不删除 不然用新结点来替代老结点
        matchQueue.push_back(root);
        for (int i = 0; i < str_bematch.size(); i++) {
            if (str_bematch[i] <= 0x20)continue; //如果是不可见字符 那么不进行匹配
            char temp_node_value = str_bematch[i];
            node_queue::iterator iter = matchQueue.begin();
            forestNode * guard_iter = NULL;
            for (; iter != matchQueue.end();) {
                if (*iter == guard_iter) {//如果遍历到哨兵 那么退出 表示这一层已经遍历完了 下面是下一层的任务
                    break;
                }
                if ((*iter)->child) {
                    node_map::iterator node_iter = (*iter)->child->end();
                    //如果子节点有*那么加入进来 并且马上匹配其子节点
                    if ((node_iter = (*iter)->child->find('*')) != (*iter)->child->end()) {
                        //在这一层新加的任务前面加一个哨兵
                        if (!guard_iter) {
                            guard_iter = node_iter->second;
                        }
                        push_back_diff(matchQueue, node_iter->second);
                        if (node_iter->second->child) {
                            node_map::iterator node_iter_temp = node_iter->second->child->find(temp_node_value);
                            if (node_iter_temp != node_iter->second->child->end()) {
                                matchQueue.push_back(node_iter_temp->second);
                            }
                        }
                    }
                    //找到了这个结点 那么放进来
                    if ((node_iter = (*iter)->child->find(temp_node_value)) != (*iter)->child->end()) {
                        //在这一层新加的任务前面加一个哨兵
                        if (push_back_diff(matchQueue, node_iter->second)) {
                            if (!guard_iter) {
                                guard_iter = node_iter->second;
                            }
                        }
                    }
                }

                if ((*iter)->ch != '*') {//这个结点不是*
                    // 那么匹配完了就删除。所以root匹配完了就删除了
                    matchQueue.erase(iter++);
                } else {
                    iter++;
                }
            }
        }
        /*匹配完了 路径上的所有的* 和所有的该匹配的都放到队列里了 
         * 然后依次从队列里面取出所有的结点 
         * 如果index！=-1 那么匹配成功，否则匹配失败。*/
        while (matchQueue.size()) {
            forestNode * node = matchQueue.front();
            matchQueue.pop_front();
            traverse(node, vec);
        }
    }

    void ForestMap::traverse(const forestNode*nodePtr, vector<int>&vec) const {
        if (nodePtr == NULL)return;
        for (int i = 0; i < nodePtr->indexs.size(); ++i) {
            vec.push_back(nodePtr->indexs.at(i));
        }
        // if (nodePtr->index != -1)vec.push_back(nodePtr->index);
        node_queue nodeQueue;
        if (nodePtr->child != NULL) {
            node_map::iterator iter = nodePtr->child->find('*');
            if (iter != nodePtr->child->end()) {
                nodeQueue.push_back(iter->second);
            }
        }
        while (nodeQueue.size() != 0) {
            node_queue::iterator iter = nodeQueue.begin(); //肯定有值
            forestNode *lpnode = *iter;
            if (lpnode->child != NULL) {
                node_map::iterator iter = lpnode->child->find('*');
                if (iter != lpnode->child->end()) {
                    nodeQueue.push_back(iter->second);
                }
            }

            for (int i = 0; i < lpnode->indexs.size(); ++i) {
                vec.push_back(lpnode->indexs.at(i));
            }
            // if (lpnode->index != -1)vec.push_back(lpnode->index);
            nodeQueue.erase(iter);
        }
    }

    void ForestMap::traverse() {
        node_queue nodeQueue;
        nodeQueue.push_back(root);
        node_queue::iterator iter = nodeQueue.begin();
        while (iter != nodeQueue.end()) {
            if ((*iter)->child != NULL) {
                node_map * lp_node_map = (*iter)->child;
                node_map::iterator node_map_iter = lp_node_map->begin();
                for (; node_map_iter != lp_node_map->end(); node_map_iter++) {
                    nodeQueue.push_back(node_map_iter->second);
                }
            }
            iter++;
        }
    }
}
