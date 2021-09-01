/* Copyright (C) 2014 Carlos Aguilar Melchor, Joris Barrier, Marc-Olivier Killijian
 * This file is part of XPIR.
 *
 *  XPIR is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  XPIR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XPIR.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PIRViewCLI.hpp"
#include "PIRController.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <cstdlib> 

#include <openssl/sha.h>
std::string sha256(const std::string str)
{
    char buf[2];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string NewString = "";
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(buf,"%02x",hash[i]);
        NewString = NewString + buf[0] + buf[1];
    }
        return NewString;
}


/**
 *	Class constructor.
 *	Param :
 *
 **/
PIRViewCLI::PIRViewCLI(PIRController* controller_) :
	PIRView(controller_)
{}

/**
 *	Message notification, display event content.
 *	Param :
 *		- MessageEvent& event : MessageEvent reference to display.
 **/
void PIRViewCLI::messageUpdate(MessageEvent& event)
{
	if(event.getMessageType() == RETRY){
		getUserInputRetry();
	}
	else if(event.getMessageType() == DEFAULT){
		std::cout << event.getMessage() << std::endl; 
	}
	else
	{
		std::string color = (event.getMessageType() == WARNING) ? ORANGE : RED;
		std::cout << BOLD  << event.getInfo() << " : " << RESET_COLOR << color << event.getMessage() << RESET_COLOR << std::endl;
	}
}

/**
 *	Catalog notification, display catalog content in a framed list.
 *	Param :
 *		- CatalogEvent& event : CatalogEvent reference to display.
 **/
void PIRViewCLI::catalogUpdate(CatalogEvent& event)
{
	using namespace std;
	cout << endl;
	cout << "##############################################" << endl;
	cout << "#                                            #" << endl;
	cout << "# Connection established                     #" << endl;
	cout << "#                                            #" << endl;
	//cout << "##############################################" << endl;
	//cout << "#                                            #" << endl;
	//cout << "# File List :                                #" << endl;
	//cout << "# °°°°°°°°°°°                                #" << endl;
	//cout << "#                                            #" << endl;

	// for (unsigned int i = 0 ; i < event.getCatalog().size() ; i++) 
	// {
	//	cout << "# " << i+1 << ") " << event.getCatalog().at(i);
	//
	//	for(unsigned int j = 0; j < 66 - event.getCatalog().at(i).length(); j++)
	//		cout << " ";
	//
	//	cout << "#" << endl;
	//}

	cout << "#                                            #" << endl;
	cout << "##############################################" << endl;
	cout << "#                                            #" << endl;
	cout << "# Which file do you want ?                   #" << endl;

	string x; 
	int choice  = -1;
	bool retry;
	MessageEvent event2(WARNING);

	do
	{
		retry = false;
		char *pathvar;
		pathvar = getenv("TARGET_FILENAME");
		x = pathvar;
		//cin >> x ;
		//cin.clear();
		//cin.get();
    string y = sha256(x);		
    cout << y << endl;
    for (unsigned int i = 0 ; i < event.getCatalog().size() ; i++) 
    {
      if (event.getCatalog().at(i) == y)
      {
        choice = i;
        break;
      }
    }
		if (choice < 0)
		{
			retry = true;
			event2.setMessage("This file doesn't exist, retry :");
			messageUpdate(event2);
		}
		break;
	}while(retry);

	controller->notifyClientChoice(choice);

	//getUserInputFile(event.getCatalog().size()) ;
}

/**
 *	Writing notification, used when the client wants to display the writing state of the files.
 *	Param :
 *		- WriteEvent& event : WriteEvent reference.
 **/
void PIRViewCLI::writeUpdate(WriteEvent& event)
{
	using namespace std;

	std::cout << "\033[1GPIRReplyWriter: Remaning Bytes to write : " << event.getSizeToWrite() -  event.getWrittenSize() << "                                  \033[5G"  << "\xd"<< std::flush;

	if(event.getSizeToWrite() == event.getWrittenSize())
	{
		cout << endl << endl <<"\t## SUCESS ! ##" << endl;
		cout << "\t   °°°°°°" << endl;
	}
}

/**
 *	Get user input for retry questions :
 *	Param :
 *		- const std::string& message : message to display.
 **/
void PIRViewCLI::getUserInputRetry()
{
	using namespace std;	
	char choice;
	cout << "Enable de reach the server, would you like to retry ? (Y/n) : ";

retrying:
	cin.clear();
	choice = cin.get();

	if (choice == 'N'||choice == 'n') {
		controller->notifyClientChoice(false);
	}
	else if (choice == 'Y' || choice == 'y'|| choice == '\n') {
		controller->notifyClientChoice(true);
	}	
	else
	{
		cout << "Bad input, retry : ";
		cin.ignore(1);
		goto retrying;
	}
}

/**
 *	Get user choice, used with updateCatalog method.
 *	Param :
 *		- int maxVulue : maximum value that the user can enter.
 **/
void PIRViewCLI::getUserInputFile(int maxValue)
{
	using namespace std;
	int choice  = -1;
	bool retry;
	MessageEvent event(WARNING);
	
	do
	{
		retry = false;
		cin >> choice ;
		cin.clear();
		cin.get();
		
		if (choice > maxValue || choice <= 0)
		{
			retry = true;
			event.setMessage("This file doesn't exist, retry :");
			messageUpdate(event);
		}
	}while(retry);

	controller->notifyClientChoice(--choice);
}


