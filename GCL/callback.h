/*****************************************************************************
*                           gcl - callback.h                                 *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
using namespace std;

#ifndef CALLBACK_H
#define CALLBACK_H
#if _MSC_VER
   #pragma once
#endif
#include "stdafx.h"

namespace gcl {

template <int i>
struct pl_holder {bool operator==(const pl_holder<i>&){return true;}};

namespace placeholder {
static const pl_holder<1> _1;
static const pl_holder<2> _2;
static const pl_holder<3> _3;
static const pl_holder<4> _4;
static const pl_holder<5> _5;
static const pl_holder<6> _6;
static const pl_holder<7> _7;
static const pl_holder<8> _8;
static const pl_holder<9> _9;
static const pl_holder<10> _10;
};

template <typename t>
struct gcl_list1
{
    gcl_list1(const t& i) : itm(i) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm]);
    }
	bool operator==(const gcl_list1<t>& lst)
	{
		return itm == lst.itm;
	}
private:
    t itm;
};

template <typename t, typename t1>
struct gcl_list2
{
    gcl_list2(const t& i, const t1& i1) : itm(i), itm1(i1) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1]);
    }
	bool operator==(const gcl_list2<t, t1>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1);
	}
private:
    t itm;
    t1 itm1;
};

template <typename t, typename t1, typename t2>
struct gcl_list3
{
    gcl_list3(const t& i, const t1& i1, const t2& i2) : itm(i), itm1(i1), itm2(i2) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2]);
    }
	bool operator==(const gcl_list3<t, t1, t2>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
};

template <typename t, typename t1, typename t2, typename t3>
struct gcl_list4
{
    gcl_list4(const t& i, const t1& i1, const t2& i2, const t3& i3) : itm(i), itm1(i1), itm2(i2), itm3(i3) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3]);
    }
	bool operator==(const gcl_list4<t, t1, t2, t3>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
};

template <typename t, typename t1, typename t2, typename t3, typename t4>
struct gcl_list5
{
    gcl_list5(const t& i, const t1& i1, const t2& i2, const t3& i3, const t4& i4) : itm(i), itm1(i1), itm2(i2), itm3(i3), itm4(i4) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
	t4 operator[](const pl_holder<5>&) const {return itm4;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3, itm4);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3], _lst[itm4]);
    }
	bool operator==(const gcl_list5<t, t1, t2, t3, t4>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3) && (itm4 == lst.itm4);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
	t4 itm4;
};

template <typename t, typename t1, typename t2, typename t3, typename t4, typename t5>
struct gcl_list6
{
    gcl_list6(const t& i, const t1& i1, const t2& i2, const t3& i3, const t4& i4, const t5& i5) : itm(i), itm1(i1), itm2(i2), itm3(i3), itm4(i4), itm5(i5) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
	t4 operator[](const pl_holder<5>&) const {return itm4;}
	t5 operator[](const pl_holder<6>&) const {return itm5;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3, itm4, itm5);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3], _lst[itm4], _lst[itm5]);
    }
	bool operator==(const gcl_list6<t, t1, t2, t3, t4, t5>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3) && (itm4 == lst.itm4) && (itm5 == lst.itm5);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
	t4 itm4;
	t5 itm5;
};

template <typename t, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6>
struct gcl_list7
{
    gcl_list7(const t& i, const t1& i1, const t2& i2, const t3& i3, const t4& i4, const t5& i5, const t6& i6) : itm(i), itm1(i1), itm2(i2), itm3(i3), itm4(i4), itm5(i5), itm6(i6) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
	t4 operator[](const pl_holder<5>&) const {return itm4;}
	t5 operator[](const pl_holder<6>&) const {return itm5;}
	t6 operator[](const pl_holder<7>&) const {return itm6;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3, itm4, itm5, itm6);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3], _lst[itm4], _lst[itm5], _lst[itm6]);
    }
	bool operator==(const gcl_list7<t, t1, t2, t3, t4, t5, t6>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3) && (itm4 == lst.itm4) && (itm5 == lst.itm5) && (itm6 == lst.itm6);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
	t4 itm4;
	t5 itm5;
	t6 itm6;
};

template <typename t, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7>
struct gcl_list8
{
    gcl_list8(const t& i, const t1& i1, const t2& i2, const t3& i3, const t4& i4, const t5& i5, const t6& i6, const t7& i7) : itm(i), itm1(i1), itm2(i2), itm3(i3), itm4(i4), itm5(i5), itm6(i6), itm7(i7) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
	t4 operator[](const pl_holder<5>&) const {return itm4;}
	t5 operator[](const pl_holder<6>&) const {return itm5;}
	t6 operator[](const pl_holder<7>&) const {return itm6;}
	t7 operator[](const pl_holder<8>&) const {return itm7;}
    template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3, itm4, itm5, itm6, itm7);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3], _lst[itm4], _lst[itm5], _lst[itm6], _lst[itm7]);
    }
	bool operator==(const gcl_list8<t, t1, t2, t3, t4, t5, t6, t7>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3) && (itm4 == lst.itm4) && (itm5 == lst.itm5) && (itm6 == lst.itm6) && (itm7 == lst.itm7);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
	t4 itm4;
	t5 itm5;
	t6 itm6;
	t7 itm7;
};

template <typename t, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7, typename t8>
struct gcl_list9
{
    gcl_list9(const t& i, const t1& i1, const t2& i2, const t3& i3, const t4& i4, const t5& i5, const t6& i6, const t7& i7, const t8& i8) : itm(i), itm1(i1), itm2(i2), itm3(i3), itm4(i4), itm5(i5), itm6(i6), itm7(i7), itm8(i8) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
	t4 operator[](const pl_holder<5>&) const {return itm4;}
	t5 operator[](const pl_holder<6>&) const {return itm5;}
	t6 operator[](const pl_holder<7>&) const {return itm6;}
	t7 operator[](const pl_holder<8>&) const {return itm7;}
	t8 operator[](const pl_holder<9>&) const {return itm8;}
	template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3, itm4, itm5, itm6, itm7, itm8);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3], _lst[itm4], _lst[itm5], _lst[itm6], _lst[itm7], _lst[itm8]);
    }
	bool operator==(const gcl_list9<t, t1, t2, t3, t4, t5, t6, t7, t8>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3) && (itm4 == lst.itm4) && (itm5 == lst.itm5) && (itm6 == lst.itm6) && (itm7 == lst.itm7) && (itm8 == lst.itm8);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
	t4 itm4;
	t5 itm5;
	t6 itm6;
	t7 itm7;
	t8 itm8;
};

template <typename t, typename t1, typename t2, typename t3, typename t4, typename t5, typename t6, typename t7, typename t8, typename t9>
struct gcl_list10
{
    gcl_list10(const t& i, const t1& i1, const t2& i2, const t3& i3, const t4& i4, const t5& i5, const t6& i6, const t7& i7, const t8& i8, const t9& i9) : itm(i), itm1(i1), itm2(i2), itm3(i3), itm4(i4), itm5(i5), itm6(i6), itm7(i7), itm8(i8), itm9(i9) {}
    t operator[](const pl_holder<1>&) const {return itm;}
    t1 operator[](const pl_holder<2>&) const {return itm1;}
	t2 operator[](const pl_holder<3>&) const {return itm2;}
	t3 operator[](const pl_holder<4>&) const {return itm3;}
	t4 operator[](const pl_holder<5>&) const {return itm4;}
	t5 operator[](const pl_holder<6>&) const {return itm5;}
	t6 operator[](const pl_holder<7>&) const {return itm6;}
	t7 operator[](const pl_holder<8>&) const {return itm7;}
	t8 operator[](const pl_holder<9>&) const {return itm8;}
	t9 operator[](const pl_holder<10>&) const {return itm9;}
	template <typename it>
    it operator[](const it& _it) const {return _it;}

    template <typename fn>
    void operator()(fn func)
    {
        func(itm, itm1, itm2, itm3, itm4, itm5, itm6, itm7, itm8, itm9);
    }
    template <typename func, typename lst>
    void operator()(func fn, const lst& _lst)
    {
        fn(_lst[itm], _lst[itm1], _lst[itm2], _lst[itm3], _lst[itm4], _lst[itm5], _lst[itm6], _lst[itm7], _lst[itm8], _lst[itm9]);
    }
	bool operator==(const gcl_list10<t, t1, t2, t3, t4, t5, t6, t7, t8, t9>& lst)
	{
		return (itm == lst.itm) && (itm1 == lst.itm1) && (itm2 == lst.itm2) && (itm3 == lst.itm3) && (itm4 == lst.itm4) && (itm5 == lst.itm5) && (itm6 == lst.itm6) && (itm7 == lst.itm7) && (itm8 == lst.itm8) && (itm9 == lst.itm9);
	}
private:
    t itm;
    t1 itm1;
	t2 itm2;
	t3 itm3;
	t4 itm4;
	t5 itm5;
	t6 itm6;
	t7 itm7;
	t8 itm8;
	t9 itm9;
};

template <typename... args>
struct gcl_func
{
private:
	struct bind_base
	{
	public:
		virtual ~bind_base() {}
		virtual void call(const args&... _args) = 0;
		virtual bool operator==(bind_base* b) = 0;
	};
	template <typename func, typename lst>
	struct gcl_binder :
		public bind_base
	{
	public:
		gcl_binder(func _fn, const lst& _lst) : fn(_fn), _list(move(_lst)) {}
		void operator()()
		{
			_list(fn);
		}
		template <typename a0>
		void operator()(const a0& _a0)
		{
			gcl_list1<a0> lst(_a0);
			_list(fn, lst);
		}
		template <typename a0, typename a1>
		void operator()(const a0& _a0, const a1& _a1)
		{
			gcl_list2<a0, a1> lst(_a0, _a1);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2)
		{
			gcl_list3<a0, a1, a2> lst(_a0, _a1, _a2);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3)
		{
			gcl_list4<a0, a1, a2, a3> lst(_a0, _a1, _a2, _a3);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3, typename a4>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4)
		{
			gcl_list5<a0, a1, a2, a3, a4> lst(_a0, _a1, _a2, _a3, _a4);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5)
		{
			gcl_list6<a0, a1, a2, a3, a4, a5> lst(_a0, _a1, _a2, _a3, _a4, _a5);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6)
		{
			gcl_list7<a0, a1, a2, a3, a4, a5, a6> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7)
		{
			gcl_list8<a0, a1, a2, a3, a4, a5, a6, a7> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7, typename a8>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7, const a8& _a8)
		{
			gcl_list9<a0, a1, a2, a3, a4, a5, a6, a7, a8> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8);
			_list(fn, lst);
		}
		template <typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7, typename a8, typename a9>
		void operator()(const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7, const a8& _a8, const a9& _a9)
		{
			gcl_list10<a0, a1, a2, a3, a4, a5, a6, a7, a8, a9> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9);
			_list(fn, lst);
		}
		bool operator==(bind_base* b)
		{
			auto ptr = dynamic_cast<gcl_binder<func, lst>*>(b);
			if(!ptr)
				return false;
			return (fn == ptr->fn) && (_list == ptr->_list);
		}
	protected:
		virtual void call(const args&... _args)
		{
			(*this)(_args...);
		}
	private:
		func fn;
		lst _list;
	};
	template <typename func, typename cl>
	struct mem_func_ref : 
		public bind_base
	{
	public:
		mem_func_ref(cl* _inst, func _fn) : inst(_inst), fn(_fn) {}
		virtual void call(const args&... _args)
		{
			(*this)(_args...);
		}
		template <typename... args2>
		void operator()(const args2&... _a0)
		{
			(inst->*fn)(_a0...);
		}
		bool operator==(bind_base* b)
		{
			auto ptr = dynamic_cast<mem_func_ref<func, cl>*>(b);
			if(!ptr)
				return false;
			return (inst == ptr->inst) && (fn == ptr->fn);
		}
	private:
		cl* inst;
		func fn;
	};
	template <typename func>
	struct func_ref : 
		public bind_base
	{
	public:
		func_ref(func _fn) : fn(_fn) {}
		virtual void call(const args&... _args)
		{
			(*this)(_args...);
		}
		template <typename... args2>
		void operator()(const args2&... _a0)
		{
			fn(_a0...);
		}
		bool operator==(bind_base* b)
		{
			auto ptr = dynamic_cast<func_ref<func>*>(b);
			if(!ptr)
				return false;
			return (fn == ptr->fn);
		}
	private:
		func fn;
	};
public:
	template <typename func, typename lst>
	explicit gcl_func(func fn, const lst& _lst)
	{
		cl_b = shared_ptr<bind_base>(new gcl_binder<func, lst>(fn, _lst));
	}
	template <typename func, typename cl>
	explicit gcl_func(func fn, cl* inst)
	{
		cl_b = shared_ptr<bind_base>(new mem_func_ref<func, cl>(inst, fn));
	}
	template <typename func>
	explicit gcl_func(func fn)
	{
		cl_b = shared_ptr<bind_base>(new func_ref<func>(fn));
	}
	void operator()(const args&... _args)
	{
		cl_b->call(_args...);
	}
	bool operator==(const gcl_func& fn)
	{
		return cl_b.get()->operator==(fn.cl_b.get());
	}
private:
	shared_ptr<bind_base> cl_b;
};

template <typename... args, typename func, typename a0>
gcl_func<args...> gcl_bind(func fn, const a0& _a0)
{
	gcl_list1<a0> lst(_a0);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0)
{
	gcl_list1<a0> lst(_a0);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1)
{
	gcl_list2<a0, a1> lst(_a0, _a1);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1)
{
	gcl_list2<a0, a1> lst(_a0, _a1);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2)
{
	gcl_list3<a0, a1, a2> lst(_a0, _a1, _a2);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2)
{
	gcl_list3<a0, a1, a2> lst(_a0, _a1, _a2);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3)
{
	gcl_list4<a0, a1, a2, a3> lst(_a0, _a1, _a2, _a3);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3)
{
	gcl_list4<a0, a1, a2, a3> lst(_a0, _a1, _a2, _a3);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3, typename a4>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4)
{
	gcl_list5<a0, a1, a2, a3, a4> lst(_a0, _a1, _a2, _a3, _a4);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3, typename a4>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4)
{
	gcl_list5<a0, a1, a2, a3, a4> lst(_a0, _a1, _a2, _a3, _a4);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5)
{
	gcl_list6<a0, a1, a2, a3, a4, a5> lst(_a0, _a1, _a2, _a3, _a4, _a5);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5)
{
	gcl_list6<a0, a1, a2, a3, a4, a5> lst(_a0, _a1, _a2, _a3, _a4, _a5);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6)
{
	gcl_list7<a0, a1, a2, a3, a4, a5, a6> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6)
{
	gcl_list7<a0, a1, a2, a3, a4, a5, a6> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7)
{
	gcl_list8<a0, a1, a2, a3, a4, a5, a6, a7> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7)
{
	gcl_list8<a0, a1, a2, a3, a4, a5, a6, a7> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7, typename a8>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7, const a8& _a8)
{
	gcl_list9<a0, a1, a2, a3, a4, a5, a6, a7, a8> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7, typename a8>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7, const a8& _a8)
{
	gcl_list9<a0, a1, a2, a3, a4, a5, a6, a7, a8> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

template <typename... args, typename func, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7, typename a8, typename a9>
gcl_func<args...> gcl_bind(func fn, const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7, const a8& _a8, const a9& _a9)
{
	gcl_list10<a0, a1, a2, a3, a4, a5, a6, a7, a8, a9> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9);
	return gcl_func<args...>(fn, lst);
}

template <typename... args, typename ret, typename... args_fun, typename cl, typename a0, typename a1, typename a2, typename a3, typename a4, typename a5, typename a6, typename a7, typename a8, typename a9>
gcl_func<args...> gcl_bind_mem(cl* inst, ret(cl::*fn)(args_fun...), const a0& _a0, const a1& _a1, const a2& _a2, const a3& _a3, const a4& _a4, const a5& _a5, const a6& _a6, const a7& _a7, const a8& _a8, const a9& _a9)
{
	gcl_list10<a0, a1, a2, a3, a4, a5, a6, a7, a8, a9> lst(_a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9);
	return gcl_func<args...>(gcl::make_func_ptr(inst, fn), lst);
}

// Creates a storage for non static Memberfunctions
template <typename _class, typename ret, typename... _args> 
const gcl_func<_args...>
	make_func_ptr(_class* class_ptr, ret(_class::*func)(_args...)) {return gcl_func<_args...>(func, class_ptr);}

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
	void operator+= (const gcl_func<_args...>& fn)
	{
		prv_coll.push_back(fn);
	}
	// Adds a delegate to the callback
	void operator+= (func_ptr ptr)
	{
		prv_coll.push_back(gcl_func<_args...>(ptr));
	}
	// Removes a delegate to the callback
	void operator-= (func_ptr ptr)
	{
		prv_coll.erase(std::remove(prv_coll.begin(), prv_coll.end(), gcl_func<_args...>(ptr)));
	}
	// Removes a delegate to the callback
	void operator-= (const gcl_func<_args...>& fn)
	{
		prv_coll.erase(std::remove(prv_coll.begin(), prv_coll.end(), fn));
	}
	// Removes a delegate to the callback
	void operator-= (unsigned idx)
	{
		prv_coll.erase(prv_coll.begin()+idx);
	}
	// Calls all bound delegates
	void operator() (const _args&... args)
	{
		for(auto& itr = prv_coll.begin(); itr != prv_coll.end(); ++itr)
			(*itr)(args...);
	}
	// Releases all bound delegates
	void release_hooks() {prv_coll.clear()}
	unsigned size() const {return prv_coll.size();}
private:
	vector<gcl_func<_args...>> prv_coll;
};
};
#endif