/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Callback function for Online Shopping Payment application
**************************************************************/

#ifndef _AUTOMATICTEMPCONTROL_H__
#define _AUTOMATICTEMPCONTROL_H__

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

/***************Book database {"ParticlePhysics", "ComputationalGeometry", "StringTheory", "Cosmos", "HeavyMetalHistory"}********/

/*************************************************Password: "MaCoDrum"**********************************************************/

void fsm_callback_event(string eventIn, string transitionIn, string *nextActiveState, string *stateOutput, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!eventIn.empty()){
		if(eventIn == "SearchBook") {
			if(stateName == "EnterOnlineLibrary" && *phase == "active") {
				*nextActiveState = "Searching";
				*stateOutput = "Enter the Book Name";
				*phase = "inactive";
				*isEvent = true;
			}		
		} else if(eventIn == "ParticlePhysics" || eventIn == "ComputationalGeometry"|| eventIn == "StringTheory"|| eventIn == 
			"Cosmos"|| eventIn == "HeavyMetalHistory") {
			if(stateName == "Searching" && *phase == "active") {
				*nextActiveState = "Login";
				*stateOutput = "Enter Login Details to Access.";
				*phase = "inactive";
				*isEvent = true;
		        } 
		} else if(eventIn != "ParticlePhysics" && eventIn != "ComputationalGeometry" && eventIn != "StringTheory" && eventIn != 
			"Cosmos" && eventIn != "HeavyMetalHistory" && stateName == "Searching" && *phase == "active") {
				*nextActiveState = "LibraryAddr";
				*stateOutput = "Not Available Online, Go to Library Building";
				*phase = "inactive";
				*isEvent = true;
		} else if(eventIn == "MaCoDrum") {
			if(stateName == "Login" && *phase == "active") {
				*nextActiveState = "Download";
				*stateOutput = "Download Link";
				*phase = "inactive";
				*isEvent = true;
			}
		} else if(eventIn != "MaCoDrum" && stateName == "Login" && *phase == "active") {
				*stateOutput = "Enter Correct Password";
				*isEvent = true;
		} else if (*phase == "active") {
			*nextActiveState = "EnterOnlineLibrary";
			*errorMsg = "Invalid Input";
			*isError = true;
			*stateOutput = "Not a Valid Input";
			if(stateName!="EnterOnlineLibrary"){ // So that the first state remains in active state even if wrong EventIn comes in.
				*phase = "inactive";
			}
			*isEvent = true;
		}
	}	
}

void fsm_callback_transition(string eventIn, string transitionIn, string *nextActiveState, string *stateOutput, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!transitionIn.empty()) {
		if (stateName == transitionIn && stateName == "Download") {
			*nextActiveState = "EnterOnlineLibrary";
			*phase = "inactive";
			*stateOutput = "";
			*isEvent = true;
		} else if (stateName == transitionIn && stateName == "LibraryAddr") {
			*nextActiveState = "EnterOnlineLibrary";
			*phase = "inactive";
			*stateOutput = "";
			*isEvent = true;
		} else if (stateName == transitionIn) {
			*phase = "active";
			*stateOutput = "";
			*isEvent = true;
		} 	
	} 
}    

#endif // _AUTOMATICTEMPCONTROL_H__
