#include "impl_pattern.h"

class widget::impl {
	int n; // private data
public:
	void draw(const widget& w) const {
		if(w.shown()) // this call to public member function requires the back-reference 
			std::cout << "drawing a const widget " << n << '\n';
	}
	void draw(const widget& w) {
		if(w.shown())
			std::cout << "drawing a non-const widget " << n << '\n';
	}
	impl(int n) : n(n) {}
};

void widget::draw() const
{
	pImpl->draw(*this);
}

void widget::draw()
{
	pImpl->draw(*this);
}

widget::widget(int n)
	: pImpl{std::make_unique<impl>(n)}
{}

widget::widget(widget&&) = default;
widget::~widget() = default;
widget& widget::operator=(widget&&) = default;
