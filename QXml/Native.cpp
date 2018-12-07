#pragma warning(disable:4996)

#include "XML.h"
#include "Debug.h"
#include "Native.h"

typedef void(*logprintf_t)(const char* format, ...);

extern logprintf_t logprintf;
extern QXMLHandler* gXMLHandler;

cell AMX_NATIVE_CALL Native::CreateXMLDocument(AMX* amx, cell* params)
{
	cell* pathAddr = NULL;
	int length;

	amx_GetAddr(amx, params[1], &pathAddr);
	amx_StrLen(pathAddr, &length);

	if (length <= 0)
	{
		logprintf("[QXml] 파일의 이름이 올바르지 않습니다.");

		return INVALID_HANDLE;
	}

	++length;

	char* path = new char[length];

	amx_GetString(path, pathAddr, 0, length);

	DEBUG << path << " 경로의 도큐멘트 생성 중.." << std::endl;

	cell handler = gXMLHandler->createDocument(std::string(path));

	if (handler == INVALID_HANDLE)
		logprintf("[QXml] %s 경로의 파일을 열 수 없습니다.", path);

	DEBUG << path << " 경로의 파일 로드 됨." << std::endl;

	return handler;
}

cell AMX_NATIVE_CALL Native::DestroyXMLDocument(AMX* amx, cell* params)
{
	cell* destAddress;
	int handle = INVALID_HANDLE;

	amx_GetAddr(amx, params[1], &destAddress);

	handle = *destAddress;

	if (handle == INVALID_HANDLE)
		return 0;

	QXMLDocument* document = gXMLHandler->getDocumentByHandle(handle);

	if (document == NULL || !document->isCreated())
		return 0;

	*destAddress = INVALID_HANDLE;

	gXMLHandler->destroyDocument(document);

	DEBUG << "도큐멘트 id: " << handle << " 이(가) 종료 됨." << std::endl;

	return 1;
}

cell AMX_NATIVE_CALL Native::CreateXMLNodePointer(AMX* amx, cell* params)
{
	QXMLDocument* document = gXMLHandler->getDocumentByHandle(params[1]);

	if (document == NULL || !document->isCreated())
	{
		logprintf("[QXml] 도큐멘트 id: %d 을(를) 찾을 수 없습니다.", params[1]);

		return INVALID_HANDLE;
	}

	cell handler = gXMLHandler->createNode(document);

	if (handler == -1)
		logprintf("[QXml] 도큐멘트 id: %d 에 대한 노드를 생성할 수 없습니다.", params[1]);
	else
		DEBUG << "도큐멘트 id: " << params[1] << "에 대한 노드 id: " << handler << "이(가) 생성 됨." << std::endl;

	return handler;
}

cell AMX_NATIVE_CALL Native::CreateXMLCloneNodePointer(AMX* amx, cell* params)
{
	if (params[1] == INVALID_HANDLE)
		return INVALID_HANDLE;

	QXMLNode* node = gXMLHandler->getNodeByHandle(params[1]);

	if (node == NULL || !node->isCreated())
	{
		logprintf("[QXml] 소스 노드 id: %d을(를) 찾을 수 없습니다.", params[1]);

		return INVALID_HANDLE;
	}

	cell handler = gXMLHandler->createNode(node);

	if (handler == INVALID_HANDLE)
		logprintf("[QXml] 소스 노드 id: %d에 대한 클론 노드를 생성할 수 없습니다.", params[1]);
	else
		DEBUG << "소스 노드 id: " << params[1] << "에 대한 클론 노드 id: " << handler << "이(가) 생성 됨." << std::endl;

	return handler;
}

