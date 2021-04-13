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
				if (globalFunct->func->GetParamCount() == numArgs) {
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
	for (std::uint32_t paramIndex = 0; paramIndex < innerFunct->GetParamCount(); ++paramIndex) {
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
	const auto functArgs = RE::MakeFunctionArguments();
	if (argvals.size() == 0) {
		return RE::MakeFunctionArguments();
	}
	if (argvals.size() == 1) {
		const auto typeValOne = argvals.at(0);
		const auto valStringOne = args.at(0);
		if (typeValOne.IsString()) {
			return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne));
		} else if (typeValOne.IsBool()) {
			return RE::MakeFunctionArguments(stob(valStringOne));
		} else if (typeValOne.IsFloat()) {
			return RE::MakeFunctionArguments(std::stof(valStringOne));
		} else if (typeValOne.IsInt()) {
			return RE::MakeFunctionArguments(std::stoi(valStringOne));
		}
	}
	if (argvals.size() == 2) {
		const auto typeValOne = argvals.at(0);
		const auto typeValtwo = argvals.at(1);
		const auto valStringOne = args.at(0);
		const auto valStringTwo = args.at(1);
		if (typeValOne.IsString()) {
			if (typeValtwo.IsString()) {
				return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), new RE::BSFixedString(valStringTwo));
			} else if (typeValtwo.IsBool()) {
				return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), stob(valStringTwo));
			} else if (typeValtwo.IsFloat()) {
				return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stof(valStringTwo));
			} else if (typeValtwo.IsInt()) {
				return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stoi(valStringTwo));
			}
		} else if (typeValOne.IsBool()) {
			if (typeValtwo.IsString()) {
				return RE::MakeFunctionArguments(stob(valStringOne), new RE::BSFixedString(valStringTwo));
			} else if (typeValtwo.IsBool()) {
				return RE::MakeFunctionArguments(stob(valStringOne), stob(valStringTwo));
			} else if (typeValtwo.IsFloat()) {
				return RE::MakeFunctionArguments(stob(valStringOne), std::stof(valStringTwo));
			} else if (typeValtwo.IsInt()) {
				return RE::MakeFunctionArguments(stob(valStringOne), std::stoi(valStringTwo));
			}
		} else if (typeValOne.IsFloat()) {
			if (typeValtwo.IsString()) {
				return RE::MakeFunctionArguments(std::stof(valStringOne), new RE::BSFixedString(valStringTwo));
			} else if (typeValtwo.IsBool()) {
				return RE::MakeFunctionArguments(std::stof(valStringOne), stob(valStringTwo));
			} else if (typeValtwo.IsFloat()) {
				return RE::MakeFunctionArguments(std::stof(valStringOne), std::stof(valStringTwo));
			} else if (typeValtwo.IsInt()) {
				return RE::MakeFunctionArguments(std::stof(valStringOne), std::stoi(valStringTwo));
			}
		} else if (typeValOne.IsInt()) {
			if (typeValtwo.IsString()) {
				return RE::MakeFunctionArguments(std::stoi(valStringOne), new RE::BSFixedString(valStringTwo));
			} else if (typeValtwo.IsBool()) {
				return RE::MakeFunctionArguments(std::stoi(valStringOne), stob(valStringTwo));
			} else if (typeValtwo.IsFloat()) {
				return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stof(valStringTwo));
			} else if (typeValtwo.IsInt()) {
				return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stoi(valStringTwo));
			}
		}
	}
	if (argvals.size() == 3) {
		const auto typeValOne = argvals.at(0);
		const auto typeValtwo = argvals.at(1);
		const auto typeValthree= argvals.at(2);
		const auto valStringOne = args.at(0);
		const auto valStringTwo = args.at(1);
		const auto valStringThree = args.at(2);
		if (typeValOne.IsString()) {
			if (typeValtwo.IsString()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), new RE::BSFixedString(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), new RE::BSFixedString(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), new RE::BSFixedString(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), new RE::BSFixedString(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsBool()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), stob(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), stob(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), stob(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), stob(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsFloat()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stof(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stof(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stof(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stof(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsInt()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stoi(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stoi(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stoi(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(new RE::BSFixedString(valStringOne), std::stoi(valStringTwo), std::stoi(valStringThree));
				}
			}
		} else if (typeValOne.IsBool()) {
			if (typeValtwo.IsString()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(stob(valStringOne), new RE::BSFixedString(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(stob(valStringOne), new RE::BSFixedString(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(stob(valStringOne), new RE::BSFixedString(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(stob(valStringOne), new RE::BSFixedString(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsBool()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(stob(valStringOne), stob(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(stob(valStringOne), stob(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(stob(valStringOne), stob(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(stob(valStringOne), stob(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsFloat()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stof(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stof(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stof(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stof(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsInt()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stoi(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stoi(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stoi(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(stob(valStringOne), std::stoi(valStringTwo), std::stoi(valStringThree));
				}
			}
		} else if (typeValOne.IsFloat()) {
			if (typeValtwo.IsString()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), new RE::BSFixedString(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), new RE::BSFixedString(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), new RE::BSFixedString(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), new RE::BSFixedString(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsBool()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), stob(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), stob(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), stob(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), stob(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsFloat()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stof(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stof(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stof(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stof(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsInt()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stoi(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stoi(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stoi(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stof(valStringOne), std::stoi(valStringTwo), std::stoi(valStringThree));
				}
			}
		} else if (typeValOne.IsInt()) {
			if (typeValtwo.IsString()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), new RE::BSFixedString(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), new RE::BSFixedString(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), new RE::BSFixedString(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), new RE::BSFixedString(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsBool()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), stob(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), stob(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), stob(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), stob(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsFloat()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stof(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stof(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stof(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stof(valStringTwo), std::stoi(valStringThree));
				}
			} else if (typeValtwo.IsInt()) {
				if (typeValthree.IsString()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stoi(valStringTwo), new RE::BSFixedString(valStringThree));
				} else if (typeValthree.IsBool()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stoi(valStringTwo), stob(valStringThree));
				} else if (typeValthree.IsFloat()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stoi(valStringTwo), std::stof(valStringThree));
				} else if (typeValthree.IsInt()) {
					return RE::MakeFunctionArguments(std::stoi(valStringOne), std::stoi(valStringTwo), std::stoi(valStringThree));
				}
			}
		}
	}
	//for (std::uint32_t paramindex = 0; paramindex < args.size(); ++paramindex) {
	//	auto typeVal = argvals.at(paramindex);
	//	auto valString = args.at(paramindex);
	//	if (typeVal.IsString()) {
	//		converted.emplace_back(RE::BSFixedString(valString));
	//	} else if (typeVal.IsBool()) {
	//		converted.emplace_back(stob(valString));
	//	} else if (typeVal.IsFloat()) {
	//		converted.emplace_back(std::stof(valString));
	//	} else if (typeVal.IsInt()) {
	//		converted.emplace_back(std::stoi(valString));
	//	}
	//}
	return functArgs;
}
void myJSInstance::TestFunct(RE::StaticFunctionTag, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	auto ObjectMap = impvm->objectTypeMap;
	std::vector<std::string> classfunctSplitParts = Splitter(classfunct.c_str(), '.');
	std::vector<std::string> functionArgs = Splitter(arglist.c_str(), ',');
	const auto globalFunct = GetGlobalFunction(impvm, classfunctSplitParts, static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;
	ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), nullptr, aaaclass);
}


void myJSInstance::CustomModules()
{
	auto& custMod = mycontext->addModule("testmod");
	//custMod.function<&myJSInstance::TestFunct>("testme");
	custMod.function<&PrintStringToConsoleFile2>("printme");
	mycontext->eval("import * as testmod from 'testmod'; globalThis.testmod = testmod; globalThis.printme = testmod.printme;", "<import>", JS_EVAL_TYPE_MODULE);
}
