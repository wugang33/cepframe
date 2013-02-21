/* 
 * File:   router.cpp
 * Author: WangquN
 *
 * Created on 2012-02-17 AM 09:22
 */

#include "processing/router.hpp"

#include "macro_log.hpp"
#include "event.hpp"
#include "processing/pn.hpp"
#include "processing/pe.hpp"

using namespace std;

namespace cep {

    router::router() : local_pe_count_(0), remote_pe_count_(0), pe_count_(0)
    , received_matrix_(), processed_matrix_(), local_pes_(NULL), remote_pes_(NULL)
    , local_pe_id2array_idx_(), remote_pe_id2array_idx_(), pe_id2_unif_idx_() {
    }

    router::~router() {
        destroy_();
    }

    void router::rebuild(const unsigned int local_pe_count, const unsigned int remote_pe_count) {
        destroy_();
        local_pe_count_ = local_pe_count;
        remote_pe_count_ = remote_pe_count;
        pe_count_ = local_pe_count_ + remote_pe_count_;
        // 0 reserved for INIT_PE_ID
        /**
         * local_pe_count_ = 2, remote_pe_count_ = 1, pe_count_ = 3
         *  |  0 1|0> array_idx
         *  |  0 1|2> unif_idx
         *  |0 1 2|3
         * -+-------+ idx
         * 0|x 0 1|2
         * 1|x 3 4|5
         * 2|x 6 7|8
         * 3|x 9 A|B
         */
        const unsigned int pow_count_2 = pe_count_ * (pe_count_ + 1);
        /*// http://www.cnblogs.com/bigshow/archive/2009/01/03/1367661.html
        // char **a = new char* [m];
        // a[0] = new char[m * n]; // 一次性分配所有空间
        // for (int i = 1; i < m; i++) {
        //     a[i] = a[i - 1] + n; // 分配每个指针所指向的数组
        // }
        next_matrix_ = new unsigned int*[pe_count_];
        next_matrix_[0] = new unsigned int[pow_count_2];
        for (int i = 1; i < count; ++i)
            next_matrix_[i] = next_matrix_[i - 1] + count;
        post_matrix_ = new unsigned int*[pe_count_];
        post_matrix_[0] = new unsigned int[pow_count_2];
        for (int i = 1; i < count; ++i)
            post_matrix_[i] = post_matrix_[i - 1] + count;*/
        received_matrix_.resize(pow_count_2, false);
        processed_matrix_.resize(pow_count_2, false);

        local_pes_ = new pe*[local_pe_count_];
        // for (int i = 0; i < local_pe_count_; ++i) local_pes_[i] = NULL;
        remote_pes_ = new pe*[remote_pe_count_];
        // for (int i = 0; i < remote_pe_count_; ++i) remote_pes_[i] = NULL;

        // pe_id2_unif_idx_.insert(pe_id2idx_map_t::value_type(pe::INIT_PE_ID, 0));
    }

    int router::set_routing_(pe* source, pe& destination, const string& exception_info) {
        if (destination.id() == pe::INIT_PE_ID) {
            MLOG_WARN << this << "->router::set_routing4" << exception_info << " destination pe[" << destination.id()
                    << "] id is 0, it cant route to out pe!" << endl;
            return -1;
        }
        pe_id2idx_map_t::const_iterator found, end = pe_id2_unif_idx_.end();
        int idx4s = -1; // if source is null then event is the first into pn
        if (source != NULL && source->id() != pe::INIT_PE_ID) {
            found = pe_id2_unif_idx_.find(source->id());
            if (found == end) {
                if (pe_id2_unif_idx_.size() >= pe_count_) { //  + 1) {
                    MLOG_WARN << this << "->router::set_routing4" << exception_info << " source pe[" << source->id()
                            << "] cant set in current full route for " << exception_info << " matrix!" << endl;
                    return -1;
                }
                if (pn::instance().id() == source->pn_id()) {
                    idx4s = local_pe_id2array_idx_.size();
                    if (idx4s >= local_pe_count_) {
                        MLOG_WARN << this << "->router::set_routing4" << exception_info << " source pe[" << source->id()
                                << "] cant set in current full local pe vector!" << endl;
                        return -1;
                    }
                    local_pe_id2array_idx_.insert(pe_id2idx_map_t::value_type(source->id(), idx4s));
                    local_pes_[idx4s] = source;
                    // idx4s = local_pe_unif_idx_(idx4s);
                } else { // remote
                    idx4s = remote_pe_id2array_idx_.size();
                    if (idx4s >= remote_pe_count_) {
                        MLOG_WARN << this << "->router::set_routing4" << exception_info << " source pe[" << source->id()
                                << "] cant set in current full remote pe vector!" << endl;
                        return -1;
                    }
                    remote_pe_id2array_idx_.insert(pe_id2idx_map_t::value_type(source->id(), idx4s));
                    remote_pes_[idx4s] = source;
                    idx4s = remote_pe_unif_idx_(idx4s);
                }
                pe_id2_unif_idx_.insert(pe_id2idx_map_t::value_type(source->id(), idx4s));
                end = pe_id2_unif_idx_.end();
            } else
                idx4s = found->second;
        }
        found = pe_id2_unif_idx_.find(destination.id());
        size_t idx4d;
        if (found == end) {
            if (pe_id2_unif_idx_.size() >= pe_count_) { // + 1) {
                MLOG_WARN << this << "->router::set_routing4" << exception_info << " destination pe[" << destination.id()
                        << "] cant set in current full route for " << exception_info << " matrix!" << endl;
                return -1;
            }
            if (pn::instance().id() == destination.pn_id()) {
                idx4d = local_pe_id2array_idx_.size();
                if (idx4d >= local_pe_count_) {
                    MLOG_WARN << this << "->router::set_routing4" << exception_info << " destination pe[" << destination.id()
                            << "] cant set in current full local pe vector!" << endl;
                    return -1;
                }
                local_pe_id2array_idx_.insert(pe_id2idx_map_t::value_type(destination.id(), idx4d));
                local_pes_[idx4d] = &destination;
                // idx4d = local_pe_unif_idx_(idx4d);
            } else { // remote
                idx4d = remote_pe_id2array_idx_.size();
                if (idx4d >= remote_pe_count_) {
                    MLOG_WARN << this << "->router::set_routing4" << exception_info << " destination pe[" << destination.id()
                            << "] cant set in current full remote pe vector!" << endl;
                    return -1;
                }
                remote_pe_id2array_idx_.insert(pe_id2idx_map_t::value_type(destination.id(), idx4d));
                remote_pes_[idx4d] = &destination;
                idx4d = remote_pe_unif_idx_(idx4d);
            }
            pe_id2_unif_idx_.insert(pe_id2idx_map_t::value_type(destination.id(), idx4d));
        } else
            idx4d = found->second;
        return matrix_idx_(idx4s, idx4d);
    }

