//////////////////////////////////////////////////////////////////////////
// 此模块封装了一些注册表操作的API函数
// 全是ANSI版函数
//////////////////////////////////////////////////////////////////////////
#ifndef	__REG_OPERATE_2008_04_14__
#define	__REG_OPERATE_2008_04_14__

#define ERROR_NON        0x00
#define ERROR_KEYNAME    0x01
#define ERROR_VALUENAME  0x02
#define ERROR_REGTYPE    0x04
#define ERROR_BUFFER     0x08
#define ERROR_OTHER      0x10

#ifndef	REGOPERATE_DLL_USE
    #define REGOPERATE_DLL
    #define REGOPERATE_API
#else
    #ifdef REGOPERATE_DLL_EXPORTS
        #define REGOPERATE_DLL  __declspec(dllexport)
    #else
        #define REGOPERATE_DLL  __declspec(dllimport)
    #endif

    #define REGOPERATE_API  __stdcall
#endif

#define REGOPERATE_DECL_FUNC(ret) REGOPERATE_DLL ret REGOPERATE_API
#define REGOPERATE_IMPL_FUNC(ret) ret REGOPERATE_API

#ifdef	__cplusplus
extern "C" {
#endif

REGOPERATE_DECL_FUNC(HKEY) reg_open_key( LPCSTR key_name, BOOL w = FALSE );

REGOPERATE_DECL_FUNC(int) reg_close_key( HKEY key_handle );

REGOPERATE_DECL_FUNC(int) reg_write_ex( HKEY key_handle, LPCSTR value_name, DWORD reg_type, BYTE const * buffer, int size );

REGOPERATE_DECL_FUNC(int) reg_write_string( HKEY key_handle, LPCSTR value_name, LPCSTR str );

REGOPERATE_DECL_FUNC(int) reg_write_long( HKEY key_handle, LPCSTR value_name, DWORD value );

REGOPERATE_DECL_FUNC(int) reg_read_ex( HKEY key_handle, LPCSTR value_name, LPBYTE buffer, LPDWORD reg_type_, LPDWORD buf_size_ );

REGOPERATE_DECL_FUNC(int) reg_read_string( HKEY key_handle, LPCSTR value_name, LPSTR buffer, DWORD size );

REGOPERATE_DECL_FUNC(int) reg_read_long( HKEY key_handle, LPCSTR value_name, LPDWORD value_ );

REGOPERATE_DECL_FUNC(int) reg_query_value_size( HKEY key_handle, LPCSTR value_name );

REGOPERATE_DECL_FUNC(BOOL) reg_exists_ex( HKEY key_handle, LPCSTR value_name );

REGOPERATE_DECL_FUNC(BOOL) reg_exists( LPCSTR key_name, LPCSTR value_name );


REGOPERATE_DECL_FUNC(int) reg_delete( LPCSTR key_name, LPCSTR value_name = NULL );

REGOPERATE_DECL_FUNC(void) reg_force_delete( LPCSTR key_name );


REGOPERATE_DECL_FUNC(BOOL) reg_error_info( int error_code, LPSTR info_buff, int size );

REGOPERATE_DECL_FUNC(DWORD) reg_cur_reg_type( void );

REGOPERATE_DECL_FUNC(DWORD) reg_cur_data_size( void );


REGOPERATE_DECL_FUNC(LPCSTR) reg_get_string( LPCSTR key_name, LPCSTR value_name );

REGOPERATE_DECL_FUNC(DWORD) reg_get_long( LPCSTR key_name, LPCSTR value_name );

REGOPERATE_DECL_FUNC(int) reg_set_string( LPCSTR key_name, LPCSTR value_name, LPCSTR str );

REGOPERATE_DECL_FUNC(int) reg_set_long( LPCSTR key_name, LPCSTR value_name, DWORD value );


typedef struct REG_VALUE
{
	LPSTR value_name;
	DWORD reg_type;
	DWORD data_size;
} REG_VALUE;
typedef REG_VALUE * REG_VALUE_ARRAY;

REGOPERATE_DECL_FUNC(int) reg_enum_value( HKEY key_handle, REG_VALUE_ARRAY * value_arr_ );

REGOPERATE_DECL_FUNC(void) reg_free_value_array( REG_VALUE_ARRAY value_arr );

#define reg_visit_val( arr, val ) do { REG_VALUE * p__ = (arr); while ( p__->value_name != NULL ) { REG_VALUE * val = p__;

#define reg_end() p__++; } } while (0);

#ifdef	__cplusplus
}
#endif

#endif	/*__REG_OPERATE_2008_04_14__*/
