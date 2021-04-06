#include "quickjspp.hpp"

qjs::Runtime* myJSInstance::runtime = new qjs::Runtime();
qjs::Context* myJSInstance::mycontext = new qjs::Context(*runtime);
myJSInstance::myJSInstance()
{
	ReinsertOSAndStd();
}

void myJSInstance::RemakeRuntime()
{
	delete runtime;
	delete mycontext;
	runtime = new qjs::Runtime();
	mycontext = new qjs::Context(*runtime);
	ReinsertOSAndStd();
}

void myJSInstance::ReinsertOSAndStd()
{
	auto myctx = mycontext->ctx;
	qjs::Utility::AddAllBaseParts(myctx);
	mycontext->eval(exposeOSAndStd, "<input>", JS_EVAL_TYPE_MODULE);
	CustomModules();
}

bool myJSInstance::RegisterFuncts(RE::BSScript::Internal::VirtualMachine* a_registry)
{
	a_registry->RegisterFunction("EvalateJsExpression", "MyPluginScript",EvalateJsExpression);
	return true;
}

void myJSInstance::EvalateJsExpression(RE::StaticFunctionTag*, RE::BSFixedString expression)
{
	try
	{
		auto returnedValue = mycontext->eval(expression.c_str(), "<cmdline>", JS_EVAL_TYPE_MODULE);
		PrintStringToConsoleFile(static_cast<std::string>(returnedValue).data());
	}
	catch (qjs::exception)
	{
		auto exc = mycontext->getException();
		PrintStringToConsoleFile(static_cast<std::string>(exc).data());
		if (static_cast<bool>(exc["stack"]))
		{
			PrintStringToConsoleFile(static_cast<std::string>(exc["stack"]).data());
		}
	}
}

void myJSInstance::TestFunct()
{
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	for (auto object_type : impvm->objectTypeMap) {
		logger::info(object_type.first.data());
	}
}

void myJSInstance::CustomModules()
{
	auto& custMod = mycontext->addModule("testmod");
	custMod.function<&myJSInstance::TestFunct>("testme");
	mycontext->eval("import * as testmod from 'testmod'; globalThis.testmod = testmod;", "<import>", JS_EVAL_TYPE_MODULE);
	
}
