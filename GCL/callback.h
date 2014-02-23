/*****************************************************************************
*                           gcl - callback.h                                 *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*****************************************************************************/
#ifndef CALLBACK_H
#define CALLBACK_H
#if _MSC_VER
   #pragma once
#endif
#include "stdafx.h"
#ifdef __cplusplus

namespace gcl {

template <typename T>
class  mem_func_ptr;

template <typename _ret_type, typename... _args>
class  mem_func_ptr<_ret_type(_args...)>
{
private:
	template <typename _ret_type, typename... _args>
	class storage_base
	{
	public:
		virtual ~storage_base() {};
		virtual void stored_func(const _args&...) = 0;
		virtual bool operator== (storage_base* base) = 0;
	};

	template <typename _class, typename _ret_type, typename... _args>
	class storage : 
		public storage_base<_ret_type, _args...>
	{
	public:
		typedef _ret_type(_class::*func_ptr)(_args...);
		typedef _class stored_class;
		storage(_class* class_ptr, const func_ptr& _func_ptr) : instance(class_ptr), mem_func(_func_ptr) {};
		void stored_func(const _args&... args)
		{
			(instance->*mem_func)(args...);
		};
		bool operator== (storage_base* base)
		{
			auto act = dynamic_cast<storage<_class, _ret_type, _args...>*>(base);
			if(!act) return false;
			if(act->get_mem_func() == mem_func && act->get_instance() == instance)
				return true;
			return false;
		};
		_class* get_instance() const {return instance;};
		const func_ptr& get_mem_func() const {return mem_func;};
	private:
		_class* instance;
		func_ptr mem_func;
	};
public:
	template <class _class>
	mem_func_ptr(_class* class_ptr, _ret_type (_class::*mem_func) (_args...)) : func_ptr(new storage<_class, _ret_type, _args...>(class_ptr, mem_func))  {};
	void operator() (_args... args)
	{
		func_ptr->stored_func(args...);
	};
	shared_ptr<storage_base<_ret_type, _args...>> get_func() const {return func_ptr;};
private:
	shared_ptr<storage_base<_ret_type, _args...>> func_ptr;
};

// Creates a storage for non static Memberfunctions
template <typename _class, typename _ret_type, typename... _args> 
const mem_func_ptr<_ret_type(_args...)>
	make_func_ptr(_class* class_ptr, _ret_type (_class::*func_ptr) (_args...)) {return mem_func_ptr<_ret_type(_args...)>(class_ptr, func_ptr);}

template <typename T>
class  callback;

template <typename _ret_type, typename... _args>
class  callback<_ret_type(_args...)>
{
	typedef typename _ret_type return_type;
	typedef return_type (*func_ptr) (_args...);
	typedef typename callback<_ret_type(_args...)> type;
	static const int arg_count = sizeof...(_args);
public:
	// Adds a delegate to the callback
	void operator+= (func_ptr ptr)
	{
		prv_coll.push_back(ptr);
	};
	// Adds a delegate to the callback
	void operator+= (const mem_func_ptr<_ret_type(_args...)>& mem_ptr)
	{
		prv_mem_coll.push_back(mem_ptr);
	};
	// Removes a delegate to the callback
	void operator-= (func_ptr ptr)
	{
		prv_coll.erase(std::remove(prv_coll.begin(), prv_coll.end(), ptr));
	};
	// Removes a delegate to the callback
	void operator-= (const mem_func_ptr<_ret_type(_args...)>& mem_ptr)
	{
		for(unsigned i = 0; i < prv_mem_coll.size(); i++)
		{
			if((*(prv_mem_coll[i].get_func().get())) == mem_ptr.get_func().get())
				prv_mem_coll.erase(prv_mem_coll.begin() + i);
		}
	};
	// Calls all bound delegates
	void operator() (const _args&... args)
	{
		for(auto& itr = prv_coll.begin(); itr < prv_coll.end(); ++itr)
			(*itr)(args...);
		for(auto& itr = prv_mem_coll.begin(); itr < prv_mem_coll.end(); ++itr)
			(*itr)(args...);
	};
	// Releases all bound delegates
	void release_hooks() {prv_coll.clear(); prv_mem_coll.clear();};
private:
	vector<func_ptr> prv_coll;
	vector<mem_func_ptr<return_type(_args...)>> prv_mem_coll;
};

}; // end namespace

#endif // cpp
#endif // header end