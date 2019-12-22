/***************************************************************************
* Amitav Shaw
* Arshpreet Singh
* amitav.shaw@gmail.com
*
* Carleton University
* 
* Coupled models and the links for Online Library system
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
#include "../atomics/MealyFSM.hpp"
#include "OnlineLibrary.h"

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
struct ExtEventIn : public in_port<string>{}; // input --> MealyFSM

/***** Define output ports for coupled model *****/
struct OutputEnterOnlineLibrary : public out_port<string>{};
struct OutputSearching : public out_port<string>{};
struct OutputLogin : public out_port<string>{};
struct OutputDownload : public out_port<string>{};
struct OutputLibraryAddr : public out_port<string>{}; 
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
    	shared_ptr<dynamic::modeling::model> EnterOnlineLibrary = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("EnterOnlineLibrary", "EnterOnlineLibrary", "active", true, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Searching = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("Searching", "Searching", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Login = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("Login", "Login", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Download = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("Download", "Download", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> LibraryAddr = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("LibraryAddr", "LibraryAddr", "inactive", false, fsm_callback_event, fsm_callback_transition);

		
	/*******MEALYFSM COUPLED MODEL********/
	dynamic::modeling::Ports iports_OnlineLibrary = {typeid(ExtEventIn)};
  	dynamic::modeling::Ports oports_OnlineLibrary = {typeid(ExtErrorOut),typeid(OutputEnterOnlineLibrary),typeid(OutputSearching),typeid(OutputLogin),typeid(OutputDownload),typeid(OutputLibraryAddr)};
	
	dynamic::modeling::Models submodels_OnlineLibrary = {EnterOnlineLibrary, Searching, Login, Download, LibraryAddr};
    	
	dynamic::modeling::EICs eics_OnlineLibrary = {
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("EnterOnlineLibrary"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("Searching"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("Login"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("Download")
        };

    	dynamic::modeling::EOCs eocs_OnlineLibrary = {
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputEnterOnlineLibrary>("EnterOnlineLibrary"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputSearching>("Searching"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputLogin>("Login"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputDownload>("Download"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputLibraryAddr>("LibraryAddr"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("Searching"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("Login"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("Download"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("LibraryAddr"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("EnterOnlineLibrary")
    	};

   	dynamic::modeling::ICs ics_OnlineLibrary = {
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("EnterOnlineLibrary","Searching"),   	  	
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Searching","Login"),   	
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Searching","LibraryAddr"),   	  	
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Download","EnterOnlineLibrary"),
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Login","Download"), 
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("LibraryAddr","EnterOnlineLibrary"),  
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Login","LibraryAddr")	
	};

   	shared_ptr<dynamic::modeling::coupled<TIME>> OnlineLibrary;
    	OnlineLibrary = make_shared<dynamic::modeling::coupled<TIME>>(
        "OnlineLibrary", submodels_OnlineLibrary, iports_OnlineLibrary, oports_OnlineLibrary, eics_OnlineLibrary, eocs_OnlineLibrary, ics_OnlineLibrary 
    	);

	/*******TOP COUPLED MODEL********/
    	dynamic::modeling::Ports iports_TOP = {};
    	dynamic::modeling::Ports oports_TOP = {typeid(ExtErrorOut),typeid(OutputEnterOnlineLibrary),typeid(OutputSearching),typeid(OutputLogin),typeid(OutputDownload),typeid(OutputLibraryAddr)};
    	dynamic::modeling::Models submodels_TOP = {input_reader, OnlineLibrary};
    	dynamic::modeling::EICs eics_TOP = {};
    	dynamic::modeling::EOCs eocs_TOP = {
        	dynamic::translate::make_EOC<ExtErrorOut,ExtErrorOut>("OnlineLibrary"), 
        	dynamic::translate::make_EOC<OutputEnterOnlineLibrary,OutputEnterOnlineLibrary>("OnlineLibrary"), 
        	dynamic::translate::make_EOC<OutputSearching,OutputSearching>("OnlineLibrary"), 
        	dynamic::translate::make_EOC<OutputLogin,OutputLogin>("OnlineLibrary"), 
        	dynamic::translate::make_EOC<OutputDownload,OutputDownload>("OnlineLibrary"), 
        	dynamic::translate::make_EOC<OutputLibraryAddr,OutputLibraryAddr>("OnlineLibrary")
	};
    	dynamic::modeling::ICs ics_TOP = {
	        dynamic::translate::make_IC<iestream_input_defs<string>::out, ExtEventIn>("input_reader","OnlineLibrary")
    	};
    	shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    	TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    	);


    	/*************** Loggers *******************/
    	static ofstream out_messages("../simulation_results/OnlineLibraryMsg.txt");
    	struct oss_sink_messages{
        static ostream& sink(){			
        	return out_messages;
        	}
    	};
    	static ofstream out_state("../simulation_results/OnlineLibraryStates.txt");
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

	