cell AMX_NATIVE_CALL Native::DestroyXMLNodePointer(AMX* amx, cell* params)
{
	cell* destArray;
	int handle = INVALID_HANDLE;

	amx_GetAddr(amx, params[1], &destArray);

	handle = *destArray;

	DEBUG << "노드 id: " << handle << "제거 시작" << std::endl;

	if (handle == INVALID_HANDLE)
		return 0;

	QXMLNode* node = gXMLHandler->getNodeByHandle(handle);

	if (node == NULL || !node->isCreated())
		return 0;

	*destArray = INVALID_HANDLE;

	DEBUG << "노드 레퍼런스 변수 초기화 됨." << std::endl;

	gXMLHandler->destroyNode(node);

	DEBUG << "노드 id: " << handle << " 이(가) 제거 됨." << std::endl;

	return 1;
}

cell AMX_NATIVE_CALL Native::SetNodeFirstChild(AMX* amx, cell* params)
{
	QXMLNode* node = gXMLHandler->getNodeByHandle(params[1]);

	if (node == NULL || !node->isCreated())
		return 0;

	XMLNode* lastNode = node->getNode();

	if (lastNode == NULL)
		lastNode = node->getQXMLDocument()->getDocument();

	char* name = NULL;

	amx_StrParam(amx, params[2], name);

	if (name != NULL && strlen(name) > 0)
		return node->setNodePointer(lastNode->FirstChildElement(name));

	return (node->setNodePointer(lastNode->FirstChildElement()));
}

cell AMX_NATIVE_CALL Native::SetNodeNextSibling(AMX* amx, cell* params)
{
	QXMLNode* node = gXMLHandler->getNodeByHandle(params[1]);

	if (node == NULL || !node->isCreated())
		return 0;

	XMLNode* nodePointer = node->getNode();

	if (node == NULL)
		return 0;

	char* name = NULL;

	amx_StrParam(amx, params[2], name);

	if (name != NULL && strlen(name) > 0)
		return node->setNodePointer(nodePointer->NextSiblingElement(name));

	return (node->setNodePointer(nodePointer->NextSiblingElement()));
}

cell AMX_NATIVE_CALL Native::GetNodeName(AMX* amx, cell* params)
{
	QXMLNode* node = gXMLHandler->getNodeByHandle(params[1]);

	if (node != NULL && node->isCreated())
	{
		XMLNode* nodePointer = node->getNode();

		if (nodePointer != NULL)
		{
			XMLElement* element = nodePointer->ToElement();

			if (element != NULL)
			{
				cell* destAddr;

				amx_GetAddr(amx, params[2], &destAddr);

				const char* name = element->Name();

				amx_SetString(destAddr, name, 0, 0, params[3]);

				return 1;
			}
		}
	}

	return 0;
}

/*cell AMX_NATIVE_CALL Native::GetNodeValue(AMX* amx, cell* params)
{
	QXMLNode* node = gXMLHandler->getNodeByHandle(params[1]);

	if (node != NULL && node->isCreated())
	{
		XMLNode* nodePointer = node->getNode();

		if (nodePointer != NULL)
		{
			XMLElement* element = nodePointer->ToElement();

			if (element != NULL)
			{
				cell* destAddr;

				amx_GetAddr(amx, params[2], &destAddr);

				const char* value = element->Value();

				amx_SetString(destAddr, value, 0, 0, params[3]);

				return 1;
			}
		}
	}

	return 0;
}*/

cell AMX_NATIVE_CALL Native::CreateXMLAttributePointer(AMX* amx, cell* params)
{
	QXMLNode* node = gXMLHandler->getNodeByHandle(params[1]);

	if (node != NULL && node->isCreated())
	{
		cell handler = gXMLHandler->createAttribute(node);

		if (handler != INVALID_HANDLE)
			DEBUG << "노드 id: " << params[1] << " 에 대한 애트리뷰트 id: " << handler << "이(가) 생성 됨." << std::endl;
		else
		{
			DEBUG << "노드 id: " << params[1] << " 에 대한 애트리뷰트를 생성할 수 없습니다." << std::endl;

			logprintf("[QXml] 노드 id %d에 대한 애트리뷰트를 생성할 수 없습니다.", node->getHandle());
		}

		return handler;
	}

	DEBUG << "노드 id: " << params[1] << "을(를) 찾을 수 없어 애트리뷰트를 생성할 수 없습니다." << std::endl;

	return INVALID_HANDLE;
}

