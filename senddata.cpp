#include<unistd.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <time.h>
#include <pthread.h>
#include <sstream>
#include <dirent.h>
#include <vector>
#include<algorithm>
#include "parse.h"
#include "senddata.h"
#include<time.h>
#include<math.h>
#include<dirent.h>
using namespace std;


bool logging = true  ; 
string l_file ="logfile.txt";
bool r_daemon = false ;

void sendHeadData(clientInfo c)
{
		char  currtime[50];
		char lastmodify[50];
		struct stat buf;
		//bool logging = true;
		time_t tim=time(NULL);
		tm *now=gmtime(&tim);
		
		if (strftime(currtime, 50,"%c", now) == 0)
				perror("Date Error");

		char * fname = new char[c.r_filename.size() + 1];
		std::copy(c.r_filename.begin(), c.r_filename.end(), fname);
		fname[c.r_filename.size()] = '\0';
		stat(fname,&buf);
		strcpy(lastmodify,ctime(&buf.st_mtime));
		delete[] fname;
		fname = NULL;
		int pos = c.r_filename.find_last_of(".");
		string c_type = c.r_filename.substr(pos+1,c.r_filename.size());
		if (c_type == "txt" || c_type == "html" || c_type == "htm")
			c.r_ctype ="text/html";
		else if(c_type == "gif" || c_type == "jpeg")
			c.r_ctype = "image/"+c_type;
		else
			c.r_ctype = " ";

		std::stringstream ss;
		ss << c.r_filesize;
		string filesize = ss.str();
		string header = c.r_type + " "+ c.r_method +" 200 OK\r\nDate: ";
		string time(currtime);
		header = header + time;
		header = header +"\r\n" +"Server: myhttpd 1.0\r\n" + "Last-Modified:";
		string lmodify(lastmodify);
		header = header + lmodify ;//+"\r\n";
			// cout<<"thread_id of current thread is: ";
			// cout<<pthread_self()<<endl;
		header = header + "Content-Type:" + c.r_ctype +"\r\n" + "Content-Length:" + filesize +"\r\n\r\n";
		if (send(c.r_acceptid, header.c_str(), strlen(header.c_str()), 0) == -1)
			perror("send");

		c.status_code = 200;
}
void sendGetData(clientInfo c)
{
			char  currtime[50];
		char lastmodify[50];
		struct stat buf;
		time_t tim=time(NULL);
		tm *now=gmtime(&tim);

		if (strftime(currtime, 50,"%c", now) == 0)
				perror("Date Error");
		char * fname = new char[c.r_filename.size() + 1];
		std::copy(c.r_filename.begin(), c.r_filename.end(), fname);
		fname[c.r_filename.size()] = '\0';
		stat(fname,&buf);
		strcpy(lastmodify,ctime(&buf.st_mtime));
		delete[] fname;
		fname = NULL;
		int pos = c.r_filename.find_last_of(".");
		string c_type = c.r_filename.substr(pos+1,c.r_filename.size());
		
		if (c_type == "txt" || c_type == "html" || c_type == "htm")
				c.r_ctype ="text/html";
		else if(c_type == "gif" || c_type == "jpeg")
				c.r_ctype = "image/"+c_type;
		else
				c.r_ctype = " ";

		std::stringstream ss;
		ss << c.r_filesize;
		string filesize = ss.str();
		string header = c.r_type + " "+ c.r_method +" 200 OK\r\nDate: ";
		string time(currtime);
		header = header + time;
		header = header +"\r\n" +"Server: myhttpd 1.0\r\n" + "Last-Modified:";
		string lmodify(lastmodify);
		header = header + lmodify ;//+"\r\n";
		header = header + "Content-Type:" + c.r_ctype +"\r\n" + "Content-Length:" + filesize +"\r\n\r\n";
			// cout<<"thread_id of current thread is: ";
			// cout<<pthread_self()<<endl;
		if (send(c.r_acceptid, header.c_str(), strlen(header.c_str()), 0) == -1)
									       perror("send");

		ifstream file;
		char *readblock;
		size_t size;
		file.open(c.r_filename.c_str());
		if (file.is_open()) {
			string read;
			file.seekg (0, ios::end);
			size = file.tellg();
			readblock = new char [size];
			file.seekg (0, ios::beg);
			file.read(readblock, size);
		} else {
			cout<<"Never went Inside"<<endl;
		}
		
		if (send(c.r_acceptid, readblock, size, 0) == -1)
	       perror("send");
		file.close();
		c.status_code = 200;
		delete [] readblock;
}

