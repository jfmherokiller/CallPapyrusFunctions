#include "TypeHandling.hpp"
#include "quickjspp.hpp"

#include <any>
#include <map>
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
	a_registry->RegisterFunction("EvalateJsExpression", "MyPluginScript", EvalateJsExpression);
	a_registry->RegisterFunction("CallGlobalFunction", "MyPluginScript", CallGlobalFunction);
	return true;
}

void myJSInstance::EvalateJsExpression(RE::StaticFunctionTag*, RE::BSFixedString expression)
{
	try {
		auto returnedValue = mycontext->eval(expression.c_str(), "<cmdline>", JS_EVAL_TYPE_MODULE);
		auto mstr = static_cast<std::string>(returnedValue);
		if (mstr != "undefined") {
			PrintStringToConsoleFile(mstr.c_str());
		}
	} catch (qjs::exception) {
		auto exc = mycontext->getException();
		PrintStringToConsoleFile(static_cast<std::string>(exc).data());
		if (static_cast<bool>(exc["stack"])) {
			PrintStringToConsoleFile(static_cast<std::string>(exc["stack"]).data());
		}
	}
}

RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* myJSInstance::GetGlobalFunction(RE::BSScript::Internal::VirtualMachine* impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs)
{
	for (auto object_type : impvm->objectTypeMap) {
		if (strcmp(object_type.first.c_str(), classfunctSplitParts.at(0).c_str()) == 0) {
			auto objectInfo = object_type.second;
			for (std::uint32_t index = 0; index < objectInfo->GetNumGlobalFuncs(); ++index) {
				const auto globalFunct = objectInfo->GetGlobalFuncIter() + index;
				if (strcmp(globalFunct->func->GetName().c_str(), classfunctSplitParts.at(1).c_str()) == 0) {
					if (globalFunct->func->GetParamCount() == numArgs) {
						return globalFunct;
					}
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
	for (std::uint32_t paramIndex = 0; paramIndex < innerFunct->GetParamCount(); ++paramIndex) {
		RE::BSFixedString paramName;
		RE::BSScript::TypeInfo paramType;
		innerFunct->GetParam(paramIndex, paramName, paramType);
		ParamData.push_back(paramType);
	}
	return ParamData;
}
bool StringToBool(std::string s, bool throw_on_error = true)
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

bool myJSInstance::HandleSingleValue(std::vector<std::string> args, std::vector<RE::BSScript::TypeInfo> argvals, RE::BSScript::TypeInfo typeValOne, std::string valStringOne, RE::BSScript::IFunctionArguments*& value1)
{

	if (typeValOne.IsString()) {
		value1 = RE::MakeFunctionArguments(BsString(valStringOne));
		return true;
	} else if (typeValOne.IsBool()) {
		value1 = RE::MakeFunctionArguments(StringToBool(valStringOne));
		return true;
	} else if (typeValOne.IsFloat()) {
		value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne));
		return true;
	} else if (typeValOne.IsInt()) {
		value1 = RE::MakeFunctionArguments(StringToInt(valStringOne));
		return true;
	}
	return false;
}

bool myJSInstance::HandleTwoValues(std::vector<std::string> args, std::vector<RE::BSScript::TypeInfo> argvals, RE::BSScript::TypeInfo typeValOne, RE::BSScript::TypeInfo typeValtwo, std::string valStringOne, std::string valStringTwo, RE::BSScript::IFunctionArguments*& value1)
{
	if (typeValOne.IsString()) {
		if (typeValtwo.IsString()) {
			value1 = RE::MakeFunctionArguments(BsString(valStringOne), BsString(valStringTwo));
			return true;
		} else if (typeValtwo.IsBool()) {
			value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToBool(valStringTwo));
			return true;
		} else if (typeValtwo.IsFloat()) {
			value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToFloat(valStringTwo));
			return true;
		} else if (typeValtwo.IsInt()) {
			value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToInt(valStringTwo));
			return true;
		}
	} else if (typeValOne.IsBool()) {
		if (typeValtwo.IsString()) {
			value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), BsString(valStringTwo));
			return true;
		} else if (typeValtwo.IsBool()) {
			value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToBool(valStringTwo));
			return true;
		} else if (typeValtwo.IsFloat()) {
			value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToFloat(valStringTwo));
			return true;
		} else if (typeValtwo.IsInt()) {
			value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToInt(valStringTwo));
			return true;
		}
	} else if (typeValOne.IsFloat()) {
		if (typeValtwo.IsString()) {
			value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), BsString(valStringTwo));
			return true;
		} else if (typeValtwo.IsBool()) {
			value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToBool(valStringTwo));
			return true;
		} else if (typeValtwo.IsFloat()) {
			value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToFloat(valStringTwo));
			return true;
		} else if (typeValtwo.IsInt()) {
			value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToInt(valStringTwo));
			return true;
		}
	} else if (typeValOne.IsInt()) {
		if (typeValtwo.IsString()) {
			value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), BsString(valStringTwo));
			return true;
		} else if (typeValtwo.IsBool()) {
			value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToBool(valStringTwo));
			return true;
		} else if (typeValtwo.IsFloat()) {
			value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToFloat(valStringTwo));
			return true;
		} else if (typeValtwo.IsInt()) {
			value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToInt(valStringTwo));
			return true;
		}
	}
	return false;
}

