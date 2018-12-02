// QXml.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "QXml.h"
#include "XML.h"
#include "Debug.h"

typedef void(*logprintf_t)(const char* format, ...);

logprintf_t logprintf;
QXMLHandler* gXMLHandler;

extern void *pAMXFunctions;

cell AMX_NATIVE_CALL CreateXMLDocument(AMX* amx, cell* params)
{
	cell* pathAddr = NULL;
	int length;

	amx_GetAddr(amx, params[1], &pathAddr);
	amx_StrLen(pathAddr, &length);

	if (length <= 0)
	{
		logprintf("[QXml] 파일의 이름이 올바르지 않습니다.");

		return -1;
	}

	++length;

	char* path = new char[length];

	amx_GetString(path, pathAddr, 0, length);

	cell handler = gXMLHandler->openDocument(std::string(path));

	if (handler == -1)
		logprintf("[QXml] 파일 %s을(를) 열 수 없습니다.", path);

	DEBUG << path << " 경로의 파일 로드 됨." << std::endl;
	
	return handler;
}

cell AMX_NATIVE_CALL DestroyXMLDocument(AMX* amx, cell* params)
{
	if (params[1] == -1)
		return 0;
	
	QXMLHandler::QXMLDocument* doc = gXMLHandler->getDocument(params[1]);

	if (doc == NULL || !doc->isOpened())
		return 0;
	
	gXMLHandler->closeDocument(params[1]);

	DEBUG << "id: " << params[1] << " 도큐멘트 종료 됨." << std::endl;

	return 1;
}

cell AMX_NATIVE_CALL CreateXMLNodePointer(AMX* amx, cell* params)
{
	QXMLHandler::QXMLDocument* doc = gXMLHandler->getDocument(params[1]);

	if (doc == NULL)
	{
		logprintf("[QXml] 도큐멘트 id %d를 찾을 수 없습니다.", params[1]);

		return -1;
	}

	cell handler = gXMLHandler->createNode(doc);

	if (handler == -1)
		logprintf("[QXml] 도큐멘트 id %d에 대한 노드를 생성할 수 없습니다.", params[1]);
	
	DEBUG << "도큐멘트 id " << params[1] << "에 대한 노드 id " << handler << "(이)가 생성 됨." << std::endl;

	return handler;
}

cell AMX_NATIVE_CALL DestroyXMLNodePointer(AMX* amx, cell* params)
{
	if (params[1] == -1)
		return 0;

	QXMLHandler::QXMLNode* node = gXMLHandler->getNode(params[1]);

	if (node == NULL || !node->isCreated())
		return 0;
	
	delete node;

	DEBUG << "id: " << " 노드가 제거 됨." << std::endl;

	return 1;
}

cell AMX_NATIVE_CALL SetNodeFirstChild(AMX* amx, cell* params)
{
	QXMLHandler::QXMLNode* node = gXMLHandler->getNode(params[1]);

	if (!node->isCreated())
		return 0;

	XMLNode* lastNode = node->getNode();

	if (lastNode == NULL)
		lastNode = node->getDocument();

	return (node->setNode(lastNode->FirstChild()));
}

cell AMX_NATIVE_CALL SetNodeNextSibling(AMX* amx, cell* params)
{
	QXMLHandler::QXMLNode* node = gXMLHandler->getNode(params[1]);

	if (node == NULL || !node->isCreated())
		return 0;

	return (node->setNode(node->getNode()->NextSibling()));
}

cell AMX_NATIVE_CALL GetNodeName(AMX* amx, cell* params)
{
	QXMLHandler::QXMLNode* node = gXMLHandler->getNode(params[1]);

	if (node != NULL && node->isCreated())
	{
		XMLElement* element = node->getNodeElement();

		if (element != NULL)
		{
			cell* destAddr;

			amx_GetAddr(amx, params[2], &destAddr);

			const char* name = element->Name();

			amx_SetString(destAddr, name, 0, 0, params[3]);

			return 1;
		}
	}

	return 0;
}

cell AMX_NATIVE_CALL GetNodeValue(AMX* amx, cell* params)
{
	QXMLHandler::QXMLNode* node = gXMLHandler->getNode(params[1]);

	if (node != NULL && node->isCreated())
	{
		XMLElement* element = node->getNodeElement();

		if (element != NULL)
		{
			cell* destAddr;

			amx_GetAddr(amx, params[2], &destAddr);

			const char* value = element->Value();

			amx_SetString(destAddr, value, 0, 0, params[3]);

			return 1;
		}
	}

	return 0;
}

cell AMX_NATIVE_CALL CreateXMLAttributePointer(AMX* amx, cell* params)
{
	QXMLHandler::QXMLNode* node = gXMLHandler->getNode(params[1]);

	if (node != NULL && node->isCreated())
	{
		cell handler = gXMLHandler->createAttribute(node);

		if (handler != -1)
			DEBUG << "노드 id " << params[1] << "에 대한 애트리뷰트 id " << handler << "(이)가 생성 됨." << std::endl;
		else
		{
			DEBUG << "노드 id " << params[1] << "에 대한 애트리뷰트를 생성할 수 없습니다." << std::endl;

			logprintf("[QXml] 노드 id %d에 대한 애트리뷰트를 생성할 수 없습니다.", node->getHandle());
		}

		return handler;
	}
	
	DEBUG << "노드 id " << params[1] << "를 찾을 수 없어 애트리뷰트를 생성할 수 없습니다." << std::endl;

	return -1;
}

