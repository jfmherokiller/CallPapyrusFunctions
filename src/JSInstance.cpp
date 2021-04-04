#include "quickjspp.hpp"



myJSInstance::myJSInstance():
	mycontext(runtime)
{
}

void myJSInstance::RemakeRuntime()
{
	runtime = qjs::Runtime();
	mycontext = qjs::Context(runtime);
	
}
