#ifndef _CCStack_H
#define _CCStack_H


template <class T, int _size> //Template for a CStack
class CStack	//This is a CStack class
{
public:
	CStack();	//default constuctor, passed a int variable for size
	bool pop(T& element);	//pop function
	bool push(T element);	//push function
	bool isEmpty();	//checks if CStack is empty
	int currSize();	//returns the current size
	bool isFull();	//checks if CStack is full
	bool copytop(T& element);

private:
	int size;	//holds size of CStack
	int top;	//index of the top location of CStack
	T *s_ptr;	//pointer holds a spot in memory
};

template <class T, int _size>	//Template constructor
CStack<T, _size> ::CStack() //passed maxsize from initial declaration of CStack
{
	size = _size;	//sets private size variable equal to size passed from declaration at main
	top = -1;	//initially there is no top, so set to -1
	s_ptr = new T[size];	//dynamically allocates memory to where pointer was stationed
}

template <class T, int _size> //Template push function
bool CStack<T, _size> ::push(T element)	//passed an element to push
{
	if (!isFull())	//if isFull function is not true (not full)
	{
		s_ptr[++top] = element; //go to next location and put the element in there
		return true;
	}
	return false;
	
}

template <class T, int _size>	//Template pop function
bool CStack<T, _size> ::pop(T& element)
{
	if (!isEmpty()) // if isEmpty function is not true (not empty)
	{
		element= s_ptr[top--];
		return true; // return the top location and then decrement top by 1
	}
	return false;
}

template <class T, int _size>	//Template pop function
bool CStack<T, _size> ::copytop(T& element)
{
	if (!isEmpty()) // if isEmpty function is not true (not empty)
	{
		element = s_ptr[top];
		return true; // return the top location and then decrement top by 1
	}
	return false;
}


template <class T, int _size>	//Template isFull function
bool CStack<T, _size> ::isFull()
{
	if (top == size - 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template <class T, int _size>	//Template isEmpty function
bool CStack<T, _size> ::isEmpty()
{
	if (top == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template <class T, int _size>	//Template currSize function
int CStack<T, _size> ::currSize()
{
	return (top + 1); //returns the actual size of CStack at the point it is called
}



#endif
