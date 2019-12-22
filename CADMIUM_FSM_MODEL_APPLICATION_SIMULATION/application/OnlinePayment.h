/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Callback function for Online Shopping Payment application
**************************************************************/

#ifndef _ONLINEPAYMENT_H__
#define _ONLINEPAYMENT_H__

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

string cardPin = "109";

void fsm_callback_event(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!eventIn.empty()){
		if(eventIn == cardPin){
		       if(stateName == "CardDetails" && *phase == "active") {
				*nextActiveState = "ConfirmDetails";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "Confirmed"){
			if(stateName == "ConfirmDetails" && *phase == "active") {
				*nextActiveState = "Completed";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "NotConfirmed"){
			if(stateName == "ConfirmDetails" && *phase == "active") {
				*nextActiveState = "Aborted";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn == "Cancel"){
			if(stateName == "Completed" && *phase == "active") {
				*nextActiveState = "Refunded";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn!=cardPin){
			if(stateName == "CardDetails" && *phase == "active") {
				*isError = true;
				*errorMsg = "Invalid Card Detail";
				*nextActiveState = "WrongDetails";
				*phase = "inactive";
				*isEvent = true;
			}
		}
	}
}
void fsm_callback_transition(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!transitionIn.empty()) {
		if ((transitionIn == stateName) && ((stateName!="Refunded") && (stateName!="Aborted") &&  (stateName!="WrongDetails"))) {
			*phase = "active";
			*isEvent = true;
		} else if(stateName == "Refunded" && transitionIn == "Refunded"){
			*nextActiveState = "CardDetails";
			*phase = "inactive";
			*isEvent = true;
		} else if(stateName == "Completed" && transitionIn == "Completed"){
			*nextActiveState = "CardDetails";
			*phase = "inactive";
			*isEvent = true;
		}else if(stateName == "Aborted" && transitionIn == "Aborted"){
			*nextActiveState = "CardDetails";
			*phase = "inactive";
			*isEvent = true;
		}else if(stateName == "WrongDetails" && transitionIn == "WrongDetails"){
			*nextActiveState = "CardDetails";
			*phase = "inactive";
			*isEvent = true;
		}		
	} 
}    

#endif // _ONLINEPAYMENT_H__
