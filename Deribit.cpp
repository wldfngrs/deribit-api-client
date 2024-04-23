// Deribit.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// deribit.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN

#include <iostream>

#include "DeribitClient.h"

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "usage: ./Deribit [CONFIG_FILE]" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];

	try {
		FIX::SessionSettings settings(configFile);
		
		DeribitClient deribitClient;
		FIX::FileStoreFactory storeFactory(settings);
		FIX::FileLogFactory logFactory(settings);
		FIX::SocketInitiator initiator(deribitClient, storeFactory, settings, logFactory);

		initiator.start();
		deribitClient.run();
		initiator.stop();

		return 0;
	}
	catch (FIX::ConfigError& e) {
		std::cout << e.what();
		return 1;
	}
}
