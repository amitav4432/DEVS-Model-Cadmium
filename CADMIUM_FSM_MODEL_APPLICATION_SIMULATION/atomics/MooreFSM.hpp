/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Atomic model for MooreFSM State
**************************************************************/

#ifndef _MooreFSM_HPP__
#define _MooreFSM_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <string>
#include <random>

#include "../data_structures/message.hpp"

//Time class header
#include <NDTime.hpp>

using namespace cadmium;
using namespace std;

//Port definition

struct MooreFSM_defs{
	struct eventIn : public in_port<string> {};
	struct transitionIn : public in_port<string> {};
	struct stateOutput : public out_port<string> {};
	struct transitionOut : public out_port<string> {};
	struct errorOut : public out_port<string> {};
};

template<typename TIME> class MooreFSM{
    public:
    // ports definition
    using input_ports=tuple<typename MooreFSM_defs::eventIn,typename MooreFSM_defs::transitionIn>;
    using output_ports=tuple<typename MooreFSM_defs::stateOutput,typename MooreFSM_defs::transitionOut,typename MooreFSM_defs::errorOut>;
   
    // state definition
    struct state_type{
        string stateOutput;
        string stateName;
        string phase;
        string nextActiveState;
        string errorMsg;
	bool isEvent;
	bool isError;
	void (*fsm_callback_event)(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError);
	void (*fsm_callback_transition)(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError);
    };
	state_type state;
        
    // default constructor
    MooreFSM() {
	state.nextActiveState = "";
	state.isError = false;
	} 
 
    MooreFSM(string stateName, string stateOutput, string phase, bool isEvent, 
void (*EventInFnPtr)(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError), 
void (*TransitionInFnPtr)(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError)) {
	state.stateName = stateName;
	state.stateOutput = stateOutput;
	state.phase = phase;
	state.isEvent = isEvent;
	state.fsm_callback_event = EventInFnPtr;
	state.fsm_callback_transition = TransitionInFnPtr;
    }  
   
    // internal transition
    void internal_transition() {
	state.isEvent = false;
	state.isError = false;
	state.nextActiveState = "";
    }
    // external transition 
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
	vector<string> bag_port_eventIn;
	vector<string> bag_port_transitionIn;
	bag_port_eventIn = get_messages<typename MooreFSM_defs::eventIn>(mbs);
	bag_port_transitionIn = get_messages<typename MooreFSM_defs::transitionIn>(mbs);
	
	if (!bag_port_eventIn.empty()) {
		state.fsm_callback_event(bag_port_eventIn[0],"", &state.nextActiveState, &state.errorMsg, state.stateName, &state.phase, &state.isEvent, &state.isError);		
	} else if(!bag_port_transitionIn.empty() ){
		state.fsm_callback_transition("", bag_port_transitionIn[0], &state.nextActiveState, &state.errorMsg, state.stateName, &state.phase, &state.isEvent, &state.isError);	
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
	if(state.isEvent) {
		get_messages<typename MooreFSM_defs::stateOutput>(bags).push_back(state.stateOutput); 
		get_messages<typename MooreFSM_defs::transitionOut>(bags).push_back(state.nextActiveState);	
		}
	if(state.isError){
		get_messages<typename MooreFSM_defs::errorOut>(bags).push_back(state.errorMsg); 
		}
        return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.isEvent) {            
            next_internal = TIME("00:00:03:000");
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        } 
	return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename MooreFSM<TIME>::state_type& i) {
        os << " State: " << i.stateName << " & Output: " << i.stateOutput; 
	os << " & errorMsg: " << i.errorMsg << " & Phase: "<< i.phase; 
        return os;
    }
};

#endif // _MooreFSM_HPP__
