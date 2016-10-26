#pragma once

#include "file_tab.h" // for term1,term2,term3

void delete_term1( term1 * );
void delete_term2( term2 * );
void delete_term3( term3 * );

double eval_term1( term1 *, double );
double eval_term2( term2 *, double * );
double eval_term3( term3 *, double * );

const char * DumpPoly1( term1 *, const char * );
const char * DumpPoly2( term2 *, const char *, const char * );
const char * DumpPoly3( term3 *, const char *, const char *, const char * );

char * printterm2( char *, term2 *, bool, const char *, const char * );
char * printterm3( char *, term3 *, bool, const char *, const char *, const char * );