
/*********************************************************
* Amitav Shaw/Arshpreet Singh
* Carleton University
*
* OFFLINE FILE TRANSFER:
* Couple models and the links
**********************************************************/


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
#include "../atomics/subnet.hpp"
#include "../atomics/sender.hpp"
#include "../atomics/receiver.hpp"
#include "../atomics/FileServerSender.hpp"
#include "../atomics/FileServerReceiver.hpp"

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
struct controlIn : public in_port<int>{}; // input reader --> Sender
struct requestIn : public in_port<int>{}; // OFT --> File Exchange Server
struct NW_requestIn : public in_port<int>{};// OFT --> Network Coupled model
struct requestInTop : public in_port<int>{}; // input reader -->Top Coupled model
struct FSSenderAckIn : public in_port<Message_t>{}; // FS_Exchange --> Network A
struct FSReceiverDataIn : public in_port<Message_t>{}; // Network A --> FS_Exchange
struct inSubnetA1 : public in_port<Message_t>{}; // Sender --> SubnetA1
struct inSubnetA2 : public in_port<Message_t>{}; // FS_Exchange --> SubnetA2
struct inSubnetB1 : public in_port<Message_t>{}; // Receiver --> SubnetB1
struct inSubnetB2 : public in_port<Message_t>{}; // FS_Exchange --> SubnetB2
struct NW_inSubnetA1 : public in_port<Message_t>{};// Network -->Network A
struct NW_inSubnetB1 : public in_port<Message_t>{};// Network -->Network B

