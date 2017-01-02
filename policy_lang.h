#ifndef POLICY_LANG_H_
#define POLICY_LANG_H_
#include <glib.h>

#if defined (__cplusplus)
extern "C" {
#endif

char* parse_policy_lang( char* s );
void  parse_attribute( GSList** l, char* a );

#if defined (__cplusplus)
}  //extern "C"
#endif

#endif // POLICY_LANG_H_
