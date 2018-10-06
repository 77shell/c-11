

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <vector>
#include <iostream>

using namespace std;

class popOnEmpty {
public:
	popOnEmpty() {
		cout << "popOnEmpty's ctor" << endl;
	}
};

class pushOnFull {
public:
	pushOnFull(int i) : _value(i) {
		cout << "pushOnFull's ctor" << endl;
	}
	int value(void) { return _value; }

	int _value;
};

class iStack {
public:
	iStack(int capacity)
		: _stack(capacity), _top(0)
		{
			cout << "iStack.size(): " << _stack.size() << endl;
			cout << "iStack.capacity(): " << _stack.capacity() << endl;
		}

	void pop(int &value) throw(popOnEmpty);
	void push(int value) throw(pushOnFull, int);

	bool empty(void) 
		try {
			return _top == 0; 
		} catch(pushOnFull) {
		}

	bool full(void) 
		try { 
			return _top >= (int)_stack.size(); 
		} catch(popOnEmpty) {
		}

private:
	std::vector<int> _stack;
	int _top;
};


void
iStack::pop(int &top_value) throw(popOnEmpty)
{
	if(empty())
                throw popOnEmpty{};

	top_value = _stack[--_top];
	cout << "iStack::pop(): " << top_value << endl;
}


void
iStack::push(int value) throw(pushOnFull, int)
try {
	cout << "iStack::push(" << value << ")\n";
	if(full())
		throw pushOnFull(value);

	if(value > 5) {
		cout << "iStack::push(), value > 5" << endl;
		throw value;
	}

	_stack[_top++] = value;
}
catch(pushOnFull obj) {
	cout << "pushOnFull's catch(), Push fail: " << obj.value() << endl;
	throw;
}
catch(int val) {
        cout << "pushOnFull's catch, value > 5" << endl;
        throw;
}
catch(...) {
	cout << "pushOnFull's call-all() :" << value << endl;
	throw;
}


int main(int argc, char *argv[])
// Function try block
	try {
		(void)argc, (void)argv;
		iStack myStack(10);
		for(int i=0; i<10; i++)
		{
			myStack.push(i);
		}

		for(int i=0; i<12; i++)
		{
			int v;
			myStack.pop(v);
		}
		exit(0);
	}

// Exception declaration: A object
	catch(pushOnFull obj) {
		cout << "main()'s catch(pushOnFull): " << obj.value() << endl;
	}

// Exception declaration: A type
	catch(popOnEmpty) {
		cout << "main()'s catch(popOnEmpty)" << endl;
	}

	catch(...) {
		cout << "main()'s catch-all()" << endl;
	}
