/* 
 * File:   SMatrixOL.h
 * Author: Administrator
 *
 * Created on 2012年3月30日, 下午1:49
 */

#ifndef SMATRIXOL_H
#define	SMATRIXOL_H
/*
 * S是稀疏的意思SPARSE 
 * OL代表是用十字链表实现
 * class SMatrixOL::OLNode;Orthogonal List
 */
#include <memory.h>
#include <assert.h>
#include <iostream>
using namespace std;

template<class T>
class SMatrixOL {
public:

    class OLNode {
    public:
        unsigned int m_row;
        unsigned int m_col;
        OLNode* m_right;
        OLNode* m_down;
        T m_elem;

        OLNode(unsigned int row, unsigned int col, T elem) : m_row(row), m_col(col), m_right(0), m_down(0), m_elem(elem) {
        }
    };
    typedef OLNode * OLink;
public:

    /*
     * @param rownr 行数
     * @param colnr 列数
     * @param reserve 保留非0元的个数 默认保留的非0元的个数
     */
    SMatrixOL(unsigned int rownr, unsigned int colnr) : m_rownr(rownr), m_colnr(colnr) {
        m_size = 0;
        m_rowhead = new OLink[rownr];
        m_colhead = new OLink[colnr];
        memset(m_rowhead, 0, sizeof (OLink) * rownr);
        memset(m_colhead, 0, sizeof (OLink) * colnr);
    }

    void insert(unsigned int row, unsigned int col, T m_elem) {
        assert(row < m_rownr && col < m_colnr);
        /*
         *处理行          
         */
        OLNode *olnode = 0;
        if (m_rowhead[row] == NULL) {
            olnode = new OLNode(row, col, m_elem);
            m_rowhead[row] = olnode;
            this->m_size++;
        } else {
            OLNode * nodeptr = m_rowhead[row];
            OLNode * pre_nodeptr = nodeptr;
            while (nodeptr && nodeptr->m_col < col) {
                pre_nodeptr = nodeptr;
                nodeptr = nodeptr->m_right;
            }
            /*没有移动的情况 第一把就找到了可以插入的位置*/
            if (pre_nodeptr == nodeptr) {
                /*
                 * 已经有了这个节点
                 * 那么就不用处理列了 那么也就是说处理列的情况了
                 */
                if (nodeptr->m_col == col) {
                    nodeptr->m_elem = m_elem;
                    return;
                } else {
                    /*nodeptr.j > j */
                    olnode = new OLNode(row, col, m_elem);
                    m_rowhead[row] = olnode;
                    olnode->m_right = nodeptr;
                    this->m_size++;
                }
            } else {
                /*
                 * 不是第一把就找到  那么nodeptr==插入的位置
                 *       6 
                 * 1 2 5 7
                 */
                if (nodeptr) {
                    olnode = new OLNode(row, col, m_elem);
                    pre_nodeptr->m_right = olnode;
                    olnode->m_right = nodeptr;
                    this->m_size++;
                } else {
                    /*       6
                     * 1 2 5
                     */
                    olnode = new OLNode(row, col, m_elem);
                    pre_nodeptr->m_right = olnode;
                    olnode->m_right = 0;
                    this->m_size++;
                }
            }
        }
        assert(olnode != 0);
        /*
         * 处理列
         */
        if (m_colhead[col] == NULL) {
            m_colhead[col] = olnode;
        } else {
            OLNode * nodeptr = m_colhead[col];
            OLNode * pre_nodeptr = nodeptr;
            while (nodeptr && nodeptr->m_row < row) {
                pre_nodeptr = nodeptr;
                nodeptr = nodeptr->m_down;
            }
            /*没有移动的情况 第一把就找到了可以插入的位置*/
            if (pre_nodeptr == nodeptr) {
                /*
                 * 已经有了这个节点
                 */
                if (nodeptr->m_row == row) {
                    assert(0 == 1);
                    nodeptr->m_elem = m_elem;
                    return;
                } else {
                    /*nodeptr.j > j */
                    m_colhead[col] = olnode;
                    olnode->m_down = nodeptr;
                }
            } else {
                /*
                 * 不是第一把就找到  那么nodeptr==插入的位置
                 *       6 
                 * 1 2 5 7
                 */
                if (nodeptr) {
                    pre_nodeptr->m_down = olnode;
                    olnode->m_down = nodeptr;
                } else {
                    /*       6
                     * 1 2 5
                     */
                    pre_nodeptr->m_down = olnode;
                    olnode->m_down = 0;
                }
            }
        }
    }

    void clear() {
        /*
         * m_rownr = 1
         * m_colnr = 2
         */
        for (unsigned int i = 0; i < m_rownr; i++) {
            OLNode *nodeptr = m_rowhead[i];
            OLNode *pre_nodeptr = nodeptr;
            while (nodeptr) {
                nodeptr = nodeptr->m_right;
                delete pre_nodeptr;
                pre_nodeptr = nodeptr;
            }
            m_rowhead[i] = 0;
            if (m_colnr > i) {
                m_colhead[i] = 0;
            }
        }
        for (int i = m_colnr; i < m_colnr; i++) {
            m_colhead[i] = 0;
        }
    }

    void traverserow() {
        for (int i = 0; i < m_rownr; i++) {
            for (int j = 0; j < m_colnr; j++) {
                if (m_rowhead[i] == 0) {
                    cout << "0" << " ";
                } else {
                    OLNode *nodeptr = m_rowhead[i];
                    if (nodeptr && nodeptr->m_col == j) {
                        cout << nodeptr->m_elem << " ";
                        nodeptr = nodeptr->m_right;
                    } else {
                        cout << "0" << " ";
                    }
                }
            }
            cout << endl;
        }
    }

    void traversecol() {
        for (int i = 0; i < m_colnr; i++) {
            for (int j = 0; j < m_rownr; j++) {
                if (m_colhead[i] == 0) {
                    cout << "0" << " ";
                } else {
                    OLNode *nodeptr = m_colhead[i];
                    if (nodeptr && nodeptr->m_row == j) {
                        cout << nodeptr->m_elem << " ";
                        nodeptr = nodeptr->m_down;
                    } else {
                        cout << "0" << " ";
                    }
                }
            }
            cout << endl;
        }
    }

    ~SMatrixOL() {
        for (int i = 0; i < m_rownr; i++) {
            OLNode *nodeptr = m_rowhead[i];
            OLNode *pre_nodeptr = nodeptr;
            while (nodeptr) {
                nodeptr = nodeptr->m_right;
                delete pre_nodeptr;
                pre_nodeptr = nodeptr;
            }
        }
        delete m_rowhead;
        delete m_colhead;
    }
public:
    unsigned int m_rownr;
    unsigned int m_colnr;
    unsigned int m_size;
    OLink * m_rowhead; //行和列表头指
    OLink * m_colhead;
private:
    SMatrixOL(const SMatrixOL& orig);

};

#endif	/* SMATRIXOL_H */



