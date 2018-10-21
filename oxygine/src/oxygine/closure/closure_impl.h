
/*

Closure library 1.1
file "closure_impl.h"

Written by Ivan Yankov aka _Winnie (woowoowoow@bk.ru)
Many thanks to Wolfhound

*/

//====================================================================================================

//general template
template <class F>
struct Closure;

//forward declaration of specialization 
template <TEMPLATE_PARAM_LIST>
struct Closure<R(PARAM_TYPE_LIST)>;

//====================================================================================================
namespace detail
{

//this template generate function, which call "pmem" member function
template <class T, TEMPLATE_PARAM_LIST, R (T::*pmem)(PARAM_TYPE_LIST)> 
 R ProxyFunc(void *p_this PARAM_FORM_ARG_LIST_COMMA) 
{ 
  return (static_cast<T *>(p_this) ->* pmem)( PARAM_ARG_LIST); 
}

//this template generate function, which call "pmem" const member function
template <class T, TEMPLATE_PARAM_LIST, R (T::*pmem)(PARAM_TYPE_LIST)const> 
 R ProxyFuncConst(void *p_this PARAM_FORM_ARG_LIST_COMMA) 
{ 
  return (static_cast<T *>(p_this) ->* pmem)(PARAM_ARG_LIST); 
}

namespace CLOSURE_NUM
{

//base for Closure
//for example, if CLOSURE_NUM is Closure2, then 
//Closure2::ClosureBase is superclass for Closure<int(long, long)>

template <TEMPLATE_PARAM_LIST>
struct ClosureBase
{
  typedef R (*p_proxy_type)(void * PARAM_FORM_ARG_LIST_COMMA);
  void *p_this;
  p_proxy_type p_proxy;
 
#if CLOSURE_FUNCTION
  std::shared_ptr< std::function< R(PARAM_TYPE_LIST)> > p_function;
#endif

  R operator()(PARAM_FORM_ARG_LIST)
  { 
	  return p_proxy(p_this PARAM_ARG_LIST_COMMA); 
  }

};

} //namespace CLOSURE_NUM

} //namespace detail

//====================================================================================================
template <TEMPLATE_PARAM_LIST>
struct Closure<R(PARAM_TYPE_LIST)>: public detail::CLOSURE_NUM::ClosureBase<R PARAM_TYPE_LIST_COMMA>
{
  //default initialization
  Closure() 
  {
    this->p_this= 0;
    this->p_proxy= 0;
  }


  //initialization with proxy function "p_proxy_function" and context "p_this".
  //signature of p_proxy_function should match template parameters of Closure<...>
  //and take additional parameter void *
  Closure(
    void *p_this,  
    typename detail::CLOSURE_NUM::ClosureBase<R PARAM_TYPE_LIST_COMMA>::p_proxy_type p_proxy_function) 
  {
    this->p_this= p_this;
    this->p_proxy= p_proxy_function;
  }

 #if CLOSURE_FUNCTION
  Closure( const std::function< R(PARAM_TYPE_LIST)>& f)
  {
	  this->p_function = std::make_shared< std::function<R(PARAM_TYPE_LIST)> >(f);
	  this->p_this = this->p_function.get();
	  this->p_proxy = callfunction;
  }

  /*
  Closure(const std::function< void(void)>& f_)
  {
      auto f = [f_](Event*) {
          f_();
      };
      
      this->p_function = std::make_shared< std::function<R(PARAM_TYPE_LIST)> >(f);
      this->p_this = this->p_function.get();
      this->p_proxy = callfunction;
  }
  */

  template<class T>
  Closure(T f)//lambda support
  {
	  this->p_function = std::make_shared< std::function<R(PARAM_TYPE_LIST)> >(f);
	  this->p_this = this->p_function.get();
	  this->p_proxy = callfunction;
  }

  static R callfunction(void* ptr PARAM_FORM_ARG_LIST_COMMA)
  {
	  std::function< R(PARAM_TYPE_LIST) > *f = (std::function< R(PARAM_TYPE_LIST) > *)ptr;
	  return (*f)(PARAM_ARG_LIST);
  }
#endif

