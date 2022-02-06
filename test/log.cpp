/*
 * log.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: martin
 */

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

char* dbgprint ( const char* format, ... )
{
  char s[1000];
  va_list varArgs ;

  va_start ( varArgs, format ) ;
  vsnprintf ( s, sizeof(s), format, varArgs ) ;
  va_end ( varArgs ) ;

  std::cout << s << std::endl;
  return nullptr;
}




