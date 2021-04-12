#include "quickjspp.hpp"
#include <map>
#include <any>
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

RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* myJSInstance::GetGlobalFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs)
{
	for (auto object_type : impvm->objectTypeMap) {
		if (strcmp(object_type.first.c_str(), classfunctSplitParts.at(0).c_str()) == 0)
		{
			auto objectInfo = object_type.second;
			for (std::uint32_t index = 0; index < objectInfo->GetNumGlobalFuncs(); ++index)
			{
				const auto globalFunct = objectInfo->GetGlobalFuncIter() + index;
				if (globalFunct->func->GetParamCount() == numArgs)
				{
					return globalFunct;
				}
			}
		}
	}
	return nullptr;
}

std::vector<RE::BSScript::TypeInfo> myJSInstance::GetFunctArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct)
{
	auto innerFunct = globalFunct->func;
	std::vector<RE::BSScript::TypeInfo> ParamData;
	for (std::uint32_t paramIndex = 0; paramIndex < innerFunct->GetParamCount(); ++paramIndex)
	{
		RE::BSFixedString paramName;
		RE::BSScript::TypeInfo paramType;
		innerFunct->GetParam(paramIndex, paramName, paramType);
		ParamData.push_back(paramType);
	}
	return ParamData;
}
bool stob(std::string s, bool throw_on_error = true)
{
	auto result = false;  // failure to assert is false

	std::istringstream is(s);
	// first try simple integer conversion
	is >> result;

	if (is.fail()) {
		// simple integer failed; try boolean
		is.clear();
		is >> std::boolalpha >> result;
	}

	if (is.fail() && throw_on_error) {
		throw std::invalid_argument(s.append(" is not convertable to bool"));
	}

	return result;
}

RE::BSScript::IFunctionArguments* myJSInstance::ConvertArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct, std::vector<std::string> args)
{
	auto argvals = GetFunctArgs(globalFunct);
	auto converted = std::vector<std::any>();
	auto functArgs = RE::MakeFunctionArguments();
	for (std::uint32_t paramindex = 0; paramindex < args.size(); ++paramindex)
	{
		auto typeVal = argvals.at(paramindex);
		auto valString = args.at(paramindex);
		if (typeVal.IsString())
		{
		converted.push_back( RE::BSFixedString(valString));
		}
		else if (typeVal.IsBool())
		{
			converted.emplace_back(stob(valString));
		}
		else if (typeVal.IsFloat())
		{
			converted.push_back(std::stof(valString));
		}
		else if (typeVal.IsInt())
		{
			converted.push_back(std::stoi(valString));
		}
	}
	return functArgs;
}
void myJSInstance::TestFunct(RE::StaticFunctionTag,RE::BSFixedString classfunct,RE::BSFixedString arglist)
{
	
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	auto ObjectMap = impvm->objectTypeMap;
	std::vector<std::string> classfunctSplitParts = Splitter(classfunct.c_str(), '.');
	std::vector<std::string> functionArgs = Splitter(arglist.c_str(), ',');
	const auto globalFunct  =GetGlobalFunction(impvm, classfunctSplitParts,static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr) return;
	ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), nullptr,aaaclass);
}


void myJSInstance::CustomModules()
{
	auto& custMod = mycontext->addModule("testmod");
	//custMod.function<&myJSInstance::TestFunct>("testme");
	custMod.function <&PrintStringToConsoleFile2>("printme");
	mycontext->eval("import * as testmod from 'testmod'; globalThis.testmod = testmod; globalThis.printme = testmod.printme;", "<import>", JS_EVAL_TYPE_MODULE);
	
}
