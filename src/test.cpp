// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

int main () {

  // std::ifstream is ("test.txt", std::ifstream::binary);
  std::ifstream is ("test.txt");
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    char * buffer = new char [length];

    std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    is.read (buffer,length);

    // char * outbuffer = new char [length*2+1];   
    // char* pHexTable="0123456789ABCDEF";
    // int iPos=0;

    // for(int i=0; i<length; i++){
    //     //assume buffer contains some binary data at this point
    //     char cHex=buffer[i];
    //     outbuffer[iPos++]=pHexTable[(cHex>>4)&0x0f];
    //     outbuffer[iPos++]=pHexTable[cHex&0x0f];
    // }
    // outbuffer[iPos]='\0';
    std::cout << buffer << std::endl;
    if (is)
      std::cout << "all characters read successfully.";
    else
      std::cout << "error: only " << is.gcount() << " could be read";
    is.close();

    // ...buffer contains the entire file...

    delete[] buffer;
    // delete[] outbuffer;
  }
  return 0;
}
