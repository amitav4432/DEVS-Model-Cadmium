# DEVS-Model-Cadmium
This folder contains the application model for Finite State Machine implemented in Cadmium

##FILES ORGANIZATION
	
FSM_Implementation_Cadmium.pdf
AutoTempControl.xml
Elevator.xml
Toaster.xml
OnlinePayment.xml
OnlineLibrary.xml
VendingFSM.xml
DEVSModelForm

makefile

atomics [This folder contains atomic models implemented in Cadmium]
	MooreFSM.hpp MealyFSM

bin [This folder will be created automatically the first time you compile the poject.
     It will contain all the executables]

build [This folder will be created automatically the first time you compile the poject.
       It will contain all the build files (.o) generated during compilation]

scripts [This folder contains the script to run all the tests and simulations]
	script.sh

input_data [This folder contains all the input data to run the model and the tests]
	MealyFSM_EventIn.txt --------------------------> Contains Events to test the Mealy Atomic Model
	MealyFSM_TransitionIn.txt ---------------------> Contains Incoming Transitions to test transitions for Mealy Atomic Model
	MooreFSM_EventIn.txt --------------------------> Contains Events to test the Moore Atomic Model 
	MooreFSM_TransitionIn.txt ---------------------> Contains Incoming Transitions to test transitions for Moore Atomic Model
	inputElevator.txt -----------------------------> input values to simulate state transitions for Elevator
	inputOnlineLibrary.txt ------------------------> input values to simulate state transitions for Online Library System
	inputOnlinePayment.txt ------------------------> input values to simulate state transitions for Online Payment System
	inputAutoTempControl.txt-----------------------> input values to simulate state transitions for Automatic Temperature Controller
	inputToaster.txt-------------------------------> input values to simulate state transitions for Toaster Machine
	inputVendingMC.txt-----------------------------> input values to simulate state transitions for Vending Machine 
	
simulation_results [It will store the outputs from simulations and tests. Output logs of all the test scenarios are present here]


	ElevatorMsg.txt -------------------------------> Messages Output of Elevator Application 
	ElevatorStates.txt ----------------------------> State Output of Elevator Application 
	ToasterMsg.txt --------------------------------> Messages Output of Toaster Application
	ToasterStates.txt -----------------------------> State Output of Toaster Application
	OnlineLibraryMsg.txt --------------------------> Messages Output of Online Library Application
	OnlineLibraryStates.txt -----------------------> State Output of Online Library Application
	AutomaticTempControllerMsg.txt ----------------> Messages Output of Automatic Temperature Controller Application
	AutomaticTempControllerState.txt --------------> State Output of Automatic Temperature Controller Application
	OnlinePaymentMsg.txt --------------------------> Messages Output of Online Payment Application
	OnlinePaymentStates.txt -----------------------> State Output of Online Payment Application
	VendingMooreFSMMessages.txt -------------------> Messages Output of Cold Drink Vending Machine Application
	VendingMooreFSMStates.txt ---------------------> State Output of Cold Drink Vending Machine Controller Application
	MealyFSM_test_output_messages.txt -------------> Messages Output of Mealy Atomic Model
	MealyFSM_test_output_state.txt ----------------> State Output of Mealy Atomic Model
	MooreFSM_test_output_messages.txt -------------> Messages Output of Moore Atomic Model
	MooreFSM_test_output_state.txt ----------------> State Output of Moore Atomic Model

test [This folder the unit test of the atomic models]
	Moore_FSM_test.cpp
	Mealy_FSM_test.cpp

application [This folder contains the Moore/Mealy applications and Header files used by each application]	
	VendingFSM.cpp      (Moore) -->Contains the Coupled Model Construction
	VendingFSM.h		    -->Contains the implementations of event and transition callbacks
	AutoTempControl.cpp (Moore) -->Contains the Coupled Model Construction
	AutoTempControl.h 	    -->Contains the implementations of event and transition callbacks
	OnlinePayment.cpp   (Moore) -->Contains the Coupled Model Construction
	OnlinePayment.h		    -->Contains the implementations of event and transition callbacks
	Elevator.cpp        (Mealy) -->Contains the Coupled Model Construction
	Elevator.h 	            -->Contains the implementations of event and transition callbacks
	OnlineLibrary.cpp   (Mealy) -->Contains the Coupled Model Construction
	OnlineLibrary.h 	    -->Contains the implementations of event and transition callbacks
	Toaster.cpp         (Mealy) -->Contains the Coupled Model Construction
	Toaster.h
	
##STEPS FOR COMPILATION AND EXECUTION OF THE PROGRAMS

0 - OfflineFileTransferSimulation.doc contains the explanation of this model

1 - Update include path in the makefile in this folder and subfolders. following lines need to be updated:
	INCLUDECADMIUM=-I ../../cadmium/include
	INCLUDEDESTIMES=-I ../../DESTimes/include
    	Update the relative path to cadmium/include from the folder where the makefile is. You need to take into account where you copied the folder during the installation process
	Example: INCLUDECADMIUM=-I ../../cadmium/include
	Do the same for the DESTimes library
   
2 - Compile the project and the tests
	2.1 - Open the terminal (Ubuntu terminal for Linux) in the CADMIUM_FSM_MODEL_APPLICATION_SIMULATION folder
	2.2 - To compile the project, run the Simulation and the tests, type in the terminal:
	      2.1 - make clean
	      2.2 - make all
	      2.3 - cd scripts
	      2.4 - chmod 777 script.sh
	      2.5 - ./script.sh
		
3 - To check the output of the tests and simulations, go to the folder simulation_results and open the output files for all the test cases run. For example, for Elevator Application, 		"ElevatorMsg.txt" and "ElevatorStates.txt" would be generated. These files need to be renamed if results are required to persist because they will be overwritten on every subsequent test run.