bool myJSInstance::HandleThreeValues(std::vector<std::string> args, std::vector<RE::BSScript::TypeInfo> argvals, RE::BSScript::TypeInfo typeValOne, RE::BSScript::TypeInfo typeValtwo, RE::BSScript::TypeInfo typeValthree, std::string valStringOne, std::string valStringTwo, std::string valStringThree, RE::BSScript::IFunctionArguments*& value1)
{
	if (typeValOne.IsString()) {
		if (typeValtwo.IsString()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), BsString(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), BsString(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), BsString(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), BsString(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsBool()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToBool(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToBool(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToBool(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToBool(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsFloat()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToFloat(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToFloat(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToFloat(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToFloat(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsInt()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToInt(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToInt(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToInt(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(BsString(valStringOne), StringToInt(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		}
	} else if (typeValOne.IsBool()) {
		if (typeValtwo.IsString()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), BsString(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), BsString(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), BsString(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), BsString(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsBool()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToBool(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToBool(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToBool(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToBool(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsFloat()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToFloat(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToFloat(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToFloat(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToFloat(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsInt()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToInt(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToInt(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToInt(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToBool(valStringOne), StringToInt(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		}
	} else if (typeValOne.IsFloat()) {
		if (typeValtwo.IsString()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), BsString(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), BsString(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), BsString(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), BsString(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsBool()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToBool(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToBool(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToBool(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToBool(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsFloat()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToFloat(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToFloat(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToFloat(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToFloat(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsInt()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToInt(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToInt(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToInt(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToFloat(valStringOne), StringToInt(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		}
	} else if (typeValOne.IsInt()) {
		if (typeValtwo.IsString()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), BsString(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), BsString(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), BsString(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), BsString(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsBool()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToBool(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToBool(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToBool(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToBool(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsFloat()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToFloat(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToFloat(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToFloat(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToFloat(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		} else if (typeValtwo.IsInt()) {
			if (typeValthree.IsString()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToInt(valStringTwo), BsString(valStringThree));
				return true;
			} else if (typeValthree.IsBool()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToInt(valStringTwo), StringToBool(valStringThree));
				return true;
			} else if (typeValthree.IsFloat()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToInt(valStringTwo), StringToFloat(valStringThree));
				return true;
			} else if (typeValthree.IsInt()) {
				value1 = RE::MakeFunctionArguments(StringToInt(valStringOne), StringToInt(valStringTwo), StringToInt(valStringThree));
				return true;
			}
		}
	}
	return false;
}

RE::BSScript::IFunctionArguments* myJSInstance::ConvertArgs(RE::BSScript::ObjectTypeInfo::GlobalFuncInfo* globalFunct, std::vector<std::string> args)
{
	auto argvals = GetFunctArgs(globalFunct);
	RE::BSScript::TypeInfo typeValOne;
	RE::BSScript::TypeInfo typeValtwo;
	RE::BSScript::TypeInfo typeValthree;
	std::string valStringOne;
	std::string valStringTwo;
	std::string valStringThree;
	RE::BSScript::IFunctionArguments* value1 = RE::MakeFunctionArguments();
	if (!argvals.empty()) {
		typeValOne = argvals.at(0);
		valStringOne = args.at(0);
	}
	if (argvals.size() >= 2) {
		typeValtwo = argvals.at(1);
		valStringTwo = args.at(1);
	}
	if (argvals.size() == 3) {
		typeValthree = argvals.at(2);
		valStringThree = args.at(2);
	}
	if (argvals.size() == 1) {
		HandleSingleValue(args, argvals, typeValOne, valStringOne, value1);
	}
	if (argvals.size() == 2) {
		HandleTwoValues(args, argvals, typeValOne, typeValtwo, valStringOne, valStringTwo, value1);
	}
	if (argvals.size() == 3) {
		HandleThreeValues(args, argvals, typeValOne, typeValtwo, typeValthree, valStringOne, valStringTwo, valStringThree, value1);
	}
	return value1;
}
void myJSInstance::CallGlobalFunction(RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	auto ObjectMap = impvm->objectTypeMap;
	std::string classfunctStr = classfunct.c_str();
	std::string arglistStr = arglist.c_str();
	// Remove all double-quote characters
	classfunctStr.erase(remove(classfunctStr.begin(), classfunctStr.end(), '\"'), classfunctStr.end());
	arglistStr.erase(remove(arglistStr.begin(), arglistStr.end(), '\"'), arglistStr.end());

	std::vector<std::string> classfunctSplitParts = Splitter(classfunctStr, '.');
	std::vector<std::string> functionArgs = Splitter(arglistStr, ',');
	const auto globalFunct = GetGlobalFunction(impvm, classfunctSplitParts, static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;
	const auto functargs = ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
}


void myJSInstance::CustomModules()
{
	auto& custMod = mycontext->addModule("testmod");
	//custMod.function<&myJSInstance::TestFunct>("testme");
	custMod.function<&PrintStringToConsoleFile2>("printme");
	mycontext->eval("import * as testmod from 'testmod'; globalThis.testmod = testmod; globalThis.printme = testmod.printme;", "<import>", JS_EVAL_TYPE_MODULE);
}
