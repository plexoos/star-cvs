/*
 * $Id: kproto.h,v 1.1 1998/09/18 00:29:12 fisyak Exp $
 *
 * $Log: kproto.h,v $
 * Revision 1.1  1998/09/18 00:29:12  fisyak
 * Add break off for Linux
 *
 * Revision 1.1.1.1  1996/03/08 15:33:01  mclareni
 * Kuip
 *
 */

extern char*  clean_word( char* );
extern char*  format_prompt( const char* );
extern char*  getline( const char* );
extern void   gl_config( const char*, int );
extern void   gl_histadd( const char* );
extern void   gl_setwidth( int );
extern char*  input_line( const char*, char );
extern void   leave_kuip(void);
extern int    len_alias( const char*, int );
extern int    len_sysfun( const char* );
extern int    len_vector( const char* );
extern char*  quote_string( char*, int );
extern void   reset_break(void);
extern char*  split_at_semicolon( char* );

extern void   k_parse_args( const char*, char**, int* );


#ifndef vms
extern void   signal_handler( int );
#else
extern int    signal_handler( const void*, const void* );
extern int    control_C_ast( int );
#endif


