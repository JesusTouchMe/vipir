// Copyright 2023 solar-mist


#include "vipir/Module.h"

#include "vipir/IR/Global.h"

#include <sstream>
#include <format>

namespace vipir
{
    Module::Module(std::string name)
        :mName(std::move(name))
    {
    }

    std::string_view Module::getName() const
    {
        return mName;
    }

    const std::vector<GlobalPtr>& Module::getGlobals() const
    {
        return mGlobals;
    }

    void Module::insertGlobal(Global* global)
    {
        mGlobals.push_back(GlobalPtr(global));
    }

    void Module::print(std::ostream& stream) const
    {
        stream << std::format("file \"{}\"\n\n", mName);

        for (const GlobalPtr& global : mGlobals)
        {
            global->print(stream);
            stream << "\n\n";
        }
    }

    void Module::emit(std::ostream& stream) const
    {
        for (const GlobalPtr& global : mGlobals)
        {
            global->emit(stream);
            stream << "\n";
        }
    }
}