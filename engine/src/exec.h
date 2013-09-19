/* Copyright (C) 2003-2013 Runtime Revolution Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#ifndef __MC_EXEC__
#define __MC_EXEC__

#ifndef __MC_EXECPT__
#include "execpt.h"
#endif

#ifndef OBJDEFS_H
#include "objdefs.h"
#endif

#ifndef OBJECT_H
#include "object.h"
#endif

////////////////////////////////////////////////////////////////////////////////

enum MCExecType
{
	/* A */ kMCExecTypeAny,
	/* B */ kMCExecTypeBoolean,
	/* N */ kMCExecTypeNumber,
	/* S */ kMCExecTypeString,
	/* M */ kMCExecTypeName,
	/* R */ kMCExecTypeArray,
	
	/* b */ kMCExecTypeBool,
	/* i */ kMCExecTypeInt,
	/* j */ kMCExecTypeInt64,
	/* u */ kMCExecTypeUInt,
	/* v */ kMCExecTypeUInt64,
	/* f */ kMCExecTypeFloat,
	/* d */ kMCExecTypeDouble,
	/* c */ kMCExecTypeNativeChar,
	
	/* X */ kMCExecTypeLegacyColor,
	/* Y */ kMCExecTypeLegacyPoint,
	/* Z */ kMCExecTypeLegacyRectangle,
	
	/* E */ kMCExecTypeEnum,
	/* T */ kMCExecTypeSet,
	/* C */ kMCExecTypeCustom,
};

struct MCExecSetTypeElementInfo
{
	const char *tag;
	uindex_t bit;
};

struct MCExecSetTypeInfo
{
	const char *name;
	uindex_t count;
	MCExecSetTypeElementInfo *elements;
};

struct MCExecEnumTypeElementInfo
{
	const char *tag;
	uindex_t value;
	bool read_only;
};

struct MCExecEnumTypeInfo
{
	const char *name;
	uindex_t count;
	MCExecEnumTypeElementInfo *elements;
};

typedef void (*MCExecCustomTypeParseProc)(MCExecContext& ctxt, MCStringRef input, void *r_output);
typedef void (*MCExecCustomTypeFormatProc)(MCExecContext& ctxt, const void *input, MCStringRef& r_output);
typedef void (*MCExecCustomTypeFreeProc)(MCExecContext& ctxt, void *input);

struct MCExecCustomTypeInfo
{
	const char *name;
	uindex_t size;
	void *parse;
	void *format;
	void *free;
};

struct MCExecMethodInfo
{
	const char *name;
	uindex_t arity;
};

#define MC_EXEC_DEFINE_EXEC_METHOD(module, tag, arity) \
	static MCExecMethodInfo _kMC##module##Exec##tag##MethodInfo = { #module "." #tag, arity }; \
	MCExecMethodInfo *kMC##module##Exec##tag##MethodInfo = &_kMC##module##Exec##tag##MethodInfo;

#define MC_EXEC_DEFINE_EVAL_METHOD(module, tag, arity) \
	static MCExecMethodInfo _kMC##module##Eval##tag##MethodInfo = { #module "." #tag, arity }; \
	MCExecMethodInfo *kMC##module##Eval##tag##MethodInfo = &_kMC##module##Eval##tag##MethodInfo;

#define MC_EXEC_DEFINE_GET_METHOD(module, tag, arity) \
	static MCExecMethodInfo _kMC##module##Get##tag##MethodInfo = { #module "." #tag, arity }; \
	MCExecMethodInfo *kMC##module##Get##tag##MethodInfo = &_kMC##module##Get##tag##MethodInfo;

#define MC_EXEC_DEFINE_SET_METHOD(module, tag, arity) \
	static MCExecMethodInfo _kMC##module##Set##tag##MethodInfo = { #module "." #tag, arity }; \
	MCExecMethodInfo *kMC##module##Set##tag##MethodInfo = &_kMC##module##Set##tag##MethodInfo;

#define MC_EXEC_DEFINE_MAKE_METHOD(module, tag, arity) \
	static MCExecMethodInfo _kMC##module##Make##tag##MethodInfo = { #module "." #tag, arity }; \
	MCExecMethodInfo *kMC##module##Make##tag##MethodInfo = &_kMC##module##Make##tag##MethodInfo;

////////////////////////////////////////////////////////////////////////////////

enum MCPropertyType
{
	kMCPropertyTypeAny,
	kMCPropertyTypeBool,
	kMCPropertyTypeInt16,
	kMCPropertyTypeInt32,
	kMCPropertyTypeUInt16,
	kMCPropertyTypeUInt32,
	kMCPropertyTypeDouble,
	kMCPropertyTypeChar,
	kMCPropertyTypeString,
	kMCPropertyTypeBinaryString,
	kMCPropertyTypeColor,
	kMCPropertyTypeRectangle,
	kMCPropertyTypePoint,
	kMCPropertyTypeInt16X2,
	kMCPropertyTypeInt16X4,
	kMCPropertyTypeArray,
	kMCPropertyTypeSet,
	kMCPropertyTypeEnum,
	kMCPropertyTypeCustom,
	kMCPropertyTypeOptionalInt16,
	kMCPropertyTypeOptionalUInt16,
	kMCPropertyTypeOptionalUInt32,
	kMCPropertyTypeOptionalString,
	kMCPropertyTypeOptionalRectangle,
	kMCPropertyTypeOptionalEnum,
    kMCPropertyTypeLinesOfString,
    kMCPropertyTypeLinesOfUInt,
    kMCPropertyTypeLinesOfUIntX2,
    kMCPropertyTypeLinesOfPoint,
    kMCPropertyTypeItemsOfUInt,

};

struct MCPropertyInfo
{
	Properties property;
	bool effective;
	MCPropertyType type;
	void *type_info;
	void *getter;
	void *setter;
	bool has_effective;
    bool is_array_prop;
};

template<typename O, typename A, void (O::*Method)(MCExecContext&, A)> inline void MCPropertyObjectThunk(MCExecContext& ctxt, MCObjectPtr obj, A arg)
{
	(static_cast<O *>(obj . object) ->* Method)(ctxt, arg);
}

template<typename O, typename A, void (O::*Method)(MCExecContext&, uint32_t, A)> inline void MCPropertyObjectPartThunk(MCExecContext& ctxt, MCObjectPtr obj, A arg)
{
	(static_cast<O *>(obj . object) ->* Method)(ctxt, obj . part_id, arg);
}

template<typename O, typename A, void (O::*Method)(MCExecContext&, MCNameRef, A)> inline void MCPropertyObjectArrayThunk(MCExecContext& ctxt, MCObjectPtr obj, MCNameRef name, A arg)
{
	(static_cast<O *>(obj . object) ->* Method)(ctxt, name, arg);
}

template<typename O, typename A, typename B, void (O::*Method)(MCExecContext&, B, A)> inline void MCPropertyObjectListThunk(MCExecContext& ctxt, MCObjectPtr obj, B count, A arg)
{
	(static_cast<O *>(obj . object) ->* Method)(ctxt, count, arg);
}

#define MCPropertyObjectThunkImp(obj, mth, typ) (void(*)(MCExecContext&,MCObjectPtr,typ))MCPropertyObjectThunk<obj,typ,&obj::mth>
#define MCPropertyObjectPartThunkImp(obj, mth, typ) (void(*)(MCExecContext&,MCObjectPtr,typ))MCPropertyObjectPartThunk<obj,typ,&obj::mth>
#define MCPropertyObjectArrayThunkImp(obj, mth, name, typ) (void(*)(MCExecContext&,MCObjectPtr,MCNameRef,typ))MCPropertyObjectArrayThunk<obj,typ,&obj::mth>
#define MCPropertyObjectListThunkImp(obj, mth, count, typ) (void(*)(MCExecContext&,MCObjectPtr,count,typ))MCPropertyObjectListThunk<obj,typ,count,&obj::mth>

#define MCPropertyObjectThunkGetAny(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCValueRef&)
#define MCPropertyObjectThunkGetBool(obj, mth) MCPropertyObjectThunkImp(obj, mth, bool&)
#define MCPropertyObjectThunkGetInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, integer_t&)
#define MCPropertyObjectThunkGetInt32(obj, mth) MCPropertyObjectThunkImp(obj, mth, integer_t&)
#define MCPropertyObjectThunkGetUInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t&)
#define MCPropertyObjectThunkGetUInt32(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t&)
#define MCPropertyObjectThunkGetOptionalInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, integer_t*&)
#define MCPropertyObjectThunkGetOptionalUInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t*&)
#define MCPropertyObjectThunkGetOptionalUInt32(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t*&)
#define MCPropertyObjectThunkGetDouble(obj, mth) MCPropertyObjectThunkImp(obj, mth, double&)
#define MCPropertyObjectThunkGetString(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCStringRef&)
#define MCPropertyObjectThunkGetBinaryString(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCDataRef&)
#define MCPropertyObjectThunkGetOptionalString(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCStringRef&)
#define MCPropertyObjectThunkGetRectangle(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCRectangle&)
#define MCPropertyObjectThunkGetOptionalRectangle(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCRectangle*&)
#define MCPropertyObjectThunkGetPoint(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCPoint&)
#define MCPropertyObjectThunkGetCustomType(obj, mth, typ) MCPropertyObjectThunkImp(obj, mth, typ&)
#define MCPropertyObjectThunkGetEnumType(obj, mth) MCPropertyObjectThunkImp(obj, mth, intenum_t&)
#define MCPropertyObjectThunkGetSetType(obj, mth) MCPropertyObjectThunkImp(obj, mth, intset_t&)
#define MCPropertyObjectThunkGetOptionalCustomType(obj, mth, typ) MCPropertyObjectThunkImp(obj, mth, typ*&)
#define MCPropertyObjectThunkGetOptionalEnumType(obj, mth) MCPropertyObjectThunkImp(obj, mth, intenum_t*&)
#define MCPropertyObjectThunkGetArray(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCArrayRef&)

#define MCPropertyObjectListThunkGetLinesOfString(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t&, MCStringRef*&)
#define MCPropertyObjectListThunkGetLinesOfUInt(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t&, uinteger_t*&)
#define MCPropertyObjectListThunkGetLinesOfPoint(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t&, MCPoint*&)
#define MCPropertyObjectListThunkGetItemsOfUInt(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t&, uinteger_t*&)

#define MCPropertyObjectThunkSetAny(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCValueRef)
#define MCPropertyObjectThunkSetBool(obj, mth) MCPropertyObjectThunkImp(obj, mth, bool)
#define MCPropertyObjectThunkSetInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, integer_t)
#define MCPropertyObjectThunkSetInt32(obj, mth) MCPropertyObjectThunkImp(obj, mth, integer_t)
#define MCPropertyObjectThunkSetUInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t)
#define MCPropertyObjectThunkSetUInt32(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t)
#define MCPropertyObjectThunkSetOptionalInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, integer_t*)
#define MCPropertyObjectThunkSetOptionalUInt16(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t*)
#define MCPropertyObjectThunkSetOptionalUInt32(obj, mth) MCPropertyObjectThunkImp(obj, mth, uinteger_t*)
#define MCPropertyObjectThunkSetDouble(obj, mth) MCPropertyObjectThunkImp(obj, mth, double)
#define MCPropertyObjectThunkSetString(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCStringRef)
#define MCPropertyObjectThunkSetBinaryString(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCDataRef)
#define MCPropertyObjectThunkSetOptionalString(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCStringRef)
#define MCPropertyObjectThunkSetRectangle(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCRectangle)
#define MCPropertyObjectThunkSetOptionalRectangle(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCRectangle*)
#define MCPropertyObjectThunkSetPoint(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCPoint)
#define MCPropertyObjectThunkSetCustomType(obj, mth, typ) MCPropertyObjectThunkImp(obj, mth, const typ&)
#define MCPropertyObjectThunkSetEnumType(obj, mth) MCPropertyObjectThunkImp(obj, mth, intenum_t)
#define MCPropertyObjectThunkSetSetType(obj, mth) MCPropertyObjectThunkImp(obj, mth, intset_t)
#define MCPropertyObjectThunkSetOptionalCustomType(obj, mth, typ) MCPropertyObjectThunkImp(obj, mth, const typ*&)
#define MCPropertyObjectThunkSetOptionalEnumType(obj, mth) MCPropertyObjectThunkImp(obj, mth, intenum_t*)
#define MCPropertyObjectThunkSetArray(obj, mth) MCPropertyObjectThunkImp(obj, mth, MCArrayRef)

#define MCPropertyObjectListThunkSetLinesOfString(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t, MCStringRef*)
#define MCPropertyObjectListThunkSetLinesOfUInt(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t, uinteger_t*)
#define MCPropertyObjectListThunkSetLinesOfPoint(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t, MCPoint*)
#define MCPropertyObjectListThunkSetItemsOfUInt(obj, mth) MCPropertyObjectListThunkImp(obj, mth, uindex_t, uinteger_t*)

#define MCPropertyObjectPartThunkGetAny(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCValueRef&)
#define MCPropertyObjectPartThunkGetBool(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, bool&)
#define MCPropertyObjectPartThunkGetInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, integer_t&)
#define MCPropertyObjectPartThunkGetInt32(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, integer_t&)
#define MCPropertyObjectPartThunkGetUInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t&)
#define MCPropertyObjectPartThunkGetUInt32(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t&)
#define MCPropertyObjectPartThunkGetOptionalInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, integer_t*&)
#define MCPropertyObjectPartThunkGetOptionalUInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t*&)
#define MCPropertyObjectPartThunkGetOptionalUInt32(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t*&)
#define MCPropertyObjectPartThunkGetDouble(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, double&)
#define MCPropertyObjectPartThunkGetString(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCStringRef&)
#define MCPropertyObjectPartThunkGetOptionalString(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCStringRef&)
#define MCPropertyObjectPartThunkGetRectangle(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCRectangle&)
#define MCPropertyObjectPartThunkGetOptionalRectangle(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCRectangle*&)
#define MCPropertyObjectPartThunkGetPoint(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCPoint&)
#define MCPropertyObjectPartThunkGetCustomType(obj, mth, typ) MCPropertyObjectPartThunkImp(obj, mth, typ&)
#define MCPropertyObjectPartThunkGetEnumType(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, intenum_t&)
#define MCPropertyObjectPartThunkGetSetType(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, intset_t&)
#define MCPropertyObjectPartThunkGetOptionalCustomType(obj, mth, typ) MCPropertyObjectPartThunkImp(obj, mth, typ*&)
#define MCPropertyObjectPartThunkGetOptionalEnumType(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, intenum_t*&)
#define MCPropertyObjectPartThunkGetArray(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCArrayRef&)

#define MCPropertyObjectPartThunkSetAny(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCValueRef)
#define MCPropertyObjectPartThunkSetBool(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, bool)
#define MCPropertyObjectPartThunkSetInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, integer_t)
#define MCPropertyObjectPartThunkSetInt32(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, integer_t)
#define MCPropertyObjectPartThunkSetUInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t)
#define MCPropertyObjectPartThunkSetUInt32(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t)
#define MCPropertyObjectPartThunkSetOptionalInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, integer_t*)
#define MCPropertyObjectPartThunkSetOptionalUInt16(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t*)
#define MCPropertyObjectPartThunkSetOptionalUInt32(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, uinteger_t*)
#define MCPropertyObjectPartThunkSetDouble(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, double)
#define MCPropertyObjectPartThunkSetString(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCStringRef)
#define MCPropertyObjectPartThunkSetOptionalString(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCStringRef)
#define MCPropertyObjectPartThunkSetRectangle(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCRectangle)
#define MCPropertyObjectPartThunkSetOptionalRectangle(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCRectangle*)
#define MCPropertyObjectPartThunkSetPoint(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCPoint)
#define MCPropertyObjectPartThunkSetCustomType(obj, mth, typ) MCPropertyObjectPartThunkImp(obj, mth, const typ&)
#define MCPropertyObjectPartThunkSetEnumType(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, intenum_t)
#define MCPropertyObjectPartThunkSetSetType(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, intset_t)
#define MCPropertyObjectPartThunkSetOptionalCustomType(obj, mth, typ) MCPropertyObjectPartThunkImp(obj, mth, const typ*&)
#define MCPropertyObjectPartThunkSetOptionalEnumType(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, intenum_t*)
#define MCPropertyObjectPartThunkSetArray(obj, mth) MCPropertyObjectPartThunkImp(obj, mth, MCArrayRef)


#define MCPropertyObjectArrayThunkGetAny(obj, mth) MCPropertyObjectArrayThunkImp(obj, mth, MCNameRef, MCValueRef&)
#define MCPropertyObjectArrayThunkGetBool(obj, mth) MCPropertyObjectArrayThunkImp(obj, mth, MCNameRef, bool&)
#define MCPropertyObjectArrayThunkGetString(obj, mth) MCPropertyObjectArrayThunkImp(obj, mth, MCNameRef, MCStringRef&)

#define MCPropertyObjectArrayThunkSetAny(obj, mth) MCPropertyObjectArrayThunkImp(obj, mth, MCNameRef, MCValueRef)
#define MCPropertyObjectArrayThunkSetBool(obj, mth) MCPropertyObjectArrayThunkImp(obj, mth, MCNameRef, bool)
#define MCPropertyObjectArrayThunkSetString(obj, mth) MCPropertyObjectArrayThunkImp(obj, mth, MCNameRef, MCStringRef)
//////////

#define DEFINE_RW_PROPERTY(prop, type, module, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MC##module##Get##tag, (void *)MC##module##Set##tag },

