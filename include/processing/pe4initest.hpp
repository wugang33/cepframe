/* 
 * File:   pe4initest.hpp
 * Author: WangquN
 *
 * Created on 2012-02-22 AM 09:15
 */

#ifndef PE4INITEST_HPP
#define	PE4INITEST_HPP

#include "processing/pe.hpp"

using namespace std;

namespace cep {
    // class event;

    class pe4initest : public pe {
    public:
        pe4initest(const xml_element& config, const xml_element& main_config);
        virtual ~pe4initest();

        // private: // Forbidden copy behave
        // error: no matching function for call to ‘cep::pe::pe()’
        // pe4initest(const pe4initest& p);
        pe4initest& operator=(const pe4initest& p);
    public:
        /** const or no resource properties initialization */
        virtual bool init();
        /** parse, check and load configuration, it prepare for dtor or startup */
        virtual bool reload();
        /** start work */
        virtual bool startup();
        /** end work */
        virtual void shutdown();
    protected:
        /** invoke by dtor and reload */
        virtual void destroy();
    public:
        /** Hadoop:Partitioner:: public int getPartition(K key, V value, int numReduceTasks) */
        virtual bool partition(const event&) const;
        virtual void push(const event&);
    public:
        virtual void reset_counter() const;

        virtual unsigned long push_counter() const;
        virtual unsigned long pop_counter() const;
        virtual double push_speed() const;
        virtual double pop_speed() const;

        virtual void logging() const;
    private:
        // friend std::ostream& operator<<(std::ostream&, const pe4unitest&);
        mutable unsigned long push_counter_, pop_counter_;
        mutable unsigned long time_ms_;
    public:

        void set_id(const pe_id_t& id) {
            id_ = id;
        }

        void set_pn_id(const pn::pn_id_t& pn_id) {
            pn_id_ = pn_id;
        }
    };
}

#endif	/* PE4INITEST_HPP */