    bool router::set_node(pe& node) {
        pe_id2idx_map_t::const_iterator found = pe_id2_unif_idx_.find(node.id());
        if (found == pe_id2_unif_idx_.end()) {
            if (pe_id2_unif_idx_.size() >= pe_count_) { // + 1) {
                MLOG_WARN << this << "->router::set_node pe[" << node.id()
                        << "] cant set in current full matrix!" << endl;
                return false;
            }
            size_t idx;
            if (pn::instance().id() == node.pn_id()) {
                idx = local_pe_id2array_idx_.size();
                if (idx >= local_pe_count_) {
                    MLOG_WARN << this << "->router::set_node pe[" << node.id()
                            << "] cant set in current full local pe vector!" << endl;
                    return false;
                }
                local_pe_id2array_idx_.insert(pe_id2idx_map_t::value_type(node.id(), idx));
                local_pes_[idx] = &node;
            } else { // remote
                idx = remote_pe_id2array_idx_.size();
                if (idx >= remote_pe_count_) {
                    MLOG_WARN << this << "->router::set_node pe[" << node.id()
                            << "] cant set in current full remote pe vector!" << endl;
                    return false;
                }
                remote_pe_id2array_idx_.insert(pe_id2idx_map_t::value_type(node.id(), idx));
                remote_pes_[idx] = &node;
                idx = remote_pe_unif_idx_(idx);
            }
            pe_id2_unif_idx_.insert(pe_id2idx_map_t::value_type(node.id(), idx));
        }
        return true;
    }

    bool router::set_routing4received(pe* source, pe& destination, const bool connect) {
        int idx = set_routing_(source, destination, "received");
        if (idx < 0) return false;
        received_matrix_[idx] = connect;
        return true;
    }

    bool router::set_routing4processed(pe* source, pe& destination, const bool connect) {
        int idx = set_routing_(source, destination, "processed");
        if (idx < 0) return false;
        processed_matrix_[idx] = connect;
        return true;
    }

    void router::destroy_() {
        /*if (next_matrix_ != NULL) {
            delete[] next_matrix_[0];
            delete[] next_matrix_;
            next_matrix_ = NULL;
        }
        if (post_matrix_ != NULL) {
            delete[] post_matrix_[0];
            delete[] post_matrix_;
            post_matrix_ = NULL;
        }*/
        if (local_pes_ != NULL) {
            delete[] local_pes_;
            local_pes_ = NULL;
        }
        if (remote_pes_ != NULL) {
            delete[] remote_pes_;
            remote_pes_ = NULL;
        }
        received_matrix_.clear();
        processed_matrix_.clear();

        local_pe_id2array_idx_.clear();
        remote_pe_id2array_idx_.clear();
        pe_id2_unif_idx_.clear();

        local_pe_count_ = 0;
        remote_pe_count_ = 0;
        pe_count_ = 0;
    }

