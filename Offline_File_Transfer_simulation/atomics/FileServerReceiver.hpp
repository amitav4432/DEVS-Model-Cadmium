/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* FileServerReceiver:
* Cadmium implementation of CD++ FileServerReceiver atomic model
**************************************************************/

#ifndef _FILESERVERRECEIVER_HPP__
#define _FILESERVERRECEIVER_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <string>
#include <random>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct FileServerReceiver_defs{
	struct storeDataS : public out_port<Message_t> {};
	struct ackOutR : public out_port<Message_t> {};
	struct dataIn : public in_port<Message_t> {};
	struct ackInR : public in_port<Message_t> {};
};

template<typename TIME> class FileServerReceiver{
    public:
    // ports definition
    using input_ports=tuple<typename FileServerReceiver_defs::dataIn,typename FileServerReceiver_defs::ackInR>;
    using output_ports=tuple<typename FileServerReceiver_defs::ackOutR,typename FileServerReceiver_defs::storeDataS>;
    // state definition
    struct state_type{
        bool transmitFile;
        Message_t packet;
	Message_t ackInR;
        int index;
	bool ack;
    }; 
    state_type state;    
    // default constructor
    FileServerReceiver() {
	state.packet.packet =0;
	state.packet.bit = 0;
	state.ackInR.packet =0;
	state.ackInR.bit = 0;
        state.transmitFile = false;
        state.index     = 0;
	state.ack 	= false;
    }     
    // internal transition
    void internal_transition() {
        state.transmitFile = false;  
	state.ack = false;
    }
    // external transition. Sends the file received to the FileServerSender 
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
	vector<Message_t> bag_port_ackInR;
	vector<Message_t> bag_port_dataIn;
	bag_port_ackInR = get_messages<typename FileServerReceiver_defs::ackInR>(mbs);
	bag_port_dataIn = get_messages<typename FileServerReceiver_defs::dataIn>(mbs);
	state.index++;
	if (!bag_port_dataIn.empty()) {
		state.packet = bag_port_dataIn[0];
		state.transmitFile = true; 
	} else if(!bag_port_ackInR.empty()){
		state.ackInR = bag_port_ackInR[0];
		state.ack = true;
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
	if(state.transmitFile){
		get_messages<typename FileServerReceiver_defs::storeDataS>(bags).push_back(state.packet); 	
	}else if(state.ack){
		get_messages<typename FileServerReceiver_defs::ackOutR>(bags).push_back(state.ackInR);  
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

    friend ostringstream& operator<<(ostringstream& os, const typename FileServerReceiver<TIME>::state_type& i) {
        os << " index: " << i.index << " & transmitting: " << i.packet; 
	os << " & ackToSender: " << i.ackInR; 
        return os;
    }
};    
#endif // _FILESERVERRECEIVER_HPP__
