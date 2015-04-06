#include "Client.h"
Client::Client(std::string ser_name, int port_no)
{
    serName = "localhost";
    portNo = port_no;
    newClient();
    //testStuff();
    //testFileStuff();
    handleInput();
}

void Client::newClient(){
    do{
        if(client!=NULL){
            delete client;
            client = NULL;
        }
        client = new ClientSock(serName, portNo);
        if(!client->isConnected()){
            std::cout << "reconnecting..." << std::endl;
            usleep(1000000);
        }
    }while(!client->isConnected());
}

void Client::handleInput(){
    bool ending = false;
    char buffer[SIZE];
    std::string serIn;
    while(!ending){
        serIn = client->readIn(BLOCK);
        if(serIn == "T"){               //test connection
            client->writeOut("t");
        }else if(serIn == "R"){         //reset conncetion (unused)
            client->writeOut("r");
            newClient();
        }else if(serIn == "F"){         //ready new file
            client->writeOut("f");
            unsigned int fSize = 0;
            readInfo((char*)&fSize, sizeof(unsigned int));   //change to unsigned long long later
            memset(buffer, 0, SIZE);
            readInfo(buffer, BLOCK);    //add handling later
            newFile(buffer);
            std::cout << "name: " << fileOut->getName() << std::endl << "size: " << fSize << std::endl;
            recvFile(fSize);            //attempt retrieval
        }else if(serIn == "E"){
            ending = true;
            std::cout << "server issued end" <<std::endl; //unused
        }else{                          //invalid command
            std::cout << "unrecognized server command: " << serIn << std::endl;
            client->writeOut("0");
        }
    }
}

int Client::readInfo(char* info, int len){
    char buffer[SIZE] = {0};
    if(client->readIn(buffer, len)<0){      //read info
        return -1;
    }
    if(client->writeOut(buffer, len)<0){    //write the info back
        return -1;
    }
    copystr(info, buffer, len);
    if(client->readIn(buffer, 7)<0){         //check success/fail
        return -1;
    }
    return atoi(buffer);
}

void Client::recvFile(unsigned int fLeft){
    char buffer[SIZE];
    int temp, c;
    std::cout << "size: " << fLeft << std::endl;
    for(c=0;fLeft >= SIZE;fLeft -= BLOCK){
        //do{
            temp = readInfo(buffer, BLOCK);
        //}while(temp < 0);
        //std::cout << "received block #" << temp << std::endl; //
        //std::cout << buffer << std::endl;
        //if(c!=temp){
            //std::cout << "WARNING: SERVER SENT " << temp << " BUT CLIENT IS WRITING " << c <<std::endl;
            //c = temp;
        //}
            fileOut->fileWrite(buffer, BLOCK);
        c++;
    }
    temp = readInfo(buffer, SIZE);
    fileOut->fileWrite(buffer, (int)fLeft);
}

void Client::copystr(char* newptr, const char* oldptr, int len){
    while((len--)>0){
        *(newptr++)=*(oldptr++);
    }
}

void Client::testStuff(){
    bool ending = false;
    std::string serIn;
    while(!ending){
        serIn = client->readIn(BLOCK);
        if(serIn == "R"){
            client->writeOut("RC");
            newClient();
            std::cout << "Client reset" << std::endl;
        }else if(serIn == "End"){
            client->writeOut("EndC");
            ending = true;
        }else{
            std::cout << serIn << std::endl;
            client->writeOut("read");
        }
    }
}

void Client::newFile(const char* file_name){
    if(fileOut!=NULL){
        delete fileOut;
        fileOut = NULL;
    }
    fileOut = new ClientFile(file_name);
}

void Client::testFileStuff(){
    char buffer[BLOCK+1];
    unsigned int fSize;
    std::cout << "preparing to read... " << std::endl;//
    client->readIn(buffer, BLOCK);
    std::string fName(buffer);
    std::cout << "read file name: " << fName.substr(0,10) << std::endl;
    memset(buffer, 0, BLOCK+1);
    client->readIn(buffer, BLOCK);
    fSize = (unsigned int)atoi(buffer);
    std::cout << "read file size: " << buffer << std::endl;
    fileOut = new ClientFile(fName);
    for(int c=(int)(fSize/BLOCK);c>0;c--){
        client->readIn(buffer, BLOCK);
        fileOut->fileWrite(buffer, BLOCK);
        //std::cout << "read " << buffer << std::endl;
    }
    //std::cout << (int)(fSize%BLOCK) << " read " << buffer << std::endl;
    memset(buffer, 0, BLOCK+1);
    client->readIn(buffer, (int)(fSize%BLOCK));
    fileOut->fileWrite(buffer, 1024);
    std::cout << buffer << std::endl << (int)(fSize%BLOCK) << std::endl;//
}

Client::~Client()
{
    delete client;
}
