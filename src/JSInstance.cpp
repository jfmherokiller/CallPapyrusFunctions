#include "quickjspp.hpp"

qjs::Runtime* myJSInstance::runtime = new qjs::Runtime();
qjs::Context* myJSInstance::mycontext = new qjs::Context(*runtime);


std::vector<std::string> Splitter(std::string basetext, const char delim)
{
	std::string buf;				 // Have a buffer string
	std::stringstream ss(basetext);	 // Insert the string into a stream

	std::vector<std::string> tokens;  // Create vector to hold our words

	while (getline(ss, buf, delim))
		tokens.push_back(buf);
	return tokens;
}
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
	mycontext->eval(exposeOSAndStd.c_str(), "<input>", JS_EVAL_TYPE_MODULE);
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
		auto mstr = static_cast<std::string>(returnedValue);
		if (mstr != "undefined")
		{
			PrintStringToConsoleFile(mstr.c_str());
		}
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

RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* myJSInstance::GetGlobalFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, int numArgs)
{
	for (auto object_type : impvm->objectTypeMap) {
		if (strcmp(object_type.first.c_str(), classfunctSplitParts.at(0).c_str()) == 0)
		{
			auto objectInfo = object_type.second;
			for (int index = 0; index < objectInfo->GetNumGlobalFuncs(); ++index)
			{
				auto globalFunct = objectInfo->GetGlobalFuncIter() + index;
				if (globalFunct->func->GetParamCount() == numArgs)
				{
					return globalFunct;
				}
			}
		}
	}
}

void myJSInstance::TestFunct(RE::StaticFunctionTag,RE::BSFixedString classfunct,RE::BSFixedString arglist)
{
	
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	auto ObjectMap = impvm->objectTypeMap;
	std::vector<std::string> classfunctSplitParts = Splitter(classfunct.c_str(), '.');
	std::vector<std::string> functionArgs = Splitter(arglist.c_str(), ',');
	std::string className;
	GetGlobalFunction(impvm, classfunctSplitParts,functionArgs.size());
	impvm->DispatchStaticCall("", "", "", "");
}


void myJSInstance::CustomModules()
{
	auto& custMod = mycontext->addModule("testmod");
	//custMod.function<&myJSInstance::TestFunct>("testme");
	custMod.function <&PrintStringToConsoleFile2>("printme");
	mycontext->eval("import * as testmod from 'testmod'; globalThis.testmod = testmod; globalThis.printme = testmod.printme;", "<import>", JS_EVAL_TYPE_MODULE);
	
}