#define DEFINE_RW_SET_PROPERTY(prop, typeinfo, module, tag) \
{ prop, false, kMCPropertyTypeSet, typeinfo, (void *)MC##module##Get##tag, (void *)MC##module##Set##tag },

#define DEFINE_RW_ENUM_PROPERTY(prop, typeinfo, module, tag) \
{ prop, false, kMCPropertyTypeEnum, typeinfo, (void *)MC##module##Get##tag, (void *)MC##module##Set##tag },

#define DEFINE_RW_CUSTOM_PROPERTY(prop, typeinfo, module, tag) \
{ prop, false, kMCPropertyTypeCustom, typeinfo, (void *)MC##module##Get##tag, (void *)MC##module##Set##tag },

#define DEFINE_RO_PROPERTY(prop, type, module, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MC##module##Get##tag, nil },

#define DEFINE_RO_SET_PROPERTY(prop, typeinfo, module, tag) \
{ prop, false, kMCPropertyTypeSet, typeinfo, (void *)MC##module##Get##tag, nil },

#define DEFINE_RO_ENUM_PROPERTY(prop, typeinfo, module, tag) \
{ prop, false, kMCPropertyTypeEnum, typeinfo, (void *)MC##module##Get##tag, nil },

#define DEFINE_RO_CUSTOM_PROPERTY(prop, typeinfo, module, tag) \
{ prop, false, kMCPropertyTypeCustom, typeinfo, (void *)MC##module##Get##tag, nil },

#define DEFINE_RO_EFFECTIVE_PROPERTY(prop, type, module, tag) \
{ prop, true, kMCPropertyType##type, nil, (void *)MC##module##GetEffective##tag, nil },


#define DEFINE_RW_OBJ_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectThunkGet##type(obj, Get##tag), (void *)MCPropertyObjectThunkSet##type(obj, Set##tag), false, false },

#define DEFINE_RO_OBJ_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectThunkGet##type(obj, Get##tag), nil, false, false },

#define DEFINE_RW_OBJ_NON_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectThunkGet##type(obj, Get##tag), (void *)MCPropertyObjectThunkSet##type(obj, Set##tag), true, false },

#define DEFINE_RO_OBJ_NON_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectThunkGet##type(obj, Get##tag), nil, true, false },

#define DEFINE_RW_OBJ_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyType##type, nil, (void *)MCPropertyObjectThunkGet##type(obj, GetEffective##tag), (void *)MCPropertyObjectThunkSet##type(obj, SetEffective##tag), true, false },

#define DEFINE_RO_OBJ_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyType##type, nil, (void *)MCPropertyObjectThunkGet##type(obj, GetEffective##tag), nil, true, false },

#define DEFINE_RW_OBJ_PART_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectPartThunkGet##type(obj, Get##tag), (void *)MCPropertyObjectPartThunkSet##type(obj, Set##tag), false, false },

#define DEFINE_RO_OBJ_PART_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectPartThunkGet##type(obj, Get##tag), nil, false, false },

#define DEFINE_RW_OBJ_PART_NON_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectPartThunkGet##type(obj, Get##tag), (void *)MCPropertyObjectPartThunkSet##type(obj, Set##tag), true, false },

#define DEFINE_RO_OBJ_PART_NON_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectPartThunkGet##type(obj, Get##tag), nil, true, false },

#define DEFINE_RW_OBJ_PART_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyType##type, nil, (void *)MCPropertyObjectPartThunkGet##type(obj, GetEffective##tag), (void *)MCPropertyObjectPartThunkSet##type(obj, SetEffective##tag), true, false },

#define DEFINE_RO_OBJ_PART_EFFECTIVE_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyType##type, nil, (void *)MCPropertyObjectPartThunkGet##type(obj, GetEffective##tag), nil, true, false },

#define DEFINE_RW_OBJ_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetCustomType(obj, Get##tag, MC##type), (void *)MCPropertyObjectThunkSetCustomType(obj, Set##tag, MC##type), false, false },

#define DEFINE_RO_OBJ_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetCustomType(obj, Get##tag, MC##type), nil, false, false },

#define DEFINE_RW_OBJ_NON_EFFECTIVE_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetCustomType(obj, Get##tag, MC##type), (void *)MCPropertyObjectThunkSetCustomType(obj, Set##tag, MC##type), true, false },

#define DEFINE_RO_OBJ_NON_EFFECTIVE_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetCustomType(obj, Get##tag, MC##type), nil, true, false },

#define DEFINE_RW_OBJ_EFFECTIVE_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetCustomType(obj, Get##tag, MC##type), (void *)MCPropertyObjectThunkSetCustomType(obj, Set##tag, MC##type), true, false },

#define DEFINE_RO_OBJ_EFFECTIVE_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetCustomType(obj, Get##tag, MC##type), nil, true, false },

#define DEFINE_RW_OBJ_PART_CUSTOM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeCustom, kMC##type##TypeInfo, (void *)MCPropertyObjectPartThunkGetCustomType(obj, Get##tag, MC##type), (void *)MCPropertyObjectPartThunkSetCustomType(obj, Set##tag, MC##type), false, false },

#define DEFINE_RO_OBJ_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetEnumType(obj, Get##tag), nil, false, false },

#define DEFINE_RW_OBJ_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetEnumType(obj, Get##tag), (void *)MCPropertyObjectThunkSetEnumType(obj, Set##tag), false, false },

#define DEFINE_RO_OBJ_PART_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectPartThunkGetEnumType(obj, Get##tag), nil, false, false },

#define DEFINE_RW_OBJ_OPTIONAL_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeOptionalEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetOptionalEnumType(obj, Get##tag), (void *)MCPropertyObjectThunkSetOptionalEnumType(obj, Set##tag), false, false },

#define DEFINE_RW_OBJ_NON_EFFECTIVE_OPTIONAL_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeOptionalEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetOptionalEnumType(obj, Get##tag), (void *)MCPropertyObjectThunkSetOptionalEnumType(obj, Set##tag), true, false },

#define DEFINE_RW_OBJ_NON_EFFECTIVE_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetEnumType(obj, Get##tag), (void *)MCPropertyObjectThunkSetEnumType(obj, Set##tag), true, false },

#define DEFINE_RO_OBJ_EFFECTIVE_ENUM_PROPERTY(prop, type, obj, tag) \
{ prop, true, kMCPropertyTypeEnum, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetEnumType(obj, GetEffective##tag), nil, true, false },

#define DEFINE_UNAVAILABLE_OBJ_PROPERTY(prop) \
{ prop, false, kMCPropertyTypeAny, nil, nil, nil, false, false },

#define DEFINE_RW_OBJ_SET_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeSet, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetSetType(obj, Get##tag), (void *)MCPropertyObjectThunkSetSetType(obj, Set##tag), false, false },

#define DEFINE_RO_OBJ_SET_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyTypeSet, kMC##type##TypeInfo, (void *)MCPropertyObjectThunkGetSetType(obj, Get##tag), nil, false, false },

#define DEFINE_RW_OBJ_ARRAY_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectArrayThunkGet##type(obj, Get##tag), (void *)MCPropertyObjectArrayThunkSet##type(obj, Set##tag), false, true },

#define DEFINE_RW_OBJ_LIST_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectListThunkGet##type(obj, Get##tag), (void *)MCPropertyObjectListThunkSet##type(obj, Set##tag), false, false },

#define DEFINE_RO_OBJ_LIST_PROPERTY(prop, type, obj, tag) \
{ prop, false, kMCPropertyType##type, nil, (void *)MCPropertyObjectListThunkGet##type(obj, Get##tag), nil, false, false },

////////////////////////////////////////////////////////////////////////////////

class MCExecContext
{
public:
	MCExecContext(MCExecPoint& ep)
		: m_ep(ep), m_stat(ES_NORMAL)
	{
	}
	
	MCExecContext(MCExecPoint& ep, MCVarref *p_it)
		: m_ep(ep), m_stat(ES_NORMAL), m_it(p_it)
	{
	}
	
	MCExecContext(MCExecContext& p_ctxt)
		: m_ep(p_ctxt.GetEP()), m_stat(ES_NORMAL), m_it(p_ctxt . m_it)
	{
	}

	//////////

	MCExecPoint& GetEP(void)
	{
		return m_ep;
	}
	
	Exec_stat GetExecStat(void)
	{
		return m_stat;
	}

	///////////

	bool HasError(void)
	{
		return m_stat != ES_NORMAL;
	}

	void Throw(void)
	{
		m_stat = ES_ERROR;
	}

	Exec_stat Catch(uint2 line, uint2 pos);

	void LegacyThrow(Exec_errors error, MCValueRef hint = nil);
	void LegacyThrow(Exec_errors error, uint32_t hint);

	void Unimplemented(void)
	{
		abort();
	}

	//////////

	bool GetCaseSensitive(void) const
	{
		return m_ep . getcasesensitive() == True;
	}

	bool GetConvertOctals(void) const
	{
		return m_ep . getconvertoctals() == True;
	}

	bool GetWholeMatches(void) const
	{
		return m_ep . getwholematches() == True;
	}

	bool GetUseUnicode(void) const
	{
		return m_ep . getuseunicode() == True;
	}

	bool GetUseSystemDate(void) const
	{
		return m_ep . getusesystemdate() == True;
	}

	char_t GetLineDelimiter(void) const
	{
		return m_ep . getlinedel();
	}

	char_t GetItemDelimiter(void) const
	{
		return m_ep . getitemdel();
	}

	char_t GetColumnDelimiter(void) const
	{
		return m_ep . getcolumndel();
	}

	char_t GetRowDelimiter(void) const
	{
		return m_ep . getrowdel();
	}

	uint2 GetCutOff(void) const
	{
		return m_ep . getcutoff();
	}
	//////////

	void SetCaseSensitive(bool p_value)
	{
		m_ep . setcasesensitive(p_value);
	}

	void SetConvertOctals(bool p_value)
	{
		m_ep . setconvertoctals(p_value);
	}
	
	void SetWholeMatches(bool p_value)
	{
		m_ep . setwholematches(p_value);
	}
	
	void SetUseUnicode(bool p_value)
	{
		m_ep . setuseunicode(p_value);
	}

	void SetUseSystemDate(bool p_value)
	{
		m_ep . setusesystemdate(p_value);
	}

	void SetCutOff(uint2 p_value)
	{
		m_ep . setcutoff(p_value);
	}

	void SetLineDelimiter(char_t p_value)
	{
		m_ep . setlinedel(p_value);
	}

	void SetItemDelimiter(char_t p_value)
	{
		m_ep . setitemdel(p_value);
	}

	void SetColumnDelimiter(char_t p_value)
	{
		m_ep . setcolumndel(p_value);
	}

	void SetRowDelimiter(char_t p_value)
	{
		m_ep . setrowdel(p_value);
	}
	//////////
	
	// Convert the given valueref to a string. If the type is not convertable
	// to a string, the empty string is returned.
	// This method should be used in cases where a string is required and
	// we want to silently convert non-stringables to empty.
	bool ForceToString(MCValueRef value, MCStringRef& r_string);
	bool ForceToBoolean(MCValueRef value, MCBooleanRef& r_boolean);

	// These attempt to convert the value as specified, returning 'true' if successeful.
	// These will raise an appropriate error if the conversion fails and
	// strict mode is on. If strict mode is off (the default) then default
	// values will be returned for things that can't be converted.
	//   (boolean - false, string - empty, number/integer/real - 0, array -
	//    empty array).
	bool ConvertToBoolean(MCValueRef value, MCBooleanRef& r_boolean);
	bool ConvertToString(MCValueRef value, MCStringRef& r_string);
	bool ConvertToNumber(MCValueRef value, MCNumberRef& r_number);
	bool ConvertToInteger(MCValueRef value, integer_t& r_integer);
	bool ConvertToUnsignedInteger(MCValueRef value, uinteger_t& r_integer);
	bool ConvertToReal(MCValueRef value, real64_t& r_real);
	bool ConvertToArray(MCValueRef value, MCArrayRef& r_array);
	bool ConvertToLegacyPoint(MCValueRef value, MCPoint& r_point);
	bool ConvertToLegacyRectangle(MCValueRef value, MCRectangle& r_rectangle);
	bool ConvertToLegacyColor(MCValueRef value, MCColor& r_color);

	// These attempt to convert the given value as specified. If conversion
	// was successful then 'r_converted' is set to true, else 'false'. If
	// an error occurs (such as out-of-memory), false is returned.
	bool TryToConvertToBoolean(MCValueRef value, bool& r_converted, MCBooleanRef& r_boolean);
	bool TryToConvertToString(MCValueRef value, bool& r_converted, MCStringRef& r_string);
	bool TryToConvertToNumber(MCValueRef value, bool& r_converted, MCNumberRef& r_number);
	bool TryToConvertToInteger(MCValueRef value, bool& r_converted, integer_t& r_integer);
	bool TryToConvertToUnsignedInteger(MCValueRef value, bool& r_converted, uinteger_t& r_integer);
	bool TryToConvertToReal(MCValueRef value, bool& r_converted, real64_t& r_real);
	bool TryToConvertToArray(MCValueRef value, bool& r_converted, MCArrayRef& r_array);
	bool TryToConvertToLegacyPoint(MCValueRef value, bool& r_converted, MCPoint& r_point);
	bool TryToConvertToLegacyRectangle(MCValueRef value, bool& r_converted, MCRectangle& r_rectangle);
	bool TryToConvertToLegacyColor(MCValueRef value, bool& r_converted, MCColor& r_color);

	//////////
	
	bool FormatBool(bool p_bool, MCStringRef& r_output);
	bool FormatReal(real64_t p_real, MCStringRef& r_output);
	bool FormatUnsignedInteger(uinteger_t p_integer, MCStringRef& r_output);
	bool FormatInteger(integer_t p_integer, MCStringRef& r_output);
	bool FormatLegacyPoint(MCPoint value, MCStringRef& r_value);
	bool FormatLegacyRectangle(MCRectangle value, MCStringRef& r_value);
	bool FormatLegacyColor(MCColor value, MCStringRef& r_value);

	//////////
	
	bool EncodeStringAsUTF8(MCStringRef string, char*& r_cstring);
	bool EncodeUnicodeStringAsUTF8(MCStringRef string, char*& r_cstring);

	//////////

	// This method evaluates the given expression returning the result in 'result'.
	// If an error is raised during the course of evaluation, 'false' is returned.
	// Note: This method throws any errors that occur.
	bool EvaluateExpression(MCExpression *expr, MCValueRef& r_result);

	//////////
	
	// Note: This method throws any errors that occur.
	bool EnsurePrintingIsAllowed(void);
	bool EnsureDiskAccessIsAllowed(void);
	bool EnsureProcessIsAllowed(void);
	bool EnsureNetworkAccessIsAllowed(void);
	bool EnsurePrivacyIsAllowed(void);

	//////////
	
	void SetItToEmpty(void);
	void SetItToValue(MCValueRef p_value);
	
	//////////

    MCHandler *GetHandler(void)
    {
        return m_ep . gethandler();
    }
    
	MCObject *GetObject(void)
	{
		return m_ep . getobj();
	}

	void SetObject(MCObject *p_object)
	{
		m_ep.setobj(p_object);
	}

	void SetParentScript(MCParentScriptUse *p_parentscript)
	{
		m_ep.setparentscript(p_parentscript);
	}

    MCParentScriptUse *GetParentScript(void)
	{
		return m_ep.getparentscript();
	}
    
    // MM-2011-02-16: Added ability to get handle of current object
    MCObjectHandle *GetObjectHandle(void);
	void SetTheResultToEmpty(void);
	void SetTheResultToValue(MCValueRef p_value);
	void SetTheResultToStaticCString(const char *p_cstring);
    void SetTheResultToNumber(real64_t p_value);
    void GiveCStringToResult(char *p_cstring);
    void SetTheResultToCString(const char *p_string);
    
private:
	MCExecPoint& m_ep;
	Exec_stat m_stat;
	MCVarref *m_it;
};

////////////////////////////////////////////////////////////////////////////////

extern MCExecMethodInfo *kMCLogicEvalIsEqualToMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsNotEqualToMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsGreaterThanMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsGreaterThanOrEqualToMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsLessThanMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsLessThanOrEqualToMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalAndMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalOrMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalNotMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsABooleanMethodInfo;
extern MCExecMethodInfo *kMCLogicEvalIsNotABooleanMethodInfo;

void MCLogicEvalIsEqualTo(MCExecContext& ctxt, MCValueRef p_left, MCValueRef p_right, bool& r_result);
void MCLogicEvalIsNotEqualTo(MCExecContext& ctxt, MCValueRef p_left, MCValueRef p_right, bool& r_result);
void MCLogicEvalIsGreaterThan(MCExecContext& ctxt, MCValueRef p_left, MCValueRef p_right, bool& r_result);
void MCLogicEvalIsGreaterThanOrEqualTo(MCExecContext& ctxt, MCValueRef p_left, MCValueRef p_right, bool& r_result);
void MCLogicEvalIsLessThan(MCExecContext& ctxt, MCValueRef p_left, MCValueRef p_right, bool& r_result);
void MCLogicEvalIsLessThanOrEqualTo(MCExecContext& ctxt, MCValueRef p_left, MCValueRef p_right, bool& r_result);

void MCLogicEvalAnd(MCExecContext& ctxt, bool p_a, bool p_b, bool& r_result);
void MCLogicEvalOr(MCExecContext& ctxt, bool p_a, bool p_b, bool& r_result);
void MCLogicEvalNot(MCExecContext& ctxt, bool p_bool, bool& r_result);

void MCLogicEvalIsABoolean(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCLogicEvalIsNotABoolean(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);

///////////

extern MCExecMethodInfo *kMCArraysEvalKeysMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalExtentsMethodInfo;
extern MCExecMethodInfo *kMCArraysExecCombineMethodInfo;
extern MCExecMethodInfo *kMCArraysExecCombineByRowMethodInfo;
extern MCExecMethodInfo *kMCArraysExecCombineByColumnMethodInfo;
extern MCExecMethodInfo *kMCArraysExecCombineAsSetMethodInfo;
extern MCExecMethodInfo *kMCArraysExecSplitMethodInfo;
extern MCExecMethodInfo *kMCArraysExecSplitByRowMethodInfo;
extern MCExecMethodInfo *kMCArraysExecSplitByColumnMethodInfo;
extern MCExecMethodInfo *kMCArraysExecSplitAsSetMethodInfo;
extern MCExecMethodInfo *kMCArraysExecUnionMethodInfo;
extern MCExecMethodInfo *kMCArraysExecIntersectMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalArrayEncodeMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalArrayDecodeMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalMatrixMultiplyMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalTransposeMatrixMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalIsAnArrayMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalIsNotAnArrayMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalIsAmongTheKeysOfMethodInfo;
extern MCExecMethodInfo *kMCArraysEvalIsNotAmongTheKeysOfMethodInfo;

void MCArraysEvalKeys(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef& r_string);
void MCArraysEvalExtents(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef& r_string);
void MCArraysExecCombine(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef p_element_delimiter, MCStringRef p_key_delimiter, MCStringRef& r_string);
void MCArraysExecCombineByRow(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef& r_string);
void MCArraysExecCombineByColumn(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef& r_string);
void MCArraysExecCombineAsSet(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef p_element_delimiter, MCStringRef& r_string);
void MCArraysExecSplit(MCExecContext& ctxt, MCStringRef p_string, MCStringRef p_element_delimiter, MCStringRef p_key_delimiter, MCArrayRef& r_array);
void MCArraysExecSplitByRow(MCExecContext& ctxt, MCStringRef p_string, MCArrayRef& r_array);
void MCArraysExecSplitByColumn(MCExecContext& ctxt, MCStringRef p_string, MCArrayRef& r_array);
void MCArraysExecSplitAsSet(MCExecContext& ctxt, MCStringRef p_string, MCStringRef p_element_delimiter, MCArrayRef& r_array);
void MCArraysExecUnion(MCExecContext& ctxt, MCArrayRef x_dst_array, MCArrayRef p_src_array);
void MCArraysExecIntersect(MCExecContext& ctxt, MCArrayRef x_dst_array, MCArrayRef p_src_array);
void MCArraysEvalArrayEncode(MCExecContext& ctxt, MCArrayRef p_array, MCStringRef& r_encoding);
void MCArraysEvalArrayDecode(MCExecContext& ctxt, MCStringRef p_encoding, MCArrayRef& r_array);
void MCArraysEvalMatrixMultiply(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCArraysEvalTransposeMatrix(MCExecContext& ctxt, MCArrayRef p_matrix, MCArrayRef& r_result);

void MCArraysEvalIsAnArray(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCArraysEvalIsNotAnArray(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCArraysEvalIsAmongTheKeysOf(MCExecContext& ctxt, MCNameRef p_key, MCArrayRef p_array, bool& r_result);
void MCArraysEvalIsNotAmongTheKeysOf(MCExecContext& ctxt, MCNameRef p_key, MCArrayRef p_array, bool& r_result);

///////////

extern MCExecMethodInfo *kMCMathEvalBaseConvertMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAbsMethodInfo;
extern MCExecMethodInfo *kMCMathEvalRoundToPrecisionMethodInfo;
extern MCExecMethodInfo *kMCMathEvalRoundMethodInfo;
extern MCExecMethodInfo *kMCMathEvalStatRoundToPrecisionMethodInfo;
extern MCExecMethodInfo *kMCMathEvalStatRoundMethodInfo;
extern MCExecMethodInfo *kMCMathEvalTruncMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAcosMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAsinMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAtanMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAtan2MethodInfo;
extern MCExecMethodInfo *kMCMathEvalCosMethodInfo;
extern MCExecMethodInfo *kMCMathEvalSinMethodInfo;
extern MCExecMethodInfo *kMCMathEvalTanMethodInfo;
extern MCExecMethodInfo *kMCMathEvalExpMethodInfo;
extern MCExecMethodInfo *kMCMathEvalExp1MethodInfo;
extern MCExecMethodInfo *kMCMathEvalExp2MethodInfo;
extern MCExecMethodInfo *kMCMathEvalExp10MethodInfo;
extern MCExecMethodInfo *kMCMathEvalLnMethodInfo;
extern MCExecMethodInfo *kMCMathEvalLn1MethodInfo;
extern MCExecMethodInfo *kMCMathEvalLog2MethodInfo;
extern MCExecMethodInfo *kMCMathEvalLog10MethodInfo;
extern MCExecMethodInfo *kMCMathEvalSqrtMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAnnuityMethodInfo;
extern MCExecMethodInfo *kMCMathEvalCompoundMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAverageMethodInfo;
extern MCExecMethodInfo *kMCMathEvalMedianMethodInfo;
extern MCExecMethodInfo *kMCMathEvalMinMethodInfo;
extern MCExecMethodInfo *kMCMathEvalMaxMethodInfo;
extern MCExecMethodInfo *kMCMathEvalStdDevMethodInfo;
extern MCExecMethodInfo *kMCMathEvalSumMethodInfo;
extern MCExecMethodInfo *kMCMathEvalRandomMethodInfo;
extern MCExecMethodInfo *kMCMathEvalBitwiseAndMethodInfo;
extern MCExecMethodInfo *kMCMathEvalBitwiseNotMethodInfo;
extern MCExecMethodInfo *kMCMathEvalBitwiseOrMethodInfo;
extern MCExecMethodInfo *kMCMathEvalBitwiseXorMethodInfo;
extern MCExecMethodInfo *kMCMathEvalDivMethodInfo;
extern MCExecMethodInfo *kMCMathEvalDivArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathEvalDivArrayByArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalSubtractMethodInfo;
extern MCExecMethodInfo *kMCMathEvalSubtractNumberFromArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalSubtractArrayFromArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalModMethodInfo;
extern MCExecMethodInfo *kMCMathEvalModArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathEvalModArrayByArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalWrapMethodInfo;
extern MCExecMethodInfo *kMCMathEvalWrapArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathEvalWrapArrayByArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalOverMethodInfo;
extern MCExecMethodInfo *kMCMathEvalOverArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathEvalOverArrayByArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAddMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAddNumberToArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalAddArrayToArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalMultiplyMethodInfo;
extern MCExecMethodInfo *kMCMathEvalMultiplyArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathEvalMultiplyArrayByArrayMethodInfo;
extern MCExecMethodInfo *kMCMathEvalPowerMethodInfo;
extern MCExecMethodInfo *kMCMathEvalIsAnIntegerMethodInfo;
extern MCExecMethodInfo *kMCMathEvalIsNotAnIntegerMethodInfo;
extern MCExecMethodInfo *kMCMathEvalIsANumberMethodInfo;
extern MCExecMethodInfo *kMCMathEvalIsNotANumberMethodInfo;
extern MCExecMethodInfo *kMCMathGetRandomSeedMethodInfo;
extern MCExecMethodInfo *kMCMathSetRandomSeedMethodInfo;

extern MCExecMethodInfo *kMCMathExecDivideNumberByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathExecDivideArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathExecDivideArrayByArrayMethodInfo;
extern MCExecMethodInfo *kMCMathExecSubtractNumberFromNumberMethodInfo;
extern MCExecMethodInfo *kMCMathExecSubtractNumberFromArrayMethodInfo;
extern MCExecMethodInfo *kMCMathExecSubtractArrayFromArrayMethodInfo;
extern MCExecMethodInfo *kMCMathExecAddNumberToNumberMethodInfo;
extern MCExecMethodInfo *kMCMathExecAddNumberToArrayMethodInfo;
extern MCExecMethodInfo *kMCMathExecAddArrayToArrayMethodInfo;
extern MCExecMethodInfo *kMCMathExecMultiplyNumberByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathExecMultiplyArrayByNumberMethodInfo;
extern MCExecMethodInfo *kMCMathExecMultiplyArrayByArrayMethodInfo;

void MCMathEvalBaseConvert(MCExecContext& ctxt, MCStringRef p_source, integer_t p_source_base, integer_t p_dest_base, MCStringRef& r_result);

void MCMathEvalAbs(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalRoundToPrecision(MCExecContext& ctxt, real64_t p_number, real64_t p_precision, real64_t& r_result);
void MCMathEvalRound(MCExecContext& ctxt, real64_t p_number, real64_t& r_result);
void MCMathEvalStatRoundToPrecision(MCExecContext& ctxt, real64_t p_number, real64_t p_precision, real64_t& r_result);
void MCMathEvalStatRound(MCExecContext& ctxt, real64_t p_number, real64_t& r_result);
void MCMathEvalTrunc(MCExecContext& ctxt, real64_t p_number, real64_t& r_result);

void MCMathEvalAcos(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalAsin(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalAtan(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalAtan2(MCExecContext& ctxt, real64_t p_y, real64_t p_x, real64_t& r_result);
void MCMathEvalCos(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalSin(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalTan(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);

void MCMathEvalExp(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalExp1(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalExp2(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalExp10(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalLn(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalLn1(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalLog2(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalLog10(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);
void MCMathEvalSqrt(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);

void MCMathEvalAnnuity(MCExecContext& ctxt, real64_t p_rate, real64_t p_periods, real64_t& r_result);
void MCMathEvalCompound(MCExecContext& ctxt, real64_t p_rate, real64_t p_periods, real64_t& r_result);

void MCMathEvalAverage(MCExecContext& ctxt, real64_t *p_values, uindex_t p_count, real64_t& r_result);
void MCMathEvalMedian(MCExecContext& ctxt, real64_t *p_values, uindex_t p_count, real64_t& r_result);
void MCMathEvalMin(MCExecContext& ctxt, real64_t *p_values, uindex_t p_count, real64_t& r_result);
void MCMathEvalMax(MCExecContext& ctxt, real64_t *p_values, uindex_t p_count, real64_t& r_result);
void MCMathEvalStdDev(MCExecContext& ctxt, real64_t *p_values, uindex_t p_count, real64_t& r_result);
void MCMathEvalSum(MCExecContext& ctxt, real64_t *p_values, uindex_t p_count, real64_t& r_result);

void MCMathEvalRandom(MCExecContext& ctxt, real64_t p_in, real64_t& r_result);

void MCMathEvalBitwiseAnd(MCExecContext& ctxt, uinteger_t p_left, uinteger_t p_right, uinteger_t& r_result);
void MCMathEvalBitwiseNot(MCExecContext& ctxt, uinteger_t p_right, uinteger_t& r_result);
void MCMathEvalBitwiseOr(MCExecContext& ctxt, uinteger_t p_left, uinteger_t p_right, uinteger_t& r_result);
void MCMathEvalBitwiseXor(MCExecContext& ctxt, uinteger_t p_left, uinteger_t p_right, uinteger_t& r_result);

void MCMathEvalDiv(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalDivArrayByNumber(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalDivArrayByArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCMathEvalSubtract(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalSubtractNumberFromArray(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalSubtractArrayFromArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCMathEvalMod(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalModArrayByNumber(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalModArrayByArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCMathEvalWrap(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalWrapArrayByNumber(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalWrapArrayByArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCMathEvalOver(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalOverArrayByNumber(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalOverArrayByArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCMathEvalAdd(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalAddNumberToArray(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalAddArrayToArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);
void MCMathEvalMultiply(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);
void MCMathEvalMultiplyArrayByNumber(MCExecContext& ctxt, MCArrayRef p_array, real64_t p_number, MCArrayRef& r_result);
void MCMathEvalMultiplyArrayByArray(MCExecContext& ctxt, MCArrayRef p_left, MCArrayRef p_right, MCArrayRef& r_result);

void MCMathEvalPower(MCExecContext& ctxt, real64_t p_left, real64_t p_right, real64_t& r_result);

void MCMathEvalIsAnInteger(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCMathEvalIsNotAnInteger(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCMathEvalIsANumber(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCMathEvalIsNotANumber(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);

void MCMathGetRandomSeed(MCExecContext& ctxt, integer_t& r_value);
void MCMathSetRandomSeed(MCExecContext& ctxt, integer_t p_value);

#define MCMathExecAddNumberToNumber(ctxt, a, b, r) MCMathEvalAdd(ctxt, a, b, r)
#define MCMathExecAddNumberToArray(ctxt, a, b, r) MCMathEvalAddNumberToArray(ctxt, b, a, r)
#define MCMathExecAddArrayToArray(ctxt, a, b, r) MCMathEvalAddArrayToArray(ctxt, a, b, r)
#define MCMathExecSubtractNumberFromNumber(ctxt, a, b, r) MCMathEvalSubtract(ctxt, b, a, r)
#define MCMathExecSubtractNumberFromArray(ctxt, a, b, r) MCMathEvalSubtractNumberFromArray(ctxt, b, a, r)
#define MCMathExecSubtractArrayFromArray(ctxt, a, b, r) MCMathEvalSubtractArrayFromArray(ctxt, b, a, r)
#define MCMathExecDivideNumberByNumber(ctxt, a, b, r) MCMathEvalOver(ctxt, a, b, r)
#define MCMathExecDivideArrayByNumber(ctxt, a, b, r) MCMathEvalOverArrayByNumber(ctxt, a, b, r)
#define MCMathExecDivideArrayByArray(ctxt, a, b, r) MCMathEvalOverArrayByArray(ctxt, a, b, r)
#define MCMathExecMultiplyNumberByNumber(ctxt, a, b, r) MCMathEvalMultiply(ctxt, a, b, r)
#define MCMathExecMultiplyArrayByNumber(ctxt, a, b, r) MCMathEvalMultiplyArrayByNumber(ctxt, a, b, r)
#define MCMathExecMultiplyArrayByArray(ctxt, a, b, r) MCMathEvalMultiplyArrayByArray(ctxt, a, b, r)

///////////

extern MCExecMethodInfo *kMCFiltersEvalBase64EncodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalBase64DecodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalBinaryEncodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalBinaryDecodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalCompressMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalDecompressMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalIsoToMacMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalMacToIsoMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalUrlEncodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalUrlDecodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalUniEncodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalUniDecodeMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalMD5DigestMethodInfo;
extern MCExecMethodInfo *kMCFiltersEvalSHA1DigestMethodInfo;

void MCFiltersEvalBase64Encode(MCExecContext& ctxt, MCDataRef p_source, MCStringRef& r_result);
void MCFiltersEvalBase64Decode(MCExecContext& ctxt, MCStringRef p_source, MCDataRef& r_result);
void MCFiltersEvalBinaryEncode(MCExecContext& ctxt, MCStringRef p_format, MCValueRef *p_params, uindex_t p_param_count, MCDataRef& r_string);
void MCFiltersEvalBinaryDecode(MCExecContext& ctxt, MCStringRef p_format, MCDataRef p_data, MCValueRef *r_results, uindex_t p_result_count, integer_t& r_done);
void MCFiltersEvalCompress(MCExecContext& ctxt, MCDataRef p_source, MCDataRef& r_result);
void MCFiltersEvalDecompress(MCExecContext& ctxt, MCDataRef p_source, MCDataRef& r_result);
void MCFiltersEvalIsoToMac(MCExecContext& ctxt, MCDataRef p_source, MCDataRef& r_result);
void MCFiltersEvalMacToIso(MCExecContext& ctxt, MCDataRef p_source, MCDataRef& r_result);
void MCFiltersEvalUrlEncode(MCExecContext& ctxt, MCStringRef p_source, MCStringRef& r_result);
void MCFiltersEvalUrlDecode(MCExecContext& ctxt, MCStringRef p_source, MCStringRef& r_result);
void MCFiltersEvalUniEncode(MCExecContext& ctxt, MCStringRef p_src, MCNameRef p_lang, MCStringRef& r_dest);
void MCFiltersEvalUniDecode(MCExecContext& ctxt, MCStringRef p_src, MCNameRef p_lang, MCStringRef& r_dest);
void MCFiltersEvalMD5Digest(MCExecContext& ctxt, MCDataRef p_src, MCDataRef& r_digest);
void MCFiltersEvalSHA1Digest(MCExecContext& ctxt, MCDataRef p_src, MCDataRef& r_digest);

///////////

extern MCExecMethodInfo *kMCStringsEvalToLowerMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalToUpperMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalNumToCharMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalCharToNumMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalNumToByteMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalByteToNumMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalLengthMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalMatchTextMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalMatchChunkMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalReplaceTextMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalFormatMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalMergeMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalConcatenateMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalConcatenateWithSpaceMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalConcatenateWithCommaMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalContainsMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalDoesNotContainMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalBeginsWithMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalEndsWithMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsAmongTheTokensOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsNotAmongTheTokensOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsAmongTheWordsOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsNotAmongTheWordsOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsAmongTheLinesOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsNotAmongTheLinesOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsAmongTheItemsOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalIsNotAmongTheItemsOfMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalItemOffsetMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalLineOffsetMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalWordOffsetMethodInfo;
extern MCExecMethodInfo *kMCStringsEvalOffsetMethodInfo;
extern MCExecMethodInfo *kMCStringsExecReplaceMethodInfo;
extern MCExecMethodInfo *kMCStringsExecFilterMethodInfo;

void MCStringsEvalToLower(MCExecContext& ctxt, MCStringRef p_string, MCStringRef& r_lower);
void MCStringsEvalToUpper(MCExecContext& ctxt, MCStringRef p_string, MCStringRef& r_lower);

void MCStringsEvalNumToChar(MCExecContext& ctxt, integer_t codepoint, MCStringRef& r_character);
void MCStringsEvalCharToNum(MCExecContext& ctxt, MCStringRef character, MCValueRef& r_codepoint);
void MCStringsEvalNumToByte(MCExecContext& ctxt, integer_t codepoint, MCStringRef& r_byte);
void MCStringsEvalByteToNum(MCExecContext& ctxt, MCStringRef byte, integer_t& r_codepoint);

void MCStringsEvalLength(MCExecContext& ctxt, MCStringRef p_string, integer_t& r_length);

void MCStringsEvalMatchText(MCExecContext& ctxt, MCStringRef p_string, MCStringRef p_pattern, MCStringRef* r_results, uindex_t p_result_count, bool& r_match);
void MCStringsEvalMatchChunk(MCExecContext& ctxt, MCStringRef p_string, MCStringRef p_pattern, MCStringRef* r_results, uindex_t p_result_count, bool& r_match);
void MCStringsEvalReplaceText(MCExecContext& ctxt, MCStringRef p_string, MCStringRef p_pattern, MCStringRef p_replacement, MCStringRef& r_result);

void MCStringsEvalFormat(MCExecContext& ctxt, MCStringRef p_format, MCValueRef* p_params, uindex_t p_param_count, MCStringRef& r_result);
void MCStringsEvalMerge(MCExecContext& ctxt, MCStringRef p_format, MCStringRef& r_string);

void MCStringsEvalConcatenate(MCExecContext& ctxt, MCStringRef p_left, MCStringRef p_right, MCStringRef& r_result);
void MCStringsEvalConcatenateWithSpace(MCExecContext& ctxt, MCStringRef p_left, MCStringRef p_right, MCStringRef& r_result);
void MCStringsEvalConcatenateWithComma(MCExecContext& ctxt, MCStringRef p_left, MCStringRef p_right, MCStringRef& r_result);

void MCStringsEvalContains(MCExecContext& ctxt, MCStringRef p_whole, MCStringRef p_part, bool& r_result);
void MCStringsEvalDoesNotContain(MCExecContext& ctxt, MCStringRef p_whole, MCStringRef p_part, bool& r_result);
void MCStringsEvalBeginsWith(MCExecContext& ctxt, MCStringRef p_whole, MCStringRef p_part, bool& r_result);
void MCStringsEvalEndsWith(MCExecContext& ctxt, MCStringRef p_whole, MCStringRef p_part, bool& r_result);

void MCStringsEvalIsAmongTheTokensOf(MCExecContext& ctxt, MCStringRef p_token, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsNotAmongTheTokensOf(MCExecContext& ctxt, MCStringRef p_token, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsAmongTheWordsOf(MCExecContext& ctxt, MCStringRef p_word, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsNotAmongTheWordsOf(MCExecContext& ctxt, MCStringRef p_word, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsAmongTheLinesOf(MCExecContext& ctxt, MCStringRef p_line, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsNotAmongTheLinesOf(MCExecContext& ctxt, MCStringRef p_line, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsAmongTheItemsOf(MCExecContext& ctxt, MCStringRef p_item, MCStringRef p_string, bool& r_result);
void MCStringsEvalIsNotAmongTheItemsOf(MCExecContext& ctxt, MCStringRef p_item, MCStringRef p_string, bool& r_result);

void MCStringsEvalItemOffset(MCExecContext& ctxt, MCStringRef p_chunk, MCStringRef p_string, uindex_t p_start_offset, uindex_t& r_result);
void MCStringsEvalLineOffset(MCExecContext& ctxt, MCStringRef p_chunk, MCStringRef p_string, uindex_t p_start_offset, uindex_t& r_result);
void MCStringsEvalWordOffset(MCExecContext& ctxt, MCStringRef p_chunk, MCStringRef p_string, uindex_t p_start_offset, uindex_t& r_result);
void MCStringsEvalOffset(MCExecContext& ctxt, MCStringRef p_chunk, MCStringRef p_string, uindex_t p_start_offset, uindex_t& r_result);

void MCStringsExecReplace(MCExecContext& ctxt, MCStringRef p_pattern, MCStringRef p_replacement, MCStringRef p_target);

void MCStringsExecFilter(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_pattern, bool p_without, MCStringRef &r_result);

void MCStringsEvalTextChunkByRange(MCExecContext& ctxt, MCStringRef p_source, Chunk_term p_chunk_type, integer_t p_first, integer_t p_last, bool p_eval_mutable, MCStringRef& x_string);
void MCStringsEvalTextChunkByExpression(MCExecContext& ctxt, MCStringRef p_source, Chunk_term p_chunk_type, integer_t p_first, bool p_eval_mutable, MCStringRef &x_string);
void MCStringsEvalTextChunkByOrdinal(MCExecContext& ctxt, MCStringRef p_source, Chunk_term p_chunk_type, Chunk_term p_ordinal_type, bool p_eval_mutable, MCStringRef& x_string);

void MCStringsSetTextChunkByRange(MCExecContext& ctxt, MCStringRef p_source, Preposition_type p_type, Chunk_term p_chunk_type, integer_t p_first, integer_t p_last, MCStringRef& x_target);
void MCStringsSetTextChunkByExpression(MCExecContext& ctxt, MCStringRef p_source, Preposition_type p_type, Chunk_term p_chunk_type, integer_t p_first, MCStringRef& x_target);
void MCStringsSetTextChunkByOrdinal(MCExecContext& ctxt, MCStringRef p_source, Preposition_type p_type, Chunk_term p_chunk_type, Chunk_term p_ordinal_type, MCStringRef& x_target);

///////////

struct MCInterfaceBackdrop;
struct MCInterfaceNamedColor;
struct MCInterfaceImagePaletteSettings;
struct MCInterfaceVisualEffect;
struct MCInterfaceVisualEffectArgument;

extern MCExecCustomTypeInfo *kMCInterfaceNamedColorTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfacePaintCompressionTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceLookAndFeelTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceBackdropTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceProcessTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceSelectionModeTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceWindowPositionTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceWindowAlignmentTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfacePaletteSettingsTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceVisualEffectTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceVisualEffectArgumentTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceButtonIconTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceTriStateTypeInfo;

extern MCExecMethodInfo *kMCInterfaceMakeCustomImagePaletteSettingsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceMakeOptimalImagePaletteSettingsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceMakeWebSafeImagePaletteSettingsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceMakeVisualEffectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceMakeVisualEffectArgumentMethodInfo;

extern MCExecMethodInfo *kMCInterfaceEvalScreenColorsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalScreenDepthMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalScreenNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalScreenRectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalScreenLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickHMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickVMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickCharMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickTextMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickCharChunkMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickChunkMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickLineMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseClickMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseHMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseVMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseCharMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseTextMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseCharChunkMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseChunkMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseLineMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseControlMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundTextMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundChunkMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundLineMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedTextMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedTextOfMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedChunkMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedChunkOfMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedLineMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedLineOfMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedLocOfMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedImageMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCapsLockKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCommandKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalControlKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalOptionKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalShiftKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalKeysDownMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMainStacksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalOpenStacksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalStacksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalTopStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalTopStackOfMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFocusedObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalColorNamesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFlushEventsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGlobalLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalLocalLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMovingControlsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalWaitDepthMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalIntersectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalIntersectWithThresholdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalWithinMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalThereIsAnObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalThereIsNotAnObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalControlAtLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalControlAtScreenLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecBeepMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecClickCmdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCloseStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCloseDefaultStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDragMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecFocusOnNothingMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecFocusOnMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGrabMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGroupControlsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGroupSelectionMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPopToLastMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPopMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPushRecentCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPushCurrentCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPushCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPlaceGroupOnCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecRemoveGroupFromCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecResetCursorsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecResetTemplateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecRevertMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSelectEmptyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSelectAllTextOfFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSelectTextOfFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSelectTextOfButtonMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSelectObjectsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecStartEditingGroupMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecStopEditingDefaultStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecStopEditingGroupMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecStopMovingObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecTypeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUndoMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUngroupObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUngroupSelectionMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCopyObjectsToContainerMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCutObjectsToContainerMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDeleteMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDeleteObjectsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDeleteObjectChunksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDisableChunkOfButtonMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecEnableChunkOfButtonMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnhiliteChunkOfButtonMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHiliteChunkOfButtonMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDisableObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecEnableObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnhiliteObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHiliteObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSaveStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSaveStackAsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecMoveObjectBetweenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecMoveObjectAlongMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHideGroupsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHideObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHideObjectWithEffectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHideMenuBarMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecHideTaskBarMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowGroupsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowAllCardsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowMarkedCardsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowCardsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowObjectWithEffectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowMenuBarMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecShowTaskBarMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPopupButtonMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDrawerStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDrawerStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDrawerStackLegacyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecDrawerStackByNameLegacyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSheetStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSheetStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecOpenStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecOpenStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPopupStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPopupStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCreateStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCreateStackWithGroupMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCreateCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCreateControlMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecCloneMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecFindMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPutIntoObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecPutIntoFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecLockCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecLockMenusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecLockMovesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecLockRecentMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecLockScreenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecLockScreenForEffectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnlockCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnlockMenusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnlockMovesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnlockRecentMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnlockScreenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecUnlockScreenWithEffectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecImportSnapshotOfScreenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecImportSnapshotOfStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecImportSnapshotOfObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecImportAudioClipMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecImportVideoClipMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecImportImageMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportSnapshotOfScreenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportSnapshotOfScreenToFileMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportSnapshotOfStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportSnapshotOfStackToFileMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportSnapshotOfObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportSnapshotOfObjectToFileMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportImageMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecExportImageToFileMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSortCardsOfStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSortFieldMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecSortContainerMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecChooseToolMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGoCardAsModeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGoCardInWindowMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGoRecentCardMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGoCardRelativeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGoCardEndMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecGoHomeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceExecVisualEffectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDialogDataMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDialogDataMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLookAndFeelMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLookAndFeelMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetScreenMouseLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetScreenMouseLocMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBackdropMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBackdropMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBufferImagesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBufferImagesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSystemFileSelectorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSystemFileSelectorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSystemColorSelectorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSystemColorSelectorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSystemPrintSelectorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSystemPrintSelectorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetPaintCompressionMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetPaintCompressionMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBrushBackColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBrushBackColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetPenBackColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetPenBackColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBrushColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBrushColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetPenColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetPenColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBrushPatternMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBrushPatternMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetPenPatternMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetPenPatternMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetFilledMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetFilledMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetPolySidesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetPolySidesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLineSizeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLineSizeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRoundRadiusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRoundRadiusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetStartAngleMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetStartAngleMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetArcAngleMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetArcAngleMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRoundEndsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRoundEndsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDashesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDashesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRecentCardsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRecentNamesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetEditBackgroundMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetEditBackgroundMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockScreenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockScreenMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetAccentColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetAccentColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetHiliteColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetHiliteColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLinkColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLinkColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLinkHiliteColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLinkHiliteColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLinkVisitedColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLinkVisitedColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetUnderlineLinksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetUnderlineLinksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSelectGroupedControlsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSelectGroupedControlsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetIconMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetIconMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetAllowInlineInputMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetAllowInlineInputMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDragDeltaMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDragDeltaMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetStackFileTypeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetStackFileTypeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetStackFileVersionMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetStackFileVersionMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetIconMenuMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetIconMenuMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetStatusIconMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetStatusIconMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetStatusIconToolTipMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetStatusIconToolTipMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetStatusIconMenuMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetStatusIconMenuMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetProcessTypeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetProcessTypeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetShowInvisiblesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetShowInvisiblesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDefaultCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDefaultCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDefaultStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDefaultStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDefaultMenubarMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDefaultMenubarMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDragSpeedMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDragSpeedMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetMoveSpeedMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetMoveSpeedMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockCursorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockErrorsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockErrorsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockMenusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockMenusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockMessagesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockMessagesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockMovesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockMovesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetLockRecentMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetLockRecentMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetIdleRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetIdleRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetIdleTicksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetIdleTicksMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBlinkRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBlinkRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRepeatRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRepeatRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRepeatDelayMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRepeatDelayMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetTypeRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetTypeRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSyncRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSyncRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetEffectRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetEffectRateMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDoubleDeltaMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDoubleDeltaMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDoubleTimeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDoubleTimeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetTooltipDelayMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetTooltipDelayMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetNavigationArrowsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetNavigationArrowsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetExtendKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetExtendKeyMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetPointerFocusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetPointerFocusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetEmacsKeyBindingsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetEmacsKeyBindingsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRaiseMenusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRaiseMenusMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetActivatePalettesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetActivatePalettesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetHidePalettesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetHidePalettesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRaisePalettesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRaisePalettesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRaiseWindowsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRaiseWindowsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetHideBackdropMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetHideBackdropMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetDontUseNavigationServicesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetDontUseNavigationServicesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetProportionalThumbsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetProportionalThumbsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSharedMemoryMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSharedMemoryMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetScreenGammaMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetScreenGammaMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSelectionModeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSelectionModeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSelectionHandleColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSelectionHandleColorMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetWindowBoundingRectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetWindowBoundingRectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetJpegQualityMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetJpegQualityMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetRelayerGroupedControlsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetRelayerGroupedControlsMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBrushMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBrushMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetEraserMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetEraserMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSprayMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSprayMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetCenteredMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetCenteredMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetGridMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetGridMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetGridSizeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetGridSizeMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetSlicesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetSlicesMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBeepLoudnessMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBeepLoudnessMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBeepPitchMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBeepPitchMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBeepDurationMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBeepDurationMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetBeepSoundMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetBeepSoundMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetToolMethodInfo;
extern MCExecMethodInfo *kMCInterfaceSetToolMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetScreenRectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceGetScreenRectsMethodInfo;

extern MCExecMethodInfo *kMCInterfaceEvalHelpStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalHomeStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalStackByValueMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedObjectAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalTopStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalClickFieldAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedFieldAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSelectedImageAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFoundFieldAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMouseControlAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalFocusedObjectAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalBinaryStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalDefaultStackAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalStackOfStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalStackOfStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSubstackOfStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSubstackOfStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalAudioClipOfStackByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalAudioClipOfStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalAudioClipOfStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalVideoClipOfStackByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalVideoClipOfStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalVideoClipOfStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalBackgroundOfStackByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalBackgroundOfStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalBackgroundOfStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfStackByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalThisCardOfStackMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfBackgroundByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfBackgroundByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfBackgroundByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfCardByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfCardByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfCardByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfCardOrStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfGroupByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfGroupByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalGroupOfGroupByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalMenubarAsObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfGroupByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfGroupByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfGroupByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfCardByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfCardByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfCardByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalObjectOfCardOrStackByIdMethodInfo;

extern MCExecMethodInfo *kMCInterfaceEvalHomeStackAsOptionalObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalHelpStackAsOptionalObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalDefaultStackAsOptionalObjectMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalStackOfOptionalStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalStackOfOptionalStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSubstackOfOptionalStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalSubstackOfOptionalStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalOptionalStackWithBackgroundByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalOptionalStackWithBackgroundByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalOptionalStackWithBackgroundByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfOptionalStackByOrdinalMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfOptionalStackByIdMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalCardOfOptionalStackByNameMethodInfo;
extern MCExecMethodInfo *kMCInterfaceEvalThisCardOfOptionalStackMethodInfo;

void MCInterfaceInitialize(MCExecContext& ctxt);
void MCInterfaceFinalize(MCExecContext& ctxt);

void MCInterfaceImagePaletteSettingsFree(MCExecContext& ctxt, MCInterfaceImagePaletteSettings& p_settings);
void MCInterfaceVisualEffectFree(MCExecContext& ctxt, MCInterfaceVisualEffect& p_effect);
void MCInterfaceVisualEffectArgumentFree(MCExecContext& ctxt, MCInterfaceVisualEffectArgument& p_arg);

bool MCInterfaceTryToResolveObject(MCExecContext& ctxt, MCStringRef long_id, MCObjectPtr& r_object);

void MCInterfaceMakeCustomImagePaletteSettings(MCExecContext& ctxt, MCColor *colors, uindex_t color_count, MCInterfaceImagePaletteSettings& r_settings);
void MCInterfaceMakeOptimalImagePaletteSettings(MCExecContext& ctxt, integer_t *count, MCInterfaceImagePaletteSettings& r_settings);
void MCInterfaceMakeWebSafeImagePaletteSettings(MCExecContext& ctxt, MCInterfaceImagePaletteSettings& r_settings);

void MCInterfaceMakeVisualEffect(MCExecContext& ctxt, MCStringRef name, MCStringRef sound, MCInterfaceVisualEffectArgument *effect_args, uindex_t count, Visual_effects type, Visual_effects direction, Visual_effects speed, Visual_effects image, MCInterfaceVisualEffect& r_effect);
void MCInterfaceMakeVisualEffectArgument(MCExecContext& ctxt, MCStringRef p_value, MCStringRef p_key, bool p_has_id, MCInterfaceVisualEffectArgument& r_arg);

void MCInterfaceEvalScreenColors(MCExecContext& ctxt, real64_t& r_colors);
void MCInterfaceEvalScreenDepth(MCExecContext& ctxt, integer_t& r_depth);
void MCInterfaceEvalScreenName(MCExecContext& ctxt, MCNameRef& r_name);
void MCInterfaceEvalScreenRect(MCExecContext& ctxt, bool p_working, bool p_plural, bool p_effective, MCStringRef& r_string);

void MCInterfaceEvalScreenLoc(MCExecContext& ctxt, MCStringRef& r_string);

void MCInterfaceEvalClickH(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceEvalClickV(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceEvalClickLoc(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalClickChar(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalClickText(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalClickCharChunk(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalClickChunk(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalClickLine(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalClickField(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalClickStack(MCExecContext& ctxt, MCStringRef& r_string);

void MCInterfaceEvalMouse(MCExecContext& ctxt, integer_t p_which, MCNameRef& r_result);
void MCInterfaceEvalMouseClick(MCExecContext& ctxt, bool& r_bool);
void MCInterfaceEvalMouseColor(MCExecContext& ctxt, MCColor& r_color);

void MCInterfaceEvalMouseH(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceEvalMouseV(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceEvalMouseLoc(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalMouseChar(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalMouseText(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalMouseCharChunk(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalMouseChunk(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalMouseLine(MCExecContext& ctxt, MCStringRef& r_result);
void MCInterfaceEvalMouseControl(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalMouseStack(MCExecContext& ctxt, MCStringRef& r_string);

void MCInterfaceEvalFoundText(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalFoundField(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalFoundChunk(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalFoundLine(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalFoundLoc(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalFoundField(MCExecContext& ctxt, MCStringRef& r_string);

void MCInterfaceEvalSelectedText(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalSelectedTextOf(MCExecContext& ctxt, MCObjectPtr p_target, MCStringRef& r_string);
void MCInterfaceEvalSelectedChunk(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalSelectedChunkOf(MCExecContext& ctxt, MCObjectPtr p_target, MCStringRef& r_string);
void MCInterfaceEvalSelectedLine(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalSelectedLineOf(MCExecContext& ctxt, MCObjectPtr p_target, MCStringRef& r_string);
void MCInterfaceEvalSelectedLoc(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalSelectedLocOf(MCExecContext& ctxt, MCObjectPtr p_target, MCStringRef& r_string);

void MCInterfaceEvalSelectedField(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalSelectedImage(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalSelectedObject(MCExecContext& ctxt, MCStringRef& r_string);

void MCInterfaceEvalCapsLockKey(MCExecContext& ctxt, MCNameRef& r_result);
void MCInterfaceEvalCommandKey(MCExecContext& ctxt, MCNameRef& r_result);
void MCInterfaceEvalControlKey(MCExecContext& ctxt, MCNameRef& r_result);
void MCInterfaceEvalOptionKey(MCExecContext& ctxt, MCNameRef& r_result);
void MCInterfaceEvalShiftKey(MCExecContext& ctxt, MCNameRef& r_result);
void MCInterfaceEvalKeysDown(MCExecContext& ctxt, MCStringRef& r_string);

void MCInterfaceEvalMainStacks(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalOpenStacks(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalStacks(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalTopStack(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalTopStackOf(MCExecContext& ctxt, integer_t p_stack_number, MCStringRef& r_string);

void MCInterfaceEvalFocusedObject(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalColorNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalFlushEvents(MCExecContext& ctxt, MCNameRef p_name, MCStringRef& r_string);
void MCInterfaceEvalGlobalLoc(MCExecContext& ctxt, MCPoint point, MCPoint& r_global_point);
void MCInterfaceEvalLocalLoc(MCExecContext& ctxt, MCPoint point, MCPoint& r_local_point);
void MCInterfaceEvalMovingControls(MCExecContext& ctxt, MCStringRef& r_string);
void MCInterfaceEvalWaitDepth(MCExecContext& ctxt, integer_t& r_depth);

void MCInterfaceEvalIntersect(MCExecContext& ctxt, MCObjectPtr p_object_a, MCObjectPtr p_object_b, bool& r_intersect);
void MCInterfaceEvalIntersectWithThreshold(MCExecContext& ctxt, MCObjectPtr p_object_a, MCObjectPtr p_object_b, MCStringRef p_threshold, bool& r_intersect);
void MCInterfaceEvalWithin(MCExecContext& ctxt, MCObjectPtr p_object, MCPoint p_point, bool& r_within);

void MCInterfaceEvalThereIsAnObject(MCExecContext& ctxt, MCChunk *p_object, bool& r_exists);
void MCInterfaceEvalThereIsNotAnObject(MCExecContext& ctxt, MCChunk *p_object, bool& r_not_exists);

void MCInterfaceEvalControlAtLoc(MCExecContext& ctxt, MCPoint p_location, MCStringRef& r_control);
void MCInterfaceEvalControlAtScreenLoc(MCExecContext& ctxt, MCPoint p_location, MCStringRef& r_control);

void MCInterfaceExecBeep(MCExecContext& ctxt, integer_t p_count);
void MCInterfaceExecClickCmd(MCExecContext& ctxt, uint2 p_button, MCPoint p_location, uint2 p_modifiers);

void MCInterfaceExecCloseStack(MCExecContext& ctxt, MCStack *p_target);
void MCInterfaceExecCloseDefaultStack(MCExecContext& ctxt);

void MCInterfaceExecDrag(MCExecContext& ctxt, uint2 p_which, MCPoint p_start, MCPoint p_end, uint2 p_modifiers);

void MCInterfaceExecFocusOnNothing(MCExecContext &ctxt);
void MCInterfaceExecFocusOn(MCExecContext &ctxt, MCObject *p_object);

void MCInterfaceExecGrab(MCExecContext &ctxt, MCControl *control);

void MCInterfaceExecGroupControls(MCExecContext& ctxt, MCObjectPtr *p_controls, uindex_t p_control_count);
void MCInterfaceExecGroupSelection(MCExecContext& ctxt);

void MCInterfaceExecPopToLast(MCExecContext& ctxt);
void MCInterfaceExecPop(MCExecContext& ctxt, MCStringRef& r_element);
void MCInterfaceExecPushRecentCard(MCExecContext& ctxt);
void MCInterfaceExecPushCurrentCard(MCExecContext& ctxt);
void MCInterfaceExecPushCard(MCExecContext& ctxt, MCCard *p_target);

void MCInterfaceExecPlaceGroupOnCard(MCExecContext& ctxt, MCObject *p_group, MCCard *p_target);
void MCInterfaceExecRemoveGroupFromCard(MCExecContext& ctxt, MCObjectPtr p_group, MCCard *p_target);
void MCInterfaceExecResetCursors(MCExecContext& ctxt);
void MCInterfaceExecResetTemplate(MCExecContext& ctxt, Reset_type type);

void MCInterfaceExecRevert(MCExecContext& ctxt);

void MCInterfaceExecSelectEmpty(MCExecContext& ctxt);
void MCInterfaceExecSelectAllTextOfField(MCExecContext& ctxt, MCObjectPtr target);
void MCInterfaceExecSelectTextOfField(MCExecContext& ctxt, Preposition_type preposition, MCObjectChunkPtr target);
void MCInterfaceExecSelectTextOfButton(MCExecContext& ctxt, Preposition_type preposition, MCObjectChunkPtr target);
void MCInterfaceExecSelectObjects(MCExecContext& ctxt, MCObjectPtr *p_controls, uindex_t p_control_count);

void MCInterfaceExecStartEditingGroup(MCExecContext& ctxt, MCGroup *p_target);

void MCInterfaceExecStopEditingDefaultStack(MCExecContext& ctxt);
void MCInterfaceExecStopEditingGroup(MCExecContext& ctxt, MCGroup *p_target);
void MCInterfaceExecStopMovingObject(MCExecContext& ctxt, MCObject *p_target);

void MCInterfaceExecType(MCExecContext& ctxt, MCStringRef p_typing, uint2 p_modifiers);

void MCInterfaceExecUndo(MCExecContext& ctxt);

void MCInterfaceExecUngroupObject(MCExecContext& ctxt, MCObject *p_group);
void MCInterfaceExecUngroupSelection(MCExecContext& ctxt);

void MCInterfaceExecCopyObjectsToContainer(MCExecContext& ctxt, MCObjectPtr *p_targets, uindex_t p_target_count, MCObjectPtr p_destination);
void MCInterfaceExecCutObjectsToContainer(MCExecContext& ctxt, MCObjectPtr *p_targets, uindex_t p_target_count, MCObjectPtr p_destination);

void MCInterfaceExecDelete(MCExecContext& ctxt);
void MCInterfaceExecDeleteObjects(MCExecContext& ctxt, MCObjectPtr *objects, uindex_t object_count);
void MCInterfaceExecDeleteObjectChunks(MCExecContext& ctxt, MCObjectChunkPtr *chunks, uindex_t chunk_count);

void MCInterfaceExecDisableChunkOfButton(MCExecContext& ctxt, MCObjectChunkPtr p_targets);
void MCInterfaceExecEnableChunkOfButton(MCExecContext& ctxt, MCObjectChunkPtr p_targets);
void MCInterfaceExecUnhiliteChunkOfButton(MCExecContext& ctxt, MCObjectChunkPtr p_targets);
void MCInterfaceExecHiliteChunkOfButton(MCExecContext& ctxt, MCObjectChunkPtr p_targets);

void MCInterfaceExecDisableObject(MCExecContext& ctxt, MCObjectPtr p_targets);
void MCInterfaceExecEnableObject(MCExecContext& ctxt, MCObjectPtr p_targets);
void MCInterfaceExecUnhiliteObject(MCExecContext& ctxt, MCObjectPtr p_targets);
void MCInterfaceExecHiliteObject(MCExecContext& ctxt, MCObjectPtr p_targets);

void MCInterfaceExecSaveStack(MCExecContext& ctxt, MCStack *p_target);
void MCInterfaceExecSaveStackAs(MCExecContext& ctxt, MCStack *p_target, MCStringRef p_new_filename);

void MCInterfaceExecMoveObjectBetween(MCExecContext& ctxt, MCObject *p_target, MCPoint p_from, MCPoint p_to, double p_duration, int p_units, bool p_wait, bool p_dispatch);
void MCInterfaceExecMoveObjectAlong(MCExecContext& ctxt, MCObject *p_target, MCPoint *p_motion, uindex_t p_motion_count, bool p_is_relative, double p_duration, int p_units, bool p_wait, bool p_dispatch);

void MCInterfaceExecHideGroups(MCExecContext& ctxt);
void MCInterfaceExecHideObject(MCExecContext& ctxt, MCObjectPtr p_target);
void MCInterfaceExecHideObjectWithEffect(MCExecContext& ctxt, MCObjectPtr p_target, MCVisualEffect *p_effect);
void MCInterfaceExecHideMenuBar(MCExecContext& ctxt);
void MCInterfaceExecHideTaskBar(MCExecContext& ctxt);

void MCInterfaceExecShowGroups(MCExecContext& ctxt);
void MCInterfaceExecShowAllCards(MCExecContext& ctxt);
void MCInterfaceExecShowMarkedCards(MCExecContext& ctxt);
void MCInterfaceExecShowCards(MCExecContext& ctxt, uint2 p_count);
void MCInterfaceExecShowObject(MCExecContext& ctxt, MCObjectPtr p_target, MCPoint *p_at);
void MCInterfaceExecShowObjectWithEffect(MCExecContext& ctxt, MCObjectPtr p_target, MCPoint *p_at, MCVisualEffect *p_effect);
void MCInterfaceExecShowMenuBar(MCExecContext& ctxt);
void MCInterfaceExecShowTaskBar(MCExecContext& ctxt);

void MCInterfaceExecPopupButton(MCExecContext& ctxt, MCButton *p_target, MCPoint *p_at);
void MCInterfaceExecDrawerStack(MCExecContext& ctxt, MCStack *p_target, MCStringRef parent, bool p_parent_is_thisstack, int p_at, int p_aligned);
void MCInterfaceExecDrawerStackByName(MCExecContext& ctxt, MCStringRef p_target, MCStringRef parent, bool p_parent_is_thisstack, int p_at, int p_aligned);
void MCInterfaceExecDrawerStackLegacy(MCExecContext& ctxt, MCStack *p_target, MCStringRef parent, bool p_parent_is_thisstack, intenum_t p_at, intenum_t p_aligned);
void MCInterfaceExecDrawerStackByNameLegacy(MCExecContext& ctxt, MCStringRef p_target, MCStringRef parent, bool p_parent_is_thisstack, intenum_t p_at, intenum_t p_aligned);
void MCInterfaceExecSheetStack(MCExecContext& ctxt, MCStack *p_target, MCStringRef parent, bool p_parent_is_thisstack);
void MCInterfaceExecSheetStackByName(MCExecContext& ctxt, MCStringRef p_target, MCStringRef parent, bool p_parent_is_thisstack);
void MCInterfaceExecOpenStack(MCExecContext& ctxt, MCStack *p_target, int p_mode);
void MCInterfaceExecOpenStackByName(MCExecContext& ctxt, MCStringRef p_target, int p_mode);
void MCInterfaceExecPopupStack(MCExecContext& ctxt, MCStack *p_target, MCPoint *p_at, int p_mode);
void MCInterfaceExecPopupStackByName(MCExecContext& ctxt, MCStringRef p_target, MCPoint *p_at, int p_mode);

void MCInterfaceExecCreateStack(MCExecContext& ctxt, MCStack *p_owner, MCStringRef p_new_name, bool p_force_invisible);
void MCInterfaceExecCreateStackWithGroup(MCExecContext& ctxt, MCGroup *p_group_to_copy, MCStringRef p_new_name, bool p_force_invisible);
void MCInterfaceExecCreateCard(MCExecContext& ctxt, MCStringRef p_new_name, bool p_force_invisible);
void MCInterfaceExecCreateControl(MCExecContext& ctxt, MCStringRef p_new_name, int p_type, MCGroup *p_container, bool p_force_invisible);

void MCInterfaceExecClone(MCExecContext& ctxt, MCObject *p_target, MCStringRef p_new_name, bool p_force_invisible);

void MCInterfaceExecFind(MCExecContext& ctxt, int p_mode, MCStringRef p_needle, MCChunk *p_target);

void MCInterfaceExecPutIntoObject(MCExecContext& ctxt, MCStringRef value, int where, MCObjectChunkPtr target);
void MCInterfaceExecPutIntoField(MCExecContext& ctxt, MCStringRef value, int where, MCObjectChunkPtr target, bool is_unicode);

void MCInterfaceExecLockCursor(MCExecContext& ctxt);
void MCInterfaceExecLockMenus(MCExecContext& ctxt);
void MCInterfaceExecLockMoves(MCExecContext& ctxt);
void MCInterfaceExecLockRecent(MCExecContext& ctxt);
void MCInterfaceExecLockScreen(MCExecContext& ctxt);
void MCInterfaceExecLockScreenForEffect(MCExecContext& ctxt, MCRectangle *region);

void MCInterfaceExecUnlockCursor(MCExecContext& ctxt);
void MCInterfaceExecUnlockMenus(MCExecContext& ctxt);
void MCInterfaceExecUnlockMoves(MCExecContext& ctxt);
void MCInterfaceExecUnlockRecent(MCExecContext& ctxt);
void MCInterfaceExecUnlockScreen(MCExecContext& ctxt);
void MCInterfaceExecUnlockScreenWithEffect(MCExecContext& ctxt, MCVisualEffect *region);

void MCInterfaceExecImportSnapshotOfScreen(MCExecContext& ctxt, MCRectangle *p_region);
void MCInterfaceExecImportSnapshotOfStack(MCExecContext& ctxt, MCStringRef p_stack, MCStringRef p_display, MCRectangle *p_region);
void MCInterfaceExecImportSnapshotOfObject(MCExecContext& ctxt, MCObject *p_target, MCRectangle *p_region, bool p_with_effects, MCPoint *p_at_size);
void MCInterfaceExecImportAudioClip(MCExecContext& ctxt, MCStringRef p_filename);
void MCInterfaceExecImportVideoClip(MCExecContext& ctxt, MCStringRef p_filename);
void MCInterfaceExecImportImage(MCExecContext& ctxt, MCStringRef p_filename, MCStringRef p_mask_filename, MCObject *p_container);

void MCInterfaceExecExportSnapshotOfScreen(MCExecContext& ctxt, MCRectangle *p_region, int format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef &r_data);
void MCInterfaceExecExportSnapshotOfScreenToFile(MCExecContext& ctxt, MCRectangle *p_region, int format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef p_filename, MCStringRef p_mask_filename);
void MCInterfaceExecExportSnapshotOfStack(MCExecContext& ctxt, MCStringRef p_stack, MCStringRef p_display, MCRectangle *p_region, int format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef &r_data);
void MCInterfaceExecExportSnapshotOfStackToFile(MCExecContext& ctxt, MCStringRef p_stack, MCStringRef p_display, MCRectangle *p_region, int format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef p_filename, MCStringRef p_mask_filename);
void MCInterfaceExecExportSnapshotOfObject(MCExecContext& ctxt, MCObject *p_target, MCRectangle *p_region, bool p_with_effects, MCPoint *p_at_size, int format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef &r_data);
void MCInterfaceExecExportSnapshotOfObjectToFile(MCExecContext& ctxt, MCObject *p_target, MCRectangle *p_region, bool p_with_effects, MCPoint *p_at_size, int format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef p_filename, MCStringRef p_mask_filename);
void MCInterfaceExecExportImage(MCExecContext& ctxt, MCImage *p_target, int p_format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef &r_data);
void MCInterfaceExecExportImageToFile(MCExecContext& ctxt, MCImage *p_target, int p_format, MCInterfaceImagePaletteSettings *p_palette, MCStringRef p_filename, MCStringRef p_mask_filename);

void MCInterfaceExecSortCardsOfStack(MCExecContext &ctxt, MCStack *p_target, bool p_ascending, int p_format, MCExpression *p_by, bool p_only_marked);
void MCInterfaceExecSortField(MCExecContext &ctxt, MCObjectPtr p_target, int p_chunk_type, bool p_ascending, int p_format, MCExpression *p_by);
void MCInterfaceExecSortContainer(MCExecContext &ctxt, MCStringRef& x_target, int p_chunk_type, bool p_ascending, int p_format, MCExpression *p_by);

void MCInterfaceExecChooseTool(MCExecContext& ctxt, int p_tool);

void MCInterfaceExecGoCardAsMode(MCExecContext& ctxt, MCCard *p_card, int p_mode, bool p_visible, bool p_this_stack);
void MCInterfaceExecGoCardInWindow(MCExecContext& ctxt, MCCard *p_card, MCStringRef p_window, bool p_visible, bool p_this_stack);
void MCInterfaceExecGoRecentCard(MCExecContext& ctxt);
void MCInterfaceExecGoCardRelative(MCExecContext& ctxt, bool p_forward, real8 p_amount);
void MCInterfaceExecGoCardEnd(MCExecContext& ctxt, bool p_is_start);
void MCInterfaceExecGoHome(MCExecContext& ctxt, MCCard *p_card);

void MCInterfaceExecVisualEffect(MCExecContext& ctxt, MCInterfaceVisualEffect p_effect);

void MCInterfaceGetDialogData(MCExecContext& ctxt, MCValueRef& r_value);
void MCInterfaceSetDialogData(MCExecContext& ctxt, MCValueRef p_value);

void MCInterfaceGetLookAndFeel(MCExecContext& ctxt, intenum_t& r_value);
void MCInterfaceSetLookAndFeel(MCExecContext& ctxt, intenum_t p_value);
void MCInterfaceGetScreenMouseLoc(MCExecContext& ctxt, MCPoint& r_value);
void MCInterfaceSetScreenMouseLoc(MCExecContext& ctxt, MCPoint p_value);

void MCInterfaceGetBackdrop(MCExecContext& ctxt, MCInterfaceBackdrop& r_backdrop);
void MCInterfaceSetBackdrop(MCExecContext& ctxt, const MCInterfaceBackdrop& p_backdrop);
void MCInterfaceGetBufferImages(MCExecContext& ctxt, bool &r_value);
void MCInterfaceSetBufferImages(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetSystemFileSelector(MCExecContext& ctxt, bool &r_value);
void MCInterfaceSetSystemFileSelector(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetSystemColorSelector(MCExecContext& ctxt, bool &r_value);
void MCInterfaceSetSystemColorSelector(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetSystemPrintSelector(MCExecContext& ctxt, bool &r_value);
void MCInterfaceSetSystemPrintSelector(MCExecContext& ctxt, bool p_value);

void MCInterfaceGetPaintCompression(MCExecContext& ctxt, intenum_t& r_value);
void MCInterfaceSetPaintCompression(MCExecContext& ctxt, intenum_t value);

void MCInterfaceGetBrushBackColor(MCExecContext& ctxt, MCValueRef& r_color);
void MCInterfaceSetBrushBackColor(MCExecContext& ctxt, MCValueRef color);
void MCInterfaceGetPenBackColor(MCExecContext& ctxt, MCValueRef& r_color);
void MCInterfaceSetPenBackColor(MCExecContext& ctxt, MCValueRef color);
void MCInterfaceGetBrushColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetBrushColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetPenColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetPenColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetBrushPattern(MCExecContext& ctxt, uinteger_t& r_pattern);
void MCInterfaceSetBrushPattern(MCExecContext& ctxt, uinteger_t pattern);
void MCInterfaceGetPenPattern(MCExecContext& ctxt, uinteger_t& r_pattern);
void MCInterfaceSetPenPattern(MCExecContext& ctxt, uinteger_t pattern);
void MCInterfaceGetFilled(MCExecContext& ctxt, bool& r_filled);
void MCInterfaceSetFilled(MCExecContext& ctxt, bool filled);
void MCInterfaceGetPolySides(MCExecContext& ctxt, uinteger_t& r_sides);
void MCInterfaceSetPolySides(MCExecContext& ctxt, uinteger_t sides);
void MCInterfaceGetLineSize(MCExecContext& ctxt, uinteger_t& r_size);
void MCInterfaceSetLineSize(MCExecContext& ctxt, uinteger_t size);
void MCInterfaceGetRoundRadius(MCExecContext& ctxt, uinteger_t& r_radius);
void MCInterfaceSetRoundRadius(MCExecContext& ctxt, uinteger_t radius);
void MCInterfaceGetStartAngle(MCExecContext& ctxt, uinteger_t& r_radius);
void MCInterfaceSetStartAngle(MCExecContext& ctxt, uinteger_t radius);
void MCInterfaceGetArcAngle(MCExecContext& ctxt, uinteger_t& r_radius);
void MCInterfaceSetArcAngle(MCExecContext& ctxt, uinteger_t radius);
void MCInterfaceGetRoundEnds(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetRoundEnds(MCExecContext& ctxt, bool value);
void MCInterfaceGetDashes(MCExecContext& ctxt, MCStringRef& r_dashes);
void MCInterfaceSetDashes(MCExecContext& ctxt, MCStringRef dashes);

void MCInterfaceGetRecentCards(MCExecContext& ctxt, MCStringRef& r_cards);
void MCInterfaceGetRecentNames(MCExecContext& ctxt, MCStringRef& r_names);

void MCInterfaceGetEditBackground(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetEditBackground(MCExecContext& ctxt, bool value);

void MCInterfaceGetLockScreen(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockScreen(MCExecContext& ctxt, bool value);

void MCInterfaceGetAccentColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetAccentColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetHiliteColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetHiliteColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetLinkColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetLinkColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetLinkHiliteColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetLinkHiliteColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetLinkVisitedColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetLinkVisitedColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color);
void MCInterfaceGetUnderlineLinks(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetUnderlineLinks(MCExecContext& ctxt, bool value);

void MCInterfaceGetSelectGroupedControls(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetSelectGroupedControls(MCExecContext& ctxt, bool value);

void MCInterfaceGetIcon(MCExecContext& ctxt, uinteger_t& r_icon);
void MCInterfaceSetIcon(MCExecContext& ctxt, uinteger_t icon);

void MCInterfaceGetAllowInlineInput(MCExecContext& ctxt, bool &r_value);
void MCInterfaceSetAllowInlineInput(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetDragDelta(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetDragDelta(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetStackFileType(MCExecContext& ctxt, MCStringRef& r_value);
void MCInterfaceSetStackFileType(MCExecContext& ctxt, MCStringRef p_value);
void MCInterfaceGetStackFileVersion(MCExecContext& ctxt, MCStringRef& r_value);
void MCInterfaceSetStackFileVersion(MCExecContext& ctxt, MCStringRef p_value);

void MCInterfaceGetIconMenu(MCExecContext& ctxt, MCStringRef& r_menu);
void MCInterfaceSetIconMenu(MCExecContext& ctxt, MCStringRef menu);
void MCInterfaceGetStatusIcon(MCExecContext& ctxt, uinteger_t& r_icon);
void MCInterfaceSetStatusIcon(MCExecContext& ctxt, uinteger_t icon);
void MCInterfaceGetStatusIconToolTip(MCExecContext& ctxt, MCStringRef& r_tooltip);
void MCInterfaceSetStatusIconToolTip(MCExecContext& ctxt, MCStringRef tooltip);
void MCInterfaceGetStatusIconMenu(MCExecContext& ctxt, MCStringRef& r_icon_menu);
void MCInterfaceSetStatusIconMenu(MCExecContext& ctxt, MCStringRef icon_menu);

void MCInterfaceGetProcessType(MCExecContext& ctxt, intenum_t& r_value);
void MCInterfaceSetProcessType(MCExecContext& ctxt, intenum_t value);
void MCInterfaceGetShowInvisibles(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetShowInvisibles(MCExecContext& ctxt, bool p_value);

void MCInterfaceGetCursor(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetCursor(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetDefaultCursor(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetDefaultCursor(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetDefaultStack(MCExecContext& ctxt, MCStringRef& r_value);
void MCInterfaceSetDefaultStack(MCExecContext& ctxt, MCStringRef p_value);
void MCInterfaceGetDefaultMenubar(MCExecContext& ctxt, MCStringRef& r_value);
void MCInterfaceSetDefaultMenubar(MCExecContext& ctxt, MCStringRef p_value);
void MCInterfaceGetDragSpeed(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetDragSpeed(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetMoveSpeed(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetMoveSpeed(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetLockCursor(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockCursor(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetLockErrors(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockErrors(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetLockMenus(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockMenus(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetLockMessages(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockMessages(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetLockMoves(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockMoves(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetLockRecent(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetLockRecent(MCExecContext& ctxt, bool p_value);

void MCInterfaceGetIdleRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetIdleRate(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetIdleTicks(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetIdleTicks(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetBlinkRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetBlinkRate(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetRepeatRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetRepeatRate(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetRepeatDelay(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetRepeatDelay(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetTypeRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetTypeRate(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetSyncRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetSyncRate(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetEffectRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetEffectRate(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetDoubleDelta(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetDoubleDelta(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetDoubleTime(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetDoubleTime(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetTooltipDelay(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetTooltipDelay(MCExecContext& ctxt, uinteger_t p_value);

void MCInterfaceGetNavigationArrows(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetNavigationArrows(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetExtendKey(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetExtendKey(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetPointerFocus(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetPointerFocus(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetEmacsKeyBindings(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetEmacsKeyBindings(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetRaiseMenus(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetRaiseMenus(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetActivatePalettes(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetActivatePalettes(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetHidePalettes(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetHidePalettes(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetRaisePalettes(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetRaisePalettes(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetRaiseWindows(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetRaiseWindows(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetHideBackdrop(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetHideBackdrop(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetDontUseNavigationServices(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetDontUseNavigationServices(MCExecContext& ctxt, bool p_value);

void MCInterfaceGetProportionalThumbs(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetProportionalThumbs(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetSharedMemory(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetSharedMemory(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetScreenGamma(MCExecContext& ctxt, double& r_value);
void MCInterfaceSetScreenGamma(MCExecContext& ctxt, double p_value);
void MCInterfaceGetSelectionMode(MCExecContext& ctxt, intenum_t& r_value);
void MCInterfaceSetSelectionMode(MCExecContext& ctxt, intenum_t p_value);
void MCInterfaceGetSelectionHandleColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color);
void MCInterfaceSetSelectionHandleColor(MCExecContext& ctxt, const MCInterfaceNamedColor& p_color);
void MCInterfaceGetWindowBoundingRect(MCExecContext& ctxt, MCRectangle& r_value);
void MCInterfaceSetWindowBoundingRect(MCExecContext& ctxt, MCRectangle p_value);
void MCInterfaceGetJpegQuality(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetJpegQuality(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetRelayerGroupedControls(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetRelayerGroupedControls(MCExecContext& ctxt, bool p_value);

void MCInterfaceGetBrush(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetBrush(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetEraser(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetEraser(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetSpray(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetSpray(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetCentered(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetCentered(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetGrid(MCExecContext& ctxt, bool& r_value);
void MCInterfaceSetGrid(MCExecContext& ctxt, bool p_value);
void MCInterfaceGetGridSize(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetGridSize(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetSlices(MCExecContext& ctxt, uinteger_t& r_value);
void MCInterfaceSetSlices(MCExecContext& ctxt, uinteger_t p_value);
void MCInterfaceGetBeepLoudness(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceSetBeepLoudness(MCExecContext& ctxt, integer_t p_value);
void MCInterfaceGetBeepPitch(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceSetBeepPitch(MCExecContext& ctxt, integer_t p_value);
void MCInterfaceGetBeepDuration(MCExecContext& ctxt, integer_t& r_value);
void MCInterfaceSetBeepDuration(MCExecContext& ctxt, integer_t p_value);
void MCInterfaceGetBeepSound(MCExecContext& ctxt, MCStringRef& r_value);
void MCInterfaceSetBeepSound(MCExecContext& ctxt, MCStringRef p_value);
void MCInterfaceGetTool(MCExecContext& ctxt, MCStringRef& r_value);
void MCInterfaceSetTool(MCExecContext& ctxt, MCStringRef p_value);

void MCInterfaceGetScreenRect(MCExecContext& ctxt, bool p_working, bool p_effective, MCRectangle& r_value);
void MCInterfaceGetScreenRects(MCExecContext& ctxt, bool p_working, bool p_effective, MCStringRef& r_value);

void MCInterfaceEvalHelpStackAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalHomeStackAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalSelectedObjectAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalTopStackAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalClickStackAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalMouseStackAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalClickFieldAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalSelectedFieldAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalSelectedImageAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalFoundFieldAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalMouseControlAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalFocusedObjectAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalBinaryStackAsObject(MCExecContext& ctxt, MCStringRef p_data, MCObjectPtr& r_object);
void MCInterfaceEvalDefaultStackAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCInterfaceEvalStackOfStackByName(MCExecContext& ctxt, MCObjectPtr p_parent, MCNameRef p_name, MCObjectPtr& r_stack);
void MCInterfaceEvalStackOfStackById(MCExecContext& ctxt, MCObjectPtr p_parent, uinteger_t p_id, MCObjectPtr& r_stack);
void MCInterfaceEvalStackByValue(MCExecContext& ctxt, MCValueRef p_value, MCObjectPtr& r_stack);
void MCInterfaceEvalSubstackOfStackByName(MCExecContext& ctxt, MCObjectPtr p_parent, MCNameRef p_name, MCObjectPtr& r_stack);
void MCInterfaceEvalSubstackOfStackById(MCExecContext& ctxt, MCObjectPtr p_parent, uinteger_t p_id, MCObjectPtr& r_stack);
void MCInterfaceEvalAudioClipOfStackByOrdinal(MCExecContext& ctxt, MCObjectPtr p_stack, uinteger_t p_ordinal_type, MCObjectPtr& r_clip);
void MCInterfaceEvalAudioClipOfStackById(MCExecContext& ctxt, MCObjectPtr p_stack, uinteger_t p_id, MCObjectPtr& r_clip);
void MCInterfaceEvalAudioClipOfStackByName(MCExecContext& ctxt, MCObjectPtr p_stack, MCNameRef p_name, MCObjectPtr& r_clip);
void MCInterfaceEvalVideoClipOfStackByOrdinal(MCExecContext& ctxt, MCObjectPtr p_stack, uinteger_t p_ordinal_type, MCObjectPtr& r_clip);
void MCInterfaceEvalVideoClipOfStackById(MCExecContext& ctxt, MCObjectPtr p_stack, uinteger_t p_id, MCObjectPtr& r_clip);
void MCInterfaceEvalVideoClipOfStackByName(MCExecContext& ctxt, MCObjectPtr p_stack, MCNameRef p_name, MCObjectPtr& r_clip);
void MCInterfaceEvalBackgroundOfStackByOrdinal(MCExecContext& ctxt, MCObjectPtr p_stack, Chunk_term p_ordinal_type, MCObjectPtr& r_bg);
void MCInterfaceEvalBackgroundOfStackById(MCExecContext& ctxt, MCObjectPtr p_stack, uinteger_t p_id, MCObjectPtr& r_bg);
void MCInterfaceEvalBackgroundOfStackByName(MCExecContext& ctxt, MCObjectPtr p_stack, MCNameRef p_name, MCObjectPtr& r_bg);
void MCInterfaceEvalStackWithBackgroundByOrdinal(MCExecContext& ctxt, MCObjectPtr p_stack, Chunk_term p_ordinal_type, MCObjectPtr& r_stack);
void MCInterfaceEvalStackWithBackgroundById(MCExecContext& ctxt, MCObjectPtr p_stack, uinteger_t p_id, MCObjectPtr& r_stack);
void MCInterfaceEvalStackWithBackgroundByName(MCExecContext& ctxt, MCObjectPtr p_stack, MCNameRef p_name, MCObjectPtr& r_stack);
void MCInterfaceEvalCardOfStackByOrdinal(MCExecContext& ctxt, MCObjectPtr p_stack, bool p_marked, Chunk_term p_ordinal_type, MCObjectPtr& r_card);
void MCInterfaceEvalThisCardOfStack(MCExecContext& ctxt, MCObjectPtr p_stack, MCObjectPtr& r_card);
void MCInterfaceEvalCardOfStackById(MCExecContext& ctxt, MCObjectPtr p_stack, bool p_marked, uinteger_t p_id, MCObjectPtr& r_card);
void MCInterfaceEvalCardOfStackByName(MCExecContext& ctxt, MCObjectPtr p_stack, bool p_marked, MCNameRef p_name, MCObjectPtr& r_card);
void MCInterfaceEvalCardOfBackgroundByOrdinal(MCExecContext& ctxt, MCObjectPtr p_background, bool p_marked, Chunk_term p_ordinal_type, MCObjectPtr& r_card);
void MCInterfaceEvalCardOfBackgroundById(MCExecContext& ctxt, MCObjectPtr p_background, bool p_marked, uinteger_t p_id, MCObjectPtr& r_card);
void MCInterfaceEvalCardOfBackgroundByName(MCExecContext& ctxt, MCObjectPtr p_background, bool p_marked, MCNameRef p_name, MCObjectPtr& r_card);
void MCInterfaceEvalGroupOfCardByOrdinal(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_parent_type, Chunk_term p_ordinal_type, MCObjectPtr& r_group);
void MCInterfaceEvalGroupOfCardById(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_parent_type, uinteger_t p_id, MCObjectPtr& r_group);
void MCInterfaceEvalGroupOfCardByName(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_parent_type, MCNameRef p_name, MCObjectPtr& r_group);
void MCInterfaceEvalGroupOfCardOrStackById(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_parent_type, uinteger_t p_id, MCObjectPtr& r_group);
void MCInterfaceEvalGroupOfGroupByOrdinal(MCExecContext& ctxt, MCObjectPtr p_group, Chunk_term p_ordinal_type, MCObjectPtr& r_group);
void MCInterfaceEvalGroupOfGroupById(MCExecContext& ctxt, MCObjectPtr p_group, uinteger_t p_id, MCObjectPtr& r_group);
void MCInterfaceEvalGroupOfGroupByName(MCExecContext& ctxt, MCObjectPtr p_group, MCNameRef p_name, MCObjectPtr& r_group);
void MCInterfaceEvalMenubarAsObject(MCExecContext& ctxt, MCObjectPtr& r_menubar);
void MCInterfaceEvalObjectOfGroupByOrdinal(MCExecContext& ctxt, MCObjectPtr p_group, Chunk_term p_object_type, Chunk_term p_ordinal_type, MCObjectPtr& r_object);
void MCInterfaceEvalObjectOfGroupById(MCExecContext& ctxt, MCObjectPtr p_group, Chunk_term p_object_type, uinteger_t p_id, MCObjectPtr& r_object);
void MCInterfaceEvalObjectOfGroupByName(MCExecContext& ctxt, MCObjectPtr p_group, Chunk_term p_object_type, MCNameRef p_name, MCObjectPtr& r_object);
void MCInterfaceEvalObjectOfCardByOrdinal(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_object_type, Chunk_term p_parent_type, Chunk_term p_ordinal_type, MCObjectPtr& r_object);
void MCInterfaceEvalObjectOfCardById(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_object_type, Chunk_term p_parent_type, uinteger_t p_id, MCObjectPtr& r_object);
void MCInterfaceEvalObjectOfCardOrStackById(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_object_type, Chunk_term p_parent_type, uinteger_t p_id, MCObjectPtr& r_object);
void MCInterfaceEvalObjectOfCardByName(MCExecContext& ctxt, MCObjectPtr p_card, Chunk_term p_object_type, Chunk_term p_parent_type, MCNameRef p_name, MCObjectPtr& r_object);

void MCInterfaceEvalFieldTextChunkByRange(MCExecContext& ctxt, MCObjectPtr p_field, Chunk_term p_chunk_type, integer_t p_first, integer_t p_last, bool p_function, MCStringRef& r_result);
void MCInterfaceEvalFieldTextChunkByExpression(MCExecContext& ctxt, MCObjectPtr p_field, Chunk_term p_chunk_type, integer_t p_first, bool p_function, MCStringRef& r_result);
void MCInterfaceEvalFieldTextChunkByOrdinal(MCExecContext& ctxt, MCObjectPtr p_field, Chunk_term p_chunk_type, Chunk_term p_ordinal_type, bool p_function, MCStringRef& r_result);

void MCInterfaceMarkField(MCField *fptr, int4 &start, int4 &end, Functions p_function, Boolean wholechunk);
void MCInterfaceEvalTextOfContainer(MCExecContext& ctxt, MCObjectPtr p_container, MCStringRef &r_text);
void MCInterfaceEvalFieldChunk(MCExecContext& ctxt, MCObjectPtr p_field, Functions p_function, MCStringRef& r_text);
///////////

struct MCInterfaceLayer;

extern MCExecCustomTypeInfo *kMCInterfaceLayerTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceShadowTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceTextAlignTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceTextStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceInkNamesTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceEncodingTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfacePlayDestinationTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfaceStackStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceCharsetTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceCompositorTypeTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceDecorationTypeInfo;

///////////

extern MCExecCustomTypeInfo *kMCInterfaceMarginsTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceLayerModeTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfaceButtonStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceButtonMenuModeTypeInfo;
extern MCExecSetTypeInfo *kMCInterfaceButtonAcceleratorModifiersTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfaceFieldStyleTypeInfo;
extern MCExecCustomTypeInfo *kMCInterfaceFlaggedRangesTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfaceGraphicFillRuleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceGraphicEditModeTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceGraphicCapStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceGraphicJoinStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceGraphicStyleTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfaceImageResizeQualityTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceImagePaintCompressionTypeInfo;

///////////

extern MCExecSetTypeInfo *kMCInterfaceMediaTypesTypeInfo;
extern MCExecCustomTypeInfo *kMCMultimediaTrackTypeInfo;
extern MCExecCustomTypeInfo *kMCMultimediaQTVRConstraintsTypeInfo;
extern MCExecCustomTypeInfo *kMCMultimediaQTVRNodeTypeInfo;
extern MCExecCustomTypeInfo *kMCMultimediaQTVRHotSpotTypeInfo;

///////////

extern MCExecEnumTypeInfo *kMCInterfaceScrollbarStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCInterfaceScrollbarOrientationTypeInfo;

///////////

extern MCExecMethodInfo *kMCDialogExecAnswerColorMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAnswerFileWithTypesMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAnswerFileWithFilterMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAnswerFileMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAnswerFolderMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAnswerNotifyMethodInfo;
extern MCExecMethodInfo *kMCDialogExecCustomAnswerDialogMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAskQuestionMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAskPasswordMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAskFileMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAskFileWithFilterMethodInfo;
extern MCExecMethodInfo *kMCDialogExecAskFileWithTypesMethodInfo;
extern MCExecMethodInfo *kMCDialogExecCustomAskDialogMethodInfo;

void MCDialogExecAnswerColor(MCExecContext &ctxt, MCColor *initial_color, MCStringRef p_title, bool p_sheet);
void MCDialogExecAnswerFile(MCExecContext &ctxt, bool p_plural, MCStringRef p_prompt, MCStringRef p_initial, MCStringRef p_title, bool p_sheet);
void MCDialogExecAnswerFileWithFilter(MCExecContext &ctxt, bool p_plural, MCStringRef p_prompt, MCStringRef p_initial, MCStringRef p_filter, MCStringRef p_title, bool p_sheet);
void MCDialogExecAnswerFileWithTypes(MCExecContext &ctxt, bool p_plural, MCStringRef p_prompt, MCStringRef p_initial, MCStringRef *p_types, uindex_t p_type_count, MCStringRef p_title, bool p_sheet);
void MCDialogExecAnswerFolder(MCExecContext &ctxt, bool p_plural, MCStringRef p_prompt, MCStringRef p_initial, MCStringRef p_title, bool p_sheet);
void MCDialogExecAnswerNotify(MCExecContext &ctxt, int p_type, MCStringRef p_prompt, MCStringRef *p_buttons, uindex_t p_button_count, MCStringRef p_title, bool p_sheet);
void MCDialogExecCustomAnswerDialog(MCExecContext &ctxt, MCNameRef p_stack, MCNameRef p_type, bool p_sheet, MCStringRef *p_arg_list, uindex_t p_arg_count, MCStringRef &r_result);

void MCDialogExecAskQuestion(MCExecContext& ctxt, int type, MCStringRef prompt, MCStringRef answer, bool hint_answer, MCStringRef title, bool as_sheet);
void MCDialogExecAskPassword(MCExecContext& ctxt, bool clear, MCStringRef prompt, MCStringRef answer, bool hint_answer, MCStringRef title, bool as_sheet);
void MCDialogExecAskFile(MCExecContext& ctxt, MCStringRef prompt, MCStringRef initial, MCStringRef title, bool as_sheet);
void MCDialogExecAskFileWithFilter(MCExecContext& ctxt, MCStringRef prompt, MCStringRef initial, MCStringRef filter, MCStringRef title, bool as_sheet);
void MCDialogExecAskFileWithTypes(MCExecContext& ctxt, MCStringRef prompt, MCStringRef intial, MCStringRef *types, uindex_t type_count, MCStringRef title, bool as_sheet);
void MCDialogExecCustomAskDialog(MCExecContext& ctxt, MCNameRef p_stack, MCNameRef p_type, bool p_sheet, MCStringRef *p_arglist, uindex_t p_arg_count, bool& r_cancelled, MCStringRef& r_result);

///////////

extern MCExecEnumTypeInfo *kMCPasteboardDragActionTypeInfo;
extern MCExecSetTypeInfo *kMCPasteboardAllowableDragActionsTypeInfo;

extern MCExecMethodInfo *kMCPasteboardEvalClipboardMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalClipboardKeysMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalDropChunkMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalDragDestinationMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalDragSourceMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalDragDropKeysMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalIsAmongTheKeysOfTheClipboardDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalIsNotAmongTheKeysOfTheClipboardDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalIsAmongTheKeysOfTheDragDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalIsNotAmongTheKeysOfTheDragDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalDragSourceAsObjectMethodInfo;
extern MCExecMethodInfo *kMCPasteboardEvalDragDestinationAsObjectMethodInfo;

extern MCExecMethodInfo *kMCPasteboardExecPasteMethodInfo;
extern MCExecMethodInfo *kMCPasteboardExecCopyMethodInfo;
extern MCExecMethodInfo *kMCPasteboardExecCopyTextToClipboardMethodInfo;
extern MCExecMethodInfo *kMCPasteboardExecCopyObjectsToClipboardMethodInfo;
extern MCExecMethodInfo *kMCPasteboardExecCutMethodInfo;
extern MCExecMethodInfo *kMCPasteboardExecCutTextToClipboardMethodInfo;
extern MCExecMethodInfo *kMCPasteboardExecCutObjectsToClipboardMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetAcceptDropMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetAcceptDropMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetDragActionMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetDragActionMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetDragImageMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetDragImageMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetDragImageOffsetMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetDragImageOffsetMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetAllowableDragActionsMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetAllowableDragActionsMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetClipboardDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetClipboardDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardGetDragDataMethodInfo;
extern MCExecMethodInfo *kMCPasteboardSetDragDataMethodInfo;

void MCPasteboardEvalClipboard(MCExecContext& ctxt, MCNameRef& r_string);
void MCPasteboardEvalClipboardKeys(MCExecContext& ctxt, MCStringRef& r_string);
void MCPasteboardEvalDropChunk(MCExecContext& ctxt, MCStringRef& r_string);
void MCPasteboardEvalDragDestination(MCExecContext& ctxt, MCStringRef& r_string);
void MCPasteboardEvalDragSource(MCExecContext& ctxt, MCStringRef& r_string);
void MCPasteboardEvalDragDropKeys(MCExecContext& ctxt, MCStringRef& r_string);

void MCPasteboardEvalIsAmongTheKeysOfTheClipboardData(MCExecContext& ctxt, MCNameRef p_key, bool& r_result);
void MCPasteboardEvalIsNotAmongTheKeysOfTheClipboardData(MCExecContext& ctxt, MCNameRef p_key, bool& r_result);
void MCPasteboardEvalIsAmongTheKeysOfTheDragData(MCExecContext& ctxt, MCNameRef p_key, bool& r_result);
void MCPasteboardEvalIsNotAmongTheKeysOfTheDragData(MCExecContext& ctxt, MCNameRef p_key, bool& r_result);

void MCPasteboardEvalDragSourceAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCPasteboardEvalDragDestinationAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);

void MCPasteboardExecPaste(MCExecContext& ctxt);

void MCPasteboardExecCopy(MCExecContext& ctxt);
void MCPasteboardExecCopyTextToClipboard(MCExecContext& ctxt, MCChunk *p_target);
void MCPasteboardExecCopyObjectsToClipboard(MCExecContext& ctxt, MCObjectPtr *p_targets, uindex_t p_target_count);
void MCPasteboardExecCut(MCExecContext& ctxt);
void MCPasteboardExecCutTextToClipboard(MCExecContext& ctxt, MCChunk *p_target);
void MCPasteboardExecCutObjectsToClipboard(MCExecContext& ctxt, MCObjectPtr *p_targets, uindex_t p_target_count);

void MCPasteboardGetAcceptDrop(MCExecContext& ctxt, bool& r_value);
void MCPasteboardSetAcceptDrop(MCExecContext& ctxt, bool p_value);
void MCPasteboardGetDragAction(MCExecContext& ctxt, intenum_t& r_value);
void MCPasteboardSetDragAction(MCExecContext& ctxt, intenum_t p_value);
void MCPasteboardGetDragImage(MCExecContext& ctxt, uinteger_t& r_value);
void MCPasteboardSetDragImage(MCExecContext& ctxt, uinteger_t p_value);
void MCPasteboardGetDragImageOffset(MCExecContext& ctxt, MCPoint*& r_value);
void MCPasteboardSetDragImageOffset(MCExecContext& ctxt, MCPoint *p_value);
void MCPasteboardGetAllowableDragActions(MCExecContext& ctxt, intset_t& r_value);
void MCPasteboardSetAllowableDragActions(MCExecContext& ctxt, intset_t p_value);

void MCPasteboardGetClipboardData(MCExecContext& ctxt, MCStringRef p_index, MCDataRef& r_data);
void MCPasteboardSetClipboardData(MCExecContext& ctxt, MCStringRef p_index, MCDataRef p_data);
void MCPasteboardGetDragData(MCExecContext& ctxt, MCStringRef p_index, MCDataRef& r_data);
void MCPasteboardSetDragData(MCExecContext& ctxt, MCStringRef p_index, MCDataRef p_data);

///////////

struct MCEngineNumberFormat;

extern MCExecCustomTypeInfo *kMCEngineNumberFormatTypeInfo;
extern MCExecSetTypeInfo *kMCEngineSecurityCategoriesTypeInfo;

extern MCExecMethodInfo *kMCEngineEvalVersionMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalBuildNumberMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalPlatformMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalEnvironmentMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalMachineMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalProcessorMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalSystemVersionMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalCommandNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalConstantNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalFunctionNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalPropertyNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalGlobalNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalLocalNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalVariableNamesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalParamMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalParamCountMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalParamsMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalResultMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalBackScriptsMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalFrontScriptsMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalPendingMessagesMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalInterruptMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalMeMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalTargetMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalTargetContentsMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalOwnerMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalScriptLimitsMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalSysErrorMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalValueMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalValueWithObjectMethodInfo;

extern MCExecMethodInfo *kMCEngineEvalValueAsObjectMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalOwnerAsObjectMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalTemplateAsObjectMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalMeAsObjectMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalMenuObjectAsObjectMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalTargetAsObjectMethodInfo;
extern MCExecMethodInfo *kMCEngineEvalErrorObjectAsObjectMethodInfo;

extern MCExecMethodInfo *kMCEngineExecGetMethodInfo;
extern MCExecMethodInfo *kMCEngineExecPutIntoVariableMethodInfo;
extern MCExecMethodInfo *kMCEngineExecPutOutputMethodInfo;
extern MCExecMethodInfo *kMCEngineExecDoMethodInfo;
extern MCExecMethodInfo *kMCEngineExecInsertScriptOfObjectIntoMethodInfo;
extern MCExecMethodInfo *kMCEngineExecQuitMethodInfo;
extern MCExecMethodInfo *kMCEngineExecCancelMessageMethodInfo;
extern MCExecMethodInfo *kMCEngineExecDeleteVariableMethodInfo;
extern MCExecMethodInfo *kMCEngineExecDeleteVariableChunksMethodInfo;
extern MCExecMethodInfo *kMCEngineExecRemoveAllScriptsFromMethodInfo;
extern MCExecMethodInfo *kMCEngineExecRemoveScriptOfObjectFromMethodInfo;
extern MCExecMethodInfo *kMCEngineExecWaitForMethodInfo;
extern MCExecMethodInfo *kMCEngineExecWaitUntilMethodInfo;
extern MCExecMethodInfo *kMCEngineExecWaitWhileMethodInfo;
extern MCExecMethodInfo *kMCEngineExecStartUsingStackMethodInfo;
extern MCExecMethodInfo *kMCEngineExecStartUsingStackByNameMethodInfo;
extern MCExecMethodInfo *kMCEngineExecStopUsingStackMethodInfo;
extern MCExecMethodInfo *kMCEngineExecStopUsingStackByNameMethodInfo;
extern MCExecMethodInfo *kMCEngineExecDispatchMethodInfo;
extern MCExecMethodInfo *kMCEngineExecSendMethodInfo;
extern MCExecMethodInfo *kMCEngineExecSendInTimeMethodInfo;
extern MCExecMethodInfo *kMCEngineExecCallMethodInfo;
extern MCExecMethodInfo *kMCEngineExecLockErrorsMethodInfo;
extern MCExecMethodInfo *kMCEngineExecLockMessagesMethodInfo;
extern MCExecMethodInfo *kMCEngineExecUnlockErrorsMethodInfo;
extern MCExecMethodInfo *kMCEngineExecUnlockMessagesMethodInfo;
extern MCExecMethodInfo *kMCEngineExecSetMethodInfo;
extern MCExecMethodInfo *kMCEngineExecReturnValueMethodInfo;
extern MCExecMethodInfo *kMCEngineSetCaseSensitiveMethodInfo;
extern MCExecMethodInfo *kMCEngineGetCaseSensitiveMethodInfo;
extern MCExecMethodInfo *kMCEngineSetCenturyCutOffMethodInfo;
extern MCExecMethodInfo *kMCEngineGetCenturyCutOffMethodInfo;
extern MCExecMethodInfo *kMCEngineSetConvertOctalsMethodInfo;
extern MCExecMethodInfo *kMCEngineGetConvertOctalsMethodInfo;
extern MCExecMethodInfo *kMCEngineSetItemDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineGetItemDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineSetLineDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineGetLineDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineSetColumnDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineGetColumnDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineSetRowDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineGetRowDelimiterMethodInfo;
extern MCExecMethodInfo *kMCEngineSetWholeMatchesMethodInfo;
extern MCExecMethodInfo *kMCEngineGetWholeMatchesMethodInfo;
extern MCExecMethodInfo *kMCEngineSetUseSystemDateMethodInfo;
extern MCExecMethodInfo *kMCEngineGetUseSystemDateMethodInfo;
extern MCExecMethodInfo *kMCEngineSetUseUnicodeMethodInfo;
extern MCExecMethodInfo *kMCEngineGetUseUnicodeMethodInfo;
extern MCExecMethodInfo *kMCEngineSetNumberFormatMethodInfo;
extern MCExecMethodInfo *kMCEngineGetNumberFormatMethodInfo;
extern MCExecMethodInfo *kMCEngineGetScriptExecutionErrorsMethodInfo;
extern MCExecMethodInfo *kMCEngineGetScriptParsingErrorsMethodInfo;
extern MCExecMethodInfo *kMCEngineGetAllowInterruptsMethodInfo;
extern MCExecMethodInfo *kMCEngineSetAllowInterruptsMethodInfo;
extern MCExecMethodInfo *kMCEngineGetExplicitVariablesMethodInfo;
extern MCExecMethodInfo *kMCEngineSetExplicitVariablesMethodInfo;
extern MCExecMethodInfo *kMCEngineGetPreserveVariablesMethodInfo;
extern MCExecMethodInfo *kMCEngineSetPreserveVariablesMethodInfo;
extern MCExecMethodInfo *kMCEngineGetStackLimitMethodInfo;
extern MCExecMethodInfo *kMCEngineGetEffectiveStackLimitMethodInfo;
extern MCExecMethodInfo *kMCEngineSetStackLimitMethodInfo;
extern MCExecMethodInfo *kMCEngineGetSecureModeMethodInfo;
extern MCExecMethodInfo *kMCEngineSetSecureModeMethodInfo;
extern MCExecMethodInfo *kMCEngineGetSecurityCategoriesMethodInfo;
extern MCExecMethodInfo *kMCEngineGetSecurityPermissionsMethodInfo;
extern MCExecMethodInfo *kMCEngineGetRecursionLimitMethodInfo;
extern MCExecMethodInfo *kMCEngineSetRecursionLimitMethodInfo;
extern MCExecMethodInfo *kMCEngineGetAddressMethodInfo;
extern MCExecMethodInfo *kMCEngineGetStacksInUseMethodInfo;

void MCEngineEvalVersion(MCExecContext& ctxt, MCNameRef& r_name);
void MCEngineEvalBuildNumber(MCExecContext& ctxt, integer_t& r_build_number);
void MCEngineEvalPlatform(MCExecContext& ctxt, MCNameRef& r_name);
void MCEngineEvalEnvironment(MCExecContext& ctxt, MCNameRef& r_name);
void MCEngineEvalMachine(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalProcessor(MCExecContext& ctxt, MCNameRef& r_name);
void MCEngineEvalSystemVersion(MCExecContext& ctxt, MCStringRef& r_string);

void MCEngineEvalCommandNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalConstantNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalFunctionNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalPropertyNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalGlobalNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalLocalNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalVariableNames(MCExecContext& ctxt, MCStringRef& r_string);

void MCEngineEvalParam(MCExecContext& ctxt, integer_t p_index, MCValueRef& r_value);
void MCEngineEvalParamCount(MCExecContext& ctxt, integer_t& r_count);
void MCEngineEvalParams(MCExecContext& ctxt, MCStringRef& r_string);

void MCEngineEvalResult(MCExecContext& ctxt, MCValueRef& r_value);

void MCEngineEvalBackScripts(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalFrontScripts(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalPendingMessages(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalInterrupt(MCExecContext& ctxt, bool& r_bool);

void MCEngineEvalMe(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalTarget(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalTargetContents(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalOwner(MCExecContext& ctxt, MCObjectPtr p_object, MCStringRef& r_string);

void MCEngineEvalScriptLimits(MCExecContext& ctxt, MCStringRef& r_string);
void MCEngineEvalSysError(MCExecContext& ctxt, uinteger_t& r_error);

void MCEngineEvalValue(MCExecContext& ctxt, MCStringRef p_script, MCValueRef& r_value);
void MCEngineEvalValueWithObject(MCExecContext& ctxt, MCStringRef p_script, MCObjectPtr p_object, MCValueRef& r_value);

bool MCEngineEvalValueAsObject(MCValueRef p_value, bool p_strict, MCObjectPtr& r_object, bool& r_parse_error);
void MCEngineEvalValueAsObject(MCExecContext& ctxt, MCValueRef p_value, MCObjectPtr& r_object);
void MCEngineEvalOwnerAsObject(MCExecContext& ctxt, MCObjectPtr p_object, MCObjectPtr& r_owner);
void MCEngineEvalTemplateAsObject(MCExecContext& ctxt, uinteger_t p_template_type, MCObjectPtr& r_object);
void MCEngineEvalMeAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCEngineEvalMenuObjectAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCEngineEvalTargetAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);
void MCEngineEvalErrorObjectAsObject(MCExecContext& ctxt, MCObjectPtr& r_object);

void MCEngineExecGet(MCExecContext& ctxt, MCValueRef value);
void MCEngineExecPutIntoVariable(MCExecContext& ctxt, MCValueRef value, int where, MCVariableChunkPtr t_target);
void MCEngineExecPutOutput(MCExecContext& ctxt, MCStringRef value, bool unicode);

void MCEngineExecDo(MCExecContext& ctxt, MCStringRef p_script, int p_line, int p_pos);
void MCEngineExecInsertScriptOfObjectInto(MCExecContext& ctxt, MCObject *p_script, bool p_in_front);
void MCEngineExecQuit(MCExecContext& ctxt, integer_t p_retcode);

void MCEngineExecCancelMessage(MCExecContext& ctxt, integer_t p_id);

void MCEngineExecDeleteVariable(MCExecContext& ctxt, MCVarref *p_target);
void MCEngineExecDeleteVariableChunks(MCExecContext& ctxt, MCVariableChunkPtr *chunk, uindex_t chunk_count);

void MCEngineExecRemoveAllScriptsFrom(MCExecContext& ctxt, bool p_in_front);
void MCEngineExecRemoveScriptOfObjectFrom(MCExecContext& ctxt, MCObject *p_script, bool p_in_front);

void MCEngineExecWaitFor(MCExecContext& ctxt, double p_delay, int p_units, bool p_messages);
void MCEngineExecWaitUntil(MCExecContext& ctxt, MCExpression *p_condition, bool p_messages);
void MCEngineExecWaitWhile(MCExecContext& ctxt, MCExpression *p_condition, bool p_messages);

void MCEngineExecStartUsingStack(MCExecContext& ctxt, MCStack *p_stack);
void MCEngineExecStartUsingStackByName(MCExecContext& ctxt, MCStringRef p_name);

void MCEngineExecStopUsingStack(MCExecContext& ctxt, MCStack *p_stack);
void MCEngineExecStopUsingStackByName(MCExecContext& ctxt, MCStringRef p_name);

void MCEngineExecDispatch(MCExecContext& ctxt, int handler_type, MCNameRef message, MCObjectPtr *target, MCParameter *params);
void MCEngineExecSend(MCExecContext& ctxt, MCStringRef script, MCObjectPtr *target);
void MCEngineExecSendInTime(MCExecContext& ctxt, MCStringRef script, MCObjectPtr target, double p_delay, int p_units);
void MCEngineExecCall(MCExecContext& ctxt, MCStringRef script, MCObjectPtr *target);

void MCEngineExecLockErrors(MCExecContext& ctxt);
void MCEngineExecLockMessages(MCExecContext& ctxt);

void MCEngineExecUnlockErrors(MCExecContext& ctxt);
void MCEngineExecUnlockMessages(MCExecContext& ctxt);

void MCEngineExecSet(MCExecContext& ctxt, MCProperty *target, MCValueRef value);
void MCEngineExecReturnValue(MCExecContext& ctxt, MCValueRef value);

void MCEngineSetCaseSensitive(MCExecContext& ctxt, bool p_value);
void MCEngineGetCaseSensitive(MCExecContext& ctxt, bool& r_value);
void MCEngineSetCenturyCutOff(MCExecContext& ctxt, integer_t p_value);
void MCEngineGetCenturyCutOff(MCExecContext& ctxt, integer_t& r_value);
void MCEngineSetConvertOctals(MCExecContext& ctxt, bool p_value);
void MCEngineGetConvertOctals(MCExecContext& ctxt, bool& r_value);
void MCEngineSetItemDelimiter(MCExecContext& ctxt, char_t p_value);
void MCEngineGetItemDelimiter(MCExecContext& ctxt, char_t& r_value);
void MCEngineSetLineDelimiter(MCExecContext& ctxt, char_t p_value);
void MCEngineGetLineDelimiter(MCExecContext& ctxt, char_t& r_value);
void MCEngineSetColumnDelimiter(MCExecContext& ctxt, char_t p_value);
void MCEngineGetColumnDelimiter(MCExecContext& ctxt, char_t& r_value);
void MCEngineSetRowDelimiter(MCExecContext& ctxt, char_t p_value);
void MCEngineGetRowDelimiter(MCExecContext& ctxt, char_t& r_value);
void MCEngineSetWholeMatches(MCExecContext& ctxt, bool p_value);
void MCEngineGetWholeMatches(MCExecContext& ctxt, bool& r_value);
void MCEngineSetUseSystemDate(MCExecContext& ctxt, bool p_value);
void MCEngineGetUseSystemDate(MCExecContext& ctxt, bool& r_value);
void MCEngineSetUseUnicode(MCExecContext& ctxt, bool p_value);
void MCEngineGetUseUnicode(MCExecContext& ctxt, bool& r_value);
void MCEngineSetNumberFormat(MCExecContext& ctxt, const MCEngineNumberFormat& format);
void MCEngineGetNumberFormat(MCExecContext& ctxt, MCEngineNumberFormat& r_format);

void MCEngineGetScriptExecutionErrors(MCExecContext& ctxt, MCStringRef &r_value);
void MCEngineGetScriptParsingErrors(MCExecContext& ctxt, MCStringRef &r_value);
void MCEngineGetAllowInterrupts(MCExecContext& ctxt, bool& r_value);
void MCEngineSetAllowInterrupts(MCExecContext& ctxt, bool p_value);
void MCEngineGetExplicitVariables(MCExecContext& ctxt, bool& r_value);
void MCEngineSetExplicitVariables(MCExecContext& ctxt, bool p_value);
void MCEngineGetPreserveVariables(MCExecContext& ctxt, bool& r_value);
void MCEngineSetPreserveVariables(MCExecContext& ctxt, bool p_value);

void MCEngineGetStackLimit(MCExecContext& ctxt, uinteger_t& r_limit);
void MCEngineGetEffectiveStackLimit(MCExecContext& ctxt, uinteger_t& r_limit);
void MCEngineSetStackLimit(MCExecContext& ctxt, uinteger_t limit);

void MCEngineGetSecureMode(MCExecContext& ctxt, bool& r_value);
void MCEngineSetSecureMode(MCExecContext& ctxt, bool p_value);
void MCEngineGetSecurityCategories(MCExecContext& ctxt, intset_t& r_value);
void MCEngineGetSecurityPermissions(MCExecContext& ctxt, intset_t& r_value);

void MCEngineGetRecursionLimit(MCExecContext& ctxt, uinteger_t& r_value);
void MCEngineSetRecursionLimit(MCExecContext& ctxt, uinteger_t p_value);

void MCEngineGetAddress(MCExecContext& ctxt, MCStringRef &r_value);
void MCEngineGetStacksInUse(MCExecContext& ctxt, MCStringRef &r_value);

///////////

extern MCExecMethodInfo *kMCFilesEvalDirectoriesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalFilesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalDiskSpaceMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalDriverNamesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalDrivesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalOpenFilesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalTempNameMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalSpecialFolderPathMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalLongFilePathMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalShortFilePathMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalOpenProcessesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalOpenProcessesIdsMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalProcessIdMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalDeleteRegistryMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalListRegistryMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalQueryRegistryMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalQueryRegistryWithTypeMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalSetRegistryMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalSetRegistryWithTypeMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalCopyResourceWithNewIdMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalCopyResourceMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalDeleteResourceMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalGetResourceMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalGetResourcesWithTypeMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalGetResourcesMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalSetResourceMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalAliasReferenceMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalThereIsAFileMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalThereIsNotAFileMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalThereIsAFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalThereIsNotAFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalThereIsAProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalThereIsNotAProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesEvalShellMethodInfo;
extern MCExecMethodInfo *kMCFilesExecDeleteFileMethodInfo;
extern MCExecMethodInfo *kMCFilesExecCloseFileMethodInfo;
extern MCExecMethodInfo *kMCFilesExecCloseDriverMethodInfo;
extern MCExecMethodInfo *kMCFilesExecCloseProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesExecLaunchUrlMethodInfo;
extern MCExecMethodInfo *kMCFilesExecLaunchDocumentMethodInfo;
extern MCExecMethodInfo *kMCFilesExecLaunchAppMethodInfo;
extern MCExecMethodInfo *kMCFilesExecOpenFileMethodInfo;
extern MCExecMethodInfo *kMCFilesExecOpenDriverMethodInfo;
extern MCExecMethodInfo *kMCFilesExecOpenProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesExecOpenElevatedProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesExecRenameMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromStdinForMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromStdinUntilMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverForMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverUntilMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverAtForMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverAtUntilMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverAtEndForMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverAtEndUntilMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverAtEndForLegacyMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromFileOrDriverAtEndUntilLegacyMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromProcessForMethodInfo;
extern MCExecMethodInfo *kMCFilesExecReadFromProcessUntilMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToStdoutMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToStderrMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToFileOrDriverMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToFileOrDriverAtMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToFileOrDriverAtEndMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToFileOrDriverAtEndLegacyMethodInfo;
extern MCExecMethodInfo *kMCFilesExecWriteToProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesExecSeekToEofInFileMethodInfo;
extern MCExecMethodInfo *kMCFilesExecSeekToEofInFileLegacyMethodInfo;
extern MCExecMethodInfo *kMCFilesExecSeekAbsoluteInFileMethodInfo;
extern MCExecMethodInfo *kMCFilesExecSeekRelativeInFileMethodInfo;
extern MCExecMethodInfo *kMCFilesExecCreateFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesExecCreateAliasMethodInfo;
extern MCExecMethodInfo *kMCFilesExecKillProcessMethodInfo;
extern MCExecMethodInfo *kMCFilesGetUMaskMethodInfo;
extern MCExecMethodInfo *kMCFilesSetUMaskMethodInfo;
extern MCExecMethodInfo *kMCFilesGetFileTypeMethodInfo;
extern MCExecMethodInfo *kMCFilesSetFileTypeMethodInfo;
extern MCExecMethodInfo *kMCFilesGetSerialControlStringMethodInfo;
extern MCExecMethodInfo *kMCFilesSetSerialControlStringMethodInfo;
extern MCExecMethodInfo *kMCFilesGetHideConsoleWindowsMethodInfo;
extern MCExecMethodInfo *kMCFilesSetHideConsoleWindowsMethodInfo;
extern MCExecMethodInfo *kMCFilesGetShellCommandMethodInfo;
extern MCExecMethodInfo *kMCFilesSetShellCommandMethodInfo;
extern MCExecMethodInfo *kMCFilesGetCurrentFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesSetCurrentFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesGetEngineFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesGetHomeFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesGetDocumentsFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesGetDesktopFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesGetTemporaryFolderMethodInfo;
extern MCExecMethodInfo *kMCFilesGetFilesMethodInfo;
extern MCExecMethodInfo *kMCFilesGetDetailedFilesMethodInfo;
extern MCExecMethodInfo *kMCFilesGetFoldersMethodInfo;
extern MCExecMethodInfo *kMCFilesGetDetailedFoldersMethodInfo;

void MCFilesEvalDirectories(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalFiles(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalDiskSpace(MCExecContext& ctxt, real64_t& r_result);
void MCFilesEvalDriverNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalDrives(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalOpenFiles(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalTempName(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalSpecialFolderPath(MCExecContext& ctxt, MCStringRef p_folder, MCStringRef& r_path);
void MCFilesEvalLongFilePath(MCExecContext& ctxt, MCStringRef p_path, MCStringRef& r_long_path);
void MCFilesEvalShortFilePath(MCExecContext& ctxt, MCStringRef p_path, MCStringRef& r_short_path);
void MCFilesEvalOpenProcesses(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalOpenProcessesIds(MCExecContext& ctxt, MCStringRef& r_string);
void MCFilesEvalProcessId(MCExecContext& ctxt, integer_t& r_pid);
void MCFilesEvalDeleteRegistry(MCExecContext& ctxt, MCStringRef p_key, bool& r_deleted);
void MCFilesEvalListRegistry(MCExecContext& ctxt, MCStringRef p_key, MCStringRef& p_list);
void MCFilesEvalQueryRegistry(MCExecContext& ctxt, MCStringRef p_key, MCStringRef& r_string);
void MCFilesEvalQueryRegistryWithType(MCExecContext& ctxt, MCStringRef p_key, MCStringRef& r_type, MCStringRef& r_string);
void MCFilesEvalSetRegistry(MCExecContext& ctxt, MCStringRef p_key, MCStringRef p_value, bool& r_set);
void MCFilesEvalSetRegistryWithType(MCExecContext& ctxt, MCStringRef p_key, MCStringRef p_value, MCStringRef p_type, bool& r_set);
void MCFilesEvalCopyResourceWithNewId(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_dest, MCStringRef p_type, MCStringRef p_name, MCStringRef p_newid, MCStringRef& r_result);
void MCFilesEvalCopyResource(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_dest, MCStringRef p_type, MCStringRef p_name, MCStringRef& r_result);
void MCFilesEvalDeleteResource(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_type, MCStringRef p_name, MCStringRef& r_result);
void MCFilesEvalGetResource(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_type, MCStringRef p_name, MCStringRef& r_result);
void MCFilesEvalGetResourcesWithType(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_type, MCStringRef& r_string);
void MCFilesEvalGetResources(MCExecContext& ctxt, MCStringRef p_source, MCStringRef& r_string);
void MCFilesEvalSetResource(MCExecContext& ctxt, MCStringRef p_source, MCStringRef p_type, MCStringRef p_id, MCStringRef p_name, MCStringRef p_flags, MCStringRef p_value, MCStringRef& r_result);
void MCFilesEvalAliasReference(MCExecContext& ctxt, MCStringRef p_path, MCStringRef& r_reference);

void MCFilesEvalThereIsAFile(MCExecContext& ctxt, MCStringRef p_path, bool& r_result);
void MCFilesEvalThereIsNotAFile(MCExecContext& ctxt, MCStringRef p_path, bool& r_result);
void MCFilesEvalThereIsAFolder(MCExecContext& ctxt, MCStringRef p_path, bool& r_result);
void MCFilesEvalThereIsNotAFolder(MCExecContext& ctxt, MCStringRef p_path, bool& r_result);
void MCFilesEvalThereIsAProcess(MCExecContext& ctxt, MCStringRef p_path, bool& r_result);
void MCFilesEvalThereIsNotAProcess(MCExecContext& ctxt, MCStringRef p_path, bool& r_result);

void MCFilesEvalShell(MCExecContext& ctxt, MCStringRef command, MCStringRef& r_output);

void MCFilesExecDeleteFile(MCExecContext& ctxt, MCStringRef p_target);

void MCFilesExecCloseFile(MCExecContext& ctxt, MCNameRef p_filename);
void MCFilesExecCloseDriver(MCExecContext& ctxt, MCNameRef p_device);
void MCFilesExecCloseProcess(MCExecContext& ctxt, MCNameRef p_process);

void MCFilesExecLaunchUrl(MCExecContext& ctxt, MCStringRef p_url);
void MCFilesExecLaunchDocument(MCExecContext& ctxt, MCStringRef p_document);
void MCFilesExecLaunchApp(MCExecContext& ctxt, MCNameRef p_app, MCStringRef p_document);

void MCFilesExecOpenFile(MCExecContext& ctxt, MCNameRef p_filename, int p_mode, bool p_text_mode);
void MCFilesExecOpenDriver(MCExecContext& ctxt, MCNameRef p_device, int p_mode, bool p_text_mode);
void MCFilesExecOpenProcess(MCExecContext& ctxt, MCNameRef p_process, int p_mode, bool p_text_mode);
void MCFilesExecOpenElevatedProcess(MCExecContext& ctxt, MCNameRef p_process, int p_mode, bool p_text_mode);

void MCFilesExecRename(MCExecContext& ctxt, MCStringRef from, MCStringRef to);

void MCFilesExecReadFromStdinFor(MCExecContext& ctxt, uint4 p_count, int p_unit_type);
void MCFilesExecReadFromStdinUntil(MCExecContext& ctxt, MCStringRef p_sentinel);
void MCFilesExecReadFromFileOrDriverFor(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, uint4 p_count, int p_unit_type, double p_max_wait, int p_time_units);
void MCFilesExecReadFromFileOrDriverUntil(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, MCStringRef p_sentinel, double p_max_wait, int p_time_units);
void MCFilesExecReadFromFileOrDriverAtFor(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, int64_t p_at, uint4 p_count, int p_unit_type, double p_max_wait, int p_time_units);
void MCFilesExecReadFromFileOrDriverAtUntil(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, int64_t p_at, MCStringRef p_sentinel, double p_max_wait, int p_time_units);
void MCFilesExecReadFromFileOrDriverAtEndFor(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, int64_t p_at, uint4 p_count, int p_unit_type, double p_max_wait, int p_time_units);
void MCFilesExecReadFromFileOrDriverAtEndUntil(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, int64_t p_at, MCStringRef p_sentinel, double p_max_wait, int p_time_units);
void MCFilesExecReadFromProcessFor(MCExecContext& ctxt, MCNameRef p_process, uint4 p_count, int p_unit_type, double p_max_wait, int p_time_units);
void MCFilesExecReadFromProcessUntil(MCExecContext& ctxt, MCNameRef p_process, MCStringRef p_sentinel, double p_max_wait, int p_time_units);

void MCFilesExecWriteToStdout(MCExecContext& ctxt, MCStringRef p_data, int p_unit_type);
void MCFilesExecWriteToStderr(MCExecContext& ctxt, MCStringRef p_data, int p_unit_type);
void MCFilesExecWriteToFileOrDriver(MCExecContext& ctxt, MCNameRef p_file, MCStringRef p_data, int p_unit_type);
void MCFilesExecWriteToFileOrDriverAt(MCExecContext& ctxt, MCNameRef p_file, MCStringRef p_data, int p_unit_type, int64_t p_at);
void MCFilesExecWriteToFileOrDriverAtEnd(MCExecContext& ctxt, MCNameRef p_file, MCStringRef p_data, int p_unit_type);
void MCFilesExecWriteToProcess(MCExecContext& ctxt, MCNameRef p_process, MCStringRef p_data, int p_unit_type);

void MCFilesExecSeekToEofInFile(MCExecContext& ctxt, MCNameRef p_file);
void MCFilesExecSeekAbsoluteInFile(MCExecContext& ctxt, int64_t p_to, MCNameRef p_file);
void MCFilesExecSeekRelativeInFile(MCExecContext& ctxt, int64_t p_by, MCNameRef p_file);

void MCFilesExecReadFromFileOrDriverAtEndForLegacy(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, intenum_t p_eof, uint4 p_count, int p_unit_type, double p_max_wait, int p_time_units);
void MCFilesExecReadFromFileOrDriverAtEndUntilLegacy(MCExecContext& ctxt, bool p_driver, MCNameRef p_file, intenum_t p_eof, MCStringRef p_sentinel, double p_max_wait, int p_time_units);
void MCFilesExecSeekToEofInFileLegacy(MCExecContext& ctxt, intenum_t p_eof);
void MCFilesExecWriteToFileOrDriverAtEndLegacy(MCExecContext& ctxt, MCNameRef p_file, MCStringRef p_data, int p_unit_type, intenum_t p_eof);

void MCFilesExecCreateFolder(MCExecContext& ctxt, MCStringRef p_filename);
void MCFilesExecCreateAlias(MCExecContext& ctxt, MCStringRef p_target_filename, MCStringRef p_alias_filename);

void MCFilesExecKillProcess(MCExecContext& ctxt, MCStringRef p_process, MCStringRef p_signal);

void MCFilesGetUMask(MCExecContext& ctxt, uinteger_t& r_value);
void MCFilesSetUMask(MCExecContext& ctxt, uinteger_t p_value);
void MCFilesGetFileType(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesSetFileType(MCExecContext& ctxt, MCStringRef p_value);

void MCFilesGetSerialControlString(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesSetSerialControlString(MCExecContext& ctxt, MCStringRef p_value);
void MCFilesGetHideConsoleWindows(MCExecContext& ctxt, bool& r_value);
void MCFilesSetHideConsoleWindows(MCExecContext& ctxt, bool p_value);

void MCFilesGetShellCommand(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesSetShellCommand(MCExecContext& ctxt, MCStringRef p_value);
void MCFilesGetCurrentFolder(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesSetCurrentFolder(MCExecContext& ctxt, MCStringRef p_value);
void MCFilesGetEngineFolder(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetHomeFolder(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetDocumentsFolder(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetDesktopFolder(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetTemporaryFolder(MCExecContext& ctxt, MCStringRef& r_value);

void MCFilesGetFiles(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetDetailedFiles(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetFolders(MCExecContext& ctxt, MCStringRef& r_value);
void MCFilesGetDetailedFolders(MCExecContext& ctxt, MCStringRef& r_value);

///////////

extern MCExecEnumTypeInfo *kMCMultimediaRecordFormatTypeInfo;

extern MCExecMethodInfo *kMCMultimediaExecAnswerEffectMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecAnswerRecordMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalQTVersionMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalQTEffectsMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalRecordCompressionTypesMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalRecordLoudnessMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalMovieMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalMCISendStringMethodInfo;
extern MCExecMethodInfo *kMCMultimediaEvalSoundMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecRecordMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecStartPlayerMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecStopPlayingMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecStopPlayingObjectMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecStopRecordingMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPrepareVideoClipMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPlayAudioClipMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPlayVideoClipMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPlayStopAudioMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPlayPlayerOperationMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPlayVideoOperationMethodInfo;
extern MCExecMethodInfo *kMCMultimediaExecPlayLastVideoOperationMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordFormatMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordFormatMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordCompressionMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordCompressionMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordInputMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordInputMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordSampleSizeMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordSampleSizeMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordChannelsMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordChannelsMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordRateMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordRateMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetPlayDestinationMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetPlayDestinationMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetPlayLoudnessMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetPlayLoudnessMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetQtIdleRateMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetQtIdleRateMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetDontUseQtMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetDontUseQtMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetDontUseQtEffectsMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetDontUseQtEffectsMethodInfo;
extern MCExecMethodInfo *kMCMultimediaGetRecordingMethodInfo;
extern MCExecMethodInfo *kMCMultimediaSetRecordingMethodInfo;

void MCMultimediaExecAnswerEffect(MCExecContext &ctxt);
void MCMultimediaExecAnswerRecord(MCExecContext &ctxt);

void MCMultimediaEvalQTVersion(MCExecContext& ctxt, MCStringRef& r_string);
void MCMultimediaEvalQTEffects(MCExecContext& ctxt, MCStringRef& r_result);
void MCMultimediaEvalRecordCompressionTypes(MCExecContext& ctxt, MCStringRef& r_string);
void MCMultimediaEvalRecordLoudness(MCExecContext& ctxt, integer_t& r_loudness);
void MCMultimediaEvalMovie(MCExecContext& ctxt, MCStringRef& r_string);
void MCMultimediaEvalMCISendString(MCExecContext& ctxt, MCStringRef p_command, MCStringRef& r_result);
void MCMultimediaEvalSound(MCExecContext& ctxt, MCStringRef& r_sound);

void MCMultimediaExecRecord(MCExecContext& ctxt, MCStringRef p_filename);
void MCMultimediaExecStartPlayer(MCExecContext& ctxt, MCPlayer *p_target);

void MCMultimediaExecStopPlaying(MCExecContext& ctxt);
void MCMultimediaExecStopPlayingObject(MCExecContext& ctxt, MCObject *p_object);
void MCMultimediaExecStopRecording(MCExecContext& ctxt);

void MCMultimediaExecPrepareVideoClip(MCExecContext& ctxt, MCStack *p_target, int p_chunk_type, MCStringRef p_clip, bool p_looping, MCPoint *p_at, MCStringRef p_options);
void MCMultimediaExecPlayAudioClip(MCExecContext& ctxt, MCStack *p_target, int p_chunk_type, MCStringRef p_clip, bool p_looping);
void MCMultimediaExecPlayVideoClip(MCExecContext& ctxt, MCStack *p_target, int p_chunk_type, MCStringRef p_clip, bool p_looping, MCPoint *p_at, MCStringRef p_options);
void MCMultimediaExecPlayStopAudio(MCExecContext& ctxt);
void MCMultimediaExecPlayPlayerOperation(MCExecContext& ctxt, MCStack *p_target, int p_chunk_type, MCStringRef p_player, int p_player_chunk_type, int p_operation);
void MCMultimediaExecPlayVideoOperation(MCExecContext& ctxt, MCStack *p_target, int p_chunk_type, MCStringRef p_clip, int p_operation);
void MCMultimediaExecPlayLastVideoOperation(MCExecContext& ctxt, int p_operation);

void MCMultimediaGetRecordFormat(MCExecContext& ctxt, intenum_t &r_value);
void MCMultimediaSetRecordFormat(MCExecContext& ctxt, intenum_t p_value);
void MCMultimediaGetRecordCompression(MCExecContext& ctxt, MCStringRef& r_value);
void MCMultimediaSetRecordCompression(MCExecContext& ctxt, MCStringRef p_value);
void MCMultimediaGetRecordInput(MCExecContext& ctxt, MCStringRef& r_value);
void MCMultimediaSetRecordInput(MCExecContext& ctxt, MCStringRef p_value);
void MCMultimediaGetRecordSampleSize(MCExecContext& ctxt, uinteger_t& r_value);
void MCMultimediaSetRecordSampleSize(MCExecContext& ctxt, uint2 p_value);
void MCMultimediaGetRecordChannels(MCExecContext& ctxt, uinteger_t& r_value);
void MCMultimediaSetRecordChannels(MCExecContext& ctxt, uint2 p_value);
void MCMultimediaGetRecordRate(MCExecContext& ctxt, double& r_value);
void MCMultimediaSetRecordRate(MCExecContext& ctxt, double p_value);

void MCMultimediaGetPlayDestination(MCExecContext& ctxt, MCStringRef& r_dest);
void MCMultimediaSetPlayDestination(MCExecContext& ctxt, MCStringRef dest);
void MCMultimediaGetPlayLoudness(MCExecContext& ctxt, uinteger_t& r_loudness);
void MCMultimediaSetPlayLoudness(MCExecContext& ctxt, uinteger_t loudness);

void MCMultimediaGetQtIdleRate(MCExecContext& ctxt, uinteger_t& r_value);
void MCMultimediaSetQtIdleRate(MCExecContext& ctxt, uinteger_t p_value);
void MCMultimediaGetDontUseQt(MCExecContext& ctxt, bool& r_value);
void MCMultimediaSetDontUseQt(MCExecContext& ctxt, bool p_value);
void MCMultimediaGetDontUseQtEffects(MCExecContext& ctxt, bool& r_value);
void MCMultimediaSetDontUseQtEffects(MCExecContext& ctxt, bool p_value);

void MCMultimediaGetRecording(MCExecContext& ctxt, bool& r_value);
void MCMultimediaSetRecording(MCExecContext& ctxt, bool p_value);

///////////

extern MCExecMethodInfo *kMCTextEvalFontNamesMethodInfo;
extern MCExecMethodInfo *kMCTextEvalFontLanguageMethodInfo;
extern MCExecMethodInfo *kMCTextEvalFontSizesMethodInfo;
extern MCExecMethodInfo *kMCTextEvalFontStylesMethodInfo;

void MCTextEvalFontNames(MCExecContext& ctxt, MCStringRef p_type, MCStringRef& r_names);
void MCTextEvalFontLanguage(MCExecContext& ctxt, MCStringRef p_font, MCNameRef& r_lang);
void MCTextEvalFontSizes(MCExecContext& ctxt, MCStringRef p_font, MCStringRef& r_sizes);
void MCTextEvalFontStyles(MCExecContext& ctxt, MCStringRef p_font, integer_t p_size, MCStringRef& r_styles);

///////////

extern MCExecMethodInfo *kMCNetworkEvalDNSServersMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalCachedUrlsMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalUrlStatusMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalHostAddressMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalPeerAddressMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalHostAddressToNameMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalHostNameToAddressMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalHostNameMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalOpenSocketsMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalHTTPProxyForURLMethodInfo;
extern MCExecMethodInfo *kMCNetworkEvalHTTPProxyForURLWithPACMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecCloseSocketMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecDeleteUrlMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecLoadUrlMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecUnloadUrlMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecOpenSocketMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecOpenSecureSocketMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecOpenDatagramSocketMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecPostToUrlMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecAcceptConnectionsOnPortMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecAcceptDatagramConnectionsOnPortMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecAcceptSecureConnectionsOnPortMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecReadFromSocketForMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecReadFromSocketUntilMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecWriteToSocketMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecPutIntoUrlMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecReturnValueAndUrlResultMethodInfo;
extern MCExecMethodInfo *kMCNetworkExecReturnValueAndUrlResultFromVarMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetUrlResponseMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetFtpProxyMethodInfo;
extern MCExecMethodInfo *kMCNetworkSetFtpProxyMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetHttpProxyMethodInfo;
extern MCExecMethodInfo *kMCNetworkSetHttpProxyMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetHttpHeadersMethodInfo;
extern MCExecMethodInfo *kMCNetworkSetHttpHeadersMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetSocketTimeoutMethodInfo;
extern MCExecMethodInfo *kMCNetworkSetSocketTimeoutMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetDefaultNetworkInterfaceMethodInfo;
extern MCExecMethodInfo *kMCNetworkSetDefaultNetworkInterfaceMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetNetworkInterfacesMethodInfo;
extern MCExecMethodInfo *kMCNetworkGetAllowDatagramBroadcastsMethodInfo;
extern MCExecMethodInfo *kMCNetworkSetAllowDatagramBroadcastsMethodInfo;

void MCNetworkEvalDNSServers(MCExecContext& ctxt, MCStringRef& r_servers);
void MCNetworkEvalCachedUrls(MCExecContext& ctxt, MCStringRef& r_string);
void MCNetworkEvalUrlStatus(MCExecContext& ctxt, MCStringRef p_url, MCStringRef& r_status);
void MCNetworkEvalHostAddress(MCExecContext& ctxt, MCNameRef p_socket, MCStringRef& r_address);
void MCNetworkEvalPeerAddress(MCExecContext& ctxt, MCNameRef p_socket, MCStringRef& r_address);
void MCNetworkEvalHostAddressToName(MCExecContext& ctxt, MCStringRef p_address, MCStringRef &r_name);
void MCNetworkEvalHostNameToAddress(MCExecContext& ctxt, MCStringRef p_hostname, MCNameRef p_message, MCStringRef& r_string);
void MCNetworkEvalHostName(MCExecContext& ctxt, MCStringRef& r_string);
void MCNetworkEvalOpenSockets(MCExecContext& ctxt, MCStringRef& r_string);

void MCNetworkEvalHTTPProxyForURL(MCExecContext& ctxt, MCStringRef p_url, MCStringRef p_host, MCStringRef& r_proxy);
void MCNetworkEvalHTTPProxyForURLWithPAC(MCExecContext& ctxt, MCStringRef p_url, MCStringRef p_host, MCStringRef p_pac, MCStringRef& r_proxy);

void MCNetworkExecCloseSocket(MCExecContext& ctxt, MCNameRef p_socket);

void MCNetworkExecDeleteUrl(MCExecContext& ctxt, MCStringRef p_target);

void MCNetworkExecLoadUrl(MCExecContext& ctxt, MCStringRef p_url, MCNameRef p_message);
void MCNetworkExecUnloadUrl(MCExecContext& ctxt, MCStringRef p_url);

void MCNetworkExecOpenSocket(MCExecContext& ctxt, MCNameRef p_name, MCNameRef p_message);
void MCNetworkExecOpenSecureSocket(MCExecContext& ctxt, MCNameRef p_name, MCNameRef p_message, bool p_with_verification);
void MCNetworkExecOpenDatagramSocket(MCExecContext& ctxt, MCNameRef p_name, MCNameRef p_message);

void MCNetworkExecPostToUrl(MCExecContext& ctxt, MCDataRef p_data, MCStringRef p_url);

void MCNetworkExecAcceptConnectionsOnPort(MCExecContext& ctxt, uint2 p_port, MCNameRef p_message);
void MCNetworkExecAcceptDatagramConnectionsOnPort(MCExecContext& ctxt, uint2 p_port, MCNameRef p_message);
void MCNetworkExecAcceptSecureConnectionsOnPort(MCExecContext& ctxt, uint2 p_port, MCNameRef p_message, bool p_with_verification);

void MCNetworkExecReadFromSocketFor(MCExecContext& ctxt, MCNameRef p_socket, uint4 p_count, int p_unit_type, MCNameRef p_message);
void MCNetworkExecReadFromSocketUntil(MCExecContext& ctxt, MCNameRef p_socket, MCStringRef p_sentinel, MCNameRef p_message);

void MCNetworkExecWriteToSocket(MCExecContext& ctxt, MCNameRef p_socket, MCStringRef p_data, MCNameRef p_message);

void MCNetworkExecPutIntoUrl(MCExecContext& ctxt, MCStringRef value, int prep, MCUrlChunkPtr url);

void MCNetworkExecReturnValueAndUrlResult(MCExecContext& ctxt, MCValueRef value, MCValueRef url_result);
void MCNetworkExecReturnValueAndUrlResultFromVar(MCExecContext& ctxt, MCValueRef result, MCVarref *variable);

void MCNetworkGetUrlResponse(MCExecContext& ctxt, MCStringRef& r_value);

void MCNetworkGetFtpProxy(MCExecContext& ctxt, MCStringRef& r_value);
void MCNetworkSetFtpProxy(MCExecContext& ctxt, MCStringRef p_value);
void MCNetworkGetHttpProxy(MCExecContext& ctxt, MCStringRef& r_value);
void MCNetworkSetHttpProxy(MCExecContext& ctxt, MCStringRef p_value);
void MCNetworkGetHttpHeaders(MCExecContext& ctxt, MCStringRef& r_value);
void MCNetworkSetHttpHeaders(MCExecContext& ctxt, MCStringRef p_value);
void MCNetworkGetSocketTimeout(MCExecContext& ctxt, double& r_value);
void MCNetworkSetSocketTimeout(MCExecContext& ctxt, double p_value);

void MCNetworkGetDefaultNetworkInterface(MCExecContext& ctxt, MCStringRef& r_value);
void MCNetworkSetDefaultNetworkInterface(MCExecContext& ctxt, MCStringRef p_value);
void MCNetworkGetNetworkInterfaces(MCExecContext& ctxt, MCStringRef& r_value);

void MCNetworkGetAllowDatagramBroadcasts(MCExecContext& ctxt, bool& r_value);
void MCNetworkSetAllowDatagramBroadcasts(MCExecContext& ctxt, bool p_value);

///////////

extern MCExecMethodInfo *kMCDateTimeEvalMillisecondsMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalSecondsMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalTicksMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalDateMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalTimeMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalDateFormatMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalMonthNamesMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalWeekDayNamesMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalIsADateMethodInfo;
extern MCExecMethodInfo *kMCDateTimeEvalIsNotADateMethodInfo;
extern MCExecMethodInfo *kMCDateTimeExecConvertMethodInfo;
extern MCExecMethodInfo *kMCDateTimeExecConvertIntoItMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetTwelveTimeMethodInfo;
extern MCExecMethodInfo *kMCDateTimeSetTwelveTimeMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetDateMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetTimeMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetMillisecondsMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetLongMillisecondsMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetSecondsMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetLongSecondsMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetTicksMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetLongTicksMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetMonthNamesMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetWeekDayNamesMethodInfo;
extern MCExecMethodInfo *kMCDateTimeGetDateFormatMethodInfo;

void MCDateTimeEvalMilliseconds(MCExecContext& ctxt, real64_t& r_real);
void MCDateTimeEvalSeconds(MCExecContext& ctxt, real64_t& r_seconds);
void MCDateTimeEvalTicks(MCExecContext& ctxt, real64_t& r_ticks);

void MCDateTimeEvalDate(MCExecContext& ctxt, MCStringRef& r_string);
void MCDateTimeEvalTime(MCExecContext& ctxt, MCStringRef& r_string);

void MCDateTimeEvalDateFormat(MCExecContext& ctxt, MCStringRef& r_string);
void MCDateTimeEvalMonthNames(MCExecContext& ctxt, MCStringRef& r_string);
void MCDateTimeEvalWeekDayNames(MCExecContext& ctxt, MCStringRef& r_string);

void MCDateTimeEvalIsADate(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCDateTimeEvalIsNotADate(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);

void MCDateTimeExecConvert(MCExecContext &ctxt, MCStringRef p_input, int p_from_first, int p_from_second, int p_to_first, int p_to_second, MCStringRef &r_output);
void MCDateTimeExecConvertIntoIt(MCExecContext &ctxt, MCStringRef p_input, int p_from_first, int p_from_second, int p_to_first, int p_to_second, MCStringRef &r_output);

void MCDateTimeGetTwelveTime(MCExecContext &ctxt, bool& r_value);
void MCDateTimeSetTwelveTime(MCExecContext &ctxt, bool p_value);

void MCDateTimeGetDate(MCExecContext &ctxt, Properties p_type, MCStringRef& r_value);
void MCDateTimeGetTime(MCExecContext &ctxt, Properties p_type, MCStringRef& r_value);
void MCDateTimeGetMilliseconds(MCExecContext &ctxt, double& r_value);
void MCDateTimeGetLongMilliseconds(MCExecContext &ctxt, double& r_value);
void MCDateTimeGetSeconds(MCExecContext &ctxt, double& r_value);
void MCDateTimeGetLongSeconds(MCExecContext &ctxt, double& r_value);
void MCDateTimeGetTicks(MCExecContext &ctxt, double& r_value);
void MCDateTimeGetLongTicks(MCExecContext &ctxt, double& r_value);

void MCDateTimeGetMonthNames(MCExecContext &ctxt, Properties p_type, MCStringRef& r_value);
void MCDateTimeGetWeekDayNames(MCExecContext &ctxt, Properties p_type, MCStringRef& r_value);
void MCDateTimeGetDateFormat(MCExecContext &ctxt, Properties p_type, MCStringRef& r_value);

///////////

extern MCExecMethodInfo *kMCScriptingEvalAlternateLanguagesMethodInfo;
extern MCExecMethodInfo *kMCScriptingExecDoAsAlternateLanguageMethodInfo;
extern MCExecMethodInfo *kMCScriptingExecSendToProgramMethodInfo;
extern MCExecMethodInfo *kMCScriptingExecReplyErrorMethodInfo;
extern MCExecMethodInfo *kMCScriptingExecReplyMethodInfo;
extern MCExecMethodInfo *kMCScriptingExecRequestAppleEventMethodInfo;
extern MCExecMethodInfo *kMCScriptingExecRequestFromProgramMethodInfo;

void MCScriptingEvalAlternateLanguages(MCExecContext& ctxt, MCStringRef& r_list);
void MCScriptingExecDoAsAlternateLanguage(MCExecContext& ctxt, MCStringRef p_script, MCStringRef p_language);

void MCScriptingExecSendToProgram(MCExecContext& ctxt, MCStringRef message, MCStringRef program, MCStringRef event_type, bool wait_for_reply);
void MCScriptingExecReplyError(MCExecContext& ctxt, MCStringRef message);
void MCScriptingExecReply(MCExecContext& ctxt, MCStringRef message, MCStringRef keyword);
void MCScriptingExecRequestAppleEvent(MCExecContext& ctxt, int type, MCStringRef program);
void MCScriptingExecRequestFromProgram(MCExecContext& ctxt, MCStringRef message, MCStringRef program);

///////////

extern MCExecMethodInfo *kMCSecurityEvalEncryptMethodInfo;
extern MCExecMethodInfo *kMCSecurityEvalCipherNamesMethodInfo;
extern MCExecMethodInfo *kMCSecurityEvalRandomBytesMethodInfo;
extern MCExecMethodInfo *kMCSecurityExecRsaEncryptMethodInfo;
extern MCExecMethodInfo *kMCSecurityExecRsaDecryptMethodInfo;
extern MCExecMethodInfo *kMCSecurityExecBlockEncryptWithPasswordMethodInfo;
extern MCExecMethodInfo *kMCSecurityExecBlockEncryptWithKeyMethodInfo;
extern MCExecMethodInfo *kMCSecurityExecBlockDecryptWithPasswordMethodInfo;
extern MCExecMethodInfo *kMCSecurityExecBlockDecryptWithKeyMethodInfo;
extern MCExecMethodInfo *kMCSecurityGetSslCertificatesMethodInfo;
extern MCExecMethodInfo *kMCSecuritySetSslCertificatesMethodInfo;

void MCSecurityEvalEncrypt(MCExecContext& ctxt, MCStringRef p_source, MCStringRef& r_dest);
void MCSecurityEvalCipherNames(MCExecContext& ctxt, MCStringRef& r_names);
void MCSecurityEvalRandomBytes(MCExecContext& ctxt, uinteger_t p_byte_count, MCDataRef& r_bytes);

void MCSecurityExecRsaEncrypt(MCExecContext& ctxt, MCStringRef p_data, bool p_is_public, MCStringRef p_key, MCStringRef p_passphrase);
void MCSecurityExecRsaDecrypt(MCExecContext& ctxt, MCStringRef p_data, bool p_is_public, MCStringRef p_key, MCStringRef p_passphrase);
void MCSecurityExecBlockEncryptWithPassword(MCExecContext& ctxt, MCStringRef p_data, MCNameRef p_cipher, MCStringRef p_password, MCStringRef p_salt, MCStringRef p_iv, uint2 p_bit_rate);
void MCSecurityExecBlockEncryptWithKey(MCExecContext& ctxt, MCStringRef p_data, MCNameRef p_cipher, MCStringRef p_key, MCStringRef p_iv, uint2 p_bit_rate);
void MCSecurityExecBlockDecryptWithPassword(MCExecContext& ctxt, MCStringRef p_data, MCNameRef p_cipher, MCStringRef p_password, MCStringRef p_salt, MCStringRef p_iv, uint2 p_bit_rate);
void MCSecurityExecBlockDecryptWithKey(MCExecContext& ctxt, MCStringRef p_data, MCNameRef p_cipher, MCStringRef p_key, MCStringRef p_iv, uint2 p_bit_rate);

void MCSecurityGetSslCertificates(MCExecContext& ctxt, MCStringRef& r_value);
void MCSecuritySetSslCertificates(MCExecContext& ctxt, MCStringRef p_value);

///////////

extern MCExecMethodInfo *kMCGraphicsEvalIsAColorMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsNotAColorMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsAPointMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsNotAPointMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsARectangleMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsNotARectangleMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsWithinMethodInfo;
extern MCExecMethodInfo *kMCGraphicsEvalIsNotWithinMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecFlipSelectionMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecFlipImageMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecResetPaintMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecCropImageMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecRotateSelectionMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecRotateImageMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecPrepareImageMethodInfo;
extern MCExecMethodInfo *kMCGraphicsExecPrepareImageFileMethodInfo;
extern MCExecMethodInfo *kMCGraphicsGetImageCacheLimitMethodInfo;
extern MCExecMethodInfo *kMCGraphicsSetImageCacheLimitMethodInfo;
extern MCExecMethodInfo *kMCGraphicsGetImageCacheUsageMethodInfo;

void MCGraphicsEvalIsAColor(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCGraphicsEvalIsNotAColor(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCGraphicsEvalIsAPoint(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCGraphicsEvalIsNotAPoint(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCGraphicsEvalIsARectangle(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);
void MCGraphicsEvalIsNotARectangle(MCExecContext& ctxt, MCValueRef p_value, bool& r_result);

void MCGraphicsEvalIsWithin(MCExecContext& ctxt, MCPoint point, MCRectangle rectangle, bool& r_result);
void MCGraphicsEvalIsNotWithin(MCExecContext& ctxt, MCPoint point, MCRectangle rectangle, bool& r_result);

void MCGraphicsExecFlipSelection(MCExecContext& ctxt, bool horizontal);
void MCGraphicsExecFlipImage(MCExecContext& ctxt, MCImage *image, bool horizontal);

void MCGraphicsExecResetPaint(MCExecContext& ctxt);

void MCGraphicsExecCropImage(MCExecContext& ctxt, MCImage *image, MCRectangle bounds);

void MCGraphicsExecRotateSelection(MCExecContext& ctxt, integer_t angle);
void MCGraphicsExecRotateImage(MCExecContext& ctxt, MCImage *image, integer_t angle);

void MCGraphicsExecPrepareImage(MCExecContext& ctxt, MCImage *image);
void MCGraphicsExecPrepareImageFile(MCExecContext& ctxt, MCStringRef filename);

void MCGraphicsGetImageCacheLimit(MCExecContext &ctxt, uinteger_t &r_value);
void MCGraphicsSetImageCacheLimit(MCExecContext &ctxt, uinteger_t p_value);
void MCGraphicsGetImageCacheUsage(MCExecContext &ctxt, uinteger_t &r_value);

///////////

extern MCExecMethodInfo *kMCLegacyEvalHasMemoryMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalHeapSpaceMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalStackSpaceMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalIsNumberMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalLicensedMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalMenusMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalScreenTypeMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalScreenVendorMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalSelectedButtonMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalSelectedButtonOfMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalTextHeightSumMethodInfo;
extern MCExecMethodInfo *kMCLegacyEvalMenuObjectMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecDoInBrowserMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecCompactStackMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecDoMenuMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecLockColormapMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecUnlockColormapMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecImportEpsMethodInfo;
extern MCExecMethodInfo *kMCLegacyExecImportHypercardStackMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetRevRuntimeBehaviourMethodInfo;
extern MCExecMethodInfo *kMCLegacySetRevRuntimeBehaviourMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetHcImportStatMethodInfo;
extern MCExecMethodInfo *kMCLegacySetHcImportStatMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetScriptTextFontMethodInfo;
extern MCExecMethodInfo *kMCLegacySetScriptTextFontMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetScriptTextSizeMethodInfo;
extern MCExecMethodInfo *kMCLegacySetScriptTextSizeMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetStackFilesMethodInfo;
extern MCExecMethodInfo *kMCLegacySetStackFilesMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetMenuBarMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetEditMenusMethodInfo;
extern MCExecMethodInfo *kMCLegacySetEditMenusMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetTextAlignMethodInfo;
extern MCExecMethodInfo *kMCLegacySetTextAlignMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetTextFontMethodInfo;
extern MCExecMethodInfo *kMCLegacySetTextFontMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetTextHeightMethodInfo;
extern MCExecMethodInfo *kMCLegacySetTextHeightMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetTextSizeMethodInfo;
extern MCExecMethodInfo *kMCLegacySetTextSizeMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetTextStyleMethodInfo;
extern MCExecMethodInfo *kMCLegacySetTextStyleMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetBufferModeMethodInfo;
extern MCExecMethodInfo *kMCLegacySetBufferModeMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetMultiEffectMethodInfo;
extern MCExecMethodInfo *kMCLegacySetMultiEffectMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPrintTextAlignMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPrintTextAlignMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPrintTextFontMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPrintTextFontMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPrintTextHeightMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPrintTextHeightMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPrintTextSizeMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPrintTextSizeMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPrintTextStyleMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPrintTextStyleMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetEditScriptsMethodInfo;
extern MCExecMethodInfo *kMCLegacySetEditScriptsMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetColorWorldMethodInfo;
extern MCExecMethodInfo *kMCLegacySetColorWorldMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetAllowKeyInFieldMethodInfo;
extern MCExecMethodInfo *kMCLegacySetAllowKeyInFieldMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetAllowFieldRedrawMethodInfo;
extern MCExecMethodInfo *kMCLegacySetAllowFieldRedrawMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetRemapColorMethodInfo;
extern MCExecMethodInfo *kMCLegacySetRemapColorMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetUserLevelMethodInfo;
extern MCExecMethodInfo *kMCLegacySetUserLevelMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetUserModifyMethodInfo;
extern MCExecMethodInfo *kMCLegacySetUserModifyMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetLockColormapMethodInfo;
extern MCExecMethodInfo *kMCLegacySetLockColormapMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPrivateColorsMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPrivateColorsMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetLongWindowTitlesMethodInfo;
extern MCExecMethodInfo *kMCLegacySetLongWindowTitlesMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetBlindTypingMethodInfo;
extern MCExecMethodInfo *kMCLegacySetBlindTypingMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetPowerKeysMethodInfo;
extern MCExecMethodInfo *kMCLegacySetPowerKeysMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetTextArrowsMethodInfo;
extern MCExecMethodInfo *kMCLegacySetTextArrowsMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetColormapMethodInfo;
extern MCExecMethodInfo *kMCLegacySetColormapMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetNoPixmapsMethodInfo;
extern MCExecMethodInfo *kMCLegacySetNoPixmapsMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetLowResolutionTimersMethodInfo;
extern MCExecMethodInfo *kMCLegacySetLowResolutionTimersMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetVcSharedMemoryMethodInfo;
extern MCExecMethodInfo *kMCLegacySetVcSharedMemoryMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetVcPlayerMethodInfo;
extern MCExecMethodInfo *kMCLegacySetVcPlayerMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetSoundChannelMethodInfo;
extern MCExecMethodInfo *kMCLegacySetSoundChannelMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetLzwKeyMethodInfo;
extern MCExecMethodInfo *kMCLegacySetLzwKeyMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetMultipleMethodInfo;
extern MCExecMethodInfo *kMCLegacySetMultipleMethodInfo;
extern MCExecMethodInfo *kMCLegacyGetMultiSpaceMethodInfo;
extern MCExecMethodInfo *kMCLegacySetMultiSpaceMethodInfo;

void MCLegacyEvalHasMemory(MCExecContext& ctxt, uinteger_t p_bytes, bool& r_bool);
void MCLegacyEvalHeapSpace(MCExecContext& ctxt, integer_t& r_bytes);
void MCLegacyEvalStackSpace(MCExecContext& ctxt, integer_t& r_bytes);
void MCLegacyEvalIsNumber(MCExecContext& ctxt, MCStringRef p_string, bool& r_bool);
void MCLegacyEvalLicensed(MCExecContext& ctxt, bool& r_licensed);
void MCLegacyEvalMenus(MCExecContext& ctxt, MCStringRef& r_string);
void MCLegacyEvalScreenType(MCExecContext& ctxt, MCNameRef& r_name);
void MCLegacyEvalScreenVendor(MCExecContext& ctxt, MCNameRef& r_name);
void MCLegacyEvalSelectedButtonOf(MCExecContext& ctxt, bool p_background, integer_t p_family, MCObjectPtr p_object, MCStringRef& r_string);
void MCLegacyEvalSelectedButton(MCExecContext& ctxt, bool p_background, integer_t p_family, MCStringRef& r_string);
void MCLegacyEvalTextHeightSum(MCExecContext& ctxt, MCObjectPtr p_object, integer_t& r_sum);
void MCLegacyEvalMenuObject(MCExecContext& ctxt, MCStringRef& r_object);

void MCLegacyExecDoInBrowser(MCExecContext& ctxt, MCStringRef p_script);
void MCLegacyExecCompactStack(MCExecContext& ctxt, MCStack *stack);

void MCLegacyExecDoMenu(MCExecContext& ctxt, MCStringRef p_option);

void MCLegacyExecLockColormap(MCExecContext& ctxt);
void MCLegacyExecUnlockColormap(MCExecContext& ctxt);

void MCLegacyExecImportEps(MCExecContext& ctxt, MCStringRef p_filename);
void MCLegacyExecImportHypercardStack(MCExecContext& ctxt, MCStringRef p_filename);

void MCLegacyGetRevRuntimeBehaviour(MCExecContext& ctxt, uinteger_t &r_value);
void MCLegacySetRevRuntimeBehaviour(MCExecContext& ctxt, uint4 p_value);
void MCLegacyGetHcImportStat(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetHcImportStat(MCExecContext& ctxt, MCStringRef p_value);
void MCLegacyGetScriptTextFont(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetScriptTextFont(MCExecContext& ctxt, MCStringRef p_value);
void MCLegacyGetScriptTextSize(MCExecContext& ctxt, uinteger_t &r_value);
void MCLegacySetScriptTextSize(MCExecContext& ctxt, uint2 p_value);

void MCLegacyGetStackFiles(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetStackFiles(MCExecContext& ctxt, MCStringRef value);

void MCLegacyGetMenuBar(MCExecContext& ctxt, MCStringRef& r_value);

void MCLegacyGetEditMenus(MCExecContext& ctxt, bool& r_value);
void MCLegacySetEditMenus(MCExecContext& ctxt, bool value);

void MCLegacyGetTextAlign(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetTextAlign(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetTextFont(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetTextFont(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetTextHeight(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetTextHeight(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetTextSize(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetTextSize(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetTextStyle(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetTextStyle(MCExecContext& ctxt, MCValueRef value);

void MCLegacyGetBufferMode(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetBufferMode(MCExecContext& ctxt, MCValueRef p_value);
void MCLegacyGetMultiEffect(MCExecContext& ctxt, bool& r_value);
void MCLegacySetMultiEffect(MCExecContext& ctxt, MCValueRef p_value);

void MCLegacyGetPrintTextAlign(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetPrintTextAlign(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetPrintTextFont(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetPrintTextFont(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetPrintTextHeight(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetPrintTextHeight(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetPrintTextSize(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetPrintTextSize(MCExecContext& ctxt, MCValueRef value);
void MCLegacyGetPrintTextStyle(MCExecContext& ctxt, MCValueRef& r_value);
void MCLegacySetPrintTextStyle(MCExecContext& ctxt, MCValueRef value);

void MCLegacyGetEditScripts(MCExecContext& ctxt, bool& r_value);
void MCLegacySetEditScripts(MCExecContext& ctxt, bool p_value);
void MCLegacyGetColorWorld(MCExecContext& ctxt, bool& r_value);
void MCLegacySetColorWorld(MCExecContext& ctxt, bool p_value);
void MCLegacyGetAllowKeyInField(MCExecContext& ctxt, bool& r_value);
void MCLegacySetAllowKeyInField(MCExecContext& ctxt, bool p_value);
void MCLegacyGetAllowFieldRedraw(MCExecContext& ctxt, bool& r_value);
void MCLegacySetAllowFieldRedraw(MCExecContext& ctxt, bool p_value);
void MCLegacyGetRemapColor(MCExecContext& ctxt, bool& r_value);
void MCLegacySetRemapColor(MCExecContext& ctxt, bool p_value);

void MCLegacyGetUserLevel(MCExecContext& ctxt, uinteger_t& r_value);
void MCLegacySetUserLevel(MCExecContext& ctxt, uinteger_t p_value);
void MCLegacyGetUserModify(MCExecContext& ctxt, bool& r_value);
void MCLegacySetUserModify(MCExecContext& ctxt, bool p_value);

void MCLegacyGetLockColormap(MCExecContext& ctxt, bool& r_value);
void MCLegacySetLockColormap(MCExecContext& ctxt, bool p_value);
void MCLegacyGetPrivateColors(MCExecContext& ctxt, bool& r_value);
void MCLegacySetPrivateColors(MCExecContext& ctxt, bool p_value);

void MCLegacyGetLongWindowTitles(MCExecContext& ctxt, bool& r_value);
void MCLegacySetLongWindowTitles(MCExecContext& ctxt, bool p_value);
void MCLegacyGetBlindTyping(MCExecContext& ctxt, bool& r_value);
void MCLegacySetBlindTyping(MCExecContext& ctxt, bool p_value);
void MCLegacyGetPowerKeys(MCExecContext& ctxt, bool& r_value);
void MCLegacySetPowerKeys(MCExecContext& ctxt, bool p_value);
void MCLegacyGetTextArrows(MCExecContext& ctxt, bool& r_value);
void MCLegacySetTextArrows(MCExecContext& ctxt, bool p_value);
void MCLegacyGetColormap(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetColormap(MCExecContext& ctxt, MCStringRef p_value);
void MCLegacyGetNoPixmaps(MCExecContext& ctxt, bool& r_value);
void MCLegacySetNoPixmaps(MCExecContext& ctxt, bool p_value);
void MCLegacyGetLowResolutionTimers(MCExecContext& ctxt, bool& r_value);
void MCLegacySetLowResolutionTimers(MCExecContext& ctxt, bool p_value);

void MCLegacyGetVcSharedMemory(MCExecContext& ctxt, bool& r_value);
void MCLegacySetVcSharedMemory(MCExecContext& ctxt, bool p_value);
void MCLegacyGetVcPlayer(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetVcPlayer(MCExecContext& ctxt, MCStringRef p_value);
void MCLegacyGetSoundChannel(MCExecContext& ctxt, uinteger_t& r_value);
void MCLegacySetSoundChannel(MCExecContext& ctxt, uinteger_t p_value);
void MCLegacyGetLzwKey(MCExecContext& ctxt, MCStringRef& r_value);
void MCLegacySetLzwKey(MCExecContext& ctxt, MCStringRef p_value);

void MCLegacyGetMultiple(MCExecContext& ctxt, bool& r_value);
void MCLegacySetMultiple(MCExecContext& ctxt, bool p_value);
void MCLegacyGetMultiSpace(MCExecContext& ctxt, uinteger_t& r_value);
void MCLegacySetMultiSpace(MCExecContext& ctxt, uinteger_t p_value);

///////////

extern MCExecMethodInfo *kMCIdeExecPutIntoMessageMethodInfo;
extern MCExecMethodInfo *kMCIdeExecEditScriptOfObjectMethodInfo;
extern MCExecMethodInfo *kMCIdeExecHideMessageBoxMethodInfo;
extern MCExecMethodInfo *kMCIdeExecShowMessageBoxMethodInfo;

void MCIdeExecPutIntoMessage(MCExecContext& ctxt, MCStringRef value, int where);

void MCIdeExecEditScriptOfObject(MCExecContext& ctxt, MCObject *p_object);
void MCIdeExecHideMessageBox(MCExecContext& ctxt);
void MCIdeExecShowMessageBox(MCExecContext& ctxt);

///////////

struct MCPrintingPrintDeviceOutput;
struct MCPrintingPrinterPageRange;

extern MCExecSetTypeInfo *kMCPrintingPrinterFeaturesTypeInfo;
extern MCExecEnumTypeInfo *kMCPrintingPrinterOrientationTypeInfo;
extern MCExecCustomTypeInfo *kMCPrintingPrintDeviceOutputTypeInfo;
extern MCExecCustomTypeInfo *kMCPrintingPrinterPageRangeTypeInfo;
extern MCExecEnumTypeInfo *kMCPrintingPrinterLinkTypeInfo;
extern MCExecEnumTypeInfo *kMCPrintingPrinterBookmarkInitialStateTypeInfo;
extern MCExecEnumTypeInfo *kMCPrintingPrintJobDuplexTypeInfo;

extern MCExecMethodInfo *kMCPrintingExecAnswerPageSetupMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecAnswerPrinterMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecCancelPrintingMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecResetPrintingMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintAnchorMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintLinkMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintNativeBookmarkMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintUnicodeBookmarkMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintBreakMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintAllCardsMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintRectOfAllCardsMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintCardMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintRectOfCardMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintSomeCardsMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintRectOfSomeCardsMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintCardIntoRectMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecPrintRectOfCardIntoRectMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecClosePrintingMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecOpenPrintingToDestinationMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecOpenPrintingMethodInfo;
extern MCExecMethodInfo *kMCPrintingExecOpenPrintingWithDialogMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrinterNamesMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintDeviceFeaturesMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintDeviceOutputMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintDeviceOutputMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintDeviceRectangleMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintDeviceRectangleMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintDeviceSettingsMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintDeviceSettingsMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintDeviceNameMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintDeviceNameMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintPageOrientationMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintPageOrientationMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintJobRangesMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobRangesMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintPageSizeMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintPageSizeMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintPageScaleMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintPageScaleMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintPageRectangleMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobNameMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintJobNameMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobCopiesMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintJobCopiesMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobDuplexMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintJobDuplexMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobCollateMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintJobCollateMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobColorMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintJobColorMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintJobPageMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintCardBordersMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintCardBordersMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintGuttersMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintGuttersMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintMarginsMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintMarginsMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintRowsFirstMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintRowsFirstMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintScaleMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintScaleMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintRotatedMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintRotatedMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintCommandMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintCommandMethodInfo;
extern MCExecMethodInfo *kMCPrintingGetPrintFontTableMethodInfo;
extern MCExecMethodInfo *kMCPrintingSetPrintFontTableMethodInfo;

void MCPrintingExecAnswerPageSetup(MCExecContext &ctxt, bool p_is_sheet);
void MCPrintingExecAnswerPrinter(MCExecContext &ctxt, bool p_is_sheet);

void MCPrintingExecCancelPrinting(MCExecContext& ctxt);
void MCPrintingExecResetPrinting(MCExecContext& ctxt);
void MCPrintingExecPrintAnchor(MCExecContext& ctxt, MCStringRef name, MCPoint location);
void MCPrintingExecPrintLink(MCExecContext& ctxt, int type, MCStringRef target, MCRectangle area);
void MCPrintingExecPrintBookmark(MCExecContext& ctxt, MCStringRef title, MCPoint location, integer_t level, bool initially_closed);
void MCPrintingExecPrintUnicodeBookmark(MCExecContext& ctxt, MCDataRef title, MCPoint location, integer_t level, bool initially_closed);
void MCPrintingExecPrintBreak(MCExecContext& ctxt);
void MCPrintingExecPrintAllCards(MCExecContext& ctxt, MCStack *stack, bool only_marked);
void MCPrintingExecPrintRectOfAllCards(MCExecContext& ctxt, MCStack *stack, bool p_only_marked, MCPoint from, MCPoint to);
void MCPrintingExecPrintCard(MCExecContext& ctxt, MCCard *target);
void MCPrintingExecPrintRectOfCard(MCExecContext& ctxt, MCCard *target, MCPoint from, MCPoint to);
void MCPrintingExecPrintSomeCards(MCExecContext& ctxt, integer_t count);
void MCPrintingExecPrintRectOfSomeCards(MCExecContext& ctxt, integer_t count, MCPoint from, MCPoint to);
void MCPrintingExecPrintCardIntoRect(MCExecContext& ctxt, MCCard *card, MCRectangle destination);
void MCPrintingExecPrintRectOfCardIntoRect(MCExecContext& ctxt, MCCard *card, MCPoint from, MCPoint to, MCRectangle destination);

void MCPrintingExecClosePrinting(MCExecContext& ctxt);

void MCPrintingExecOpenPrintingToDestination(MCExecContext& ctxt, MCStringRef p_destination, MCStringRef p_filename, MCArrayRef p_options);
void MCPrintingExecOpenPrinting(MCExecContext& ctxt);
void MCPrintingExecOpenPrintingWithDialog(MCExecContext& ctxt, bool p_as_sheet);

void MCPrintingGetPrinterNames(MCExecContext& ctxt, MCStringRef& r_printers);

void MCPrintingGetPrintDeviceFeatures(MCExecContext& ctxt, unsigned int& r_features);
void MCPrintingSetPrintDeviceOutput(MCExecContext& ctxt, const MCPrintingPrintDeviceOutput& output);
void MCPrintingGetPrintDeviceOutput(MCExecContext& ctxt, MCPrintingPrintDeviceOutput& r_output);
void MCPrintingGetPrintDeviceRectangle(MCExecContext& ctxt, MCRectangle &r_rectangle);
void MCPrintingGetPrintDeviceRectangle(MCExecContext& ctxt, MCRectangle &r_rectangle);
void MCPrintingGetPrintDeviceSettings(MCExecContext& ctxt, MCDataRef &r_settings);
void MCPrintingSetPrintDeviceSettings(MCExecContext& ctxt, MCDataRef p_settings);
void MCPrintingGetPrintDeviceName(MCExecContext& ctxt, MCStringRef &r_name);
void MCPrintingSetPrintDeviceName(MCExecContext& ctxt, MCStringRef p_name);

void MCPrintingGetPrintPageOrientation(MCExecContext& ctxt, int& r_orientation);
void MCPrintingSetPrintPageOrientation(MCExecContext& ctxt, int orientation);

void MCPrintingSetPrintJobRanges(MCExecContext& ctxt, const MCPrintingPrinterPageRange& p_ranges);
void MCPrintingGetPrintJobRanges(MCExecContext& ctxt, MCPrintingPrinterPageRange& r_ranges);

void MCPrintingSetPrintPageSize(MCExecContext& ctxt, integer_t p_value[2]);
void MCPrintingGetPrintPageSize(MCExecContext& ctxt, integer_t r_value[2]);
void MCPrintingSetPrintPageScale(MCExecContext& ctxt, double p_value);
void MCPrintingGetPrintPageScale(MCExecContext& ctxt, double &r_value);
void MCPrintingGetPrintPageRectangle(MCExecContext& ctxt, MCRectangle &r_value);

void MCPrintingGetPrintJobName(MCExecContext& ctxt, MCStringRef &r_value);
void MCPrintingSetPrintJobName(MCExecContext& ctxt, MCStringRef p_value);
void MCPrintingGetPrintJobCopies(MCExecContext& ctxt, integer_t &r_value);
void MCPrintingSetPrintJobCopies(MCExecContext& ctxt, integer_t p_value);
void MCPrintingGetPrintJobDuplex(MCExecContext& ctxt, intenum_t &r_value);
void MCPrintingSetPrintJobDuplex(MCExecContext& ctxt, intenum_t p_value);
void MCPrintingGetPrintJobCollate(MCExecContext& ctxt, bool &r_value);
void MCPrintingSetPrintJobCollate(MCExecContext& ctxt, bool p_value);
void MCPrintingGetPrintJobColor(MCExecContext& ctxt, bool &r_value);
void MCPrintingSetPrintJobColor(MCExecContext& ctxt, bool p_value);
void MCPrintingGetPrintJobPage(MCExecContext& ctxt, integer_t &r_value);

void MCPrintingGetPrintCardBorders(MCExecContext& ctxt, bool &r_card_borders);
void MCPrintingSetPrintCardBorders(MCExecContext& ctxt, bool p_card_borders);
void MCPrintingGetPrintGutters(MCExecContext& ctxt, integer_t r_gutters[2]);
void MCPrintingSetPrintGutters(MCExecContext& ctxt, integer_t p_gutters[2]);
void MCPrintingGetPrintMargins(MCExecContext& ctxt, integer_t r_margins[4]);
void MCPrintingSetPrintMargins(MCExecContext& ctxt, integer_t p_margins[4]);
void MCPrintingGetPrintRowsFirst(MCExecContext& ctxt, bool &r_rows_first);
void MCPrintingSetPrintRowsFirst(MCExecContext& ctxt, bool p_rows_first);
void MCPrintingGetPrintScale(MCExecContext& ctxt, double &r_scale);
void MCPrintingSetPrintScale(MCExecContext& ctxt, double p_scale);
void MCPrintingGetPrintRotated(MCExecContext& ctxt, bool &r_rotated);
void MCPrintingSetPrintRotated(MCExecContext& ctxt, bool p_rotated);
void MCPrintingGetPrintCommand(MCExecContext& ctxt, MCStringRef &r_command);
void MCPrintingSetPrintCommand(MCExecContext& ctxt, MCStringRef p_command);
void MCPrintingGetPrintFontTable(MCExecContext& ctxt, MCStringRef &r_table);
void MCPrintingSetPrintFontTable(MCExecContext& ctxt, MCStringRef p_table);

///////////

extern MCExecEnumTypeInfo *kMCServerErrorModeTypeInfo;
extern MCExecEnumTypeInfo *kMCServerOutputLineEndingsTypeInfo;
extern MCExecEnumTypeInfo *kMCServerOutputTextEncodingTypeInfo;

extern MCExecMethodInfo *kMCServerExecPutHeaderMethodInfo;
extern MCExecMethodInfo *kMCServerExecPutBinaryOutputMethodInfo;
extern MCExecMethodInfo *kMCServerExecPutContentMethodInfo;
extern MCExecMethodInfo *kMCServerExecPutMarkupMethodInfo;
extern MCExecMethodInfo *kMCServerExecPutCookieMethodInfo;
extern MCExecMethodInfo *kMCServerExecDeleteSessionMethodInfo;
extern MCExecMethodInfo *kMCServerExecStartSessionMethodInfo;
extern MCExecMethodInfo *kMCServerExecStopSessionMethodInfo;
extern MCExecMethodInfo *kMCServerExecIncludeMethodInfo;
extern MCExecMethodInfo *kMCServerExecEchoMethodInfo;
extern MCExecMethodInfo *kMCServerGetErrorModeMethodInfo;
extern MCExecMethodInfo *kMCServerSetErrorModeMethodInfo;
extern MCExecMethodInfo *kMCServerGetOutputLineEndingMethodInfo;
extern MCExecMethodInfo *kMCServerSetOutputLineEndingMethodInfo;
extern MCExecMethodInfo *kMCServerGetOutputTextEncodingMethodInfo;
extern MCExecMethodInfo *kMCServerSetOutputTextEncodingMethodInfo;
extern MCExecMethodInfo *kMCServerGetSessionSavePathMethodInfo;
extern MCExecMethodInfo *kMCServerSetSessionSavePathMethodInfo;
extern MCExecMethodInfo *kMCServerGetSessionLifetimeMethodInfo;
extern MCExecMethodInfo *kMCServerSetSessionLifetimeMethodInfo;
extern MCExecMethodInfo *kMCServerGetSessionCookieNameMethodInfo;
extern MCExecMethodInfo *kMCServerSetSessionCookieNameMethodInfo;
extern MCExecMethodInfo *kMCServerGetSessionIdMethodInfo;
extern MCExecMethodInfo *kMCServerSetSessionIdMethodInfo;

void MCServerExecPutHeader(MCExecContext& ctxt, MCStringRef value, bool as_new);
void MCServerExecPutBinaryOutput(MCExecContext& ctxt, MCStringRef value);
void MCServerExecPutContent(MCExecContext& ctxt, MCStringRef value, bool is_unicode);
void MCServerExecPutMarkup(MCExecContext& ctxt, MCStringRef value, bool is_unicode);
void MCServerExecPutCookie(MCExecContext& ctxt, MCStringRef name, MCStringRef value, uinteger_t expires, MCStringRef path, MCStringRef domain, bool is_secure, bool http_only);

void MCServerExecDeleteSession(MCExecContext& ctxt);
void MCServerExecStartSession(MCExecContext& ctxt);
void MCServerExecStopSession(MCExecContext& ctxt);

void MCServerExecInclude(MCExecContext& ctxt, MCStringRef filename, bool is_require);
void MCServerExecEcho(MCExecContext& ctxt, MCStringRef data);

void MCServerGetErrorMode(MCExecContext& ctxt, intenum_t& r_value);
void MCServerSetErrorMode(MCExecContext& ctxt, intenum_t p_value);
void MCServerGetOutputLineEnding(MCExecContext& ctxt, intenum_t& r_value);
void MCServerSetOutputLineEnding(MCExecContext& ctxt, intenum_t p_value);
void MCServerGetOutputTextEncoding(MCExecContext& ctxt, intenum_t& r_value);
void MCServerSetOutputTextEncoding(MCExecContext& ctxt, intenum_t p_value);
void MCServerGetSessionSavePath(MCExecContext& ctxt, MCStringRef &r_value);
void MCServerSetSessionSavePath(MCExecContext& ctxt, MCStringRef p_value);
void MCServerGetSessionLifetime(MCExecContext& ctxt, uinteger_t& r_value);
void MCServerSetSessionLifetime(MCExecContext& ctxt, uinteger_t p_value);
void MCServerGetSessionCookieName(MCExecContext& ctxt, MCStringRef &r_value);
void MCServerSetSessionCookieName(MCExecContext& ctxt, MCStringRef p_value);
void MCServerGetSessionId(MCExecContext& ctxt, MCStringRef &r_value);
void MCServerSetSessionId(MCExecContext& ctxt, MCStringRef p_value);

///////////

extern MCExecMethodInfo *kMCDebuggingExecBreakpointMethodInfo;
extern MCExecMethodInfo *kMCDebuggingExecDebugDoMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetTraceAbortMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetTraceAbortMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetTraceDelayMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetTraceDelayMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetTraceReturnMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetTraceReturnMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetTraceStackMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetTraceStackMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetTraceUntilMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetTraceUntilMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetMessageMessagesMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetMessageMessagesMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetBreakpointsMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetBreakpointsMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetDebugContextMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetDebugContextMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetExecutionContextsMethodInfo;
extern MCExecMethodInfo *kMCDebuggingGetWatchedVariablesMethodInfo;
extern MCExecMethodInfo *kMCDebuggingSetWatchedVariablesMethodInfo;

void MCDebuggingExecBreakpoint(MCExecContext& ctxt, uinteger_t p_line, uinteger_t p_pos);
void MCDebuggingExecDebugDo(MCExecContext& ctxt, MCStringRef p_script, uinteger_t p_line, uinteger_t p_pos);

void MCDebuggingGetTraceAbort(MCExecContext& ctxtm, bool& r_value);
void MCDebuggingSetTraceAbort(MCExecContext& ctxtm, bool p_value);
void MCDebuggingGetTraceDelay(MCExecContext& ctxt, uinteger_t& r_value);
void MCDebuggingSetTraceDelay(MCExecContext& ctxt, uinteger_t p_value);
void MCDebuggingGetTraceReturn(MCExecContext& ctxtm, bool& r_value);
void MCDebuggingSetTraceReturn(MCExecContext& ctxtm, bool p_value);
void MCDebuggingGetTraceStack(MCExecContext& ctxt, MCStringRef& r_value);
void MCDebuggingSetTraceStack(MCExecContext& ctxt, MCStringRef p_value);
void MCDebuggingGetTraceUntil(MCExecContext& ctxt, uinteger_t& r_value);
void MCDebuggingSetTraceUntil(MCExecContext& ctxt, uinteger_t p_value);
void MCDebuggingGetMessageMessages(MCExecContext& ctxtm, bool& r_value);
void MCDebuggingSetMessageMessages(MCExecContext& ctxtm, bool p_value);

void MCDebuggingGetBreakpoints(MCExecContext& ctxt, MCStringRef& r_value);
void MCDebuggingSetBreakpoints(MCExecContext& ctxt, MCStringRef p_value);
void MCDebuggingGetDebugContext(MCExecContext& ctxt, MCStringRef& r_value);
void MCDebuggingSetDebugContext(MCExecContext& ctxt, MCStringRef p_value);
void MCDebuggingGetExecutionContexts(MCExecContext& ctxt, MCStringRef& r_value);
void MCDebuggingGetWatchedVariables(MCExecContext& ctxt, MCStringRef& r_value);
void MCDebuggingSetWatchedVariables(MCExecContext& ctxt, MCStringRef p_value);

///////////

extern MCExecMethodInfo *kMCTextMessagingExecComposeTextMessageMethodInfo;
extern MCExecMethodInfo *kMCTextMessagingGetCanComposeTextMessageMethodInfo;

void MCTextMessagingGetCanComposeTextMessage(MCExecContext& ctxt, bool& r_result);
void MCTextMessagingExecComposeTextMessage(MCExecContext& ctxt, MCStringRef p_recipients, MCStringRef p_body);

///////////

extern MCExecMethodInfo *kMCIdleTimerExecLockIdleTimerMethodInfo;
extern MCExecMethodInfo *kMCIdleTimerExecUnlockIdleTimerMethodIndo;
extern MCExecMethodInfo *kMCIdleTimerGetidleTimerLockedMethodInfo;

void MCIdleTimerExecLockIdleTimer(MCExecContext& ctxt);
void MCIdleTimerExecUnlockIdleTimer(MCExecContext& ctxt);
void MCIdleTimerGetIdleTimerLocked(MCExecContext& ctxt, bool& r_result);

//////////

extern MCExecMethodInfo* kMCStoreGetCanMakePurchaseMethodInfo;
extern MCExecMethodInfo* kMCStoreExecEnablePurchaseUpdatesMethodInfo;
extern MCExecMethodInfo* kMCStoreExecDisablePurchaseUpdatesMethodInfo;
extern MCExecMethodInfo* kMCStoreExecRestorePurchaseMethodInfo;
extern MCExecMethodInfo* kMCStoreGetPurchaseListMethodInfo;
extern MCExecMethodInfo* kMCStoreExecCreatePurchaseMethodInfo;
extern MCExecMethodInfo* kMCStoreGetPurchaseStateMethodInfo;
extern MCExecMethodInfo* kMCStoreGetPurchaseErrorMethodInfo;
extern MCExecMethodInfo* kMCStoreGetPurchasePropertyMethodInfo;
extern MCExecMethodInfo* kMCStoreSetPurchasePropertyMethodInfo;
extern MCExecMethodInfo* kMCStoreExecSendPurchaseRequestMethodInfo;
extern MCExecMethodInfo* kMCStoreExecConfirmPurchaseDeliveryMethodInfo;
extern MCExecMethodInfo* kMCStoreExecRequestProductDetailsMethodInfo;
extern MCExecMethodInfo* kMCStoreExecPurchaseVerifyMethodInfo;


void MCStoreGetCanMakePurchase(MCExecContext& ctxt, bool& r_result);
void MCStoreExecEnablePurchaseUpdates(MCExecContext& ctxt);
void MCStoreExecDisablePurchaseUpdates(MCExecContext& ctxt);
void MCStoreExecRestorePurchases(MCExecContext& ctxt);
void MCStoreGetPurchaseList(MCExecContext& ctxt, MCStringRef& r_list);
void MCStoreExecCreatePurchase(MCExecContext& ctxt, MCStringRef p_product_id, uint32_t& r_id);
void MCStoreGetPurchaseState(MCExecContext& ctxt, int p_id, MCStringRef& r_state);
void MCStoreGetPurchaseError(MCExecContext& ctxt, int p_id, MCStringRef& r_error);
void MCStoreGetPurchaseProperty(MCExecContext& ctxt, int p_id, MCStringRef p_prop_name);
void MCStoreSetPurchaseProperty(MCExecContext& ctxt, int p_id, MCStringRef p_prop_name, uint32_t p_quantity);
void MCStoreExecSendPurchaseRequest(MCExecContext& ctxt, uint32_t p_id);
void MCStoreExecConfirmPurchaseDelivery(MCExecContext& ctxt, uint32_t p_id);
void MCStoreExecRequestProductDetails(MCExecContext& ctxt, MCStringRef p_product_id);
void MCStoreExecPurchaseVerify(MCExecContext& ctxt, uint32_t p_id, bool p_verified);

///////////

extern MCExecSetTypeInfo *kMCOrientationOrientationsTypeInfo;
extern MCExecEnumTypeInfo *kMCOrientationOrientationTypeInfo;

extern MCExecMethodInfo *kMCOrientationGetDeviceOrientationMethodInfo;
extern MCExecMethodInfo *kMCOrientationGetOrientationMethodInfo;
extern MCExecMethodInfo *kMCOrientationGetAllowedOrientationsMethodInfo;
extern MCExecMethodInfo *kMCOrientationSetAllowedOrientationsMethodInfo;
extern MCExecMethodInfo *kMCOrientationGetOrientationLockedMethodInfo;
extern MCExecMethodInfo *kMCOrientationExecLockOrientationMethodInfo;
extern MCExecMethodInfo *kMCOrientationExecUnlockOrientationMethodInfo;

void MCOrientationGetDeviceOrientation(MCExecContext& ctxt, intenum_t& r_orientation);
void MCOrientationGetOrientation(MCExecContext& ctxt, intenum_t& r_orientation);
void MCOrientationGetAllowedOrientations(MCExecContext& ctxt, intset_t& r_orientation);
void MCOrientationSetAllowedOrientations(MCExecContext& ctxt, intset_t p_orientations);
void MCOrientationGetOrientationLocked(MCExecContext& ctxt, bool& r_locked);
void MCOrientationExecLockOrientation(MCExecContext& ctxt);
void MCOrientationExecUnlockOrientation(MCExecContext& ctxt);

///////////

extern MCExecMethodInfo *kMCMailExecSendEmailMethodInfo;
extern MCExecMethodInfo *kMCMailExecComposeMailMethodInfo;
extern MCExecMethodInfo *kMCMailExecComposeUnicodeMailMethodInfo;
extern MCExecMethodInfo *kMCMailExecComposeHtmlMailMethodInfo;
extern MCExecMethodInfo *kMCMailGetCanSendMailMethodInfo;

void MCMailExecSendEmail(MCExecContext& ctxt, MCStringRef p_to, MCStringRef p_cc, MCStringRef p_subject, MCStringRef p_body);
void MCMailExecComposeMail(MCExecContext& ctxt, MCStringRef p_to, MCStringRef p_cc, MCStringRef p_bcc, MCStringRef p_subject, MCStringRef p_body, MCArrayRef p_attachments);
void MCMailExecComposeUnicodeMail(MCExecContext& ctxt, MCStringRef p_to, MCStringRef p_cc, MCStringRef p_bcc, MCStringRef p_subject, MCStringRef p_body, MCArrayRef p_attachments);
void MCMailExecComposeHtmlMail(MCExecContext& ctxt, MCStringRef p_to, MCStringRef p_cc, MCStringRef p_bcc, MCStringRef p_subject, MCStringRef p_body, MCArrayRef p_attachments);
void MCMailGetCanSendMail(MCExecContext& ctxt, bool& r_result);

///////////

extern MCExecMethodInfo *kMCAddressBookExecPickContactMethodInfo;
extern MCExecMethodInfo *kMCAddressBookExecShowContactMethodInfo;
extern MCExecMethodInfo *kMCAddressBookExecCreateContactMethodInfo;
extern MCExecMethodInfo *kMCAddressBookExecUpdateContactMethodInfo;
extern MCExecMethodInfo *kMCAddressBookGetContactDataMethodInfo;
extern MCExecMethodInfo *kMCAddressBookExecRemoveContactMethodInfo;
extern MCExecMethodInfo *kMCAddressBookExecAddContactMethodInfo;
extern MCExecMethodInfo *kMCAddressBookExecFindContactMethodInfo;

void MCAddressBookExecPickContact(MCExecContext& ctxt);
void MCAddressBookExecShowContact(MCExecContext& ctxt, int32_t p_contact_id);
void MCAddressBookExecCreateContact(MCExecContext& ctxt);
void MCAddressBookExecUpdateContact(MCExecContext& ctxt, MCArrayRef p_contact, MCStringRef p_title, MCStringRef p_message, MCStringRef p_alternate_name);
void MCAddressBookGetContactData(MCExecContext& ctxt, int32_t p_contact_id, MCArrayRef& r_contact_data);
void MCAddressBookExecRemoveContact(MCExecContext& ctxt, int32_t p_contact_id);
void MCAddressBookExecAddContact(MCExecContext &ctxt, MCArrayRef p_contact);
void MCAddressBookExecFindContact(MCExecContext& ctxt, MCStringRef p_contact_name);

///////////

struct MCAdTopLeft;

extern MCExecCustomTypeInfo* kMCAdTopLeftTypeInfo;

extern MCExecMethodInfo* kMCAdExecRegisterWithInteractiveMethodInfo;
extern MCExecMethodInfo* kMCAdExecCreateAdMethodInfo;
extern MCExecMethodInfo* kMCAdExecDeleteAdMethodInfo;
extern MCExecMethodInfo* kMCAdSetVisibleOfAdMethodInfo;
extern MCExecMethodInfo* kMCAdGetVisibleOfAdMethodInfo;
extern MCExecMethodInfo* kMCAdGetTopLeftOfAdMethodInfo;
extern MCExecMethodInfo* kMCAdSetTopLeftOfAdMethodInfo;
extern MCExecMethodInfo* kMCAdGetAdsMethodInfo;

void MCAdExecRegisterWithInneractive(MCExecContext& ctxt, MCStringRef p_key);
void MCAdExecCreateAd(MCExecContext& ctxt, MCStringRef p_name, MCStringRef p_type, MCAdTopLeft p_topleft, MCArrayRef p_metadata);
void MCAdExecDeleteAd(MCExecContext& ctxt, MCStringRef p_name);
void MCAdSetVisibleOfAd(MCExecContext& ctxt, MCStringRef p_name, bool p_visible);
void MCAdGetVisibleOfAd(MCExecContext& ctxt, MCStringRef p_name, bool& r_visible);
void MCAdGetTopLeftOfAd(MCExecContext& ctxt, MCStringRef p_name, MCAdTopLeft& r_topleft);
void MCAdSetTopLeftOfAd(MCExecContext& ctxt, MCStringRef p_name, const MCAdTopLeft& p_topleft);
void MCAdGetAds(MCExecContext& ctxt, MCStringRef& r_ads);

///////////

struct MCNativeControlIdentifier;

extern MCExecCustomTypeInfo *kMCNativeControlColorTypeInfo;
extern MCExecCustomTypeInfo *kMCNativeControlRangeTypeInfo;
extern MCExecCustomTypeInfo *kMCNativeControlIdentifierTypeInfo;
extern MCExecCustomTypeInfo *kMCNativeControlDecelerationRateTypeInfo;
extern MCExecCustomTypeInfo *kMCNativeControlIndicatorInsetsTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlIndicatorStyleTypeInfo;

extern MCExecEnumTypeInfo *kMCNativeControlPlaybackStateTypeInfo;
extern MCExecSetTypeInfo *kMCNativeControlLoadStateTypeInfo;

extern MCExecEnumTypeInfo *kMCNativeControlInputCapitalizationTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputAutocorrectionTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputKeyboardTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputKeyboardStyleTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputReturnKeyTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputContentTypeTypeInfo;
extern MCExecSetTypeInfo *kMCNativeControlInputDataDetectorTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputTextAlignTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlInputVerticalAlignTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlClearButtonModeTypeInfo;
extern MCExecEnumTypeInfo *kMCNativeControlBorderStyleTypeInfo;

extern MCExecMethodInfo* kMCNativeControlExecCreateControlMethodInfo;
extern MCExecMethodInfo* kMCNativeControlExecDeleteControlMethodInfo;
extern MCExecMethodInfo* kMCNativeControlExecSetPropertyMethodInfo;
extern MCExecMethodInfo* kMCNativeControlExecGetPropertyMethodInfo;
extern MCExecMethodInfo* kMCNativeControlExecDoMethodInfo;
extern MCExecMethodInfo* kMCNativeControlGetTargetMethodInfo;
extern MCExecMethodInfo* kMCNativeControlGetControlListMethodInfo;

void MCNativeControlExecCreateControl(MCExecContext& ctxt, MCStringRef p_type_name, MCStringRef p_control_name);
void MCNativeControlExecDeleteControl(MCExecContext& ctxt, MCStringRef p_control_name);
void MCNativeControlExecGet(MCExecContext& ctxt, MCStringRef p_control_name, MCStringRef p_property_name, MCValueRef& r_result);
void MCNativeControlExecSet(MCExecContext& ctxt, MCStringRef p_control_name, MCStringRef p_property_name, MCValueRef p_value);
void MCNativeControlExecDo(MCExecContext& ctxt, MCStringRef p_control_name, MCStringRef p_action_name, MCValueRef *p_arguments, uindex_t p_argument_count);
void MCNativeControlGetTarget(MCExecContext& ctxt, MCNativeControlIdentifier& r_target);
void MCNativeControlGetControlList(MCExecContext& ctxt, MCStringRef& r_list);

//////////

extern MCExecEnumTypeInfo *kMCSensorTypeTypeInfo;

extern MCExecMethodInfo *kMCSensorExecStartTrackingSensorMethodInfo;
extern MCExecMethodInfo *kMCSensorExecStopTrackingSensorMethodInfo;
extern MCExecMethodInfo *kMCSensorGetSensorAvailableMethodInfo;
extern MCExecMethodInfo *kMCSensorGetDetailedLocationOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetLocationOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetDetailedHeadingOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetHeadingOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetDetailedAccelerationOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetAccelerationOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetDetailedRotationRateOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetRotationRateOfDeviceMethodInfo;
extern MCExecMethodInfo *kMCSensorGetLocationCalibrationMethodInfo;
extern MCExecMethodInfo *kMCSensorSetLocationCalibrationMethodInfo;

void MCSensorExecStartTrackingSensor(MCExecContext& ctxt, intenum_t p_sensor, bool p_loosely);
void MCSensorExecStopTrackingSensor(MCExecContext& ctxt, intenum_t p_sensor);
void MCSensorGetSensorAvailable(MCExecContext& ctxt, intenum_t p_sensor, bool& r_available);
void MCSensorGetDetailedLocationOfDevice(MCExecContext& ctxt, MCArrayRef &r_detailed_location);
void MCSensorGetLocationOfDevice(MCExecContext& ctxt, MCStringRef &r_location);
void MCSensorGetDetailedHeadingOfDevice(MCExecContext& ctxt, MCArrayRef &r_detailed_heading);
void MCSensorGetHeadingOfDevice(MCExecContext& ctxt, MCStringRef &r_heading);
void MCSensorGetDetailedAccelerationOfDevice(MCExecContext& ctxt, MCArrayRef &r_detailed_acceleration);
void MCSensorGetAccelerationOfDevice(MCExecContext& ctxt, MCStringRef &r_acceleration);
void MCSensorGetDetailedRotationRateOfDevice(MCExecContext& ctxt, MCArrayRef &r_detailed_rotation_rate);
void MCSensorGetRotationRateOfDevice(MCExecContext& ctxt, MCStringRef &r_rotation_rate);
void MCSensorSetLocationCalibrationTimeout(MCExecContext& ctxt, int32_t p_timeout);
void MCSensorGetLocationCalibrationTimeout(MCExecContext& ctxt, int32_t& r_timeout);

//////////

extern MCExecEnumTypeInfo *kMCPickButtonTypeTypeInfo;
extern MCExecEnumTypeInfo *kMCPickCameraSourceTypeTypeInfo;
extern MCExecSetTypeInfo *kMCPickCameraFeaturesTypeInfo;
extern MCExecSetTypeInfo *kMCPickCamerasFeaturesTypeInfo;
extern MCExecSetTypeInfo *kMCPickMediaTypesTypeInfo;
extern MCExecEnumTypeInfo *kMCPickPhotoSourceTypeTypeInfo;

extern MCExecMethodInfo *kMCPickExecPickDateMethodInfo;
extern MCExecMethodInfo *kMCPickExecPickTimeMethodInfo;
extern MCExecMethodInfo *kMCPickExecPickDateAndTimeMethodInfo;
extern MCExecMethodInfo *kMCPickExecPickOptionByIndexMethodInfo;
extern MCExecMethodInfo *kMCPickGetSpecificCameraFeaturesMethodInfo;
extern MCExecMethodInfo *kMCPickGetCameraFeaturesMethodInfo;
extern MCExecMethodInfo *kMCPickExecPickMediaMethodInfo;
extern MCExecMethodInfo *kMCPickExecPickPhotoAndResizeMethodInfo;
extern MCExecMethodInfo *kMCPickExecPickPhotoMethodInfo;

void MCPickExecPickDate(MCExecContext& ctxt, MCStringRef p_current, MCStringRef p_start, MCStringRef p_end, intenum_t p_buttons, MCRectangle p_button_rect);
void MCPickExecPickTime(MCExecContext &ctxt, MCStringRef p_current, MCStringRef p_start, MCStringRef p_end, int32_t *p_step, intenum_t p_buttons, MCRectangle p_button_rect);
void MCPickExecPickDateAndTime(MCExecContext &ctxt, MCStringRef p_current, MCStringRef p_start, MCStringRef p_end, int32_t *p_step, intenum_t p_buttons, MCRectangle p_button_rect);
void MCPickExecPickOptionByIndex(MCExecContext &ctxt, int p_chunk_type, MCStringRef *p_option_lists, uindex_t p_option_list_count, uindex_t *p_initial_indices, uindex_t p_indices_count, bool p_use_hilite_type, bool p_use_picker, bool p_use_cancel, bool p_use_done, MCRectangle p_button_rect);
void MCPickGetSpecificCameraFeatures(MCExecContext& ctxt, intenum_t p_source, intset_t& r_features);
void MCPickGetCameraFeatures(MCExecContext& ctxt, intset_t& r_features);
void MCPickExecPickMedia(MCExecContext &ctxt, intset_t p_allowed_types, bool p_multiple);
void MCPickExecPickPhotoAndResize(MCExecContext& ctxt, intenum_t p_source, uinteger_t p_width, uinteger_t p_height);
void MCPickExecPickPhoto(MCExecContext& ctxt, intenum_t p_source);

///////////

extern MCExecMethodInfo* kMCCalendarExecShowEventMethodInfo;
extern MCExecMethodInfo* kMCCalendarGetEventDataMethodInfo;
extern MCExecMethodInfo* kMCCalendarExecCreateEventMethodInfo;
extern MCExecMethodInfo* kMCCalendarExecUpdateEventMethodInfo;
extern MCExecMethodInfo* kMCCalendarExecRemoveEventMethodInfo;
extern MCExecMethodInfo* kMCCalendarExecAddEventMethodInfo;
extern MCExecMethodInfo* kMCCalendarGetCalendarsMethodInfo;
extern MCExecMethodInfo* kMCCalendarExecFindEventMethodInfo;


void MCCalendarExecShowEvent(MCExecContext& ctxt, MCStringRef p_id);
void MCCalendarGetEventData(MCExecContext& ctxt, MCStringRef p_id, MCArrayRef& r_data);
void MCCalendarExecCreateEvent(MCExecContext& ctxt);
void MCCalendarExecUpdateEvent(MCExecContext& ctxt, MCStringRef p_id);
void MCCalendarExecRemoveEvent(MCExecContext& ctxt, MCStringRef p_id);
void MCCalendarExecAddEvent(MCExecContext& ctxt, MCArrayRef p_data);
void MCCalendarGetCalendars(MCExecContext& ctxt);
void MCCalendarExecFindEvent(MCExecContext& ctxt, MCStringRef p_id, bool& r_found);


///////////

extern MCExecEnumTypeInfo* kMCStorePurchasePropertyTypeInfo;

extern MCExecMethodInfo* kMCNotificationExecCreateLocalNotificationMethodInfo;
extern MCExecMethodInfo* kMCNotificationGetRegisteredNotificationsMethodInfo;
extern MCExecMethodInfo* kMCNotificationGetDetailsMethodInfo;
extern MCExecMethodInfo* kMCNotificationExecCancelLocalNotificationMethodInfo;
extern MCExecMethodInfo* kMCNotificationExecCancelAllLocalNotificationsMethodInfo;
extern MCExecMethodInfo* kMCNotificationGetNotificationBadgeValueMethodInfo;
extern MCExecMethodInfo* kMCNotificationSetNotificationBadgeValueMethodInfo;

void MCNotificationExecCreateLocalNotification(MCExecContext& ctxt, MCStringRef p_alert_body, MCStringRef p_alert_action, MCStringRef p_user_info, MCDateTime p_date, bool p_play_sound, int32_t p_badge_value);
void MCNotificationGetRegisteredNotifications(MCExecContext& ctxt);
void MCNotificationGetDetails(MCExecContext& ctxt, int32_t p_id, MCArrayRef& r_details);
void MCNotificationExecCancelLocalNotification(MCExecContext& ctxt, int32_t p_id);
void MCNotificationExecCancelAllLocalNotifications(MCExecContext& ctxt);
void MCNotificationGetNotificationBadgeValue(MCExecContext& ctxt);
void MCNotificationSetNotificationBadgeValue(MCExecContext& ctxt, uint32_t p_badge_value);

///////////

extern MCExecEnumTypeInfo* kMCBusyIndicatorTypeInfo;
extern MCExecEnumTypeInfo* kMCActivityIndicatorTypeInfo;

extern MCExecMethodInfo* kMCBusyIndicatorExecStartActivityIndicatorMethodInfo;
extern MCExecMethodInfo* kMCBusyIndicatorExecStopActivityIndicatorMethodInfo;
extern MCExecMethodInfo* kMCBusyIndicatorExecStartBusyIndicatorMethodInfo;
extern MCExecMethodInfo* kMCBusyIndicatorExecStopBusyIndicatorMethodInfo;


void MCBusyIndicatorExecStartActivityIndicator(MCExecContext& ctxt, intenum_t p_indicator, integer_t* p_location_x, integer_t* p_location_y);
void MCBusyIndicatorExecStopActivityIndicator(MCExecContext& ctxt);
void MCBusyIndicatorExecStartBusyIndicator(MCExecContext& ctxt, intenum_t p_indicator, MCStringRef p_label, int32_t p_opacity);
void MCBusyIndicatorExecStopBusyIndicator(MCExecContext& ctxt);

////////////

extern MCExecEnumTypeInfo* kMCSoundAudioCategoryTypeInfo;

extern MCExecMethodInfo* kMCSoundExecPlaySoundOnChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundExecStopPlayingOnChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundExecPausePlayingOnChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundExecResumePlayingOnChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundExecDeleteSoundChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundSetVolumeOfChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundGetVolumeOfChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundGetStatusOfChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundGetSoundOfChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundGetNextSoundOfChannelMethodInfo;
extern MCExecMethodInfo* kMCSoundGetSoundChannelsMethodInfo;
extern MCExecMethodInfo* kMCSoundSetAudioCategoryMethodInfo;

void MCSoundExecPlaySoundOnChannel(MCExecContext& ctxt, MCStringRef p_channel, MCStringRef p_file, intenum_t p_type);
void MCSoundExecStopPlayingOnChannel(MCExecContext& ctxt, MCStringRef p_channel);
void MCSoundExecPausePlayingOnChannel(MCExecContext& ctxt, MCStringRef p_channel);
void MCSoundExecResumePlayingOnChannel(MCExecContext& ctxt, MCStringRef p_channel);
void MCSoundExecDeleteSoundChannel(MCExecContext& ctxt, MCStringRef p_channel);
void MCSoundSetVolumeOfChannel(MCExecContext& ctxt, MCStringRef p_channel, int32_t p_volume);
void MCSoundGetVolumeOfChannel(MCExecContext& ctxt, MCStringRef p_channel, int32_t& r_volume);
void MCSoundGetStatusOfChannel(MCExecContext& ctxt, MCStringRef p_channel, intenum_t& r_status);
void MCSoundGetSoundOfChannel(MCExecContext& ctxt, MCStringRef p_channel, MCStringRef &r_sound);
void MCSoundGetNextSoundOfChannel(MCExecContext& ctxt, MCStringRef p_channel, MCStringRef &r_sound);
void MCSoundGetSoundChannels(MCExecContext& ctxt, MCStringRef &r_channels);
void MCSoundSetAudioCategory(MCExecContext &ctxt, intenum_t p_category);

/////////////

extern MCExecEnumTypeInfo* kMCMiscKeyboardTypeTypeInfo;
extern MCExecEnumTypeInfo* kMCMiscKeyboardReturnTypeTypeInfo;
extern MCExecEnumTypeInfo* kMCMiscStatusBarStyleTypeInfo;

extern MCExecMethodInfo* kMCMiscGetDeviceTokenMethodInfo;
extern MCExecMethodInfo* kMCMiscGetLaunchUrlMethodInfo;
extern MCExecMethodInfo* kMCMiscExecBeepMethodInfo;
extern MCExecMethodInfo* kMCMiscExecVibrateMethodInfo;
extern MCExecMethodInfo* kMCMiscGetDeviceResolutionMethodInfo;
extern MCExecMethodInfo* kMCMiscSetUseDeviceResolutionMethodInfo;
extern MCExecMethodInfo* kMCMiscGetDeviceScaleMethodInfo;
extern MCExecMethodInfo* kMCMiscGetPixelDensityMethodInfo;
extern MCExecMethodInfo* kMCMiscSetStatusBarStyleMethodInfo;
extern MCExecMethodInfo* kMCMiscExecShowStatusBarMethodInfo;
extern MCExecMethodInfo* kMCMiscExecHideStatusBarMethodInfo;
extern MCExecMethodInfo* kMCMiscSetKeyboardTypeMethodInfo;
extern MCExecMethodInfo* kMCMiscSetKeyboardReturnKeyMethodInfo;
extern MCExecMethodInfo* kMCMiscGetPreferredLanguagesMethodInfo;
extern MCExecMethodInfo* kMCMiscGetPreferredLocaleMethodInfo;
extern MCExecMethodInfo* kMCMiscExecClearTouchesMethodInfo;
extern MCExecMethodInfo* kMCMiscGetSystemIdentifierMethodInfo;
extern MCExecMethodInfo* kMCMiscGetApplicationIdentifierMethodInfo;
extern MCExecMethodInfo* kMCMiscSetReachabilityTargetMethodInfo;
extern MCExecMethodInfo* kMCMiscGetReachabilityTargetMethodInfo;
extern MCExecMethodInfo* kMCMiscExecExportImageToAlbumMethodInfo;
extern MCExecMethodInfo* kMCMiscSetRedrawIntervalMethodInfo;
extern MCExecMethodInfo* kMCMiscSetAnimatedAutorotationMethodInfo;
extern MCExecMethodInfo* kMCMiscGetDoNotBackupFileMethodInfo;
extern MCExecMethodInfo* kMCMiscSetDoNotBackupFileMethodInfo;
extern MCExecMethodInfo* kMCMiscGetFileDataProtectionMethodInfo;
extern MCExecMethodInfo* kMCMiscSetFileDataProtectionMethodInfo;
extern MCExecMethodInfo* kMCMiscExecLibDownloadUrlToFileMethodInfo;
extern MCExecMethodInfo* kMCMiscGetBuildInfoMethodInfo;


void MCMiscGetDeviceToken(MCExecContext& ctxt, MCStringRef& r_token);
void MCMiscGetLaunchUrl(MCExecContext& ctxt, MCStringRef& r_url);

void MCMiscExecBeep(MCExecContext& ctxt, int32_t* p_number_of_times);
void MCMiscExecVibrate(MCExecContext& ctxt, int32_t* p_number_of_times);

void MCMiscGetDeviceResolution(MCExecContext& ctxt, MCStringRef& r_resolution);
void MCMiscSetUseDeviceResolution(MCExecContext& ctxt, bool p_use_device_res, bool p_use_control_device_res);
void MCMiscGetDeviceScale(MCExecContext& ctxt, real64_t& r_scale);
void MCMiscGetPixelDensity(MCExecContext& ctxt, real64_t& r_density);

void MCMiscSetStatusBarStyle(MCExecContext& ctxt, intenum_t p_style);
void MCMiscExecShowStatusBar(MCExecContext& ctxt);
void MCMiscExecHideStatusBar(MCExecContext& ctxt);

void MCMiscSetKeyboardType(MCExecContext& ctxt, intenum_t p_keyboard_type);
void MCMiscSetKeyboardReturnKey(MCExecContext& ctxt, intenum_t p_keyboard_return_key);

void MCMiscGetPreferredLanguages(MCExecContext& ctxt, MCStringRef& r_preferred_languages);
void MCMiscGetCurrentLocale(MCExecContext& ctxt, MCStringRef& r_current_locale);

void MCMiscExecClearTouches(MCExecContext& ctxt);

void MCMiscGetSystemIdentifier(MCExecContext& ctxt, MCStringRef& r_identifier);
void MCMiscGetApplicationIdentifier(MCExecContext& ctxt, MCStringRef& r_identifier);

void MCMiscSetReachabilityTarget(MCExecContext& ctxt, MCStringRef p_hostname);
void MCMiscGetReachabilityTarget(MCExecContext& ctxt, MCStringRef& r_hostname);

void MCMiscExecExportImageToAlbum(MCExecContext& ctxt, MCStringRef p_data, MCStringRef p_file_name);

void MCMiscSetRedrawInterval(MCExecContext& ctxt, int32_t p_interval);
void MCMiscSetAnimateAutorotation(MCExecContext& ctxt, bool p_enabled);

void MCMiscGetDoNotBackupFile(MCExecContext& ctxt, MCStringRef p_path, bool& r_no_backup);
void MCMiscSetDoNotBackupFile(MCExecContext& ctxt, MCStringRef p_path, bool p_no_backup);
void MCMiscGetFileDataProtection(MCExecContext& ctxt, MCStringRef p_path, MCStringRef& p_protection_string);
void MCMiscSetFileDataProtection(MCExecContext& ctxt, MCStringRef p_path, MCStringRef p_protection_string);

void MCMiscExecLibUrlDownloadToFile(MCExecContext& ctxt, MCStringRef p_url, MCStringRef p_filename);

void MCMiscGetBuildInfo(MCExecContext& ctxt, MCStringRef p_key, MCStringRef& r_value);

#endif
