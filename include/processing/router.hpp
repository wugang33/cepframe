/* 
 * File:   router.hpp
 * Author: WangquN
 *
 * Created on 2012-02-17 AM 09:22
 */

#ifndef PROCESSING_ROUTER_HPP
#define	PROCESSING_ROUTER_HPP

#include <ostream>
#include <map>

#include <boost/dynamic_bitset.hpp>

using std::string;
using std::map;

namespace cep {
    class event;
    // class remote_pe; // remote pe is just executed init() pe!
    class pe;

    class router {
    public:
        typedef unsigned short int pe_id_t;
        typedef map<pe_id_t, size_t> pe_id2idx_map_t;
        router();
        ~router();
    private:

        router(router& arg) { // Forbidden copy behave
            // MLOG_INFO << this << "->router copy from[" << &arg << "] ctor execute!" << endl;
        }

        router& operator=(const router& arg) { // Forbidden copy behave
            if (this == &arg) // effective c++ 16
                return *this;
            // MLOG_INFO << this << "->router assignment operator(" << &arg << ") execute!" << endl;
            return *this;
        }
    public:

        const unsigned int local_pe_count() const {
            return local_pe_count_;
        }

        const unsigned int remote_pe_count() const {
            return remote_pe_count_;
        }
    public:
        void rebuild(const unsigned int local_pe_count, const unsigned int remote_pe_count);

        bool set_node(pe& node);
        bool set_routing4received(pe* source, pe& destination, const bool connect = true);
        bool set_routing4processed(pe* source, pe& destination, const bool connect = true);
    private:
        void destroy_();

        /*int local_pe_unif_idx_(const int local_pe_array_idx) {
            return 1 + local_pe_array_idx; // 0 reserved for INIT_PE_ID
        }*/

        int remote_pe_unif_idx_(const int remote_pe_array_idx) {
            // return 1 + local_pe_count_ + remote_pe_array_idx; // 0 reserved for INIT_PE_ID
            return local_pe_count_ + remote_pe_array_idx; // 0 reserved for INIT_PE_ID
        }

        /*int local_pe_array_idx(const int local_pe_unif_idx) {
            return local_pe_unif_idx - 1; // 0 reserved for INIT_PE_ID
        }
        int remote_pe_array_idx(const int remote_pe_unif_idx) {
            return remote_pe_unif_idx - 1 - local_pe_count_; // 0 reserved for INIT_PE_ID
        }*/

        int matrix_idx_(const int source_pe_unif_idx, const int destination_pe_unif_idx) const {
            return pe_count_ * (source_pe_unif_idx + 1) + destination_pe_unif_idx;
        }

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
         * 
         * @param idx
         * @return 
         */
        bool matrix2pe_array_idx_(size_t& idx) const {
            idx = idx % pe_count_;
            /*if (idx <= local_pe_count_) {
                idx -= 1;
                return true;
            } else {
                idx -= (1 + local_pe_count_);
                return false;
            }*/
            if (idx < local_pe_count_) return true;
            else {
                idx -= local_pe_count_;
                return false;
            }
        }

        int set_routing_(pe* source, pe& destination, const string& exception_info);
    public:
        // next_local_pe post_local_pe
        // next_remote_pe post_remote_pe
        /** routing to others local pe after the current pe processing */
        // size_t routing4local(const event&, pe* pes[]);
        /** routing to others local pe at the same time of current pe receiving */
        // size_t routing4local(const event&, pe* pes[], char overload_tag);

        /** routing to others local pe after the current pe processed */
        size_t routing4processed(const event&, pe* pes[]) const;
        /** routing to others local pe at the same time of current pe receiving */
        size_t routing4received(const event&, pe* pes[]) const;
        /** routing to others remote pe after the current pe processed */
        size_t routing4processed(const event&, pe* pes[], char overload_tag) const;
        /** routing to others remote pe at the same time of current pe receiving */
        size_t routing4received(const event&, pe* pes[], char overload_tag) const;

    private:
        friend std::ostream& operator<<(std::ostream&, const router&);

        unsigned int local_pe_count_, remote_pe_count_, pe_count_;
        boost::dynamic_bitset<unsigned char> received_matrix_, processed_matrix_;
        /* const */
        pe **local_pes_, **remote_pes_;
        pe_id2idx_map_t local_pe_id2array_idx_, remote_pe_id2array_idx_;
        /* {pe_id=unif_idx of matrix, ...} */
        pe_id2idx_map_t pe_id2_unif_idx_;
    };
}

#endif	/* PROCESSING_ROUTER_HPP */
