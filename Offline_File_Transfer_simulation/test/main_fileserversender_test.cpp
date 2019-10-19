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
#include "../atomics/FileServerSender.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/

/***** Define output ports for coupled model *****/
struct top_out: public out_port<Message_t>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Message_t : public iestream_input<Message_t,T> {
    public:
        InputReader_Message_t () = default;
        InputReader_Message_t (const char* file_path) : iestream_input<Message_t,T>(file_path) {}
};

template<typename T>
class InputReader_Int : public iestream_input<int,T> {
    public:
        InputReader_Int () = default;
        InputReader_Int (const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(){

    /****** Input Reader atomic model instantiation *******************/
    const char * i_input_data = "../input_data/fileserversender_requestin_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader_ReqIn;
    input_reader_ReqIn = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char*>("input_reader_ReqIn", move(i_input_data));

    const char * i_input_dataIn = "../input_data/fileserversender_dataIn_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader_dataIn;
    input_reader_dataIn = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_dataIn", move(i_input_dataIn));

    const char * i_input_ackIn = "../input_data/fileserversender_ackIn_test.txt";
    shared_ptr<dynamic::modeling::model> input_reader_ackIn;
    input_reader_ackIn = dynamic::translate::make_dynamic_atomic_model<InputReader_Message_t, TIME, const char*>("input_reader_ackIn", move(i_input_ackIn));


    /****** FileServerSender atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> FileServerSender1;
    FileServerSender1 = dynamic::translate::make_dynamic_atomic_model<FileServerSender, TIME>("FileServerSender1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(top_out)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_ReqIn, input_reader_dataIn, input_reader_ackIn, FileServerSender1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<FileServerSender_defs::ackOutS,top_out>("FileServerSender1"),
	dynamic::translate::make_EOC<FileServerSender_defs::dataOutS,top_out>("FileServerSender1")
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out,FileServerSender_defs::requestIn>("input_reader_ReqIn","FileServerSender1"),
	dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,FileServerSender_defs::FileIn>("input_reader_dataIn","FileServerSender1"),
	dynamic::translate::make_IC<iestream_input_defs<Message_t>::out,FileServerSender_defs::ackIn>("input_reader_ackIn","FileServerSender1")

    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/FileServerSender_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/FileServerSender_test_output_state.txt");
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
