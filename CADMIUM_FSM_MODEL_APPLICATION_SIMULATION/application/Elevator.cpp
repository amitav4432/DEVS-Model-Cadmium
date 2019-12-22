/***************************************************************************
* Amitav Shaw
* Arshpreet Singh
* amitav.shaw@gmail.com
*
* Carleton University
* 
* Coupled models and the links for Elevator system
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
#include "Elevator.h"

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
struct OutputIdle : public out_port<string>{};
struct OutputMovingUp : public out_port<string>{};
struct OutputMovingDown : public out_port<string>{};
struct OutputTop : public out_port<string>{};
struct OutputBottom : public out_port<string>{}; 
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
    	shared_ptr<dynamic::modeling::model> Idle = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("Idle", "Idle", "active", true, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> MovingUp = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("MovingUp", "MovingUp", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> MovingDown = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("MovingDown", "MovingDown", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Top = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("Top", "Top", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	shared_ptr<dynamic::modeling::model> Bottom = dynamic::translate::make_dynamic_atomic_model<MealyFSM, TIME>("Bottom", "Bottom", "inactive", false, fsm_callback_event, fsm_callback_transition);
    	
		
	/*******MEALYFSM COUPLED MODEL********/
	dynamic::modeling::Ports iports_Elevator = {typeid(ExtEventIn)};
  	dynamic::modeling::Ports oports_Elevator = {typeid(ExtErrorOut),typeid(OutputIdle),typeid(OutputMovingUp),typeid(OutputMovingDown),typeid(OutputTop),typeid(OutputBottom)};
	
	dynamic::modeling::Models submodels_Elevator = {Idle, MovingUp, MovingDown, Top, Bottom};
    	
	dynamic::modeling::EICs eics_Elevator = {
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("Idle"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("MovingUp"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("MovingDown"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("Top"),
        dynamic::translate::make_EIC<ExtEventIn, MealyFSM_defs::eventIn>("Bottom")
   	};

    	dynamic::modeling::EOCs eocs_Elevator = {
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputIdle>("Idle"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputMovingUp>("MovingUp"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputMovingDown>("MovingDown"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputTop>("Top"),
        dynamic::translate::make_EOC<MealyFSM_defs::stateOutput,OutputBottom>("Bottom"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("MovingUp"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("MovingDown"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("Top"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("Bottom"),
        dynamic::translate::make_EOC<MealyFSM_defs::errorOut,ExtErrorOut>("Idle")
    	};

   	dynamic::modeling::ICs ics_Elevator = {
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Idle","MovingUp"),   	  	
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Idle","MovingDown"),   	
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("MovingUp","Top"),   	  	
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Top","MovingDown"),
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("Bottom","MovingUp"),
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("MovingUp","Idle"), 
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("MovingDown","Idle"),  
	dynamic::translate::make_IC<MealyFSM_defs::transitionOut, MealyFSM_defs::transitionIn>("MovingDown","Bottom") 	
	};

   	shared_ptr<dynamic::modeling::coupled<TIME>> Elevator;
    	Elevator = make_shared<dynamic::modeling::coupled<TIME>>(
        "Elevator", submodels_Elevator, iports_Elevator, oports_Elevator, eics_Elevator, eocs_Elevator, ics_Elevator 
    	);

	/*******TOP COUPLED MODEL********/
    	dynamic::modeling::Ports iports_TOP = {};
    	dynamic::modeling::Ports oports_TOP = {typeid(ExtErrorOut),typeid(OutputIdle),typeid(OutputMovingUp),typeid(OutputMovingDown),typeid(OutputTop),typeid(OutputBottom)};
    	dynamic::modeling::Models submodels_TOP = {input_reader,Elevator};
    	dynamic::modeling::EICs eics_TOP = {};
    	dynamic::modeling::EOCs eocs_TOP = {
        	dynamic::translate::make_EOC<ExtErrorOut,ExtErrorOut>("Elevator"), 
        	dynamic::translate::make_EOC<OutputIdle,OutputIdle>("Elevator"), 
        	dynamic::translate::make_EOC<OutputMovingUp,OutputMovingUp>("Elevator"), 
        	dynamic::translate::make_EOC<OutputMovingDown,OutputMovingDown>("Elevator"), 
        	dynamic::translate::make_EOC<OutputTop,OutputTop>("Elevator"), 
        	dynamic::translate::make_EOC<OutputBottom,OutputBottom>("Elevator") 
	};
    	dynamic::modeling::ICs ics_TOP = {
	        dynamic::translate::make_IC<iestream_input_defs<string>::out, ExtEventIn>("input_reader","Elevator")
    	};
    	shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    	TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    	);


    	/*************** Loggers *******************/
    	static ofstream out_messages("../simulation_results/ElevatorMsg.txt");
    	struct oss_sink_messages{
        static ostream& sink(){			
        	return out_messages;
        	}
    	};
    	static ofstream out_state("../simulation_results/ElevatorStates.txt");
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

	
