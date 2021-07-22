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
		auto ObjectClassName = std::string(object_type.first.c_str());
		if ((ObjectClassName == ClassName) || ObjectClassName.starts_with(ClassName)) {
			auto objectInfo = object_type.second;
			for (std::uint32_t index = 0; index < objectInfo->GetNumGlobalFuncs(); ++index) {
				const auto globalFunct = objectInfo->GetGlobalFuncIter() + index;
				const auto globalFunctName = std::string(globalFunct->func->GetName().c_str());
				if ((FunctionName == globalFunctName) || (globalFunctName.starts_with(FunctionName))) {
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
void myJSInstance::CallInstanceFunction([[maybe_unused]] RE::StaticFunctionTag* aaa,RE::BSFixedString Instance, RE::BSFixedString classfunct, RE::BSFixedString arglist)
{
	std::vector<std::string> classfunctSplitParts = RemoveQuotesAndSplit(classfunct, '.');
	std::vector<std::string> functionArgs = RemoveQuotesAndSplit(arglist, ',');
	auto impvm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	auto actualInststring = RemoveDoubleQuoteChars(Instance);
	RE::BSScript::ObjectTypeInfo* FoundType = nullptr;
	RE::VMHandle ObjectVmHandle = StringToVmHandle<RE::TESForm>(impvm, actualInststring);
	for (const auto& item : impvm->attachedScripts) {
		if(item.first == ObjectVmHandle) {
			for (const auto& itemtwo : item.second) {
				auto ScriptName = boost::algorithm::to_lower_copy(std::string(itemtwo->GetTypeInfo()->GetName()));
				if((ClassNamePart == ScriptName)|| (ScriptName.starts_with(ClassNamePart))) {
					FoundType = itemtwo->GetTypeInfo();
				}
			}
		}
	}
	const auto globalFunct = GetMemberFunctionT(FoundType, FunctionNamePart, static_cast<std::uint32_t>(functionArgs.size()));
	if (globalFunct == nullptr)
		return;
	const auto functargs = ConvertArgs(globalFunct, functionArgs);
	RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> aaaclass;
	auto classname = globalFunct->func->GetObjectTypeName();
	auto functionname = globalFunct->func->GetName();
	impvm->DispatchMethodCall(ObjectVmHandle, classname, functionname, functargs, aaaclass);
	//impvm->DispatchStaticCall(globalFunct->func->GetObjectTypeName(), globalFunct->func->GetName(), functargs, aaaclass);
}
MemberFunctInfoPtr myJSInstance::GetMemberFunctionT(RE::BSScript::ObjectTypeInfo* pInfo, const std::string& FunctionPart, uint32_t NumerOfArgs)
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
