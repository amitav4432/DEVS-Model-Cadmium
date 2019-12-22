
/**************************************************************
* Amitav Shaw
* Arshpreet Singh
* amitav.shaw@gmail.com
*
* Carleton University
* 
* Coupled models and the links for Cold Drink Vending Machine
***************************************************************/


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
#include "VendingFSM.h"

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
struct StateOutputZero : public out_port<string>{}; //MooreFSM --> User
struct StateOutputOne : public out_port<string>{}; //MooreFSM --> User
struct StateOutputTwo : public out_port<string>{}; //MooreFSM --> User
struct StateOutputThree : public out_port<string>{}; //MooreFSM --> User
struct StateOutputFour : public out_port<string>{}; //MooreFSM --> User
struct StateOutputFive : public out_port<string>{}; //MooreFSM --> User
struct ExtErrorOut : public out_port<string>{}; // MooreFSM --> User

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
    	shared_ptr<dynamic::modeling::model> zero = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("zero","zero", "InsertLoonieOrToonie", "active", true, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> one = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("one", "one", "LoonieInserted", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> two = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("two", "two", "TwoDollarsInserted", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> three = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("three", "three", "SmallDrink", "inactive", false,fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> four = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("four", "four", "FourDollarsInserted", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> five = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("five", "five", "BigDrink", "inactive", false, fsm_callback_event, fsm_callback_transition);

		
	/*******MooreFSM COUPLED MODEL********/
	dynamic::modeling::Ports iports_VendingMooreFSMCoupled = {typeid(ExtEventIn)};
  	dynamic::modeling::Ports oports_VendingMooreFSMCoupled = {typeid(ExtErrorOut),typeid(StateOutputZero),typeid(StateOutputOne),typeid(StateOutputTwo),typeid(StateOutputThree),typeid(StateOutputFour),typeid(StateOutputFive)};
	
	dynamic::modeling::Models submodels_VendingMooreFSMCoupled = {zero, one, two, three, four, five};
    	
	dynamic::modeling::EICs eics_VendingMooreFSMCoupled = {
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("zero"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("one"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("two"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("three"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("four"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("five")
   	 };

    	dynamic::modeling::EOCs eocs_VendingMooreFSMCoupled = {
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputZero>("zero"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputOne>("one"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputTwo>("two"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputThree>("three"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputFour>("four"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputFive>("five"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,StateOutputZero>("zero"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("one"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("two"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("three"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("four"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("five")
    	};

   	dynamic::modeling::ICs ics_VendingMooreFSMCoupled = {
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("zero","one"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("zero","two"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("one","two"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("one","three"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("two","three"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("two","four"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("three","four"),
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("three","five"),   
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("four","five"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("three","zero"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("five","zero")   	
	};

   	shared_ptr<dynamic::modeling::coupled<TIME>> VendingMooreFSMCoupled;
    	VendingMooreFSMCoupled = make_shared<dynamic::modeling::coupled<TIME>>(
        "VendingMooreFSMCoupled", submodels_VendingMooreFSMCoupled, iports_VendingMooreFSMCoupled, oports_VendingMooreFSMCoupled, eics_VendingMooreFSMCoupled, eocs_VendingMooreFSMCoupled, ics_VendingMooreFSMCoupled 
    	);

	/*******TOP COUPLED MODEL********/
    	dynamic::modeling::Ports iports_TOP = {};
    	dynamic::modeling::Ports oports_TOP = {typeid(ExtErrorOut),typeid(StateOutputZero),typeid(StateOutputOne),typeid(StateOutputTwo),typeid(StateOutputThree),typeid(StateOutputFour),typeid(StateOutputFive)};
    	dynamic::modeling::Models submodels_TOP = {input_reader,VendingMooreFSMCoupled};
    	dynamic::modeling::EICs eics_TOP = {};
    	dynamic::modeling::EOCs eocs_TOP = {
        	dynamic::translate::make_EOC<ExtErrorOut,ExtErrorOut>("VendingMooreFSMCoupled"), 
        	dynamic::translate::make_EOC<StateOutputZero,StateOutputZero>("VendingMooreFSMCoupled"), 
        	dynamic::translate::make_EOC<StateOutputOne,StateOutputOne>("VendingMooreFSMCoupled"), 
        	dynamic::translate::make_EOC<StateOutputTwo,StateOutputTwo>("VendingMooreFSMCoupled"), 
        	dynamic::translate::make_EOC<StateOutputThree,StateOutputThree>("VendingMooreFSMCoupled"), 
        	dynamic::translate::make_EOC<StateOutputFour,StateOutputFour>("VendingMooreFSMCoupled"), 
        	dynamic::translate::make_EOC<StateOutputFive,StateOutputFive>("VendingMooreFSMCoupled")
	};
    	dynamic::modeling::ICs ics_TOP = {
	        dynamic::translate::make_IC<iestream_input_defs<string>::out, ExtEventIn>("input_reader","VendingMooreFSMCoupled")
    	};
    	shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    	TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    	);


    	/*************** Loggers *******************/
    	static ofstream out_messages("../simulation_results/VendingFSMMessages.txt");
    	struct oss_sink_messages{
        static ostream& sink(){			
        	return out_messages;
        	}
    	};
    	static ofstream out_state("../simulation_results/VendingFSMStates.txt");
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

	