cell AMX_NATIVE_CALL Native::DestroyXMLAttributePointer(AMX* amx, cell* params)
{
	cell* destAddress;
	int handle = INVALID_HANDLE;

	amx_GetAddr(amx, params[1], &destAddress);

	handle = *destAddress;

	if (handle == INVALID_HANDLE)
		return 0;

	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(handle);

	if (attribute == NULL || !attribute->isCreated())
		return 0;

	*destAddress = INVALID_HANDLE;

	gXMLHandler->destroyAttribute(attribute);

	DEBUG << "애트리뷰트 id: " << handle << " (이)가 제거 됨." << std::endl;

	return 0;
}

cell AMX_NATIVE_CALL Native::SetAttributeFirst(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute == NULL || !attribute->isCreated() || !attribute->updateElement())
	{
		DEBUG << "애트리뷰트가 생성되지 않았거나 노드 엘리먼트 업데이트를 실패했습니다." << std::endl;

		return 0;
	}

	return (attribute->setAttributePointer(attribute->getElement()->FirstAttribute()));
}

cell AMX_NATIVE_CALL Native::SetAttributeNext(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute == NULL || !attribute->isCreated() || attribute->getElement() == NULL || attribute->getAttributePointer() == NULL)
	{
		DEBUG << "애트리뷰트가 생성되지 않았거나 노드 엘리먼트 업데이트를 실패했습니다." << std::endl;

		return 0;
	}

	return (attribute->setAttributePointer(attribute->getAttributePointer()->Next()));
}

cell AMX_NATIVE_CALL Native::GetAttributeName(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute != NULL && attribute->isCreated())
	{
		const XMLAttribute* attributePointer = attribute->getAttributePointer();

		if (attributePointer != NULL)
		{
			cell* destAddr;

			amx_GetAddr(amx, params[2], &destAddr);

			const char* name = attributePointer->Name();

			amx_SetString(destAddr, name, 0, 0, params[3]);

			return 1;
		}
	}

	return 0;
}

cell AMX_NATIVE_CALL Native::GetAttributeValue(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute != NULL && attribute->isCreated())
	{
		const XMLAttribute* attributePointer = attribute->getAttributePointer();

		if (attributePointer != NULL)
		{
			const char* value = attributePointer->Value();

			if (value != NULL && strlen(value) > 0)
			{
				cell* destAddr;

				amx_GetAddr(amx, params[2], &destAddr);
				amx_SetString(destAddr, value, 0, 0, params[3]);

				return 1;
			}
		}
	}

	return 0;
}

cell AMX_NATIVE_CALL Native::GetAttributeValueInt(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute != NULL && attribute->isCreated())
	{
		const XMLAttribute* attributePointer = attribute->getAttributePointer();

		if (attributePointer != NULL)
			return attributePointer->IntValue();
	}

	return 0;
}

cell AMX_NATIVE_CALL Native::GetAttributeValueFloat(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute != NULL && attribute->isCreated())
	{
		const XMLAttribute* attributePointer = attribute->getAttributePointer();

		if (attributePointer != NULL)
		{
			float floatValue = attributePointer->FloatValue();

			return amx_ftoc(floatValue);
		}
	}

	return 0;
}

cell AMX_NATIVE_CALL Native::GetAttributeValueBool(AMX* amx, cell* params)
{
	QXMLAttribute* attribute = gXMLHandler->getAttributeByHandle(params[1]);

	if (attribute != NULL && attribute->isCreated())
	{
		const XMLAttribute* attributePointer = attribute->getAttributePointer();

		if (attributePointer != NULL)
			return (static_cast<int>(attributePointer->BoolValue()));
	}

	return 0;
}
