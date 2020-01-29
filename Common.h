/********************************************************************
 * Filename:	 Common
 * File type:	 Header File
 * Date Created: 08/16/2005
 * Author:		 Travis McPhail
 *
 * Purpose:		 A common file that has the necessary includes
 *				 needed for multiple projects. (Ex. defining macros
 *				 that can be used in multiple projects in Comp 360.)
 * NOTE:		 PROJECT SPECIFIC FILES SHOULD NOT BE INCLUDED HERE
 ********************************************************************/

#ifndef FRAMEWORK_COMMON_H
#define FRAMEWORK_COMMON_H

/* Visual studio doesn't inherently know about glut, so we have provided the header in Common/
 * other platforms will probably need a different include here.
 * For example, on Mac OS try #include <GLUT/glut.h> and linking with -framework GLUT.
 * The GLUT header includes the other necessary OpenGL headers in a platform-independent
 * manner. 
 */

// Normal C libraries to include in any project
#include <stdio.h>
#include <assert.h>

/**************************************************
 * Deletion Macros
 * ------------------------------------------------
 * NOTE: In your projects it is recommend to use the new
 *		 keyword when allocating memory and using delete
 *		 to deallocate memory.
 *
 *		 These macros have an implicit check to see if the
 *		 pointer to deleted is NULL
 */

// Definition of NULL
#ifndef NULL
#define NULL 0
#endif

#ifndef DELETE_OPS
#define CLEAN_DELETE( p ) if( p ) delete p
#define CLEAN_ARRAY_DELETE( p ) if( p ) delete [] p
#endif

#endif /* FRAMEWORK_COMMON_H */