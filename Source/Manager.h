/*============================================
 *	Author:			Cody Cook
 *	Contact:		codycook96@gmail.com
 * 	Project:		Print Manager Version 1.0
 *	Last Change: 	7/5/2017
 *============================================ */
 
#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <string>
#include <ctime>
#include "Database.h"

//For debugging output
#include <QTextStream>
#include <sstream>

/*
 *	Manager Class
 *	
 *	This class is the top level class, providing all necessary
 * 	methods needed to operate the application, such as opening 
 *	and saving, searching prints, inserting a new print, 
 * 	accessing prints, and outputting prints. These methods are
 * 	meant to be called from an interfaceing class, either 
 * 	using command line or some GUI.
 */

class Manager{
	private:
		int d_size;					//Database size at end of last session
		double s_time;				//Time at the end of the last session
        std::string path;
        Database database;			//Database object containing all the prints and their access methods
		std::vector<Print*> query;	//Vector containing prints relevant to the search term
	public:
		
		//Constructor
		Manager(){
            importMeta("test.txt");			//Get metadata from previous session(s)
            database.import("database.dat", d_size);	//Import database to db
		}
		
		//Destructor
		virtual ~Manager(){						//Destructor is virtual because it would not be called otherwise
            s_time = curTime();                 //Current time
			d_size = database.size();			//Gets size of database
			saveMeta("metadata.dat");			//Save metadata to metadata file
		}
		
        double curTime(){
            time_t timer;
            struct tm y2k = {0};	//Time object to be set to January 1st 20000
            y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
            y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
            time(&timer);			//Time object set to current time
            return difftime(timer,mktime(&y2k));	//Return double of seconds since January 1st 2000
         }

		//Import Metada from File
		void importMeta(std::string filename){
            std::ifstream file ("metadata.dat");				//File object of Metadat file
			std::string line;							//String for each line of file
			std::string word;							//String for actual data element in each line
			int i = 0;									//Index for progressing down each line
			if (file.is_open()){
				while(std::getline(file,line)){
					std::istringstream iss(line);		//Turn string of line into stream to further partition
					std::getline(iss, word, '\t');		//Skip first word that is just a label  
					std::getline(iss, word, '\t');		//Get actual value
					switch(i){							//Switch statement for assigning each line's data to a variable
						case 0:
							d_size = std::stoi(word);	//Convert string of first line element to integer for database size
                            break;
						case 1:
							s_time = std::stod(word);	//Convert string of second line element to double for session time
                            break;
                        case 2:
                            path = word;
                            break;
						default:
							break;
					}
					i++;
				}
			}
		}
		
		//Save Metada to File
		void saveMeta(std::string filename){
			std::ofstream file (filename);						//Metadata file
			file << std::fixed << std::setprecision(0);			//Ensure doubles aren't save in scientific notation
			file << "Last Database Size:\t" << d_size << "\n";	//Save current database size
			file << "Last Session Time:\t" << s_time << "\n";	//Save current time
            file << "Last Path:\t" << path << "\n";             //Save path
		}
		
        std::string caseChange(std::string& in){
            for(int i = 0; i<in.length(); i++){
                 in[i] = toupper(in[i]);
            }
            return in;
        }

		//Search and Return All Relevant Prints
		void search(std::string search_link, std::string search_name, std::string search_subs, 
					std::string search_date, std::string search_type, std::string search_note){
			query.clear();								//Usure query vector is empty
			bool found;									//Flag to check if each print matches criteria
			std::vector<Print*> list = database.list();	//Get ordered list of prints
			int sz = list.size();						
			query.reserve(sz);							//Reserve space for query size of print list
			for(int i = 0; i < sz; i++){
				found = true;
				if(search_link != ""){	
                    if(caseChange(list.at(i)->getLink()).find(caseChange(search_link)) == std::string::npos){	//If link search term is NOT contained within link string
						found = false;
					}
				}
				if(search_name != ""){	
                    if(caseChange(list.at(i)->getName()).find(caseChange(search_name)) == std::string::npos){	//If name search term is NOT contained within name string
						found = false;
					}
				}
				if(search_subs != ""){	
                    if(caseChange(list.at(i)->getSubs()).find(caseChange(search_subs)) == std::string::npos){	//If subs search term is NOT contained within subs string
						found = false;
					}
				}
				if(search_date != ""){	
                    if(caseChange(list.at(i)->getDate()).find(caseChange(search_date)) == std::string::npos){	//If date search term is NOT contained within date string
						found = false;
					}
				}
				if(search_type != ""){	
                    if(caseChange(list.at(i)->getType()).find(caseChange(search_type)) == std::string::npos){	//If type search term is NOT contained within type string
						found = false;
					}
				}
				if(search_note != ""){	
                    if(caseChange(list.at(i)->getNote()).find(caseChange(search_note)) == std::string::npos){	//If note search term is NOT contained within note string
						found = false;
					}
				}
				if(found){
					query.push_back(list.at(i));
				}
			}
		}
		
		//Access print by link
		std::string access(std::string link){				//**Could possibly be accessed by some other identifying item
			try{
				return database.access(link)->getLink();	//**Or change access so it doesn't return link we already have
			}
			catch(std::string error){
				std::cout << error << "\n";
				return "";
			}
		}
		
		//Insert Print 
		void insert(std::string link, std::string name, std::string subs, 
					std::string date, std::string type, std::string note){
			Print* newP = new Print(link, name, subs, date, type, note, 0.0, 0);	//Create new print
			database.insert(newP);													//Insert print into database
		}

        Print* getPrint(std::string link){
            std::vector<Print*> list = database.list();
            int sz = list.size();
            for(int i = 0; i < sz; i++){
                if(list.at(i)->getLink() == link){
                    return list.at(i);
                }
            }
            return nullptr;
        }

       void import(std::string link){
           Print* newP = new Print(link, "", "", "", "", "", 0.0, 0);
           newP->access();
           database.insert(newP);
       }

       void remove(std::string link){
            try{
                database.remove(link);
            }
            catch(std::string error){
                std::cout << error << "\n";
            }
        }

       //Get Path
       std::string getPath(){
            return path;
       }

       //Set Patj
       void setPath(std::string newPath){
            path = newPath;
       }

        //Get Query
        std::vector<Print*> getQuery(){
            return query;
        }

        //List All Prints
        std::vector<Print*> getAll(){
            return database.list();
        }

		//Output Prints in Query
		std::ostream& printQuery(std::ostream& out){
			int sz = query.size();
			for(int i = 0; i < sz; i++){
				out << *query.at(i) << "\n";
			}
		}
		
		//Output All Prints
		std::ostream& printAll(std::ostream& out){
			database.print(out);
			return out;
		}
		
};

#endif
