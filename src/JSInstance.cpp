myJSInstance::myJSInstance() = default;

bool myJSInstance::RegisterFuncts(BSScriptVmPtr a_registry)
{
	a_registry->RegisterFunction("CallGlobalFunction", "CallGlobalFuncts", CallGlobalFunction);
	a_registry->RegisterFunction("CallInstanceFunction", "CallGlobalFuncts", CallInstanceFunction);
	return true;
}

globalFunctInfoPtr myJSInstance::GetGlobalFunction(BSScriptVmPtr impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs)
{
	for (const auto& object_type : impvm->objectTypeMap) {
        auto ClassName = classfunctSplitParts.at(0);
        auto FunctionName = classfunctSplitParts.at(1);
		if (strcmp(object_type.first.c_str(),ClassName.c_str()) == 0) {
			auto objectInfo = object_type.second;
			for (std::uint32_t index = 0; index < objectInfo->GetNumGlobalFuncs(); ++index) {
				const auto globalFunct = objectInfo->GetGlobalFuncIter() + index;
				if (strcmp(globalFunct->func->GetName().c_str(), FunctionName.c_str()) == 0) {
					if (globalFunct->func->GetParamCount() == numArgs) {
						return globalFunct;
					}
				}
			}
		}
	}
	return nullptr;
}
MemberFunctInfoPtr myJSInstance::GetMemberFunction(BSScriptVmPtr impvm, std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs)
{
	for (const auto& object_type : impvm->objectTypeMap) {
		auto ClassName = classfunctSplitParts.at(0);
		auto FunctionName = classfunctSplitParts.at(1);
		if (strcmp(object_type.first.c_str(), ClassName.c_str()) == 0) {
			auto objectInfo = object_type.second;
			for (std::uint32_t index = 0; index < objectInfo->GetNumMemberFuncs(); ++index) {
				const auto globalFunct = objectInfo->GetMemberFuncIter() + index;
				if (strcmp(globalFunct->func->GetName().c_str(), FunctionName.c_str()) == 0) {
					if (globalFunct->func->GetParamCount() == numArgs) {
						return globalFunct;
					}
				}
			}
		}
	}
	return nullptr;
}
std::vector<RE::BSScript::TypeInfo> getFunctArgsBody(const RE::BSTSmartPointer<RE::BSScript::IFunction>& innerFunct)
{
    std::vector<RE::BSScript::TypeInfo> ParamData;
    for (uint32_t paramIndex = 0; paramIndex < innerFunct->GetParamCount(); ++paramIndex) {
        RE::BSFixedString paramName;
        RE::BSScript::TypeInfo paramType;
        innerFunct->GetParam(paramIndex, paramName, paramType);
        ParamData.push_back(paramType);
    }
    return ParamData;
}
RE::BSScript::IFunctionArguments* getArgumentsBody(std::vector<std::string>& args, std::vector<RE::BSScript::TypeInfo>& argvals)
{
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
        TypeHandling::HandleSingleValue(typeValOne, valStringOne, value1);
    }
    if (argvals.size() == 2) {
        TypeHandling::HandleTwoValues(typeValOne, typeValtwo, valStringOne, valStringTwo, value1);
    }
    if (argvals.size() == 3) {
        TypeHandling::HandleThreeValues(typeValOne, typeValtwo, typeValthree, valStringOne, valStringTwo, valStringThree, value1);
    }
    return value1;
}
template <class T>
std::vector<RE::BSScript::TypeInfo> GetFunctArgs(T* globalFunct)
{
    auto innerFunct = globalFunct->func;
    return getFunctArgsBody(innerFunct);
}

template<class T>
RE::BSScript::IFunctionArguments* ConvertArgs(T* globalFunct, std::vector<std::string> args)
{
	auto argvals = GetFunctArgs(globalFunct);
	return getArgumentsBody(args, argvals);
}
//these are the functions passed to the papyrus engine
void myJSInstance::CallGlobalFunction([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	std::vector<std::string> classfunctSplitParts = RemoveQuotesAndSplit(classfunct, '.');
	std::vector<std::string> functionArgs = RemoveQuotesAndSplit(arglist, ',');
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	const auto globalFunct = GetGlobalFunction(impvm, classfunctSplitParts, static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;
	const auto functargs = ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
}
void myJSInstance::CallInstanceFunction([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	std::vector<std::string> classfunctSplitParts = RemoveQuotesAndSplit(classfunct, '.');
	std::vector<std::string> functionArgs = RemoveQuotesAndSplit(arglist, ',');

	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	//grab form from vector
	const auto ObjectVmHandle = StringToVmHandle<RE::TESForm>(impvm, functionArgs.front());
	functionArgs.erase(functionArgs.begin());
	const auto globalFunct = GetMemberFunction(impvm, classfunctSplitParts, static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;
	const auto functargs = ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	impvm->DispatchMethodCall(ObjectVmHandle, globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
	//impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
}