/***** Define output ports for coupled model *****/
struct FSReceiverAckOut : public out_port<Message_t>{}; //To Network A
struct FSSenderDataOut : public out_port<Message_t>{};  //To Network B
struct outSubnetA1 : public out_port<Message_t>{}; // Network A --> FS_Exchange
struct outSubnetA2 : public out_port<Message_t>{}; // Network A --> Sender
struct outSubnetB1 : public out_port<Message_t>{}; // Network B --> FS_Exchange
struct outSubnetB2 : public out_port<Message_t>{}; // Network B --> Receiver
struct NW_outSubnetA2 : public out_port<Message_t>{}; // Network --> Sender
struct NW_outSubnetB2 : public out_port<Message_t>{}; // Network --> Receiver
struct ackReceived_OFT: out_port<int>{}; // To Sender
struct packetSentOut: out_port<int>{}; // To Sender

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 3) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the control_input and request_input txt files " << endl;
        return 1; 
    }
   	 /****** Input Reader atomic model instantiation *******************/
   	 string input = argv[1];
    	const char * i_input = input.c_str();
    	shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));
    	string input1 = argv[2];
    	const char * i_input1 = input1.c_str();
    	shared_ptr<dynamic::modeling::model> input_reader1 = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader1" , move(i_input1));
	/****** Sender atomic model instantiation *******************/
    	shared_ptr<dynamic::modeling::model> sender1 = dynamic::translate::make_dynamic_atomic_model<Sender, TIME>("sender1");
    	/****** Receiver atomic model instantiation *******************/
    	shared_ptr<dynamic::modeling::model> receiver1 = dynamic::translate::make_dynamic_atomic_model<Receiver, TIME>("receiver1");
    	/****** Subnet atomic models instantiation *******************/
	shared_ptr<dynamic::modeling::model> subnetA1 = dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnetA1");
    	shared_ptr<dynamic::modeling::model> subnetA2 = dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnetA2");
	shared_ptr<dynamic::modeling::model> subnetB1 = dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnetB1");
    	shared_ptr<dynamic::modeling::model> subnetB2 = dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnetB2");
	/****** File Server Exchange atomic models instantiation *******************/
	shared_ptr<dynamic::modeling::model> FileServerReceiver1 = dynamic::translate::make_dynamic_atomic_model<FileServerReceiver, TIME>("FileServerReceiver1");
	shared_ptr<dynamic::modeling::model> FileServerSender1 = dynamic::translate::make_dynamic_atomic_model<FileServerSender, TIME>("FileServerSender1");

	
	/*******FILE_EXCHANGE_SERVER COUPLED MODEL********/
	dynamic::modeling::Ports iports_FS_Exchange = {typeid(requestIn),typeid(FSReceiverDataIn),typeid(FSSenderAckIn)};
  	dynamic::modeling::Ports oports_FS_Exchange = {typeid(FSReceiverAckOut),typeid(FSSenderDataOut)};
	dynamic::modeling::Models submodels_FS_Exchange = {FileServerReceiver1, FileServerSender1};

    	dynamic::modeling::EICs eics_FS_Exchange = {
        dynamic::translate::make_EIC<requestIn, FileServerSender_defs::requestIn>("FileServerSender1"),
        dynamic::translate::make_EIC<FSReceiverDataIn, FileServerReceiver_defs::dataIn>("FileServerReceiver1"),
	dynamic::translate::make_EIC<FSSenderAckIn, FileServerSender_defs::ackIn>("FileServerSender1")
   	 };

    	dynamic::modeling::EOCs eocs_FS_Exchange = {
        dynamic::translate::make_EOC<FileServerReceiver_defs::ackOutR,FSReceiverAckOut>("FileServerReceiver1"),
        dynamic::translate::make_EOC<FileServerSender_defs::dataOutS,FSSenderDataOut>("FileServerSender1")
    	};

   	dynamic::modeling::ICs ics_FS_Exchange = {
	dynamic::translate::make_IC<FileServerReceiver_defs::storeDataS, FileServerSender_defs::FileIn>("FileServerReceiver1","FileServerSender1"),   	
	dynamic::translate::make_IC<FileServerSender_defs::ackOutS, FileServerReceiver_defs::ackInR>("FileServerSender1","FileServerReceiver1")	
	};

   	shared_ptr<dynamic::modeling::coupled<TIME>> FS_Exchange;
    	FS_Exchange = make_shared<dynamic::modeling::coupled<TIME>>(
        "FS_Exchange", submodels_FS_Exchange, iports_FS_Exchange, oports_FS_Exchange, eics_FS_Exchange, eocs_FS_Exchange, ics_FS_Exchange 
    	);

	/*******ACCESS NETWORK_A COUPLED MODEL********/
	dynamic::modeling::Ports iports_Network_A = {typeid(inSubnetA1),typeid(inSubnetA2)};
    	dynamic::modeling::Ports oports_Network_A = {typeid(outSubnetA1),typeid(outSubnetA2)};
    	dynamic::modeling::Models submodels_Network_A = {subnetA1, subnetA2};
    	dynamic::modeling::EICs eics_Network_A = {
        dynamic::translate::make_EIC<inSubnetA1, Subnet_defs::in>("subnetA1"),
        dynamic::translate::make_EIC<inSubnetA2, Subnet_defs::in>("subnetA2")
    	};
    	dynamic::modeling::EOCs eocs_Network_A = {
        dynamic::translate::make_EOC<Subnet_defs::out,outSubnetA1>("subnetA1"),
        dynamic::translate::make_EOC<Subnet_defs::out,outSubnetA2>("subnetA2")
    	};
    	dynamic::modeling::ICs ics_Network_A = {};
    	shared_ptr<dynamic::modeling::coupled<TIME>> Network_A;
    	Network_A = make_shared<dynamic::modeling::coupled<TIME>>(
        "Network_A", submodels_Network_A, iports_Network_A, oports_Network_A, eics_Network_A, eocs_Network_A, ics_Network_A 
    	);

	/*******ACCESS NETWORK_B COUPLED MODEL********/
	dynamic::modeling::Ports iports_Network_B = {typeid(inSubnetB1),typeid(inSubnetB2)};
    	dynamic::modeling::Ports oports_Network_B = {typeid(outSubnetB1),typeid(outSubnetB2)};
    	dynamic::modeling::Models submodels_Network_B = {subnetB1, subnetB2};
    	dynamic::modeling::EICs eics_Network_B = {
        dynamic::translate::make_EIC<inSubnetB1, Subnet_defs::in>("subnetB1"),
        dynamic::translate::make_EIC<inSubnetB2, Subnet_defs::in>("subnetB2")
    	};
    	dynamic::modeling::EOCs eocs_Network_B = {
        dynamic::translate::make_EOC<Subnet_defs::out,outSubnetB1>("subnetB1"),
        dynamic::translate::make_EOC<Subnet_defs::out,outSubnetB2>("subnetB2")
    	};
    	dynamic::modeling::ICs ics_Network_B = {};
    	shared_ptr<dynamic::modeling::coupled<TIME>> Network_B;
    	Network_B = make_shared<dynamic::modeling::coupled<TIME>>(
        "Network_B", submodels_Network_B, iports_Network_B, oports_Network_B, eics_Network_B, eocs_Network_B, ics_Network_B 
   	);

    	/*******NETWORKS COUPLED MODEL********/
    	dynamic::modeling::Ports iports_Network = {typeid(NW_inSubnetA1),typeid(NW_inSubnetB1),typeid(NW_requestIn)};
    	dynamic::modeling::Ports oports_Network = {typeid(NW_outSubnetA2),typeid(NW_outSubnetB2)};
    	dynamic::modeling::Models submodels_Network = {FS_Exchange, Network_A, Network_B};
    	dynamic::modeling::EICs eics_Network = {
	dynamic::translate::make_EIC<NW_inSubnetA1, inSubnetA1>("Network_A"),
        dynamic::translate::make_EIC<NW_inSubnetB1, inSubnetB1>("Network_B"),
	dynamic::translate::make_EIC<NW_requestIn, requestIn>("FS_Exchange")
    	};

    	dynamic::modeling::EOCs eocs_Network = {
        dynamic::translate::make_EOC<outSubnetA2, NW_outSubnetA2>("Network_A"),
        dynamic::translate::make_EOC<outSubnetB2, NW_outSubnetB2>("Network_B")
    	};

    	dynamic::modeling::ICs ics_Network = {
	dynamic::translate::make_IC<outSubnetA1,FSReceiverDataIn>("Network_A","FS_Exchange"),
        dynamic::translate::make_IC<outSubnetB1,FSSenderAckIn>("Network_B","FS_Exchange"),
        dynamic::translate::make_IC<FSReceiverAckOut,inSubnetA2>("FS_Exchange","Network_A"),
        dynamic::translate::make_IC<FSSenderDataOut, inSubnetB2>("FS_Exchange","Network_B")
	};
    	shared_ptr<dynamic::modeling::coupled<TIME>> Network;
    	Network = make_shared<dynamic::modeling::coupled<TIME>>(
        "Network", submodels_Network, iports_Network, oports_Network, eics_Network, eocs_Network, ics_Network 
    	);

	
    	/*******OFFLINE FILE TRANSFER SIMULATOR COUPLED MODEL********/
    	dynamic::modeling::Ports iports_OFT = {typeid(controlIn),typeid(requestInTop)};
	dynamic::modeling::Ports oports_OFT = {typeid(ackReceived_OFT), typeid(packetSentOut)};
	dynamic::modeling::Models submodels_OFT = {sender1, receiver1, Network};
	dynamic::modeling::EICs eics_OFT = {
	cadmium::dynamic::translate::make_EIC<controlIn, Sender_defs::controlIn>("sender1"),
	cadmium::dynamic::translate::make_EIC<requestInTop, NW_requestIn>("Network")
	};
	dynamic::modeling::EOCs eocs_OFT = {
        dynamic::translate::make_EOC<Sender_defs::ackReceivedOut,ackReceived_OFT>("sender1"),
	dynamic::translate::make_EOC<Sender_defs::packetSentOut,packetSentOut>("sender1")
    	};
   	dynamic::modeling::ICs ics_OFT = {
        dynamic::translate::make_IC<Sender_defs::dataOut, NW_inSubnetA1>("sender1","Network"),
        dynamic::translate::make_IC<NW_outSubnetA2, Sender_defs::ackIn>("Network","sender1"),
        dynamic::translate::make_IC<Receiver_defs::out, NW_inSubnetB1>("receiver1","Network"),
        dynamic::translate::make_IC<NW_outSubnetB2, Receiver_defs::in>("Network","receiver1")
    	};
    	shared_ptr<dynamic::modeling::coupled<TIME>> OFT;
    	OFT = make_shared<dynamic::modeling::coupled<TIME>>(
        "OFT", submodels_OFT, iports_OFT, oports_OFT, eics_OFT, eocs_OFT, ics_OFT 
    	);

    	/*******TOP COUPLED MODEL********/
    	dynamic::modeling::Ports iports_TOP = {};
    	dynamic::modeling::Ports oports_TOP = {typeid(ackReceived_OFT),typeid(packetSentOut)};
    	dynamic::modeling::Models submodels_TOP = {input_reader,input_reader1, OFT};
    	dynamic::modeling::EICs eics_TOP = {};
    	dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<ackReceived_OFT,ackReceived_OFT>("OFT"), dynamic::translate::make_EOC<packetSentOut,packetSentOut>("OFT")
    	};
    	dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out, controlIn>("input_reader","OFT"),
	dynamic::translate::make_IC<iestream_input_defs<int>::out, requestInTop>("input_reader1","OFT")
    	};
    	shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    	TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    	);

    	/*************** Loggers *******************/
    	static ofstream out_messages("../simulation_results/offlineFT_output_messages.txt");
    	struct oss_sink_messages{
        static ostream& sink(){			
        	return out_messages;
        	}
    	};
    	static ofstream out_state("../simulation_results/offlineFT_output_state.txt");
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