void sendGetData_dir(clientInfo c)
{
	cout << "hey ========" << c.r_filename << endl;
			char  currtime[50];
		char lastmodify[50];
		struct stat buf;
		time_t tim=time(NULL);
		tm *now=gmtime(&tim);

		if (strftime(currtime, 50,"%c", now) == 0)
				perror("Date Error");
		char * fname = new char[c.r_filename.size() + 1];
		std::copy(c.r_filename.begin(), c.r_filename.end(), fname);
		fname[c.r_filename.size()] = '\0';
		stat(fname,&buf);
		strcpy(lastmodify,ctime(&buf.st_mtime));
		delete[] fname;
		fname = NULL;

		c.r_ctype ="text/html";


	    string result = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'></head><body><h1>Directory listing for ";  
	    result = result + c.r_filename + "</h1><hr><ul>"  ; 



        DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (c.r_filename.c_str())) != NULL) {
  		/* print all the files and directories within directory */
  		while ((ent = readdir (dir)) != NULL) {
  		string temp(ent->d_name) ; 	
  		if(temp[0] == '.') continue ;
  		if(temp == ".." || temp == ".") continue ; 
  		if(c.file_root[c.file_root.size()-1] != '/')c.file_root += "/" ;
  		result  = result + "<li><a href=\""+c.file_root+temp+ "\">"+temp+"</a></li>"; 
    	// printf ("%s\n", ent->d_name);
  		}
  		closedir (dir);
		} else {
  		/* could not open directory */
  		perror ("error in accessing directory data\n");
		}

		result  = result + "</ul><hr></body></html>" ; 

		string filesize = to_string(result.size());


		string header = c.r_type + " "+ c.r_method +" 200 OK\r\nDate: ";
		string time(currtime);
		header = header + time;
		header = header +"\r\n" +"Server: myhttpd 1.0\r\n" + "Last-Modified:";
		string lmodify(lastmodify);
		header = header + lmodify ;//+"\r\n";
		header = header + "Content-Type:" + c.r_ctype +"\r\n" + "Content-Length:" + filesize +"\r\n\r\n";
			// cout<<"thread_id of current thread is: ";
			// cout<<pthread_self()<<endl;
		if (send(c.r_acceptid, header.c_str(), strlen(header.c_str()), 0) == -1)
									       perror("send");


		if (send(c.r_acceptid, result.c_str(), result.size(), 0) == -1)
	       perror("send");

		c.status_code = 200;
		// delete [] readblock;
}



void SendData::transfer_data(clientInfo c)				// Sending data
{
 
	if(c.status_file ==true && c.r_type =="GET") 
	{
		sendGetData(c);

	}
	else if(c.status_dir ==true && c.r_type =="GET") 
	{
		sendGetData_dir(c);

	} 
	 else if(c.status_file ==true && c.r_type =="HEAD") 
	{
		sendHeadData(c);

	} 
	else 
	{
		if (c.rootcheck == true) 
		{

			write(c.r_acceptid,"Error 404: File Not Found",25);
			c.status_code = 404;
		} 
		else 
		{
			c.status_code = 404;
		}
		//listingDir(c);
	}

	if(logging) // If request need to be logged
		generatingLog(c);

	displaylog(c);
	
	close(c.r_acceptid);

}


void SendData::generatingLog(clientInfo c)
{
	ofstream logfile;
	string file = l_file;
	logfile.open(file.c_str(), std::ios::app);
	if( !logfile ) {
		// file couldn't be opened
	    cout << "Error: file could not be opened" << endl;
	}

	logfile<<c.r_ip<<"  ["<<c.r_time<<"]  ["<<c.r_servetime<<"]  "<<c.r_firstline<<" "<<c.status_code<<" "<<c.r_filesize<<endl;
	logfile.close();
}

void SendData::displaylog(clientInfo c)
{

	time_t tim=time(NULL);
	tm *now=gmtime(&tim);
	char currtime[50];
	if (strftime(currtime, 50,"%x:%X", now) == 0)
		perror("Date Panga");
	string servetime(currtime);
	c.r_servetime = servetime;
	if(!r_daemon)
	{
		usleep(0);
		 cout<<c.r_firstline<<" "<<"thread_id:"<<pthread_self()<< " time : "<< clock()<<endl;
	}
	else
	cout<<c.r_ip<<"  ["<<c.r_time<<"]  ["<<c.r_servetime<<"]  "<<c.r_firstline<<" "<<c.status_code<<" "<<c.r_filesize<<" "<<"thread_id:"<<pthread_self()<<endl;

}



