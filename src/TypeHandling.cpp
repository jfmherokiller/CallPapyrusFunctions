//
// Created by peter on 6/15/21.
//
#include "TypeHandling.hpp"
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
bool TypeHandling::HandleSingleValue(const RE::BSScript::TypeInfo& typeValOne, const std::string& valStringOne, RE::BSScript::IFunctionArguments*& value1)
{
    if (typeValOne.IsString()) {
        value1 = RE::MakeFunctionArguments<RE::BSFixedString>(BsString(valStringOne));
        return true;
    } else if (typeValOne.IsBool()) {
        value1 = RE::MakeFunctionArguments<bool>(StringToBool(valStringOne));
        return true;
    } else if (typeValOne.IsFloat()) {
        value1 = RE::MakeFunctionArguments<float>(StringToFloat(valStringOne));
        return true;
    } else if (typeValOne.IsInt()) {
        value1 = RE::MakeFunctionArguments<int>(StringToInt(valStringOne));
        return true;
    } else if (typeValOne.IsObject()) {
        auto TheTESForm = StringToForm<RE::TESForm>(valStringOne);
        if(TheTESForm->GetFormType() == RE::FormType::ActorCharacter) {
            value1 = RE::MakeFunctionArguments(TheTESForm->As<RE::TESActorBase>()->AsReference()->GetTemplateActorBase());
            return true;
        }
    }
    return false;
}
bool TypeHandling::HandleTwoValues(const RE::BSScript::TypeInfo& typeValOne, const RE::BSScript::TypeInfo& typeValtwo, const std::string& valStringOne, const std::string& valStringTwo, RE::BSScript::IFunctionArguments*& value1)
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
bool TypeHandling::HandleThreeValues(const RE::BSScript::TypeInfo& typeValOne, const RE::BSScript::TypeInfo& typeValtwo, const RE::BSScript::TypeInfo& typeValthree, const std::string& valStringOne, const std::string& valStringTwo, const std::string& valStringThree, RE::BSScript::IFunctionArguments*& value1)
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
