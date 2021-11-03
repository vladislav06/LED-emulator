#pragma comment( lib, "wsock32.lib" )
// lad_emul.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "UDP.h"
#include <string>

using namespace std;
Address source;
Address destinaton;
Socket sck;

uint8_t protocolVerison = 170;

uint8_t ackOK[]{ protocolVerison, 254,1 };
uint8_t id[]{ protocolVerison, 246,255,255,255,255,128,128 };
uint8_t deviceName[]{ protocolVerison, 245,116,101,115,116};//test
uint8_t ledLenght[]{ protocolVerison, 247,60 };
/*
uint8_t deviceHierarchy[]{ protocolVerison, 244, //init
					 51,  52,  53,  //group name
					 14,  //group name stop byte
					 255,  144,  144,  144,  144,  0, //device id,s
					 255,  144,  144,  144,  144,  1,
					 255,  144,  144,  144,  144,  2,
					 255,  144,  144,  144,  144,  3,
					 255,  144,  144,  144,  144,  4,
					 170,  170,//stop bytes for device id's
					 54,  55,  56,  //group name
					 14,  //group name stop byte
					 255,  144,  144,  144,  145,  0, //device id,s
					 255,  144,  144,  144,  145,  1,
					 170,  170, };//stop bytes for device id's
					 */
uint8_t deviceHierarchy[]{ protocolVerison, 244 }; //init
uint8_t datACK[]{ protocolVerison, 250,1 };
uint8_t latestEF1[]{ protocolVerison, 1,255,128,128,140 };//170.1.r.g.b.w
uint8_t latestEF2[]{ protocolVerison, 2,40,255 };//170.1.r.g.b.w
uint8_t latestEF3[]{ protocolVerison, 3,255,128,128,0,34,150,10,255 };//170.1.r.g.b.w.r.g.b.w
uint8_t latestGroup[]{ protocolVerison, 3,244,128,128,0,34,150,10,255 };//170.1.r.g.b.w.r.g.b.w

int (*parsers[256])(uint8_t data[], int length);


void print(uint8_t data[], int length) {
	length += 2;
	for (int i = 2; i < length; i++) {
		cout << (int)data[i] << "|";
	}
	cout << endl;
}
string printIp(Address adress) {
	string ip = "";
	ip += to_string(source.GetA());
	ip += ".";
	ip += to_string(source.GetB());
	ip += ".";
	ip += to_string(source.GetC());
	ip += ".";
	ip += to_string(source.GetD());
	return ip;
}



void addParser(uint8_t id, int (*parser)(uint8_t data[], int length)) {
	parsers[id] = parser;
}


int ack(uint8_t data[], int lenght) {
	if (lenght != 3)
		return 1;
	if ((data[2] == 1)) {
		sck.Send(source, ackOK, sizeof(ackOK));
		cout << "ack  " << printIp(source) << endl;
	}
	return 0;
}

int idReq(uint8_t data[], int lenght) {
	if (lenght != 2)
		return 1;

	sck.Send(source, id, sizeof(id));
	cout << "idReq  " << printIp(source) << endl;

	return 0;
}

int name(uint8_t data[], int lenght) {
	if (lenght != 2)
		return 1;

	sck.Send(source, deviceName, sizeof(deviceName));
	cout << "name  " << printIp(source) << endl;

	return 0;
}

int ledCount(uint8_t data[], int lenght) {
	if (lenght != 2)
		return 1;

	sck.Send(source, ledLenght, sizeof(ledLenght));
	cout << "ledCount  " << printIp(source) << endl;

	return 0;
}

int eff1(uint8_t data[], int lenght) {
	//recive data
	if (lenght == 6) {
		sck.Send(source, datACK, sizeof(datACK));
		cout << "data " << printIp(source) << " = ";
		print(data, 4);
		//latestEF1 state
		memcpy(latestEF1, data, sizeof(latestEF1));
	}
	//send latest state
	if (lenght == 2) {	//170.1
		sck.Send(source, latestEF1, sizeof(latestEF1));
		cout << "req  " << printIp(source) << " = effect 1\n";
	}
	return 0;
}

int eff2(uint8_t data[], int lenght) {
	//recive data
	if (lenght == 4) {
		sck.Send(source, datACK, sizeof(datACK));
		cout << "data " << printIp(source) << " = ";
		print(data, 2);
		//remember state
		memcpy(latestEF2, data, sizeof(latestEF2));
	}
	//send latest state
	if (lenght == 2) {	//170.2
		sck.Send(source, latestEF2, sizeof(latestEF2));
		cout << "req  " << printIp(source) << " = effect 2\n";
	}

	return 0;
}
int eff3(uint8_t data[], int lenght) {
	//receive data
	if (lenght == 10) {
		sck.Send(source, datACK, sizeof(datACK));
		cout << "data " << printIp(source) << " = ";
		print(data, lenght);
		memcpy(latestEF3, data, sizeof(latestEF3));
	}
	if (lenght == 2)
	{
		sck.Send(source, latestEF3, sizeof(latestEF3));
		cout << "req  " << printIp(source) << " = effect 3\n";
	}

	return 0;
}
int deviceGroup(uint8_t data[], int lenght) {
	if (lenght == 2)
	{
		sck.Send(source, deviceHierarchy, sizeof(deviceHierarchy));
		cout << "req  " << printIp(source) << " = group list\n";
		return 0;
	}

	//memcpy(deviceHierarchy, data, lenght);
	print(data, lenght);
	return 0;
}
int connectAllDevices(uint8_t data[], int lenght) {
	if (lenght == 2)
	{
		sck.Send(source, datACK, sizeof(datACK));
		cout << "req  " << printIp(source) << " = connect all devices\n";
		return 0;
	}
}


int def(uint8_t data[], int lenght) {
	if (lenght == 0) {
		return 0;
	}
	return 1;
}

int main()
{
	uint8_t bufer[20];
	int length = 0;


	memset(bufer, 0, 20);//init buffer
	//init parsers
	for (int i = 0; i < 255; i++) {
		parsers[i] = &def;
	}
	//add parsers
	addParser(254, &ack);
	addParser(244, &deviceGroup);
	addParser(247, &ledCount);
	addParser(246, &idReq);
	addParser(245, &name);
	addParser(241, &connectAllDevices);
	addParser(1, &eff1);
	addParser(2, &eff2);
	addParser(3, &eff3);

	sck.Open(1111);
	sck.NonBlock();

	while (true) {
		memset(bufer, 0, 50);//null buffer
		length = sck.Receive(source, bufer, 20);
		if (bufer[0] != protocolVerison)
			continue;
		//call apropriate parser and check for return code
		//cout << (int)bufer[1] << endl;
		if (parsers[bufer[1]](bufer, length) != 0) {
			cout << "error ocured while parsing a packet!:\n";

		}
	}
}
