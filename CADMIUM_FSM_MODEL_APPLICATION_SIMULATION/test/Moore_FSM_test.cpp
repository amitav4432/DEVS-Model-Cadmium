
/*************************************************************
* Amitav Shaw
* Arshpreet Singh
* 
* Carleton University
*
* Test for MooreFSM Atomic Model
**************************************************************/
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
#define STRING_DS
//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/MooreFSM.hpp"
#include "../application/VendingFSM.h"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct top_out: public out_port<string>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<Message_t,T> {
    public:
        InputReader_Message_t () = default;
        InputReader_Message_t (const char* file_path) : iestream_input<Message_t,T>(file_path) {}
};

template<typename T>
class InputReader_String : public iestream_input<string,T> {
    public:
        InputReader_String() = default;
    	InputReader_String(const char* file_path) : iestream_input<string,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 3) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the EventIn input and TransitionIn input txt file" << endl;
        return 1; 
    }

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data = argv[1]; //"../input_data/FSM_EventIn.txt";
    shared_ptr<dynamic::modeling::model> input_reader_EventIn;
    input_reader_EventIn = dynamic::translate::make_dynamic_atomic_model<InputReader_String, TIME, const char*>("input_reader_EventIn", move(i_input_data));

    const char * i_input_data1 = argv[2]; //"../input_data/FSM_TransitionIn.txt";
    shared_ptr<dynamic::modeling::model> input_reader_TransitionIn;
    input_reader_TransitionIn = dynamic::translate::make_dynamic_atomic_model<InputReader_String, TIME, const char*>("input_reader_TransitionIn", move(i_input_data1));



    /****** MooreFSM atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> MooreFSM1;
    MooreFSM1 = dynamic::translate::make_dynamic_atomic_model<MooreFSM, TIME>("MooreFSM1","zero","InsertLoonieOrToonie","active",true,fsm_callback_event,fsm_callback_transition);

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_EventIn,input_reader_TransitionIn, MooreFSM1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<MooreFSM_defs::transitionOut,top_out>("MooreFSM1"),
	dynamic::translate::make_EOC<MooreFSM_defs::stateOutput,top_out>("MooreFSM1"),
	dynamic::translate::make_EOC<MooreFSM_defs::errorOut,top_out>("MooreFSM1")	
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<string>::out,MooreFSM_defs::eventIn>("input_reader_EventIn","MooreFSM1"),
	dynamic::translate::make_IC<iestream_input_defs<string>::out,MooreFSM_defs::transitionIn>("input_reader_TransitionIn","MooreFSM1")
    };

    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/MooreFSM_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/MooreFSM_test_output_state.txt");
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
    r.run_until(NDTime("04:00:00:000"));
    return 0;
}
