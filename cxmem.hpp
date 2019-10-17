#pragma once

#include <cstdlib>
#include <exception>
#include <utility>
#include <string>
#include <iterator>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <limits>

namespace mem {

    enum class mode {
        COPY,//Copies the data
        MOVE,//Moves as a T const&&
        //UB_MOVE,//moves as a R&&, const_cast undefined behavior
    };

    enum class rawset {
        NONE,
        ZERO,
    };

    constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    class memex : public std::exception {
        std::string _what;
    public:
        memex(std::string const& w) : _what(w) {}
        const char * what()const noexcept{ return _what.c_str(); }
    };

    /*template <typename T>
    T* alloc(std::size_t num){
        if(num==0)
            return nullptr;
        T* tmp = (T*)std::malloc(num*sizeof(T)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::alloc failed");
        *((std::size_t*)tmp) = num;
        tmp = (T*)(((std::size_t*)tmp)+1);
        for(std::size_t i = 0; i < num; i++){
            new (&tmp[i]) T{};
        }
        return tmp;
    }*/

    template <typename T, typename... Tx>
    T* alloc(std::size_t num, Tx const&... args){
        if(num==0)
            return nullptr;
        T* tmp = (T*)std::malloc(num*sizeof(T)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::alloc failed");
        *((std::size_t*)tmp) = num;
        tmp = (T*)(((std::size_t*)tmp)+1);
        for(std::size_t i = 0; i < num; i++){
            new (&tmp[i]) T{args...};
        }
        return tmp;
    }

    template <typename T, mode Mode = mode::COPY>
    T* alloc(std::initializer_list<T> il){
        using ilsize = typename std::initializer_list<T>::size_type;
        T* tmp = (T*)std::malloc(il.size()*sizeof(T)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::alloc failed");
        *((std::size_t*)tmp) = il.size();
        tmp = (T*)(((std::size_t*)tmp)+1);
        auto iter = il.begin();
        for(ilsize i = 0; i < il.size(); i++){
            if constexpr(Mode == mode::COPY){
                new (&tmp[i]) T{*(iter++)};
            }else if constexpr(Mode == mode::MOVE){
                //will only work for T const&& move constructors
                new (&tmp[i]) T{std::move(*(iter++))};
            }/*else{
                //Note that this is undefined behavior as
                //iterator lists may be placed in read-only
                //storage
                new (&tmp[i]) T{const_cast<T&&>(std::move(*(iter++)))};
            }*/
        }
        return tmp;
    }

    template <typename T>
    std::size_t size(T const * const arr){
        if(arr==nullptr)
            return 0;
        return *((std::size_t*)arr-1);
    }

    template <typename T>
    void dealloc(T*& arr){
        if(arr == nullptr)
            return;
        void* full_arr = ((std::size_t*)arr-1);
        std::size_t old = *((std::size_t*)arr-1);
        while(old>0){
            arr[--old].~T();
        }
        std::free(full_arr);
        arr = nullptr;
    }

    template <typename T>
    void shrink(T*& arr, std::size_t num){
        if(arr == nullptr || num == 0)
            return;
        std::size_t old = *((std::size_t*)arr-1);
        if((((std::intmax_t)old)-((std::intmax_t)num)) < 0){
            dealloc(arr);
            return;
        }
        void* full_arr = ((std::size_t*)arr-1);
        for(std::size_t i = old; i > old-num; --i){
            arr[i].~T();
        }
        T* tmp = (T*)std::realloc(full_arr,(old-num)*sizeof(T)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::shrink failed");
        *((std::size_t*)tmp) = old-num;
        arr = (T*)(((std::size_t*)tmp)+1);
    }

    template <typename T, typename... Tx>
    void grow(T*& arr, std::size_t num, Tx const&... args){
        if(arr == nullptr || num == 0)
            return;
        void* full_arr = ((std::size_t*)arr-1);
        std::size_t old = *((std::size_t*)arr-1);
        T* tmp = (T*)std::realloc(full_arr,(old+num)*sizeof(T)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::grow failed");
        *((std::size_t*)tmp) = old+num;
        arr = (T*)(((std::size_t*)tmp)+1);
        for(std::size_t i = old; i < old+num; i++){
            new (&arr[i]) T{args...};
        }
    }

    template <mode Mode = mode::COPY, typename T>
    void grow(T*& arr, std::initializer_list<T> il){
        if(arr == nullptr || il.size() == 0)
            return;
        void* full_arr = ((std::size_t*)arr-1);
        std::size_t old = *((std::size_t*)arr-1);
        T* tmp = (T*)std::realloc(full_arr,(old+il.size())*sizeof(T));
        if(!tmp)
            throw memex("mem::grow failed");
        *((std::size_t*)tmp) = old+il.size();
        arr = (T*)(((std::size_t*)tmp)+1);
        auto iter = il.begin();
        for(std::size_t i = old; i < old+il.size(); i++){
            if constexpr(Mode == mode::COPY){
                new (&tmp[i]) T{*(iter++)};
            }else if constexpr(Mode == mode::MOVE){
                //will only work for T const&& move constructors
                new (&tmp[i]) T{std::move(*(iter++))};
            }/*else{
                //Note that this is undefined behavior as
                //iterator lists may be placed in read-only
                //storage
                new (&tmp[i]) T{const_cast<T&&>(std::move(*(iter++)))};
            }*/
        }
    }

    template <typename T>
    void copy(T*& arr, T const * const src){
        std::size_t num = size(arr);
        for(std::size_t i = 0; i < num; i++){
            arr[i].~T();
            new (&arr[i]) T{src[i]};
        }
    }

    template <typename T>
    T* copy(T const * const arr){
        std::size_t num = size(arr);
        T* tmp = (T*)std::malloc(num*sizeof(T)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::copy failed");
        *((std::size_t*)tmp) = num;
        tmp = (T*)(((std::size_t*)tmp)+1);
        for(std::size_t i = 0; i < num; i++){
            new (&tmp[i]) T{arr[i]};
        }
        return tmp;
    }

    template <typename T, typename... Tx>
    void set(T*& arr, std::size_t pos, std::size_t num, Tx const&... args){
        if(arr == nullptr || num == 0)
            return;
        if(num == npos)
            num = size(arr);
        for(std::size_t i = pos; i < pos+num; i++){
            arr[i].~T();
            new (&arr[i]) T{args...};
        }
    }

    template <mode Mode = mode::COPY, typename T>
    void set(T*& arr, std::size_t pos, std::initializer_list<T> il){
        if(arr == nullptr || il.size() == 0)
            return;
        auto iter = il.begin();
        for(std::size_t i = pos; i < pos+il.size(); i++){
            arr[i].~T();
            if constexpr(Mode == mode::COPY){
                new (&arr[i]) T{*(iter++)};
            }else if constexpr(Mode == mode::MOVE){
                //will only work for T const&& move constructors
                new (&arr[i]) T{std::move(*(iter++))};
            }else{
                //Note that this is undefined behavior as
                //iterator lists may be placed in read-only
                //storage
                new (&arr[i]) T{const_cast<T&&>(std::move(*(iter++)))};
            }
        }
    }


    template <mode Mode = mode::COPY, typename T>
    void set(T*& arr, std::initializer_list<T> il){
        if(arr == nullptr || il.size() == 0)
            return;
        auto iter = il.begin();
        for(std::size_t i = 0; i < il.size(); i++){
            arr[i].~T();
            if constexpr(Mode == mode::COPY){
                new (&arr[i]) T{*(iter++)};
            }else if constexpr(Mode == mode::MOVE){
                //will only work for T const&& move constructors
                new (&arr[i]) T{std::move(*(iter++))};
            }else{
                //Note that this is undefined behavior as
                //iterator lists may be placed in read-only
                //storage
                new (&arr[i]) T{const_cast<T&&>(std::move(*(iter++)))};
            }
        }
    }

    template <typename T>
    void move(T*& arr, T* const src){
        std::size_t num = size(arr);
        for(std::size_t i = 0; i < num; i++){
            arr[i].~T();
            new (&arr[i]) T{std::move(src[i])};
        }
    }

    //void* specializations

    template <rawset RawSet = rawset::NONE>
    void* alloc(std::size_t bytes){
        if(bytes==0)
            return nullptr;
        //sizeof(void) is UB, but is usually expected to be 1 byte
        void* tmp;
        if constexpr(RawSet == rawset::NONE){
            tmp = std::malloc(bytes+sizeof(std::size_t));
        }else{
            tmp = std::calloc(bytes+sizeof(std::size_t),1);
        }
        if(!tmp)
            throw memex("mem::alloc<void*> failed");
        *((std::size_t*)tmp) = bytes;
        tmp = (void*)(((std::size_t*)tmp)+1);
        return tmp;
    }

    template<>
    void dealloc<void>(void*& rawarr){
        if(rawarr == nullptr)
            return;
        void* full_arr = ((std::size_t*)rawarr-1);
        std::free(full_arr);
        rawarr = nullptr;
    }

    template <>
    void shrink<void>(void*& rawarr, std::size_t num){
        if(rawarr == nullptr || num == 0)
            return;
        std::size_t old = *((std::size_t*)rawarr-1);
        if((((std::intmax_t)old)-((std::intmax_t)num)) < 0){
            dealloc(rawarr);
            return;
        }
        void* full_arr = ((std::size_t*)rawarr-1);
        //sizeof(void) is UB, but is usually expected to be 1 byte
        void* tmp = std::realloc(full_arr,(old-num)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::shrink<void*> failed");
        *((std::size_t*)tmp) = old-num;
        rawarr = (void*)(((std::size_t*)tmp)+1);
    }

    template <rawset RawSet = rawset::NONE>
    void grow(void*& arr, std::size_t num){
        if(arr == nullptr || num == 0)
            return;
        void* full_arr = ((std::size_t*)arr-1);
        std::size_t old = *((std::size_t*)arr-1);
        void* tmp = std::realloc(full_arr,(old+num)+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::grow<void*> failed");
        *((std::size_t*)tmp) = old+num;
        arr = (void*)(((std::size_t*)tmp)+1);
        if constexpr(RawSet == rawset::ZERO)
            std::memset(arr+old,0,num);
    }

    void copy(void*& arr, void const * const src){
        std::size_t num = size(arr);
        std::memcpy(arr,src,num);
    }

    void* copy(void const * const arr){
        std::size_t num = size(arr);
        void* tmp = std::malloc(num+sizeof(std::size_t));
        if(!tmp)
            throw memex("mem::copy<void*> failed");
        *((std::size_t*)tmp) = num;
        tmp = (void*)(((std::size_t*)tmp)+1);
        std::memcpy(tmp,arr,num);
        return tmp;
    }
}
