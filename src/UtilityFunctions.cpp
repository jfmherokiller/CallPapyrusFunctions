std::vector<std::string> Splitter(const std::string& basetext, const char delim)
{
	std::string buf;				 // Have a buffer string
	std::stringstream ss(basetext);	 // Insert the string into a stream

	std::vector<std::string> tokens;  // Create vector to hold our words

	while (getline(ss, buf, delim))
		tokens.push_back(buf);
	return tokens;
}
std::string RemoveDoubleQuoteChars(RE::BSFixedString input) {
	std::string innerstring = input.c_str();
	innerstring.erase(remove(innerstring.begin(), innerstring.end(), '\"'), innerstring.end());
	return innerstring;
}
std::vector<std::string> RemoveQuotesAndSplit(RE::BSFixedString input,char splitterchar) {
	auto input2 = RemoveDoubleQuoteChars(input);
	auto splittedString = Splitter(input2,splitterchar);
	return splittedString;
}
template <typename T>
T* StringToForm(const std::string& formHex)
{
	const RE::FormID Playerform = std::strtoul(formHex.c_str(), nullptr, 16);
	const auto RefTesting2 = RE::TESForm::LookupByID<T>(Playerform);
	return RefTesting2;
}
template <typename T>
RE::VMHandle StringToVmHandle(RE::BSScript::Internal::VirtualMachine* impvm,const std::string& formHex)
{
	auto policy = impvm->GetObjectHandlePolicy();
	const RE::FormID Playerform = std::strtoul(formHex.c_str(), nullptr, 16);
	const auto RefTesting2 = RE::TESForm::LookupByID<T>(Playerform);
	const auto ObjectVmHandle = policy->GetHandleForObject(RefTesting2->GetFormType(), RefTesting2);
	return ObjectVmHandle;
}