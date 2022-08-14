#pragma once
#include <iostream>
#include <memory>
/**
 * Any类型 实现
 *  打包到派生类中
*/
class Any
{
public:
    Any() = default;
	~Any() = default;
	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;

    	// 这个构造函数可以让Any类型接收任意其它的数据
	template<typename T>  // T:int    Derive<int>
	Any(T data) : base_(std::make_shared<Derive<T>>(data))
	{

    }

	// 这个方法能把Any对象里面存储的data数据提取出来
	template<typename T>
	T cast()
	{
		// 我们怎么从base_找到它所指向的Derive对象，从它里面取出data成员变量
		// 基类指针 =》 派生类指针   RTTI
		Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
		if (pd == nullptr)
		{
			throw "type is unmatch!";
		}
		return pd->data_;
	}

private:
    //基类类型
    class Base
    {
    public:
        virtual ~Base()= default;;
    
    };
    //派生类类型
    template<typename T>
    class Derive:public Base
    {
    public:
     Derive(T data):data_(data)
     {
        
     }
     T data_;//保存了任意的其他类型
    };
private:
    std::shared_ptr<Base> base_;
};

