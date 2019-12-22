/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Callback function for Toaster application
**************************************************************/

#ifndef _TOASTER_H__
#define _TOASTER_H__

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


void fsm_callback_event(string eventIn, string transitionIn, string *nextActiveState, string *stateOutput, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!eventIn.empty()){
		if(eventIn == "On") {
			if(stateName == "Idle" && *phase == "active") {
				*nextActiveState = "SetRegulator";
				*stateOutput = "SwitchedOn";
				*phase = "inactive";
				*isEvent = true;
			}		
		} else if(eventIn == "Regulator1") {
			if(stateName == "SetRegulator" && *phase == "active") {
				*nextActiveState = "Toasting";
				*stateOutput = "Set To Regulator Level 1";
				*phase = "inactive";
				*isEvent = true;
		        } 
		} else if(eventIn == "Regulator2") {
			if(stateName == "SetRegulator" && *phase == "active") {
				*nextActiveState = "Toasting";
				*stateOutput = "Set To Regulator Level 2";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "Regulator3") {
			if(stateName == "SetRegulator" && *phase == "active") {
				*nextActiveState = "Toasting";
				*stateOutput = "Set To Regulator Level 3";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "TimerOut") {
			if(stateName == "Toasting" && *phase == "active") {
				*nextActiveState = "ThermostatOff";
				*stateOutput = "Toasting done";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "SwitchOff") {
			if(stateName == "ThermostatOff" && *phase == "active") {
				*nextActiveState = "Idle";
				*stateOutput = "Switching Off the Toaster";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if (*phase == "active") {
			*nextActiveState = "Idle";
			*errorMsg = "Invalid Input";
			*isError = true;
			*stateOutput = "Not a Valid Input";
			if(stateName!="Idle"){ // So that the first state remains in active state even if wrong EventIn comes in.
				*phase = "inactive";
			}
			*isEvent = true;
			}
		}
}

void fsm_callback_transition(string eventIn, string transitionIn, string *nextActiveState, string *stateOutput, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!transitionIn.empty()) {
		if (stateName == transitionIn) {
			*phase = "active";
			*stateOutput = "";
			*isEvent = true;
		} 	
	} 
}    


#endif // _TOASTER_H__
