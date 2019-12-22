/***************************************************************************
* Amitav Shaw
* Arshpreet Singh
* amitav.shaw@gmail.com
*
* Carleton University
* 
* Coupled models and the links for Automatic Temperature Controller system
***************************************************************************/


//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/message.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/MooreFSM.hpp"
#include "AutoTempControl.h"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct ExtEventIn : public in_port<string>{}; // input --> MooreFSM

/***** Define output ports for coupled model *****/
struct OutputTempSet : public out_port<string>{};
struct OutputTempSense : public out_port<string>{};
struct OutputAirCondOn : public out_port<string>{};
struct OutputHeaterOn : public out_port<string>{};
struct OutputCODetectOk : public out_port<string>{}; 
struct OutputCODetectAlarm : public out_port<string>{};
struct ExtErrorOut : public out_port<string>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_String : public iestream_input<string,T> {
public:
    InputReader_String() = default;
    InputReader_String(const char* file_path) : iestream_input<string,T>(file_path) {}
};


int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the event_input txt files " << endl;
        return 1; 
    }
   	/****** Input Reader atomic model instantiation *******************/
   	string input = argv[1];
    	const char * i_input = input.c_str();
    	shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_String, TIME, const char* >("input_reader" , move(i_input));
/******************************************************************************************************************************************************************************
* State definitions: Each state is instantiated with its state Name, State Output, initial state (active/inactive), isEvent (true for the first state so that it can be entered
* The first state is kept active and rest inactive during initialization so that it can process the first event and simulate the state machine
*******************************************************************************************************************************************************************************/
    	shared_ptr<dynamic::modeling::model> TemperatureSet = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("TempSet", "TempSet", "Enter Reference Temp.", "active", true, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> TemperatureSensed = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("TempSense", "TempSense", "Sensing Room Temp.",  "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> AirConditionOn = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("AirCondOn", "AirCondOn", "AC ON", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> HeaterOn = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("HeaterOn", "HeaterOn", "Heater Turned ON", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> CODetectOk = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("CODetectOk", "CODetectOk", "CO Not Detected", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> CODetectAlarm = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("CODetectAlarm", "CODetectAlarm", "CO Detected", "inactive", false, fsm_callback_event, fsm_callback_transition);

		
	/*******MooreFSM COUPLED MODEL********/
	dynamic::modeling::Ports iports_AutomaticTempController = {typeid(ExtEventIn)};
  	dynamic::modeling::Ports oports_AutomaticTempController = {typeid(ExtErrorOut),typeid(OutputTempSet),typeid(OutputTempSense),typeid(OutputAirCondOn),typeid(OutputHeaterOn),typeid(OutputCODetectOk),typeid(OutputCODetectAlarm)};
	
	dynamic::modeling::Models submodels_AutomaticTempController = {TemperatureSet, TemperatureSensed, HeaterOn, AirConditionOn, CODetectOk, CODetectAlarm};
    	
	dynamic::modeling::EICs eics_AutomaticTempController = {
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("TempSet"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("TempSense"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("AirCondOn"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("HeaterOn"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("CODetectOk"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("CODetectAlarm")
   	};

    	dynamic::modeling::EOCs eocs_AutomaticTempController = {
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputTempSet>("TempSet"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputTempSense>("TempSense"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputAirCondOn>("AirCondOn"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputHeaterOn>("HeaterOn"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputCODetectOk>("CODetectOk"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputCODetectAlarm>("CODetectAlarm"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputTempSet>("TempSet"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("TempSense"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("AirCondOn"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("HeaterOn"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("CODetectOk"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("CODetectAlarm")
    	};

   	dynamic::modeling::ICs ics_AutomaticTempController = {
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("TempSet","TempSense"),   	  	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("TempSense","AirCondOn"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("TempSense","HeaterOn"),   	  	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("HeaterOn","CODetectAlarm"),
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("HeaterOn","CODetectOk"),
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("AirCondOn","TempSet"), 
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("CODetectOk","TempSet"),  
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("CODetectAlarm","TempSet"),
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("TempSense","TempSet")   	
	};

   	shared_ptr<dynamic::modeling::coupled<TIME>> AutomaticTempController;
    	AutomaticTempController = make_shared<dynamic::modeling::coupled<TIME>>(
        "AutomaticTempController", submodels_AutomaticTempController, iports_AutomaticTempController, oports_AutomaticTempController, eics_AutomaticTempController, eocs_AutomaticTempController, ics_AutomaticTempController 
    	);

	/*******TOP COUPLED MODEL********/
    	dynamic::modeling::Ports iports_TOP = {};
    	dynamic::modeling::Ports oports_TOP = {typeid(ExtErrorOut),typeid(OutputTempSet),typeid(OutputTempSense),typeid(OutputAirCondOn),typeid(OutputHeaterOn),typeid(OutputCODetectOk),typeid(OutputCODetectAlarm)};
    	dynamic::modeling::Models submodels_TOP = {input_reader,AutomaticTempController};
    	dynamic::modeling::EICs eics_TOP = {};
    	dynamic::modeling::EOCs eocs_TOP = {
        	dynamic::translate::make_EOC<ExtErrorOut,ExtErrorOut>("AutomaticTempController"), 
        	dynamic::translate::make_EOC<OutputTempSet,OutputTempSet>("AutomaticTempController"), 
        	dynamic::translate::make_EOC<OutputTempSense,OutputTempSense>("AutomaticTempController"), 
        	dynamic::translate::make_EOC<OutputAirCondOn,OutputAirCondOn>("AutomaticTempController"), 
        	dynamic::translate::make_EOC<OutputHeaterOn,OutputHeaterOn>("AutomaticTempController"), 
        	dynamic::translate::make_EOC<OutputCODetectOk,OutputCODetectOk>("AutomaticTempController"), 
        	dynamic::translate::make_EOC<OutputCODetectAlarm,OutputCODetectAlarm>("AutomaticTempController")
	};
    	dynamic::modeling::ICs ics_TOP = {
	        dynamic::translate::make_IC<iestream_input_defs<string>::out, ExtEventIn>("input_reader","AutomaticTempController")
    	};
    	shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    	TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    	);


    	/*************** Loggers *******************/
    	static ofstream out_messages("../simulation_results/AutoTempControlMsg.txt");
    	struct oss_sink_messages{
        static ostream& sink(){			
        	return out_messages;
        	}
    	};
    	static ofstream out_state("../simulation_results/AutoTempControlStates.txt");
    	struct oss_sink_state{
        	static ostream& sink(){
            		return out_state;
    		}
    	};
   
    	using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    	using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    	using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    	using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

   	using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    	/************** Runner call ************************/ 
    	dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    	r.run_until_passivate();
    	return 0;
}

	
