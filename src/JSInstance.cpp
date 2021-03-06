myJSInstance::myJSInstance() = default;

bool myJSInstance::RegisterFuncts(BSScriptVmPtr a_registry)
{
	auto Classname = "CallGlobalFuncts";
	a_registry->RegisterFunction("CallGlobalFunction", Classname, CallGlobalFunction);
	a_registry->RegisterFunction("CallInstanceFunction", Classname, CallInstanceFunction);
	a_registry->RegisterFunction("CallGlobalFunctionAsync",Classname, CallGlobalFunctionAsync);
	a_registry->RegisterFunction("CallInstanceFunctionAsync",Classname, CallInstanceFunctionAsync);
	return true;
}

MemberFunctInfoPtr myJSInstance::GetMemberFunctionT(const RE::BSScript::ObjectTypeInfo* pInfo, const std::string& FunctionPart, uint32_t NumerOfArgs)
{
    for (std::uint32_t index = 0; index < pInfo->GetNumMemberFuncs(); ++index) {
        const auto globalFunct = pInfo->GetMemberFuncIter() + index;
        const auto globalFunctName = std::string(globalFunct->func->GetName().c_str());
        if ((FunctionPart == globalFunctName) || (globalFunctName.starts_with(FunctionPart))) {
            if (globalFunct->func->GetParamCount() == NumerOfArgs) {
                return globalFunct;
            }
        }
    }
    return nullptr;
}

globalFunctInfoPtr myJSInstance::GetGlobalFunction(std::vector<std::string> classfunctSplitParts, std::uint32_t numArgs)
{
    auto ClassName = classfunctSplitParts.at(0);
    auto FunctionName = classfunctSplitParts.at(1);
	auto calculatedClass = GetClassFromName(ClassName);
	if(calculatedClass == nullptr) {
		return nullptr;
	}
    for (std::uint32_t index = 0; index < calculatedClass->GetNumGlobalFuncs(); ++index) {
        const auto globalFunct = calculatedClass->GetGlobalFuncIter() + index;
        const auto globalFunctName = std::string(globalFunct->func->GetName().c_str());
        if ((FunctionName == globalFunctName) || (globalFunctName.starts_with(FunctionName))) {
            if (globalFunct->func->GetParamCount() == numArgs) {
                return globalFunct;
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
	return MakeFunctionArgs::getArgumentsBody(args, argvals);
}
//these are the functions passed to the papyrus engine
void myJSInstance::CallGlobalFunctionAsync([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	call_async([=] {
		myJSInstance::CallGlobalFunction(aaa, classfunct, arglist);
	});
}

void myJSInstance::CallGlobalFunction([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	std::vector<std::string> classfunctSplitParts = RemoveQuotesAndSplit(classfunct, '.');
	std::vector<std::string> functionArgs = RemoveQuotesAndSplit(arglist, ',');
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	const auto globalFunct = GetGlobalFunction( classfunctSplitParts, static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;

	const auto functargs = ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
}
void myJSInstance::CallInstanceFunctionAsync([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString Instance, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	call_async([=] {
		CallInstanceFunction(aaa, Instance, classfunct, arglist);
	});
}
void myJSInstance::CallInstanceFunction([[maybe_unused]] RE::StaticFunctionTag* aaa, RE::BSFixedString Instance, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	std::vector<std::string> classfunctSplitParts = RemoveQuotesAndSplit(classfunct, '.');
	std::vector<std::string> functionArgs = RemoveQuotesAndSplit(arglist, ',');
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	auto actualInststring = RemoveDoubleQuoteChars(Instance);
	auto ClassNamePart = boost::algorithm::to_lower_copy(classfunctSplitParts.at(0));

	RE::VMHandle ObjectVmHandle = StringToVmHandle<RE::TESForm>(impvm, actualInststring);
	const auto mattached_scripts = GetAttachedScriptsFromVmHandle(impvm,ObjectVmHandle);
	const RE::BSScript::ObjectTypeInfo* FoundType = getDesiredTypeInfoFromScripts(ClassNamePart,mattached_scripts);
	const auto globalFunct = GetMemberFunctionT(FoundType, classfunctSplitParts.at(1), static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;

	const auto functargs = ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	auto classname = globalFunct->func->GetObjectTypeName();
	auto functionname = globalFunct->func->GetName();
	impvm->DispatchMethodCall(ObjectVmHandle, classname, functionname, functargs, aaaclass);
	//impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
}
