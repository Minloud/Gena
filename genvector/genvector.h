/*
  genvector - Generalized Vector
  Pseudo-templated, Cpp-like dynamic linear array for custom data types.
  Copyright (c) 2017, Dmitry D. Chernov
*/

#ifndef ZZ_GENA_GENVECTOR_H_IG
#define ZZ_GENA_GENVECTOR_H_IG

typedef void* gvec_h;

#include "gvec_internal.inc"

/******************************************************************************/

/* Instantiation macros, static approach. */

#define \
GVEC_INSTANTIATE( tpTypeInfo, tpSurname, tpPassBy, tpReturnBy ) \
  ZZ_GVEC_INSTANTIATE(tpTypeInfo,tpSurname,tpPassBy,tpReturnBy)

#define \
GVEC_INSTANTIATE_EX( tpTypeInfo, tpSurname, tpPassBy, tpReturnBy, tpAssignBy ) \
  ZZ_GVEC_INSTANTIATE_EX(tpTypeInfo,tpSurname,tpPassBy,tpReturnBy,tpAssignBy)

/* Instantiation macros, modular approach. */

#define \
GVEC_H_DECLARE( tpTypeInfo, tpSurname, tpPassBy, tpReturnBy ) \
  ZZ_GVEC_H_DECLARE(tpTypeInfo,tpSurname,tpPassBy,tpReturnBy)

#define \
GVEC_C_DEFINE( tpTypeInfo, tpSurname, tpPassBy, tpReturnBy ) \
  ZZ_GVEC_C_DEFINE(tpTypeInfo,tpSurname,tpPassBy,tpReturnBy)

#define \
GVEC_H_DECLARE_EX( tpTypeInfo, tpSurname, tpPassBy, tpReturnBy, tpAssignBy ) \
  ZZ_GVEC_H_DECLARE_EX(tpTypeInfo,tpSurname,tpPassBy,tpReturnBy,tpAssignBy)

#define \
GVEC_C_DEFINE_EX( tpTypeInfo, tpSurname, tpPassBy, tpReturnBy, tpAssignBy ) \
  ZZ_GVEC_C_DEFINE_EX(tpTypeInfo,tpSurname,tpPassBy,tpReturnBy,tpAssignBy)

/******************************************************************************/

/* General-purpose functions. */

extern gvec_h gvec_set( gvec_h handle, gvec_h source );
extern gvec_h gvec_copy( gvec_h handle );
extern void gvec_free( gvec_h handle );

extern void gvec_clear( gvec_h handle );
extern void gvec_reduce( gvec_h handle, size_t new_count );
extern void gvec_remove( gvec_h handle, size_t position, size_t count );
extern void gvec_drop( gvec_h handle );

extern size_t gvec_count( gvec_h handle );
extern size_t gvec_size( gvec_h handle );
extern gena_bool gvec_empty( gvec_h handle );

/******************************************************************************/

/* Pseudo-templated functions to be specialized at type instantiation. */
/* NOTE: Do not call this directly. Instead, use the instantiation macros. */

#define ZZ_GVEC_FUNCTIONS_LIST( Surname, TypeName, PassType, ReturnType ) \
\
extern gvec_##Surname##_h gvec_##Surname##_new( size_t min_count ); \
\
extern gena_bool gvec_##Surname##_assign( gvec_##Surname##_h* phandle, \
  size_t count, const PassType value ); \
\
extern gena_bool gvec_##Surname##_resize( gvec_##Surname##_h* phandle, \
  size_t new_count, const PassType value ); \
\
extern gena_bool gvec_##Surname##_reserve( gvec_##Surname##_h* phandle, \
  size_t min_count ); \
\
extern gena_bool gvec_##Surname##_shrink( gvec_##Surname##_h* phandle ); \
\
extern gena_bool gvec_##Surname##_insert( gvec_##Surname##_h* phandle, \
  size_t position, size_t count, const PassType value ); \
\
extern gena_bool gvec_##Surname##_push( gvec_##Surname##_h* phandle, \
  const PassType value ); \
\
extern ReturnType gvec_##Surname##_pop( gvec_##Surname##_h handle ); \
\
extern TypeName* gvec_##Surname##_at( gvec_##Surname##_h handle, \
  size_t position ); \
\
extern ReturnType gvec_##Surname##_first( gvec_##Surname##_h handle ); \
\
extern ReturnType gvec_##Surname##_last( gvec_##Surname##_h handle ); \
\
ZZ_GENA_FUNCTIONS_LIST_END

/******************************************************************************/

/* Pseudo-templates. User-type management functions are defined there. */
#include "gvec_template.inc"

#endif /* ZZ_GENA_GENVECTOR_H_IG */