    /** routing to others local pe after the current pe processed */
    size_t router::routing4processed(const event& evt, pe* pes[]) const {
        size_t idx = 0;
        if (pe::INIT_PE_ID != evt.pe_id()) {
            pe_id2idx_map_t::const_iterator found = pe_id2_unif_idx_.find(evt.pe_id());
            if (found == pe_id2_unif_idx_.end()) return 0;
            idx = (1 + found->second) * pe_count_;
        }
        size_t count = 0;
        for (size_t i = 0; i < local_pe_count_; ++i, ++idx) {
            if (processed_matrix_[idx] && local_pes_[i]->partition(evt)) {
                pes[count++] = local_pes_[i];
                /*int array_idx = idx;
                if (matrix2pe_array_idx_(array_idx)) {
                    if (local_pes_[array_idx]->partition(evt))
                        pes[count++] = local_pes_[array_idx];
                }*/
                // else pes[count++] = remote_pes[array_idx];
            }
        }
        return count;
    }

    /** routing to others local pe at the same time of current pe receiving */
    size_t router::routing4received(const event& evt, pe* pes[]) const {
        size_t idx = 0;
        if (pe::INIT_PE_ID != evt.pe_id()) {
            pe_id2idx_map_t::const_iterator found = pe_id2_unif_idx_.find(evt.pe_id());
            if (found == pe_id2_unif_idx_.end()) return 0;
            idx = (1 + found->second) * pe_count_;
        }
        size_t count = 0;
        for (size_t i = 0; i < local_pe_count_; ++i, ++idx) {
            if (received_matrix_[idx] && local_pes_[i]->partition(evt)) {
                pes[count++] = local_pes_[i];
            }
        }
        return count;
    }

    /** routing to others remote pe after the current pe processed */
    size_t router::routing4processed(const event& evt, pe* pes[], char) const {
        size_t idx = 0;
        if (pe::INIT_PE_ID != evt.pe_id()) {
            pe_id2idx_map_t::const_iterator found = pe_id2_unif_idx_.find(evt.pe_id());
            if (found == pe_id2_unif_idx_.end()) return 0;
            idx = (1 + found->second) * pe_count_ + local_pe_count_;
        } else
            idx = local_pe_count_;
        size_t count = 0;
        for (size_t i = 0; i < remote_pe_count_; ++i, ++idx) {
            if (processed_matrix_[idx] && remote_pes_[i]->partition(evt)) {
                pes[count++] = remote_pes_[i];
            }
        }
        return count;
    }

    /** routing to others remote pe at the same time of current pe receiving */
    size_t router::routing4received(const event& evt, pe* pes[], char) const {
        size_t idx = 0;
        if (pe::INIT_PE_ID != evt.pe_id()) {
            pe_id2idx_map_t::const_iterator found = pe_id2_unif_idx_.find(evt.pe_id());
            if (found == pe_id2_unif_idx_.end()) return 0;
            idx = (1 + found->second) * pe_count_ + local_pe_count_;
        } else
            idx = local_pe_count_;
        // cout << "router::routing4received to event[" << evt.pe_id()
        //         << "] routing matrix idx is " << idx << endl;
        size_t count = 0;
        for (size_t i = 0; i < remote_pe_count_; ++i, ++idx) {
            // cout << "router::routing4received to event[" << evt.pe_id()
            //         << "] routing matrix[" << idx << "] is " << received_matrix_[idx]
            //         << " and remote_pes_[" << i << "]->partition(evt) is "
            //         << remote_pes_[i]->partition(evt) << endl;
            if (received_matrix_[idx] && remote_pes_[i]->partition(evt)) {
                pes[count++] = remote_pes_[i];
            }
        }
        return count;
    }

    std::ostream& operator<<(std::ostream& s, const router& r) {
        s << &r << "->router: pe count local=" << r.local_pe_count_ << ", remote=" << r.remote_pe_count_;
        if (r.local_pe_count_ > 0 || r.remote_pe_count_ > 0) {
            s << "\n received  matrix:" << r.received_matrix_ << "\n processed matrix:" << r.processed_matrix_;
            router::pe_id2idx_map_t::const_iterator itr = r.pe_id2_unif_idx_.begin();
            router::pe_id2idx_map_t::const_iterator end = r.pe_id2_unif_idx_.end();
            size_t idx;
            pe* ptr_pe;
            for (; itr != end; ++itr) {
                if (itr->first == pe::INIT_PE_ID) continue;
                idx = itr->second;
                if (r.matrix2pe_array_idx_(idx)) {
                    ptr_pe = r.local_pes_[idx];
                    s << "\n local pe[";
                } else {
                    ptr_pe = r.remote_pes_[idx];
                    s << "\n remote pe[";
                }
                s << idx << '@' << itr->second << "]=";
                if (ptr_pe)
                    s << ptr_pe << "->pe[" << ptr_pe->id() << '@' << ptr_pe->pn_id() << ']';
                else
                    s << "NULL";
            }
            /*s << "\n local pe:";
            for (int i = 0; i < r.local_pe_count_; ++i)
                s << "\n  " << i << '>' << r.local_pes_[i];
            s << "\n remote pe:";
            for (int i = 0; i < r.remote_pe_count_; ++i)
                s << "\n  " << i << '>' << r.remote_pes_[i];*/
        }
        return s;
    }
}
