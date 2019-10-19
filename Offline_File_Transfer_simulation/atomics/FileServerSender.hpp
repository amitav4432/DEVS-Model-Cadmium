/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* FileServerSender:
* Cadmium implementation of CD++ FileServerSender atomic model
***************************************************************/

#ifndef _FILESERVERSENDER_HPP__
#define _FILESERVERSENDER_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <string>
#include <random>

#include "../data_structures/message.hpp"
#include "../atomics/filesystem.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct FileServerSender_defs{
	struct ackOutS : public out_port<Message_t> {}; //to FileServerReceiver
	struct requestIn : public in_port<int> {}; //input
	struct dataOutS : public out_port<Message_t> {};//to Subnet B
	struct ackIn : public in_port<Message_t> {}; // from subnet B
	struct FileIn : public in_port<Message_t> {}; // from FileServerReceiver
};

template<typename TIME> class FileServerSender{
    public:
    // ports definition
    using input_ports=tuple<typename FileServerSender_defs::requestIn,typename FileServerSender_defs::FileIn,typename FileServerSender_defs::ackIn>;
    using output_ports=tuple<typename FileServerSender_defs::ackOutS,typename FileServerSender_defs::dataOutS>;
    // state definition
    struct state_type{
        bool transmitFile;
        Message_t packet;
	Message_t packet_ack;
        int index;
	int request;
	bool ack;
    }; 
    state_type state;    
    // default constructor
    FileServerSender() {
        state.transmitFile = false;
        state.index     = 0;
	state.ack = false;
	state.request = 0;
    }     
    // internal transition
    void internal_transition() {	
        state.transmitFile = false;  
	state.request = 0;
	state.ack = false;
	state.packet.packet = 0;
	state.packet.bit = 0;
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
        vector<int> bag_port_requestIn;
	vector<Message_t> bag_port_ackIn;
	vector<Message_t> bag_port_FileIn;
        bag_port_requestIn = get_messages<typename FileServerSender_defs::requestIn>(mbs);
	bag_port_ackIn = get_messages<typename FileServerSender_defs::ackIn>(mbs);
	bag_port_FileIn = get_messages<typename FileServerSender_defs::FileIn>(mbs);
	state.index++;
	if(!bag_port_FileIn.empty())
		tmpFile.push(bag_port_FileIn[0]);
	if (!bag_port_ackIn.empty()) {
		state.ack=true;
		state.packet_ack = bag_port_ackIn[0];
	}
	if (!bag_port_requestIn.empty()){
		if(!tmpFile.empty()){
			state.request = bag_port_requestIn[0];
			state.packet = tmpFile.front();
			tmpFile.pop();
			state.transmitFile = true;
		}
	}
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
        typename make_message_bags<output_ports>::type bags;
		vector<Message_t> bag_port_dataOutS; 
		vector<Message_t> bag_port_ackOutS;
		if(state.transmitFile){
			get_messages<typename FileServerSender_defs::dataOutS>(bags).push_back(state.packet); 	
		}else if(state.ack){
			get_messages<typename FileServerSender_defs::ackOutS>(bags).push_back(state.packet_ack);  
		}
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.transmitFile || state.ack) {            
            next_internal = TIME("00:00:03:000");
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }    
        return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename FileServerSender<TIME>::state_type& i) {
        os << " index: " << i.index << " & transmittingToReceiver: " << i.packet; 
	os << " & ackFromReceiver: " << i.packet_ack;
	os << " & requestIn: " << i.request;
        return os;
    }
};    
#endif // _FILESERVERSENDER_HPP__
