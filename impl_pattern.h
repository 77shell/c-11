
#include <iostream>
#include <memory>
#include <experimental/propagate_const>
 

class widget {
	class impl;
	std::experimental::propagate_const<std::unique_ptr<impl>> pImpl;
public:
	void draw() const; // public API that will be forwarded to the implementation
	void draw();
	bool shown() const { return true; } // public API that implementation has to call
	widget(int);
	~widget(); // defined in the implementation file, where impl is a complete type
	widget(widget&&); // defined in the implementation file
	// Note: calling draw() on moved-from object is UB
	widget(const widget&) = delete;
	widget& operator=(widget&&); // defined in the implementation file
	widget& operator=(const widget&) = delete;
};
