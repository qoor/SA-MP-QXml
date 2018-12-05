#include "stdafx.h"
#include "XML.h"
#include "Debug.h"
#include "IdAllocator.h"

IdAllocator QXMLDocument::documentIdAllocator;
IdAllocator QXMLNode::nodeIdAllocator;
IdAllocator QXMLAttribute::attributeIdAllocator;

QXMLDocument::QXMLDocument(const std::string& path) : document(NULL)
{
	if (path.length() > 0)
	{
		handle = documentIdAllocator.AllocateId();

		if (handle != INVALID_HANDLE)
		{
			std::string fullPath = "scriptfiles/";

			document = new XMLDocument();

			fullPath.append(path);

			if (document->LoadFile(fullPath.c_str()) == XML_SUCCESS)
				created = true;
			else
				documentIdAllocator.DisallocateId(handle);
		}
	}
}

QXMLDocument::~QXMLDocument()
{
	if (document != NULL)
		delete document;

	if (handle != INVALID_HANDLE)
		documentIdAllocator.DisallocateId(handle);
}

QXMLNode::QXMLNode(QXMLDocument* document) : qxmlDocument(NULL), node(NULL)
{
	if (document->isCreated())
	{
		handle = nodeIdAllocator.AllocateId();

		if (handle != INVALID_HANDLE)
		{
			this->qxmlDocument = document;
			created = true;
		}
	}
}

QXMLNode::QXMLNode(const QXMLNode* node) : qxmlDocument(NULL), node(NULL)
{
	if (node->isCreated())
	{
		handle = nodeIdAllocator.AllocateId();

		if (handle != INVALID_HANDLE)
		{
			qxmlDocument = node->qxmlDocument;
			this->node = node->node;
			created = true;
		}
	}
}

QXMLNode::~QXMLNode()
{
	if (handle != INVALID_HANDLE)
		nodeIdAllocator.DisallocateId(handle);
}

int QXMLNode::setNodePointer(XMLNode* nodePointer)
{
	if (nodePointer != NULL && isCreated())
	{
		this->node = nodePointer;

		return 1;
	}

	return 0;
}

QXMLAttribute::QXMLAttribute(QXMLNode* node) : qxmlNode(NULL), attribute(NULL), element(NULL)
{
	if (node->isCreated())
	{
		handle = attributeIdAllocator.AllocateId();

		if (handle != INVALID_HANDLE)
		{
			this->qxmlNode = node;
			created = true;
		}
	}
}

QXMLAttribute::~QXMLAttribute()
{
	if (handle != INVALID_HANDLE)
		attributeIdAllocator.DisallocateId(handle);
}

int QXMLAttribute::setAttributePointer(const XMLAttribute* attributePointer)
{
	if (attributePointer != NULL && isCreated())
	{
		this->attribute = const_cast<XMLAttribute*>(attributePointer);

		return 1;
	}

	return 0;
}

int QXMLAttribute::updateElement()
{
	if (!isCreated() || qxmlNode == NULL)
		return 0;

	XMLNode* node = qxmlNode->getNode();

	if (node == NULL)
		return 0;

	XMLElement* sourceElement = node->ToElement();

	if (sourceElement == NULL)
		return 0;
	
	element = sourceElement;

	return 1;
}

int QXMLHandler::createDocument(const std::string& path)
{
	QXMLDocument* document = new QXMLDocument(path);

	DEBUG << "µµÅ¥¸àÆ® °´Ã¼ »ý¼º µÊ." << std::endl;

	if (!document->isCreated())
	{
		delete document;

		return INVALID_HANDLE;
	}

	DEBUG << "µµÅ¥¸àÆ® °´Ã¼ ÄÁÅ×ÀÌ³Ê¿¡ ÀÔ·Â." << std::endl;

	int handle = document->getHandle();

	documentList.insert(std::unordered_map<int, QXMLDocument*>::value_type(handle, document));

	return handle;
}

QXMLDocument* QXMLHandler::getDocumentByHandle(int handle)
{
	if (handle != INVALID_HANDLE)
	{
		std::unordered_map<int, QXMLDocument*>::iterator it = documentList.find(handle);

		if (it != documentList.end())
			return it->second;
	}

	return NULL;
}

int QXMLHandler::destroyDocument(const QXMLDocument* document)
{
	if (document != NULL && document->isCreated())
	{
		int handle = document->getHandle();

		if (handle != INVALID_HANDLE)
			documentList.erase(handle);

		delete document;

		typedef std::unordered_map<int, QXMLNode*>::iterator nodeIterType;

		for (nodeIterType it = nodeList.begin(); it != nodeList.end(); ++it)
		{
			if (it->second->getQXMLDocument() == document)
				destroyNode(it->second);
		}
	}

	return 0;
}

int QXMLHandler::createNode(QXMLDocument* document)
{
	QXMLNode* node = new QXMLNode(document);

	if (!node->isCreated())
	{
		delete node;

		return INVALID_HANDLE;
	}

	int handle = node->getHandle();

	nodeList.insert(std::unordered_map<int, QXMLNode*>::value_type(handle, node));

	return handle;
}

int QXMLHandler::createNode(const QXMLNode* sourceNode)
{
	QXMLNode* node = new QXMLNode(sourceNode);

	if (!node->isCreated())
	{
		delete node;

		return INVALID_HANDLE;
	}

	int handle = node->getHandle();

	nodeList.insert(std::unordered_map<int, QXMLNode*>::value_type(handle, node));

	return handle;
}

QXMLNode* QXMLHandler::getNodeByHandle(int handle)
{
	if (handle != INVALID_HANDLE)
	{
		std::unordered_map<int, QXMLNode*>::iterator it = nodeList.find(handle);

		if (it != nodeList.end())
			return it->second;
	}

	return NULL;
}

int QXMLHandler::destroyNode(QXMLNode* node)
{
	if (node != NULL && node->isCreated())
	{
		int handle = node->getHandle();

		if (handle != INVALID_HANDLE)
			nodeList.erase(handle);

		delete node;

		typedef std::unordered_map<int, QXMLAttribute*>::iterator attributeIdItterator;

		for (attributeIdItterator it = attributeList.begin(); it != attributeList.end(); ++it)
		{
			if (it->second->getQXMLNode() == node)
				destroyAttribute(it->second);
		}
	}

	return 0;
}

int QXMLHandler::createAttribute(QXMLNode* node)
{
	QXMLAttribute* attribute = new QXMLAttribute(node);

	if (!attribute->isCreated())
	{
		delete attribute;

		return INVALID_HANDLE;
	}

	int handle = attribute->getHandle();

	attributeList.insert(std::unordered_map<int, QXMLAttribute*>::value_type(handle, attribute));

	return handle;
}

QXMLAttribute* QXMLHandler::getAttributeByHandle(int handle)
{
	if (handle != INVALID_HANDLE)
	{
		std::unordered_map<int, QXMLAttribute*>::iterator it = attributeList.find(handle);

		if (it != attributeList.end())
			return it->second;
	}

	return NULL;
}

int QXMLHandler::destroyAttribute(const QXMLAttribute* attribute)
{
	if (attribute != NULL && attribute->isCreated())
	{
		int handle = attribute->getHandle();

		if (handle != INVALID_HANDLE)
			attributeList.erase(handle);

		delete attribute;

		return 1;
	}

	return 0;
}
