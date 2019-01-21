#include<unistd.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include<algorithm>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "parse.h"
#include "senddata.h"
#include<time.h>
#include <math.h>


using namespace std;
string rootdir ="/home/archit/";

void  Parse::parseRequest(clientIdentity c_id)
{
	int i=1;
	int recvbytes=0;
	char buffer[4096];
	clientInfo c_info_obj;

	char *p,*pbuffer;
	fcntl( c_id.acceptId, O_NONBLOCK, 0 );
	
	if((recvbytes = (recv(c_id.acceptId, buffer, sizeof(buffer),0))) == -1)
		perror("Receive:");
	buffer[recvbytes] = '\0';
	string header_string(buffer);

	int current = 0;
	
	int next = header_string.find_first_of("\r\n", current );

 	if(next >= 0) {
		c_info_obj.r_firstline = header_string.substr( current, next - current);
		string first_line_data[3];
		pbuffer = new char[c_info_obj.r_firstline.size() + 1];
		copy(c_info_obj.r_firstline.begin(), c_info_obj.r_firstline.end(), pbuffer);
		pbuffer[c_info_obj.r_firstline.size()] = '\0';

		p=strtok (pbuffer," /"); 
		first_line_data[0].assign(p);
		while (i < 3) {
			  p=strtok(NULL," ");
			  first_line_data[i].assign(p);
			  i++;
		}
		i = 0 ;

		c_info_obj.r_acceptid = c_id.acceptId;
		c_info_obj.r_portno = c_id.portno;
		c_info_obj.r_ip = c_id.ip;
		c_info_obj.r_time = c_id.requesttime;
		c_info_obj.r_type = first_line_data[0];
		c_info_obj.r_filename = first_line_data[1];

		
		int ii = c_info_obj.r_filename.find_first_of("/",0);
		int jj = c_info_obj.r_filename.find_first_of("/",ii+1);
		// cout << " II jj " << ii << " "<< jj <<endl;
		if(jj >= 0)
				c_info_obj.rootcheck = false;
		else
				c_info_obj.rootcheck = true;

		//============================================
			
		c_info_obj.file_root = c_info_obj.r_filename ;
		// if(c_info_obj.file_root[c_info_obj.file_root.size()-1] != '/') c_info_obj.file_root = c_info_obj.file_root + "/" ;      
		cout << "file Root "<< c_info_obj.file_root << endl; 
		//============================================

		c_info_obj.r_filename = rootdir + c_info_obj.r_filename;
		c_info_obj.r_method = first_line_data[2];
		// cout << "pre file name "<< c_info_obj.r_filename << endl;
		
		delete [] pbuffer;  // only memor block is deleted not pointer
		pbuffer = NULL;
		cout << c_info_obj.r_filename  << " check1 "<<endl;
		checkRequest(c_info_obj) ; 
		// changeFolder(c_info_obj);
	}
	else {
		write(c_info_obj.r_acceptid,"Error: Bad Request, Retry",25);
		close (c_info_obj.r_acceptid);
	}
}


bool Parse::fileExists(char *filename)						
{
	struct stat filenamecheck;
    if (stat(filename, &filenamecheck) != -1)
    		if(filenamecheck.st_mode & S_IFREG)
       	   		return true;
    
	return false;
}

bool Parse::dirExists(char *addr)						
{
	struct stat dircheck;
    // if (stat(filename, &filenamecheck) != -1)
       	   	// return true;
    
	// return false;
	if( stat( addr, &dircheck) != 0 )
		return false;
    	// printf( "cannot access %s\n", pathname );
	else if( dircheck.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows 
    	return true ;
    	// printf( "%s is a directory\n", pathname );
	// else
 //    	printf( "%s is no directory\n", pathname );
    return false;
}


// Check type of request
void Parse::checkRequest(clientInfo c_info_obj)
{

	int pos = c_info_obj.r_filename.find_last_of("/");
	// int next = c_info_obj.r_filename.find_first_of(".",pos);
	// if(next > 0) {
		// Getting file name
		char * fname = new char[c_info_obj.r_filename.size() + 1];
		copy(c_info_obj.r_filename.begin(), c_info_obj.r_filename.end(), fname);
		fname[c_info_obj.r_filename.size()] = '\0';
		ifstream::pos_type size = 0;


		cout << fname << "  check2"<<endl;


		if((fileExists(fname) || dirExists(fname)) && c_info_obj.r_type == "GET") {

			c_info_obj.status_file = false;
			c_info_obj.r_filesize = 0;
			c_info_obj.status_dir = false;

			if(fileExists(fname))
			{
				ifstream file;
				file.open(fname);
				if (file.is_open()) {
					file.seekg (0, ios::end);
					size = file.tellg();
				}
				file.close();
			c_info_obj.status_file = true;
			c_info_obj.r_filesize = (int)size;
			}
			else 
			c_info_obj.status_dir = true;


			delete [] fname;
			fname = NULL;
				SendData S;
				S.transfer_data(c_info_obj);

		} 
		else if(fileExists(fname) && c_info_obj.r_type == "HEAD") {
			c_info_obj.status_file = true;
			c_info_obj.r_filesize = 0;
			delete [] fname;
			fname = NULL;
				SendData S;
				S.transfer_data(c_info_obj);

		} 
		else {
			delete [] fname;
			fname = NULL;
			c_info_obj.r_filesize = 0;
			c_info_obj.status_file = false;
				SendData S;
				S.transfer_data(c_info_obj);
		}
	// } else {
	// 	c_info_obj.r_filesize = 0;
	// 	c_info_obj.status_file = false;
	// 		SendData S;
	// 		S.transfer_data(c_info_obj);
	// }
}


// // Sort the request based on file size
// bool sortRequest(const clientInfo& lhs, const clientInfo& rhs)
// {
//   return lhs.r_filesize < rhs.r_filesize;
// }

