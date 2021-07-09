

std::vector<std::string> Splitter(const std::string& basetext, const char delim)
{
	std::string buf;				 // Have a buffer string
	std::stringstream ss(basetext);	 // Insert the string into a stream

	std::vector<std::string> tokens;  // Create vector to hold our words

	while (getline(ss, buf, delim))
		tokens.push_back(buf);
	return tokens;
}
std::string RemoveDoubleQuoteChars(const RE::BSFixedString& input) {
	std::string innerstring = input.c_str();
	innerstring.erase(remove(innerstring.begin(), innerstring.end(), '\"'), innerstring.end());
	return innerstring;
}
std::vector<std::string> RemoveQuotesAndSplit(const RE::BSFixedString& input,char splitterchar) {
	auto input2 = RemoveDoubleQuoteChars(input);
	auto splittedString = Splitter(input2,splitterchar);
	return splittedString;
}
void StringToObject(RE::BSScript::Internal::VirtualMachine* impvm, const std::string& formHex,const RE::BSScript::TypeInfo& TypeArg, RE::BSTSmartPointer<RE::BSScript::Object>& myObject)
{
    auto policy = impvm->GetObjectHandlePolicy();
    auto classPtr = RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> (TypeArg.GetTypeInfo());
    RE::BSTSmartPointer<RE::BSScript::Object> objectPtr;
    RE::BSTSmartPointer<RE::BSScript::Object> objectPtrO;
	auto TypeArgString = TypeArg.GetTypeInfo()->GetName();
    const RE::FormID Playerform = std::strtoul(formHex.c_str(), nullptr, 16);
    const auto FormRef = RE::TESForm::LookupByID(Playerform)->AsReference();
    const auto ObjectVmHandle = policy->GetHandleForObject(FormRef->GetFormType(), FormRef);
    impvm->FindBoundObject(ObjectVmHandle, TypeArgString, objectPtr);
	impvm->CastObject(objectPtr,classPtr,objectPtrO);
	myObject = objectPtrO;
}
