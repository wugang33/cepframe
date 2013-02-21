/* 
 * File:   forest_map.h
 * Author: wugang
 *
 * Created on 2011年8月14日, 上午10:58
 */
#ifndef FOREST_MAP_H
#define	FOREST_MAP_H
#include<vector>
#include<map>
#include<iostream>
#include<string>
#include<list>
using namespace std;
#ifndef NULL
#define NULL 0
#endif

namespace cep {

    class ForestMap {
    private:

        class forestNode {
        public:

            forestNode() {
                child = NULL;
               // index = -1;
            }

            ~forestNode() {
            }
        private:
            forestNode(const forestNode&node);
            forestNode& operator=(const forestNode&node);
        public:
            map<char, forestNode*> * child;
            vector<int> indexs;
            //int index;
            char ch; //这个结点代表的char
        };
        typedef map<char, forestNode*> node_map;
        typedef list<forestNode*> node_queue;
    public:
        void match(const string&, vector<int>&) const;
        void traverse(const forestNode*nodePtr, vector<int>&) const;
        int buildForest(const vector<string>*);
        int buildForest(const string& data, int index);
        void cleanAll();
        void traverse();

        ForestMap() {
            root = new forestNode;
        }

        virtual ~ForestMap() {
            cleanAll();
        }
    private:
        bool push_back_diff(node_queue&, forestNode*) const;
        forestNode* root;

        ForestMap(const ForestMap& orig) {
        }
    private:
    };
}

#endif	/* FOREST_MAP_H */

