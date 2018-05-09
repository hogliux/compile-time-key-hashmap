//
//  main.cpp
//  HashMap
//
//  Created by Fabian Renn-Giles on 04/05/2018.
//  Copyright © 2018 Fabian Renn-Giles. All rights reserved.
//

#include <iostream>
#include <unordered_map>

#define IDENTIFIER(x) \
    [] { return x; }

template <auto... x> struct CompileTimeValue { static constexpr char data[] = {x...}; };

template <typename, typename> struct ConcatCompileTimeValue {};
template <char... a, char... b>
struct ConcatCompileTimeValue<CompileTimeValue<a...>, CompileTimeValue<b...>>
{
    using type = CompileTimeValue<a..., b...>;
};

// Notes:
// [1]  just using IDENTIFIER's type directly as C++ will create a new lambda with a destinct
//      type everytime IDENTIFIER is used - even if the lambda is identical. That's why we need
//      to c-string

//==============================================================================
template <typename Identifier, int N = 0>
constexpr auto identifier2type (Identifier identifier)
{
    constexpr auto str = identifier();
    if constexpr (str[N] == 0)
        return CompileTimeValue<>{};
    else
        return typename ConcatCompileTimeValue<CompileTimeValue<str[N]>, decltype (identifier2type<Identifier, N+1> (identifier))>::type {};
}

//==============================================================================
template <typename KeyType, typename ValueType, typename Tag = void>
class CompileTimeMap
{
public:
    CompileTimeMap() = delete;
    
    static ValueType& get_runtime (const KeyType& identifier)
    {
        return getRuntimeMap()[identifier]();
    }
    
    template <typename T>
    static ValueType& get (T identifier)
    {
        using CompileTimeValueType = decltype (identifier2type (identifier));
        return Holder<CompileTimeValueType>::get();
    }
    
    
    //constexpr const ValueType& get (const char )
private:
    //==============================================================================
    template <typename CompileTimeValueType>
    struct Holder
    {
        Holder ()
        {
            KeyType key (CompileTimeValueType::data, sizeof (CompileTimeValueType::data));
            getRuntimeMap()[key] = get;
        }
        
        static ValueType& get()
        {
            static Holder<CompileTimeValueType> v;
            return v.storage;
        }
        
        ValueType storage;
    };
    
    static std::unordered_map<KeyType, ValueType& (*)()>& getRuntimeMap()
    {
        static std::unordered_map<KeyType, ValueType& (*)()> map;
        return map;
    }
};


int main(int argc, const char * argv[]) {
    CompileTimeMap<std::string, int>::get (IDENTIFIER ("hello")) = 5;
    CompileTimeMap<std::string, int>::get (IDENTIFIER ("heyho")) = 6;
    
    std::cout << CompileTimeMap<std::string, int>::get (IDENTIFIER ("hello")) << std::endl;
    std::cout << CompileTimeMap<std::string, int>::get (IDENTIFIER ("heyho")) << std::endl;
    
    std::cout << CompileTimeMap<std::string, int>::get_runtime ("hello") << std::endl;
    std::cout << CompileTimeMap<std::string, int>::get_runtime ("heyho") << std::endl;
   
    return 0;
}
