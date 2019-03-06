/*============================================
 *	Author:			Cody Cook
 *	Contact:		codycook96@gmail.com
 * 	Project:		Print Manager Version 1.0
 *	Last Change: 	6/30/2017
 *============================================ */
 
#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <math.h>
#include <iostream>
#include <type_traits>

/*
 *	Heap Class
 *	
 *	This class defines a typical, templated heap data structure.
 *	It is a binary tree built on a vector (array) in which a 
 *	parent node is always greater than a child node. The 
 * 	relationship between sibling in cousin nodes is not 
 *	specified, but overall order is still maintained with 
 *	accesseing items because like a queue, items are only 
 *	accessed from the top. Then when the top item is removed,
 *	the last item is put in its place and then sorted down until
 *	all parents are greater than their children. 
 *
 */
template<typename T>
class Heap{
	private:
		std::vector<T> heap;	//Vector that contains items and with which heap rules are enforced
	public:
		static const bool IS_POINTER = std::is_pointer<T>::value; 	//True if the templated value contains a poniter

		//Constructor
		Heap(){}
		
		//Destructor
		~Heap(){
			int sz = heap.size();
			if(IS_POINTER){						//Assuming that whenever templated item is a pointer, 
				for(int i = 0; i < sz; i++){	//it points to a dynamically allocated object
					delete pop();
				}
			}
		}
		
		//TODO - Make assignment operator <--------------------------------------------------------------------------------------------------<<< 
		
		//Reserve Space for Vector
		void reserve(int newCap){
			heap.reserve(newCap);
		}
		
		//Size
		int size(){
			return heap.size();
		}
		
		//Index
		T at(int i){
			if(i < 0 || i >= size())	//Check if index is out of bounds
				throw "Heap Error: Index out of bounds";
			return heap.at(i);
		}
		
		//Swap Two Items on Vector
		void swap(int a, int b){
			int size = heap.size();
			if(a >= size || b >= size)	//Check that indices are in bounds
				throw "Heap Error: Swap out of bounds";
			T temp = heap.at(a);
			heap.at(a) = heap.at(b);
			heap.at(b) = temp;
		}
		
		//Insert Item onto Heap
		void insert(T newP){
			int i = heap.size();				//Index i back of vector
			heap.push_back(newP);				//Insert new item onto back of vector
			while(i > 0){
				int a = ((i+1)/2)-1;			//Index a parent of i
				if(IS_POINTER){					//Checks if template item is a pointer
					if(*heap.at(i) > *heap.at(a)){	//If it is a pointer, dereference to compare
						swap(i, a);						//Swap parent and left child
						i = a;							//Set index to new position of original item
					}
					else{
						i = 0;							//End loop
					}
				}
				else{
					if(heap.at(i) > heap.at(a)){	//If it is not a pointer, just compare items
						swap(i, a);						//Swap parent and left child
						i = a;							//Set index to new position of original item
					}
					else{
						i = 0;							//End loop
					}
				}
				
			}
		}
		
		//Remove Items from Top of Heap
		T pop(){
			int z = heap.size() - 1; 	//Index last item in vector
			swap(0, z);					//Swap the last element and top element
			T back = heap.at(z);		//Copy item from last to back
			heap.pop_back();			//Remove last item
			if(heap.size() > 0)
				sift(0);					//call sift() on first item in vector
			return back;				//return back
		}
		
		//Sort Down Until Parent is Greater than Children
		/*
		 *	Summary:
		 *	This function essentially compares the parent node
		 *	to its children and if it is smaller than either 
		 *	of them, it gets swapped with it and then you 
		 *	start the same check at the new, lower position.
		 *	This recursive calling continues until both
		 *	children are larger than the parent or it is the 
		 * 	bottom of the heap.
		 *
		 *	You may notice much of the same code is repeated; 
		 *	this is due to the fact that it considers cases 
		 *	where the template item is a pointer and when isn't
 		 */
		void sift(int i){
			int size = heap.size();
			int a = i*2 + 1;	//Index a to left child of i
			int b = i*2 + 2;	//Index b to right child of i
			if(i >= size){		//Check if i is out of bounds
				throw "Error: Sift out of bounds";
			}
			//No children exist
			if(a >= size)		
				return;			//Stop if i has no children
			//Only right child exists
			if((b >= size)){
				if(IS_POINTER){					//Checks if template item is a pointer
					if(*heap.at(i) < *heap.at(a)){	//Checks parent is less than left child
						swap(i, a);						//Swaps left child and parent
						sift(a);						//Start new sift at new position of original item
					}
					return;
				}
				else{
					if(heap.at(i) < heap.at(a)){	//Checks if parent is less than left child
						swap(i, a);						//Swaps left child and parent
						sift(a);						//Starts new sift at new position of original item
					}
					return;
				}
			//Both children exist	
			}
			if(IS_POINTER){					//Checks if template item is a pointer
				if(!(*heap.at(a) < *heap.at(b))){	//Checks if left child is less than right child
					if(*heap.at(i) < *heap.at(a)){		//Checks if parent is less than left child
						swap(i, a);							//Swaps parent and left child
						sift(a);							//Starts new sift at new position of original item
					}
				}
				else{
					if(*heap.at(i) < *heap.at(b)){	//Checks if parent is less than right child
						swap(i, b);						//Swaps parent and right child
						sift(b);						//Starts new sift at new position of original item
					}
				}
			}
			else{
				if(!(heap.at(a) < heap.at(b))){		//Checks if left child is less than right child
					if(heap.at(i) < heap.at(a)){		//Checks if parent is less than left child
						swap(i, a);							//Swaps parent and left child
						sift(a);							//Starts new sift at new position of original item
					}
				}
				else{
					if(heap.at(i) < heap.at(b)){	//Checks if parent is less than right child
						swap(i, b);						//Swaps parent and right child
						sift(b);						//Starts new sift at new position of original item
					}
				}
			}
		}
		
		//Sort Entire Heap
		void sort(){
			int sz = heap.size();
			std::vector<T> temp; 				//Temporary vector
			for(int i = 0; i < sz; i++){		
				temp.push_back(this->pop());	//Pop off top into temporary vector
			}
			heap = temp;						//Set heap vector to temporary sorted vector
		}
		
		//Output Heap
		template<typename U>
		friend std::ostream& operator<<(std::ostream& out, Heap<U> hp);	//Couldnt pass Heap as const ref because when it pops it inherently changes the heap
};

template<typename T>
std::ostream& operator<<(std::ostream& out, Heap<T> hp) {
	int sz = hp.heap.size(); 
	for(int i = 0; i < sz; i++){	
		out << *hp.pop() << "\n";	//Pop top item onto stream and insert new line
	}
	return out;
}

#endif