//
// Created by peter on 7/8/21.
//

#pragma once

namespace MakeFunctionArgs
{
    class FunctionArguments: public RE::BSScript::IFunctionArguments
    {
    private:
        RE::BSScrapArray<RE::BSScript::Variable> m_Variables;

    public:
        FunctionArguments() noexcept = default;
        FunctionArguments(size_t cappacity)
        {
            m_Variables.reserve(cappacity);
        }
        FunctionArguments(const std::vector<std::string>& args, const std::vector<RE::BSScript::TypeInfo>& argTypes)
        {
            assert(args.size() == argTypes.size());

            using namespace RE::BSScript;
            using RawType = TypeInfo::RawType;

            m_Variables.reserve(argTypes.size());
            for (size_t i = 0; i < args.size(); i++)
            {
                const auto& arg = args[i];
                const auto& argType = argTypes[i];

                std::optional<Variable> scriptVariable;
                switch (argType.GetRawType())
                {
                case RawType::kObject:
                {
                    // TODO
                    break;
                }
                case RawType::kString:
                {
                    scriptVariable.emplace();
                    scriptVariable->SetString(arg);

                    break;
                }
                case RawType::kInt:
                {
                    scriptVariable.emplace();
                    scriptVariable->SetSInt(std::stoi(arg));

                    break;
                }
                case RawType::kFloat:
                {
                    scriptVariable.emplace();
                    scriptVariable->SetFloat(std::stof(arg));

                    break;
                }
                case RawType::kBool:
                {
                    scriptVariable.emplace();
                    scriptVariable->SetBool(arg == "1" || arg == "true" || arg == "TRUE");

                    break;
                }
				case RawType::kNone:
				case RawType::kNoneArray:
				case RawType::kObjectArray:
				case RawType::kStringArray:
				case RawType::kIntArray:
				case RawType::kFloatArray:
				case RawType::kBoolArray:
				case RawType::kArraysEnd:
					break;
				};

                if (scriptVariable)
                {
                    m_Variables.emplace_back(std::move(*scriptVariable));
                }
            }
        }
        ~FunctionArguments() noexcept = default;

    public:
        void PushVariable(RE::BSScript::Variable variable)
        {
            m_Variables.emplace_back(std::move(variable));
        }

    public:
        bool operator()(RE::BSScrapArray<RE::BSScript::Variable>& destination) const override
        {
            destination = m_Variables;
            return true;
        }
    };

    inline RE::BSScript::IFunctionArguments* getArgumentsBody(std::vector<std::string>& args, std::vector<RE::BSScript::TypeInfo>& argTypes)
    {
        return new FunctionArguments(args, argTypes);
    }
}


