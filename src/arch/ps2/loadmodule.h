/*********************************************************************
 * Sif functions for module (.irx) loading
 * Quite easy rev engineered from util demos..
 * These functions are not in any way complete or really tested!
 *                      -pukko
 */

#ifndef _LOAD_MODULE_H_
#define _LOAD_MODULE_H_

int _lf_bind(int arg0);

int _sifLoadModule(char *moduleName, int a, void *b, int c);

#endif
