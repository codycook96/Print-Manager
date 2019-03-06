/*============================================
 *	Author:			Cody Cook
 *	Contact:		codycook96@gmail.com
 * 	Project:		Print Manager Version 1.0
 *	Last Change: 	7/5/2017
 *============================================ */

#ifndef PRINT_H
#define PRINT_H

#include <string>
#include <iostream>
#include <sstream> 
//#include "CurTime.h"
#include <ctime>
#include <math.h>
#include <iomanip>


/*	
 *	Print Class
 *
 *	This class represents substation print files.
 *	It contains information about the drawing, 
 *	the file, and meta data such as the last 
 *	time it was accessed and the number of times
 *	it was accessed. 
 */
class Print{
	private:
		std::string link;	//Full file name of print file and extension - not path
		std::string name;	//Name on the actual drawing
		std::string subs;	//Substation print belongs to
		std::string date;	//Date on drawing or date of revision
		std::string type;	//Type of print as it corresponds to a specific part of the substation
		std::string note;	//Extra comments or tags 
        double tyme;		//Time of the last access of print - used for rating prints on relevance
		int numb;			//Number of times print was accessed - used for rating prints on relevance
		
	public:
		
		//Constructor
		Print(std::string newLink, std::string newName, std::string newSubs, std::string newDate, 
			std::string newType, std::string newNote, double newTime, int newNumb): 
			link(newLink), name(newName), subs(newSubs), date(newDate), type(newType), note(newNote), 
            tyme(newTime), numb(newNumb) {
            if((tyme < 0.00001) && (numb == 0)) {	//If print does not have time or numb from the database it is new
				this->access();						//Therefore its time is now the current time and access now 1
			}
		}
		
		//Assignment Operator
		Print& operator=(Print& that){
			if(&that == this)		//Check for self comparison
				return *this;
			link = that.getLink();
			name = that.getName();
			subs = that.getSubs();
			date = that.getDate();
			type = that.getType();
			note = that.getNote();
            tyme = that.getTime();
			numb = that.getNumb();
			return *this;
		}
		
		//Destructor
		~Print(){}

        double curTime(){

            time_t timer;
            struct tm y2k = {0};	//Time object to be set to January 1st 20000
            y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
            y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
            time(&timer);			//Time object set to current time
            return difftime(timer,mktime(&y2k));	//Return double of seconds since January 1st 2000

            //return 1.01;
         }

		//Access Print
		std::string access(){
			numb++;				//Incrememnt number of accesses
            tyme = curTime();	//Set time last accessed to current time
			return link;		//Return file name
		}
		
		//Get File Name
		std::string getLink(){
			return link;
		}
		//Get Drawing Name
		std::string getName(){
			return name;
		}
		//Get Substation
		std::string getSubs(){
			return subs;
		}
		//Get Date on Print
		std::string getDate(){
			return date;
		}
		//Get Type of Print
		std::string getType(){
			return type;
		}
		//Get Notes about Print
		std::string getNote(){
			return note;
		}
		//Get Time Print was Last Accessed
		double getTime(){
            return tyme;
		}
		//Get Number of Times Print has been Accessed
		int getNumb(){
			return numb;
		}
		
        //Set File Name
        void setLink(std::string getLink){
            link = getLink;
        }
        //Set Drawing Name
        void setName(std::string getName){
            name = getName;
        }
        //Set Substation
        void setSubs(std::string getSubs){
            subs = getSubs;
        }
        //Set Date on Print
        void setDate(std::string getDate){
            date = getDate;
        }
        //Set Type of Print
        void setType(std::string getType){
            type = getType;
        }
        //Set Notes about Print
        void setNote(std::string getNote){
            note = getNote;
        }

		//Get Print Score based on Accesses
		double score(){
            double timeCurrent = curTime();	//Get current time for comparison with time last accessed
			/*	
			 *	Scoring Algorithm:
			 *
			 *	This sorting algorithm uses logarithms to create an exponential decay scale where - 
			 *
			 *	The number of times accessed give diminishing returns as they grow larger, i.e. 
			 *	the difference of score between prints with 1 access and 2 accesses will be greater 
			 *	than that of prints with 8 accesses and 9 accesses.
			 *
			 *	The time last accessed give diminishing returns as it gets farther from the current
			 *	time, i.e. the score difference between prints accesed 1 minute ago and 5 minutes 
			 *	ago is greater than that of prints accessed 1 hour ago and one 5 hours ago.
			 *
			 *	Also the time accessed is graded more highly than the number of times accessed 
			 *	initially, i.e. A print accessed 1 minute ago with only one access should score
			 *	more highly than one accessed 5 times 5 hours ago. However, when the first print 
			 *	still has 1 access but the access was now 10 hours and the other print still 
			 *	has 5 accesses now 15 hours ago, the second print should score more highly.
			 *	
			 *	For more follow this link to a picture of an Excel table for using conditional 
			 *	formatting to demonstrate the gradient of scoring and as a reference for different
			 *	score scenarios:
			 *	
			 *	http://imgur.com/a/0CnXk
			 */
            return ((1+log(numb)/log(120))/(1+log(timeCurrent - tyme + 1)/log(6)))*10;
		}

		//Comparisoon Operator
		friend bool operator == (Print& lhs, Print& rhs){
            /*
            std::string a = lhs.getLink();
            std::string b = rhs.getLink();

            for(int i = 0; i<a.length(); i++){
                 a[i] = toupper(a[i]);
            }
            for(int i = 0; i<b.length(); i++){
                 b[i] = toupper(b[i]);
            }
            */
            return lhs.getLink() == rhs.getLink();	//Prints are considered equal if they contain the same file name
		}
		//Less Than Operator
		friend bool operator < (Print& lhs, Print& rhs){
			return lhs.score() < rhs.score();		//See score description above
		}
		//Greater Than Operator		
		friend bool operator > (Print& lhs, Print& rhs){
			return lhs.score() > rhs.score();		//See score description above
		}
	
		//Save All Data

        /*
        std::ofstream& save(std::ofstream out){
			out << std::fixed << std::setprecision(0);	//Ensure double time doesn't output in scientific form
			out << link << "\t" << name << "\t" << subs << "\t" << date << "\t" << type << "\t" << note; 	
            //out << time << "\t" << numb << "\t";
		}
        */
		
		//Display Necessary Data
		friend std::ostream& operator<<(std::ostream& out, const Print& pr){
			out << std::fixed << std::setprecision(0);	//Ensure double time doesn't output in scientific form
			out << pr.link << "\t" << pr.name << "\t" << pr.subs << "\t";
			out << pr.date << "\t" << pr.type << "\t" << pr.note << "\t";
            out << pr.tyme << "\t" << pr.numb << "\t";
			return out;
		}


};



#endif 
