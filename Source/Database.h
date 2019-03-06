/*============================================
 *	Author:			Cody Cook
 *	Contact:		codycook96@gmail.com
 * 	Project:		Print Manager Version 1.0
 *	Last Change: 	7/5/2017
 *============================================ */

#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Print.h"
#include "Heap.h"

/*
 *	Database Class
 *	
 *	This class uses heap data structure to manage
 *	print files, importing them from  and exporting 
 *	them to a .dat file, access prints, extracting a 
 * 	sorted vector of prints, and overall providing a 
 * 	cleaner interface with the prints to the Manager
 */

class Database{
	private:
		Heap<Print*> prints;	//A heap data structure of type Print (See Heap.h and Print.h)
	public:
	
		//Constructor
		Database(){}
		
		//Destructor
		virtual ~Database(){		//Destuctor is virtual because it would not be called otherwise
			save("database.dat");
		}
		
		//Size
		int size(){
			return prints.size();
		}

		//Reserve Space in Heap
		void reserve(int newCap){
			prints.reserve(newCap);
		}
		
		//Import Prints from File
		void import(std::string filename, int lastSize){
			if(lastSize > 0)
				reserve(lastSize);	//If there is a previous size from metadata, resize heap to that size
			
			std::string line;		//String for each successive line in the file
			std::string word;		//String for each data element in the line
			
			std::string link = "";	//String for link element of each line
			std::string name = "";	//String for name element of each line
			std::string subs = "";	//String for subs element of each line
			std::string date = "";	//String for date element of each line
			std::string type = "";	//String for type element of each line
			std::string note = "";	//String for note element of each line
			double time = 0.0;		//Double for time element of each line
			int numb = 0;			//Integer for numb element of each line	
			
			std::ifstream file (filename);	//Database file	
			
			if (file.is_open()){
				while(std::getline(file,line)){
					std::istringstream iss(line);	//String for each line
					int i = 0;
					link = "";
					name = "";
					subs = "";
					date = "";
					type = "";
					note = "";
					time = 0.0;
					numb = 0;
					while(std::getline(iss, word, '\t')){	//Get individual elements from line
						switch(i){							//Assign each element to a different successive variable
							case 0:
								if(word != "")		//String with special non-typable characters used for empty elements ⌐NULL¬
									link = word;
								break;
							case 1:
								if(word != "")
									name = word;
								break;
							case 2:
								if(word != "")
									subs = word;
								break;
							case 3:
								if(word != "")
									date = word;
								break;
							case 4:
								if(word != "")
									type = word;
								break;
							case 5:
								if(word != "")
									note = word;
								break;								
							case 6:
								if(word != "")
									time = std::stod(word);		//Convert string to double
								break;
							case 7:
								if(word != "")
									numb = std::stoi(word);		//Convert string to integer
								break;
						}
						i++;
					}
					Print* newPrint = new Print(link, name, subs, date, type, note, time, numb);	//Create Print object
					if((time < 0.00001) && (numb == 0)) {		//If there is no data for time or access, this implies its a new print, give it a new access
						newPrint->access();
					}
					prints.insert(newPrint);					//Insert print to heap
				}
			file.close();
			}
		}
		
		//Store Data to Database file
		void save(std::string filename){				
			std::ofstream file (filename);
			int sz = prints.size();
			if (file.is_open()){
				for(int i = 0; i < sz; i++){
					file << *(prints.pop()) << "\n";	//Pop all prints onto new lines of database file
				}
			}
		}
		
		//List Prints in Order in a Vector
		std::vector<Print*> list(){
			std::vector<Print*> ls;
			prints.sort();				//Build in sort method from heap class
			for(int i = 0; i < prints.size(); i++){
				ls.push_back(prints.at(i));		//Copy elements in order to vector
			}
			return ls;
		}
		
		//Access Single Print
		Print* access(std::string get_link){
			std::string access_link = "";
			int sz = prints.size();
			for(int i = 0; i < sz; i++){
				if(get_link == prints.at(i)->getLink()){
					access_link = prints.at(i)->access();
					prints.swap(0, i);		//Force accessed print to the top
					prints.sift(0);			//Sift accessed print down to correct position
					prints.sort();			//Sort all prints for good measure
					return prints.at(i);
				}
			}
			throw "Database Error: Print not found during access";
		}
		
		//Insert Print onto Heap
		void insert(Print* newP){
			prints.insert(newP);	//Mainly use to give insert functionality to higher up Manager class
		}
		
        //Remove Print
        void remove(std::string link){
            access(link);
            prints.pop();
        }

		//Output Database
		std::ostream& print(std::ostream& out){
			out << prints;
			return out;
		}	
};

#endif
