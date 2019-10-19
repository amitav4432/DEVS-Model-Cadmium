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

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) top_model/main.cpp -o build/main_top.o
	
main_subnet_test.o: test/main_subnet_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_subnet_test.cpp -o build/main_subnet_test.o

main_sender_test.o: test/main_sender_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_sender_test.cpp -o build/main_sender_test.o

main_receiver_test.o: test/main_receiver_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_receiver_test.cpp -o build/main_receiver_test.o
main_fileserversender_test.o: test/main_fileserversender_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_fileserversender_test.cpp -o build/main_fileserversender_test.o
main_fileserverreceiver_test.o: test/main_fileserverreceiver_test.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_fileserverreceiver_test.cpp -o build/main_fileserverreceiver_test.o


tests: main_subnet_test.o main_sender_test.o main_receiver_test.o message.o main_fileserversender_test.o main_fileserverreceiver_test.o
		$(CC) -g -o bin/SUBNET_TEST build/main_subnet_test.o build/message.o
		$(CC) -g -o bin/SENDER_TEST build/main_sender_test.o build/message.o 
		$(CC) -g -o bin/RECEIVER_TEST build/main_receiver_test.o build/message.o 
		$(CC) -g -o bin/FILESERVERSENDER_TEST build/main_fileserversender_test.o build/message.o  
		$(CC) -g -o bin/FILESERVERRECEIVER_TEST build/main_fileserverreceiver_test.o build/message.o		

#TARGET TO COMPILE ONLY OFFLINE_FILE_TRANSFER SIMULATOR
simulator: main_top.o message.o 
	$(CC) -g -o bin/OFFLINE_FILE_TRANSFER build/main_top.o build/message.o 
	
#TARGET TO COMPILE EVERYTHING (OFFLINE_FILE_TRANSFER SIMULATOR + TESTS TOGETHER)
all: simulator tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*