  private:
    struct dummy { void nonnull() {} };
    typedef void (dummy::*safe_bool)();
  
  public:
    //return value which can be implicitly casted to bool.
    //true, if object initialized with some non-NULL function
    //false, otherwise
    operator safe_bool() const
    { 
        if(this->p_proxy)
          return &dummy::nonnull;
        else        
          return 0;
    }
  
    bool operator!() const
    { 
        return !safe_bool(*this);
    }

	bool operator == (const Closure &c) const
	{
		return this->p_proxy == c.p_proxy && this->p_this == c.p_this
#if CLOSURE_FUNCTION
			&& this->p_function == c.p_function
#endif
		;
	}

};

//====================================================================================================
namespace detail
{
  namespace CLOSURE_NUM
  {

    template <class T, TEMPLATE_PARAM_LIST> 
    struct CreateClosureHelper
    {
      template <R (T::*p_mem)(PARAM_TYPE_LIST)>
        Closure<R(PARAM_TYPE_LIST)> Init(T *p_this)
      {
        Closure<R(PARAM_TYPE_LIST)> c(
          p_this,
          ProxyFunc<T, R PARAM_TYPE_LIST_COMMA , p_mem>);
        return c;
      }
    }; 

    template <class T, TEMPLATE_PARAM_LIST> 
    struct CreateClosureHelperConst
    {
      template <R (T::*p_mem)(PARAM_TYPE_LIST)const>
        Closure<R(PARAM_TYPE_LIST)> Init(const T *p_this)
      {
        Closure<R(PARAM_TYPE_LIST)> c(
          const_cast<void*>(static_cast<const void*>(p_this)),
          ProxyFuncConst<T, R PARAM_TYPE_LIST_COMMA, p_mem>);
        return c;
      }
    };
  } //namespace CLOSURE_NUM

  //helper function, to deduce return and parameters types of given pointer to member function
  template <class T, TEMPLATE_PARAM_LIST> 
  CLOSURE_NUM::CreateClosureHelper<T, R PARAM_TYPE_LIST_COMMA> CreateClosure(R (T::*)(PARAM_TYPE_LIST))
  {
    return CLOSURE_NUM::CreateClosureHelper<T, R PARAM_TYPE_LIST_COMMA>();
  }

#if CLOSURE_FUNCTION
  template <TEMPLATE_PARAM_LIST> 
  Closure<R (PARAM_TYPE_LIST)> CreateClosureF(const std::function< R (PARAM_TYPE_LIST) > & f )
  {
	  return Closure<R(PARAM_TYPE_LIST)>(f);
  }
  
  /*

  template <TEMPLATE_PARAM_LIST>
  Closure<R(PARAM_TYPE_LIST)> CreateClosureF( R (*f) (PARAM_TYPE_LIST) )
  {
	  std::function<R(PARAM_TYPE_LIST)> fn = f;
	  return Closure<R(PARAM_TYPE_LIST)>(fn);
  }
  */
#endif

  
  //helper function, to deduce return and parameters types of given pointer to member const function
  template <class T, TEMPLATE_PARAM_LIST> 
  CLOSURE_NUM::CreateClosureHelperConst<T, R PARAM_TYPE_LIST_COMMA> CreateClosure(R (T::*)(PARAM_TYPE_LIST)const)
  {
    return CLOSURE_NUM::CreateClosureHelperConst<T, R PARAM_TYPE_LIST_COMMA>();
  }
} //namespace detail

//====================================================================================================


#undef TEMPLATE_PARAM_LIST
#undef PARAM_TYPE_LIST
#undef PARAM_TYPE_LIST_COMMA
#undef PARAM_FORM_ARG_LIST
#undef PARAM_FORM_ARG_LIST_COMMA
#undef PARAM_ARG_LIST
#undef PARAM_ARG_LIST_COMMA
#undef CLOSURE_NUM

