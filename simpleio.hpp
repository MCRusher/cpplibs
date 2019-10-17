#include <ios>
#include <iostream>
#include <string>
#include <exception>
#include <sstream>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <cctype>
#include <cwctype>

namespace io {
    class ioex : public std::exception {
        std::string _what;
    public:
        ioex(std::string const& w) : _what(w) {}
        const char * what()const noexcept{ return _what.c_str(); }
    };

    enum class mode {
        RETRY,
        THROW,
    };

    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::ostream, T1>>
    println(T1 const& arg, Tx const&... args){
        ((std::cout << arg) << ... << args) << std::endl;
    }
    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::ostream, T1>>
    print(T1 const& arg, Tx const&... args){
        ((std::cout << arg) << ... << args) << std::flush;
    }
    template<typename... Tx>
    void println(std::ostream& ostrm, Tx const&... args){
        (ostrm << ... << args) << std::endl;
    }
    template<typename... Tx>
    void print(std::ostream& ostrm, Tx const&... args){
        (ostrm << ... << args) << std::flush;
    }

    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::wostream, T1>>
    wprintln(T1 const& arg, Tx const&... args){
        ((std::wcout << arg) << ... << args) << std::endl;
    }
    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::wostream, T1>>
    wprint(T1 const& arg, Tx const&... args){
        ((std::wcout << arg) << ... << args) << std::flush;
    }
    template<typename... Tx>
    void wprintln(std::wostream& wostrm, Tx const&... args){
        (wostrm << ... << args) << std::endl;
    }
    template<typename... Tx>
    void print(std::wostream& wostrm, Tx const&... args){
        (wostrm << ... << args) << std::flush;
    }

    //input

    std::string readln(){
        std::string res;
        std::getline(std::cin,res);
        return res;
    }
    std::string readln(std::istream& istrm){
        std::string res;
        std::getline(istrm,res);
        return res;
    }

    void readln(std::string& res){
        std::getline(std::cin,res);
    }
    void readln(std::istream& istrm, std::string& res){
        std::getline(istrm,res);
    }

