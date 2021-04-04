#include "quickjspp.hpp"

class myJSInstance
{
private:
	qjs::Runtime runtime;
	qjs::Context mycontext;
public:
	myJSInstance():mycontext(runtime)
	{
		
	}
};
