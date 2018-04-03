#include <iostream>
#include "listint.h"

using namespace std;

  // Initialize an empty list
ListInt::ListInt()
{
  cout << "***Creating ListInt object" << endl;
  head = NULL;
}

// Destructor (free all memory for each item)
ListInt::~ListInt() // deconstructor
{
  cout << "***Deleteing ListInt object" << endl;
  IntItem *tmp;
  // Add your code here to delete all IntItem structures in the list
  while(head != NULL) {  // same as while(!is_empty());
  	tmp=head; // pointer to head pointer
  	head=head->next;
  	delete tmp;
  } 
}

// Returns true if the list is empty
bool ListInt::is_empty()
{
	return head==NULL;
}

// Returns the number of items in the list 
int ListInt::size()
{
	return size;
}

// Prints each integer item in the list (separated by a space)
void ListInt::print_list()
{

}

// Add a new integer, new_val, to the back of the list
void ListInt::push_back(int new_val)
{

}

// Add a new integer, new_val, to the front of the list
// pushing every other item back one
void ListInt::push_front(int new_val)
{

}

// Remove the front item it there is one
void ListInt::pop_front()
{

}

// Return the item value at the front of the list
int ListInt::front()
{

}


// IntItem *ListInt::find(int find_val)
// {

// }

// bool ListInt::remove(int del_val)
// {

// }

// ListInt &ListInt::operator+(const ListInt &rhs)
// {

// } 