    template<mode Mode, typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::istream, T1>>
    readln(T1& arg, Tx&... args){
        std::string tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(std::cin,tmp);
            std::istringstream res(tmp);
            if(!(res >> arg)){
                std::ostringstream buf;
                buf << "readln(std::cin,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            std::getline(std::cin,tmp);
            std::istringstream res(tmp);
            while(!(res >> arg)){
                res.clear();
                std::getline(std::cin,tmp);
                res.str(tmp);
            }
        }

        if constexpr(sizeof...(args)!=0)
            readln<Mode>(args...);
    }
    template<mode Mode, typename... Tx>
    void readln(bool& arg, Tx&... args){
        auto eqnocase = [](std::string const& s1, std::string const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](char const c1, char const c2){
                    return std::tolower(c1) == std::tolower(c2);
                });
        };
        std::string tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(std::cin,tmp);
            if(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                std::ostringstream buf;
                buf << "readln(std::cin,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }else{
            std::getline(std::cin,tmp);
            while(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                std::getline(std::cin,tmp);
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }

        if constexpr(sizeof...(args)!=0)
            readln<Mode>(args...);
    }
    template<mode Mode, typename T1, typename... Tx>
    void readln(std::istream& istrm, T1& arg, Tx&... args){
        std::string tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(istrm,tmp);
            std::istringstream res(tmp);
            if(!(res >> arg)){
                std::ostringstream buf;
                buf << "readln(std::istream,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            std::getline(istrm,tmp);
            std::istringstream res(tmp);
            while(!(res >> arg)){
                res.clear();
                std::getline(istrm,tmp);
                res.str(tmp);
            }
        }

        if constexpr(sizeof...(args)!=0)
            readln<Mode>(istrm,args...);
    }
    template<mode Mode, typename... Tx>
    void readln(std::istream& istrm, bool& arg, Tx&... args){
        auto eqnocase = [](std::string const& s1, std::string const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](char const c1, char const c2){
                    return std::tolower(c1) == std::tolower(c2);
                });
        };
        std::string tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(istrm,tmp);
            if(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                std::ostringstream buf;
                buf << "readln(std::istream,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }else{
            std::getline(istrm,tmp);
            while(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                std::getline(istrm,tmp);
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }

        if constexpr(sizeof...(args)!=0)
            readln<Mode>(istrm,args...);
    }

    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::istream, T1>>
    readln(T1& arg, Tx&... args){
        readln<mode::RETRY>(arg,args...);
    }
    template<typename T1, typename... Tx>
    void readln(std::istream& istrm, T1& arg, Tx&... args){
        readln<mode::RETRY>(istrm,arg,args...);
    }

    char readchar(){
        char c = std::cin.get();
        if(!std::cin)
            throw ioex("io::readchar(std::cin) failed");
        return c;
    }
    char readchar(std::istream& istrm){
        char c = istrm.get();
        if(!istrm)
            throw ioex("io::readchar(std::istream) failed");
        return c;
    }

    std::string read(){
        std::string res;
        std::cin >> res;
        std::cin.ignore();
        return res;
    }
    std::string read(std::istream& istrm){
        std::string res;
        istrm >> res;
        istrm.ignore();
        return res;
    }

    void read(std::string& res){
        std::cin >> res;
        std::cin.ignore();
    }
    void read(std::istream& istrm, std::string& res){
        istrm >> res;
        istrm.ignore();
    }

    template<mode Mode, typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::istream, T1>>
    read(T1& arg, Tx&... args){
        if constexpr(Mode == mode::THROW){
            if(!(std::cin >> arg)){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "read(std::cin,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            while(!(std::cin >> arg)){
                //clears the entire line and resets stream state
                if(!std::cin){
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                }
            }
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            read<Mode>(args...);
        else
            std::cin.ignore();
    }
    template<mode Mode, typename... Tx>
    void read(bool& arg, Tx&... args){
        auto eqnocase = [](std::string const& s1, std::string const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](char const c1, char const c2){
                    return std::tolower(c1) == std::tolower(c2);
                });
        };
        std::string tmp;
        if constexpr(Mode == mode::THROW){
            std::cin >> tmp;
            if(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "read(std::cin,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }else{
            std::cin >> tmp;
            while(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                //clears the entire line and resets stream state
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::cin >> tmp;
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            read<Mode>(args...);
        else
            std::cin.ignore();
    }
    template<mode Mode, typename T1, typename... Tx>
    void read(std::istream& istrm, T1& arg, Tx&... args){
        if constexpr(Mode == mode::THROW){
            if(!(istrm >> arg)){
                istrm.clear();
                istrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "read(std::istream,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            while(!(istrm >> arg)){
                //clears the entire line and resets stream state
                if(!istrm){
                    istrm.clear();
                    istrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                }
            }
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            read<Mode>(istrm,args...);
        else
            istrm.ignore();
    }
    template<mode Mode, typename... Tx>
    void read(std::istream& istrm, bool& arg, Tx&... args){
        auto eqnocase = [](std::string const& s1, std::string const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](char const c1, char const c2){
                    return std::tolower(c1) == std::tolower(c2);
                });
        };
        std::string tmp;
        if constexpr(Mode == mode::THROW){
            istrm >> tmp;
            if(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                istrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "read(std::istream,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }else{
            istrm >> tmp;
            while(!eqnocase(tmp,"true") && !eqnocase(tmp,"false")){
                //clears the entire line and resets stream state
                istrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                istrm >> tmp;
            }
            arg = (eqnocase(tmp,"true") ? true : false);
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            read<Mode>(istrm,args...);
        else
            istrm.ignore();
    }

    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::istream, T1>>
    read(T1& arg, Tx&... args){
        read<mode::RETRY>(arg,args...);
    }
    template<typename T1, typename... Tx>
    void read(std::istream& istrm, T1& arg, Tx&... args){
        read<mode::RETRY>(istrm,arg,args...);
    }

    //wide read versions

    std::wstring wreadln(){
        std::wstring res;
        std::getline(std::wcin,res);
        return res;
    }
    std::wstring wreadln(std::wistream& wistrm){
        std::wstring res;
        std::getline(wistrm,res);
        return res;
    }

    void wreadln(std::wstring& res){
        std::getline(std::wcin,res);
    }
    void wreadln(std::wistream& wistrm, std::wstring& res){
        std::getline(wistrm,res);
    }

    template<mode Mode, typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::wistream, T1>>
    wreadln(T1& arg, Tx&... args){
        std::wstring tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(std::wcin,tmp);
            std::wistringstream res(tmp);
            if(!(res >> arg)){
                std::ostringstream buf;
                buf << "wreadln(std::wcin,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            std::getline(std::wcin,tmp);
            std::wistringstream res(tmp);
            while(!(res >> arg)){
                res.clear();
                std::getline(std::wcin,tmp);
                res.str(tmp);
            }
        }

        if constexpr(sizeof...(args)!=0)
            wreadln<Mode>(args...);
    }
    template<mode Mode, typename... Tx>
    void wreadln(bool& arg, Tx&... args){
        auto weqnocase = [](std::wstring const& s1, std::wstring const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](wchar_t const c1, wchar_t const c2){
                    return std::towlower(c1) == std::towlower(c2);
                });
        };
        std::wstring tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(std::wcin,tmp);
            if(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                std::ostringstream buf;
                buf << "wreadln(std::wistream,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (weqnocase(tmp,"true") ? true : false);
        }else{
            std::getline(std::wcin,tmp);
            while(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                std::getline(std::wcin,tmp);
            }
            arg = (weqnocase(tmp,L"true") ? true : false);
        }

        if constexpr(sizeof...(args)!=0)
            wreadln<Mode>(args...);
    }
    template<mode Mode, typename T1, typename... Tx>
    void wreadln(std::wistream& wistrm, T1& arg, Tx&... args){
        std::wstring tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(wistrm,tmp);
            std::wistringstream res(tmp);
            if(!(res >> arg)){
                std::ostringstream buf;
                buf << "wreadln(std::wistream,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            std::getline(wistrm,tmp);
            std::wistringstream res(tmp);
            while(!(res >> arg)){
                res.clear();
                std::getline(wistrm,tmp);
                res.str(tmp);
            }
        }

        if constexpr(sizeof...(args)!=0)
            wreadln<Mode>(wistrm,args...);
    }
    template<mode Mode, typename... Tx>
    void wreadln(std::wistream& wistrm, bool& arg, Tx&... args){
        auto weqnocase = [](std::wstring const& s1, std::wstring const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](wchar_t const c1, wchar_t const c2){
                    return std::towlower(c1) == std::towlower(c2);
                });
        };
        std::wstring tmp;
        if constexpr(Mode == mode::THROW){
            std::getline(wistrm,tmp);
            if(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                std::ostringstream buf;
                buf << "wreadln(std::wistream,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (weqnocase(tmp,"true") ? true : false);
        }else{
            std::getline(wistrm,tmp);
            while(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                std::getline(wistrm,tmp);
            }
            arg = (weqnocase(tmp,L"true") ? true : false);
        }

        if constexpr(sizeof...(args)!=0)
            wreadln<Mode>(wistrm,args...);
    }
    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::wistream, T1>>
    wreadln(T1& arg, Tx&... args){
        wreadln<mode::RETRY>(arg,args...);
    }
    template<typename T1, typename... Tx>
    void wreadln(std::wistream& wistrm, T1& arg, Tx&... args){
        wreadln<mode::RETRY>(wistrm,arg,args...);
    }

    char wreadchar(){
        wchar_t c = std::wcin.get();
        if(!std::wcin)
            throw ioex("io::wreadchar(std::wcin) failed");
        return c;
    }
    char wreadchar(std::wistream& wistrm){
        char c = wistrm.get();
        if(!wistrm)
            throw ioex("io::readchar(std::wistream) failed");
        return c;
    }

    std::wstring wread(){
        std::wstring res;
        std::wcin >> res;
        std::wcin.ignore();
        return res;
    }
    std::wstring wread(std::wistream& wistrm){
        std::wstring res;
        wistrm >> res;
        wistrm.ignore();
        return res;
    }

    void wread(std::wstring& res){
        std::wcin >> res;
        std::wcin.ignore();
    }
    void wread(std::wistream& wistrm, std::wstring& res){
        wistrm >> res;
        wistrm.ignore();
    }

    template<mode Mode, typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::wistream, T1>>
    wread(T1& arg, Tx&... args){
        if constexpr(Mode == mode::THROW){
            if(!(std::wcin >> arg)){
                std::wcin.clear();
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "wread(std::wcin,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            while(!(std::wcin >> arg)){
                //clears the entire line and resets stream state
                if(!std::wcin){
                    std::wcin.clear();
                    std::wcin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                }
            }
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            wread<Mode>(args...);
        else
            std::wcin.ignore();
    }
    template<mode Mode, typename... Tx>
    void wread(bool& arg, Tx&... args){
        auto weqnocase = [](std::wstring const& s1, std::wstring const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](wchar_t const c1, wchar_t const c2){
                    return std::towlower(c1) == std::towlower(c2);
                });
        };
        std::wstring tmp;
        if constexpr(Mode == mode::THROW){
            std::wcin >> tmp;
            if(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "read(std::wistream,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (weqnocase(tmp,L"true") ? true : false);
        }else{
            std::wcin >> tmp;
            while(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                //clears the entire line and resets stream state
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::wcin >> tmp;
            }
            arg = (weqnocase(tmp,L"true") ? true : false);
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            wread<Mode>(args...);
        else
            std::wcin.ignore();
    }
    template<mode Mode, typename T1, typename... Tx>
    void wread(std::wistream& wistrm, T1& arg, Tx&... args){
        if constexpr(Mode == mode::THROW){
            if(!(wistrm >> arg)){
                wistrm.clear();
                wistrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "wread(std::wistream,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
        }else{
            while(!(wistrm >> arg)){
                //clears the entire line and resets stream state
                if(!wistrm){
                    wistrm.clear();
                    wistrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                }
            }
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            wread<Mode>(wistrm,args...);
        else
            wistrm.ignore();
    }
    template<mode Mode, typename... Tx>
    void wread(std::wistream& wistrm, bool& arg, Tx&... args){
        auto weqnocase = [](std::wstring const& s1, std::wstring const& s2){
            return std::equal(
                s1.begin(),s1.end(),
                s2.begin(),s2.end(),
                [](wchar_t const c1, wchar_t const c2){
                    return std::towlower(c1) == std::towlower(c2);
                });
        };
        std::wstring tmp;
        if constexpr(Mode == mode::THROW){
            wistrm >> tmp;
            if(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                wistrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                std::ostringstream buf;
                buf << "read(std::wistream,bool,...) failed for typeid \""
                << typeid(arg).name()
                << "\" with "
                << sizeof...(args)
                << " other arguments remaining.";
                throw ioex(buf.str());
            }
            arg = (weqnocase(tmp,L"true") ? true : false);
        }else{
            wistrm >> tmp;
            while(!weqnocase(tmp,L"true") && !weqnocase(tmp,L"false")){
                //clears the entire line and resets stream state
                wistrm.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                wistrm >> tmp;
            }
            arg = (weqnocase(tmp,L"true") ? true : false);
        }
        //the template recursive base case
        if constexpr(sizeof...(args)!=0)
            wread<Mode>(wistrm,args...);
        else
            wistrm.ignore();
    }

    template<typename T1, typename... Tx>
    std::enable_if_t<!std::is_base_of_v<std::wistream, T1>>
    wread(T1& arg, Tx&... args){
        wread<mode::RETRY>(arg,args...);
    }
    template<typename T1, typename... Tx>
    void wread(std::wistream& wistrm, T1& arg, Tx&... args){
        wread<mode::RETRY>(wistrm,arg,args...);
    }
}
