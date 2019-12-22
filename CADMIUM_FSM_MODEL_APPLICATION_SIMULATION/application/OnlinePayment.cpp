/*****************************************************************
* Amitav Shaw
* Arshpreet Singh
* amitav.shaw@gmail.com
*
* Carleton University
* 
* Coupled models and the links for Online Shopping Payment system
******************************************************************/


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
#define STRING_DS 1
//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/MooreFSM.hpp"
#include "OnlinePayment.h"

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
struct OutputCardDetails : public out_port<string>{};
struct OutputConfirmDetails : public out_port<string>{};
struct OutputCompleted : public out_port<string>{};
struct OutputAborted : public out_port<string>{};
struct OutputRefunded : public out_port<string>{}; 
struct OutputWrongDetails : public out_port<string>{};
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
    	shared_ptr<dynamic::modeling::model> CardDetails = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("CardDetails", "CardDetails", "Enter Card Details", "active", true, fsm_callback_event,fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> ConfirmDetails = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("ConfirmDetails", "ConfirmDetails", "Confirm your details", "inactive",false,fsm_callback_event,fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Completed = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("Completed", "Completed", "Transaction Completed","inactive",false,fsm_callback_event,fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Aborted = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("Aborted", "Aborted","Transaction Aborted", "inactive",false,fsm_callback_event,fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Refunded = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("Refunded", "Refunded", "Transaction Refunded", "inactive",false,fsm_callback_event,fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> WrongDetails = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("WrongDetails", "WrongDetails", "Incorrect details", "inactive",false,fsm_callback_event,fsm_callback_transition);

		
	/*******MooreFSM COUPLED MODEL********/
	dynamic::modeling::Ports iports_OnlineShoppingPayment = {typeid(ExtEventIn)};
  	dynamic::modeling::Ports oports_OnlineShoppingPayment = {typeid(ExtErrorOut),typeid(OutputCardDetails),typeid(OutputConfirmDetails),typeid(OutputCompleted),typeid(OutputAborted),typeid(OutputRefunded),typeid(OutputWrongDetails)};
	
	dynamic::modeling::Models submodels_OnlineShoppingPayment = {CardDetails, ConfirmDetails, Completed, Aborted, Refunded, WrongDetails};
    	
	dynamic::modeling::EICs eics_OnlineShoppingPayment = {
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("CardDetails"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("ConfirmDetails"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("Completed"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("Aborted"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("Refunded"),
        dynamic::translate::make_EIC<ExtEventIn, MooreFSM_defs::eventIn>("WrongDetails")
   	};

    	dynamic::modeling::EOCs eocs_OnlineShoppingPayment = {
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputCardDetails>("CardDetails"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputConfirmDetails>("ConfirmDetails"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputCompleted>("Completed"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputAborted>("Aborted"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputRefunded>("Refunded"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputWrongDetails>("WrongDetails"),
        dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,OutputCardDetails>("CardDetails"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("ConfirmDetails"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("Completed"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("Aborted"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("Refunded"),
        dynamic::translate::make_EOC<MooreFSM_defs::errorOut,ExtErrorOut>("WrongDetails")
    	};

   	dynamic::modeling::ICs ics_OnlineShoppingPayment = {
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("CardDetails","ConfirmDetails"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("CardDetails","WrongDetails"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("ConfirmDetails","Completed"),   	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("ConfirmDetails","Aborted"),   	  	
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("Completed","Refunded"),
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("Completed","CardDetails"), 
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("Refunded","CardDetails"), 
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("Aborted","CardDetails"), 
	dynamic::translate::make_IC<MooreFSM_defs::transitionOut, MooreFSM_defs::transitionIn>("WrongDetails","CardDetails")    	
	};

   	shared_ptr<dynamic::modeling::coupled<TIME>> OnlineShoppingPayment;
    	OnlineShoppingPayment = make_shared<dynamic::modeling::coupled<TIME>>(
        "OnlineShoppingPayment", submodels_OnlineShoppingPayment, iports_OnlineShoppingPayment, oports_OnlineShoppingPayment, eics_OnlineShoppingPayment, eocs_OnlineShoppingPayment, ics_OnlineShoppingPayment 
    	);

	/*******TOP COUPLED MODEL********/
    	dynamic::modeling::Ports iports_TOP = {};
    	dynamic::modeling::Ports oports_TOP = {typeid(ExtErrorOut),typeid(OutputCardDetails),typeid(OutputConfirmDetails),typeid(OutputCompleted),typeid(OutputAborted),typeid(OutputRefunded),typeid(OutputWrongDetails)};
    	dynamic::modeling::Models submodels_TOP = {input_reader,OnlineShoppingPayment};
    	dynamic::modeling::EICs eics_TOP = {};
    	dynamic::modeling::EOCs eocs_TOP = {
        	dynamic::translate::make_EOC<ExtErrorOut,ExtErrorOut>("OnlineShoppingPayment"), 
        	dynamic::translate::make_EOC<OutputCardDetails,OutputCardDetails>("OnlineShoppingPayment"), 
        	dynamic::translate::make_EOC<OutputConfirmDetails,OutputConfirmDetails>("OnlineShoppingPayment"), 
        	dynamic::translate::make_EOC<OutputCompleted,OutputCompleted>("OnlineShoppingPayment"), 
        	dynamic::translate::make_EOC<OutputAborted,OutputAborted>("OnlineShoppingPayment"), 
        	dynamic::translate::make_EOC<OutputRefunded,OutputRefunded>("OnlineShoppingPayment"), 
        	dynamic::translate::make_EOC<OutputWrongDetails,OutputWrongDetails>("OnlineShoppingPayment")
	};
    	dynamic::modeling::ICs ics_TOP = {
	        dynamic::translate::make_IC<iestream_input_defs<string>::out, ExtEventIn>("input_reader","OnlineShoppingPayment")
    	};
    	shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    	TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    	);


    	/*************** Loggers *******************/
    	static ofstream out_messages("../simulation_results/OnlinePaymentMsg.txt");
    	struct oss_sink_messages{
        static ostream& sink(){			
        	return out_messages;
        	}
    	};
    	static ofstream out_state("../simulation_results/OnlinePaymentStates.txt");
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

	
