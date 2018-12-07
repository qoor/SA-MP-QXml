#pragma once

#include "../AMX/amx/amx.h"
#include "../AMX/plugincommon.h"

namespace Native
{
	cell AMX_NATIVE_CALL CreateXMLDocument(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL DestroyXMLDocument(AMX* amx, cell* params);

	cell AMX_NATIVE_CALL CreateXMLNodePointer(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL CreateXMLCloneNodePointer(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL DestroyXMLNodePointer(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SetNodeFirstChild(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SetNodeNextSibling(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL GetNodeName(AMX* amx, cell* params);
	//cell AMX_NATIVE_CALL GetNodeValue(AMX* amx, cell* params);

	cell AMX_NATIVE_CALL CreateXMLAttributePointer(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL DestroyXMLAttributePointer(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SetAttributeFirst(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL SetAttributeNext(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL GetAttributeName(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL GetAttributeValue(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL GetAttributeValueInt(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL GetAttributeValueFloat(AMX* amx, cell* params);
	cell AMX_NATIVE_CALL GetAttributeValueBool(AMX* amx, cell* params);
};
