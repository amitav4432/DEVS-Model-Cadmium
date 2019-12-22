CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
message.o: data_structures/message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/message.cpp -o build/message.o

VendingFSM.o: application/VendingFSM.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) application/VendingFSM.cpp -o build/VendingFSM.o

OnlinePayment.o: application/OnlinePayment.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) application/OnlinePayment.cpp -o build/OnlinePayment.o

AutoTempControl.o: application/OnlinePayment.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) application/AutoTempControl.cpp -o build/AutoTempControl.o

OnlineLibrary.o: application/OnlineLibrary.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) application/OnlineLibrary.cpp -o build/OnlineLibrary.o

Elevator.o: application/Elevator.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) application/Elevator.cpp -o build/Elevator.o

Toaster.o: application/Toaster.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) application/Toaster.cpp -o build/Toaster.o
		
Moore_FSM_tests.o: test/Moore_FSM_test.cpp 
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/Moore_FSM_test.cpp -o build/Moore_FSM_test.o

Mealy_FSM_tests.o: test/Mealy_FSM_test.cpp 
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/Mealy_FSM_test.cpp -o build/Mealy_FSM_test.o

tests: Moore_FSM_tests.o Mealy_FSM_tests.o
	$(CC) -g -o bin/MOOREFSM_TEST build/Moore_FSM_test.o build/message.o
	$(CC) -g -o bin/MEALYFSM_TEST build/Mealy_FSM_test.o build/message.o		

#TARGET TO COMPILE FSM APPLICATIONS
simulator: VendingFSM.o OnlinePayment.o AutoTempControl.o OnlineLibrary.o Elevator.o Toaster.o message.o 
	$(CC) -g -o bin/VENDINGFSM build/VendingFSM.o build/message.o
	$(CC) -g -o bin/ONLINEPAYMENT build/OnlinePayment.o build/message.o 
	$(CC) -g -o bin/AUTOTEMPCONTROL build/AutoTempControl.o build/message.o
	$(CC) -g -o bin/ONLINELIBRARY build/OnlineLibrary.o build/message.o
	$(CC) -g -o bin/ELEVATOR build/Elevator.o build/message.o
	$(CC) -g -o bin/TOASTER build/Toaster.o build/message.o
	
#TARGET TO COMPILE EVERYTHING (FSM + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*
