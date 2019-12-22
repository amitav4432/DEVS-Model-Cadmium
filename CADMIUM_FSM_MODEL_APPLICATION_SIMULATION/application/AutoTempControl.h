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


int ReferenceTemp = 80;
int TemperatureSensed = 0;
float COlevel = 0;
bool flag = false;
bool isInteger(const std::string & s);

void fsm_callback_event(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!eventIn.empty()){
		if(isInteger(eventIn)) {
			TemperatureSensed = stoi(eventIn);
			flag = true;
                }
		if(eventIn == "setTemperature" && stateName == "TempSet" && *phase == "active"){
			*nextActiveState = "TempSense";
			*phase = "inactive";
			*isEvent = true;
		} else if (flag && stateName == "TempSense" && *phase == "active"){
			if(TemperatureSensed < ReferenceTemp && TemperatureSensed > -40){
				*nextActiveState = "HeaterOn";
				*phase = "inactive";
				*isEvent = true;
				flag = false;
			} else if (TemperatureSensed > ReferenceTemp && TemperatureSensed < 120){
				*nextActiveState = "AirCondOn";
				*phase = "inactive";
				*isEvent = true;
				flag = false;
			} else if (-40 > TemperatureSensed || TemperatureSensed > 120){
				*isError = true;
				*nextActiveState = "TempSet";
				*errorMsg = "Invalid Temperature sensed";
			}		
		}else if (!flag && eventIn!= "setTemperature"){
			*isError = true;
			*errorMsg = "Invalid Input";
			*nextActiveState = "TempSet";	
		}
	}
}
void fsm_callback_transition(string eventIn, string transitionIn, string *nextActiveState, string *errorMsg, string stateName, string *phase, bool *isEvent, bool *isError){		
	if(!transitionIn.empty()) {
		if ((transitionIn == stateName) && (stateName =="TempSense")) {
			if(flag && TemperatureSensed < ReferenceTemp && TemperatureSensed > -40){
				*nextActiveState = "HeaterOn";
				TemperatureSensed = 0;
			} else if (flag && TemperatureSensed > ReferenceTemp && TemperatureSensed < 120){
				*nextActiveState = "AirCondOn";
				TemperatureSensed = 0;
			} else if (flag && (-40 > TemperatureSensed || TemperatureSensed > 120)){
				*isError = true;
				*nextActiveState = "TempSet";
				*errorMsg = "Invalid Temperature sensed";
			}
			*phase = "active";
			*isEvent = true;
		} else if (stateName == "AirCondOn" && transitionIn == "AirCondOn") {
			*nextActiveState = "TempSet";
			*phase = "inactive";
			*isEvent = true;
		} else if(stateName == "HeaterOn" && transitionIn == "HeaterOn"){
			COlevel = rand()%100 + 20;
			if(COlevel < 50){
				*nextActiveState = "CODetectOK";
			}
			else if (COlevel >50) {
				*nextActiveState = "CODetectAlarm";
			}	
			*phase = "inactive";
			*isEvent = true;
		} else if(stateName == "CODetectOK" && transitionIn == "CODetectOK"){
			*nextActiveState = "TempSet";
			*phase = "inactive";
			*isEvent = true;
		} else if(stateName == "CODetectAlarm" && transitionIn == "CODetectAlarm"){
			*nextActiveState = "TempSet";
			*phase = "inactive";
			*isEvent = true;
		}else if(stateName == "TempSet" && transitionIn == "TempSet"){
			*phase = "active";
			*isEvent = true;
		}		
	} 
}    

bool isInteger(const std::string &s)
{
   if(s.empty()||((!isdigit(s[0]))&&(s[0]!='-')&&(s[0]!='+'))) { 
	return false;
   }
   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}
#endif // _AUTOMATICTEMPCONTROL_H__
