  template <typename T>
  struct ResultValue
  {
#ifdef VULKAN_HPP_HAS_NOEXCEPT
    ResultValue( Result r, T & v ) VULKAN_HPP_NOEXCEPT(VULKAN_HPP_NOEXCEPT(T(v)))
#else
    ResultValue( Result r, T & v )
#endif
      : result( r )
      , value( v )
    {}

#ifdef VULKAN_HPP_HAS_NOEXCEPT
    ResultValue( Result r, T && v ) VULKAN_HPP_NOEXCEPT(VULKAN_HPP_NOEXCEPT(T(std::move(v))))
#else
    ResultValue( Result r, T && v )
#endif
      : result( r )
      , value( std::move( v ) )
    {}

    Result  result;
    T       value;

    operator std::tuple<Result &, T &>() VULKAN_HPP_NOEXCEPT
    {
      return std::tuple<Result &, T &>( result, value );
    }

	// std::expected-look alike
    bool has_value() const VULKAN_HPP_NOEXCEPT
    {
      return result == vk::Result::eSuccess;
    }

    T const * operator->() const VULKAN_HPP_NOEXCEPT
    {
      assert( has_value() );
      return &value;
    }

    T * operator->() VULKAN_HPP_NOEXCEPT
    {
      assert( has_value() );
      return &value;
    }

    T const & operator*() const VULKAN_HPP_NOEXCEPT
    {
      assert(has_value ());
      return value;
    }

    T & operator*() VULKAN_HPP_NOEXCEPT
    {
      assert(has_value ());
      return value;
    }
  };

#if !defined( VULKAN_HPP_NO_SMART_HANDLE )
  template <typename Type, typename Dispatch>
  struct ResultValue<UniqueHandle<Type, Dispatch>>
  {
#ifdef VULKAN_HPP_HAS_NOEXCEPT
    ResultValue(Result r, UniqueHandle<Type, Dispatch> && v) VULKAN_HPP_NOEXCEPT
#else
    ResultValue(Result r, UniqueHandle<Type, Dispatch> && v)
#endif
      : result(r)
      , value(std::move(v))
    {}

    VULKAN_HPP_DEPRECATED(
      "asTuple() on an l-value is deprecated, as it implicitly moves the UniqueHandle out of the ResultValue. Use asTuple() on an r-value instead, requiring to explicitly move the UniqueHandle." )
      std::tuple<Result, UniqueHandle<Type, Dispatch>>
      asTuple() &
    {
      return std::make_tuple( result, std::move( value ) );
    }

    std::tuple<Result, UniqueHandle<Type, Dispatch>> asTuple() &&
    {
      return std::make_tuple( result, std::move( value ) );
    }

    Result                       result;
    UniqueHandle<Type, Dispatch>  value;
  };

  template <typename Type, typename Dispatch>
  struct ResultValue<std::vector<UniqueHandle<Type, Dispatch>>>
  {
#  ifdef VULKAN_HPP_HAS_NOEXCEPT
    ResultValue( Result r, std::vector<UniqueHandle<Type, Dispatch>> && v ) VULKAN_HPP_NOEXCEPT
#  else
    ResultValue( Result r, std::vector<UniqueHandle<Type, Dispatch>> && v )
#  endif
      : result( r )
      , value( std::move( v ) )
    {}

    VULKAN_HPP_DEPRECATED(
      "asTuple() on an l-value is deprecated, as it implicitly moves the UniqueHandle out of the ResultValue. Use asTuple() on an r-value instead, requiring to explicitly move the UniqueHandle." )
      std::tuple<Result, std::vector<UniqueHandle<Type, Dispatch>>>
      asTuple() &
    {
      return std::make_tuple( result, std::move( value ) );
    }

    std::tuple<Result, std::vector<UniqueHandle<Type, Dispatch>>> asTuple() &&
    {
      return std::make_tuple( result, std::move( value ) );
    }

    Result                                    result;
    std::vector<UniqueHandle<Type, Dispatch>> value;
  };
#endif

  template <typename T>
  struct ResultValueType
  {
#ifdef VULKAN_HPP_NO_EXCEPTIONS
	using type = ResultValue<T>;
#else
	using type = T;
#endif
  };

  template <>
  struct ResultValueType<void>
  {
#ifdef VULKAN_HPP_NO_EXCEPTIONS
    using type = Result;
#else
    using type = void;
#endif
  };

  namespace detail
  {
    template <typename T>
    VULKAN_HPP_CONSTEXPR bool ignore( T const & ) VULKAN_HPP_NOEXCEPT
    {
      return true;
    }

    VULKAN_HPP_INLINE typename ResultValueType<void>::type createResultValueType( Result result )
    {
#if defined( VULKAN_HPP_NO_EXCEPTIONS )
      return result;
#else
      ignore( result );
#endif
    }

    template <typename T>
    VULKAN_HPP_INLINE typename ResultValueType<T>::type createResultValueType( Result result, T & data )
    {
#if defined( VULKAN_HPP_NO_EXCEPTIONS )
      return ResultValue<T>( result, data );
#else
      ignore( result );
      return data;
#endif
    }

    template <typename T>
    VULKAN_HPP_INLINE typename ResultValueType<T>::type createResultValueType( Result result, T && data )
    {
#if defined( VULKAN_HPP_NO_EXCEPTIONS )
      return ResultValue<T>( result, std::move( data ) );
#else
      ignore( result );
      return std::move( data );
#endif
    }
  }  // namespace detail
