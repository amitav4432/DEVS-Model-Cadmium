/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Callback function for Elevator application
**************************************************************/

#ifndef _ELEVATOR_H__
#define _ELEVATOR_H__

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
		if(eventIn == "GoUp") {
			if(stateName == "Idle" && *phase == "active") {
				*nextActiveState = "MovingUp";
				*stateOutput = "Going Up..";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "Bottom" && *phase == "active") {
				*nextActiveState = "MovingUp";
				*stateOutput = "Going Up from Bottom..";
				*phase = "inactive";
				*isEvent = true;
			}		
		} else if(eventIn == "GoDown") {
			if(stateName == "Idle" && *phase == "active") {
				*nextActiveState = "MovingDown";
				*stateOutput = "Going Down..";
				*phase = "inactive";
				*isEvent = true;
		        } else if(stateName == "Top" && *phase == "active"){
				*nextActiveState = "MovingDown";
				*stateOutput = "Going Down from Top..";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "GoToTop") {
			if(stateName == "MovingUp" && *phase == "active") {
				*nextActiveState = "Top";
				*stateOutput = "Going To Top..";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "GoToBottom") {
			if(stateName == "MovingDown" && *phase == "active") {
				*nextActiveState = "Bottom";
				*stateOutput = "Going To Bottom..";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "Halt") {
			if(stateName == "MovingDown" && *phase == "active") {
				*nextActiveState = "Idle";
				*stateOutput = "Halting After Moving Up";
				*phase = "inactive";
				*isEvent = true;
			}else if(stateName == "MovingUp" && *phase == "active") {
				*nextActiveState = "Idle";
				*stateOutput = "Halting After Moving Up";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "GoToBottom") {
			if(stateName == "MovingUp" && *phase == "active") {
				*nextActiveState = "Idle";
				*stateOutput = "Halting After Moving Down";
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


#endif // _ELEVATOR_H__
