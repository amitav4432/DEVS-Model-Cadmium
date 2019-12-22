echo "Running all the Applications and Tests...."

cd ../bin
./MEALYFSM_TEST ../input_data/MealyFSM_EventIn.txt ../input_data/MealyFSM_TransitionIn.txt 
./MOOREFSM_TEST ../input_data/MooreFSM_EventIn.txt ../input_data/MooreFSM_TransitionIn.txt 
./AUTOTEMPCONTROL ../input_data/inputAutoTempControl.txt 
./VENDINGFSM ../input_data/inputVendingMC.txt 
./TOASTER ../input_data/inputToaster.txt 
./ELEVATOR ../input_data/inputElevator.txt 
./ONLINEPAYMENT ../input_data/inputOnlinePayment.txt 
./ONLINELIBRARY ../input_data/inputOnlineLibrary.txt 


echo "Done. Please check the logs in the simulation_results folder in the Project Directory."