cell AMX_NATIVE_CALL DestroyXMLAttributePointer(AMX* amx, cell* params)
{
	QXMLHandler::QXMLAttribute* attribute = gXMLHandler->getAttribute(params[1]);

	if (attribute != NULL && attribute->isCreated())
	{
		delete attribute;

		return 1;
	}

	return 0;
}

cell AMX_NATIVE_CALL SetAttributeFirst(AMX* amx, cell* params)
{
	QXMLHandler::QXMLAttribute* attribute = gXMLHandler->getAttribute(params[1]);

	if (attribute == NULL || !attribute->isCreated() || !attribute->updateElement())
		return 0;

	return (attribute->setAttribute(attribute->getElement()->FirstAttribute()));
}

cell AMX_NATIVE_CALL SetAttributeNext(AMX* amx, cell* params)
{
	QXMLHandler::QXMLAttribute* attribute = gXMLHandler->getAttribute(params[1]);

	if (attribute == NULL || !attribute->isCreated() || attribute->getElement() == NULL)
		return 0;

	return (attribute->setAttribute(attribute->getAttribute()->Next()));
}

cell AMX_NATIVE_CALL GetAttributeName(AMX* amx, cell* params)
{
	QXMLHandler::QXMLAttribute* attribute = gXMLHandler->getAttribute(params[1]);

	if (attribute != NULL || attribute->isCreated())
	{
		if (attribute->getAttribute() != NULL)
		{
			cell* destAddr;
			const char* name;

			amx_GetAddr(amx, params[2], &destAddr);

			name = attribute->getAttribute()->Name();
			
			if (name != NULL)
				amx_SetString(destAddr, name, 0, 0, params[3]);

			return 1;
		}
	}

	return 0;
}

cell AMX_NATIVE_CALL GetAttributeValue(AMX* amx, cell* params)
{
	QXMLHandler::QXMLAttribute* attribute = gXMLHandler->getAttribute(params[1]);

	if (attribute != NULL || attribute->isCreated())
	{
		if (attribute->getAttribute() != NULL)
		{
			cell* destAddr;
			const char* value;

			amx_GetAddr(amx, params[2], &destAddr);

			value = attribute->getAttribute()->Value();

			if (value != NULL)
				amx_SetString(destAddr, value, 0, 0, params[3]);

			return 1;
		}
	}

	return 0;
}

AMX_NATIVE_INFO projectNatives[] = {
	{"CreateXMLDocument", CreateXMLDocument},
	{"DestroyXMLDocument", DestroyXMLDocument},
	//{"xml_save", XMLSave},
	{"CreateXMLNodePointer", CreateXMLNodePointer},
	{"DestroyXMLNodePointer", DestroyXMLNodePointer},
	{"SetNodeFirstChild", SetNodeFirstChild},
	{"SetNodeNextSibling", SetNodeNextSibling},
	{"GetNodeName", GetNodeName},
	{"GetNodeValue", GetNodeValue},

	{"CreateXMLAttributePointer", CreateXMLAttributePointer},
	{"DestroyXMLAttributePointer", DestroyXMLAttributePointer},
	{"SetAttributeFirst", SetAttributeFirst},
	{"SetAttributeNext", SetAttributeNext},
	{"GetAttributeName", GetAttributeName},
	{"GetAttributeValue", GetAttributeValue},
	/*{"xml_clonepointer", XMLClonePointer},
	{"xml_killpointer", XMLDestroyPointer},
	{"xml_pointer_getfilehandle", XMLPointerGetFile},
	{"xml_pointer_getname", XMLPointerGetName},
	{"xml_pointer_getvalue", XMLPointerGetValue},
	{"xml_pointer_getvalue_int", XMLPointerGetValueInt},
	{"xml_pointer_getvalue_float", XMLPointerGetValueFloat},

	{"xml_pointer_root", XMLPointerJumpRoot},
	{"xml_pointer_parentnode", XMLPointerJumpParent},
	{"xml_pointer_childnode", XMLPointerJumpChild},
	{"xml_pointer_nextnode", XMLPointerJumpNext},

	{"xml_pointer_childattr", XMLPointerJumpAttr},
	{"xml_pointer_nextattr", XMLPointerNextAttr},

	{"xml_pointer_delete", XMLElementDelete},
	{"xml_pointer_appendnode", XMLElementAppendNode},
	{"xml_pointer_prependnode", XMLElementAppendNode},*/
	{0, 0}
};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = static_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

	gXMLHandler = new QXMLHandler();

	Debug::Initialize("QXml_Debug.txt");

	logprintf("[QXml] Plugin version %s loaded.", PLUGIN_VERSION);

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	logprintf("QXml unloaded.");
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, projectNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	return AMX_ERR_NONE;
}
