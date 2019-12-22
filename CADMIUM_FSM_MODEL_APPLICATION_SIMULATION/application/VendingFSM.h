/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Callback function for Vending Machine application
**************************************************************/

#ifndef _VEND_H__
#define _VEND_H__

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


void fsm_callback_event(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!eventIn.empty()){
		if(eventIn == "one"){
		       if(stateName == "zero" && *phase == "active") {
				*nextActiveState = "one";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "one" && *phase == "active") {
				*nextActiveState = "two";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "two" && *phase == "active") {
				*nextActiveState = "three";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "three" && *phase == "active") {
				*nextActiveState = "four";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "four" && *phase == "active") {
				*nextActiveState = "five";
				*phase = "inactive";
				*isEvent = true;
			}	
		} else if(eventIn == "two"){
			if(stateName == "one" && *phase == "active") {
				*nextActiveState = "three";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "zero" && *phase == "active") {
				*nextActiveState = "two";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "two" && *phase == "active") {
				*nextActiveState = "four";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "three" && *phase == "active") {
				*nextActiveState = "five";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "PushButton"){
			if(stateName == "three" && *phase == "active") {
				*nextActiveState = "zero";
				*phase = "inactive";
				*isEvent = true;
			} else if(stateName == "five" && *phase == "active") {
				*nextActiveState = "zero";
				*phase = "inactive";
				*isEvent = true;
			}
		}
		else if(eventIn!="one" && eventIn!= "two" && eventIn!="PushButton"){
			*isError = true;
			*errorMsg = "Try Again...";
			*nextActiveState = "zero";
			*phase = "inactive";
			*isEvent = true;
			}
	}
}
void fsm_callback_transition(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!transitionIn.empty()) {
		if (transitionIn == stateName) {
			*phase = "active";
			*isEvent = true;
		} 		
	} 
}    

#endif // _VEND_H__
