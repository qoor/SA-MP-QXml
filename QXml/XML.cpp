#include "stdafx.h"
#include "XML.h"
#include "Debug.h"

int QXMLHandler::docCount = -1;
int QXMLHandler::nodeCount = -1;
int QXMLHandler::attributeCount = -1;

extern QXMLHandler* gXMLHandler;

QXMLHandler::QXMLHandler()
{

}

QXMLHandler::~QXMLHandler()
{

}

QXMLHandler::QXMLDocument::QXMLDocument(const std::string& path)
{
	std::string realPath = "scriptfiles/";

	realPath.append(path);

	this->path = path;
	this->doc = new XMLDocument();

	if (doc->LoadFile(realPath.c_str()) == XML_SUCCESS)
	{
		success = true;
		handle = (++docCount);
	}
}

QXMLHandler::QXMLDocument::~QXMLDocument()
{
	if (isOpened())
	{
		if (handle == docCount)
			--docCount;
		
		delete doc;
	}
}

int QXMLHandler::QXMLDocument::isOpened()
{
	return success;
}

int QXMLHandler::QXMLDocument::getHandle()
{
	if (success)
		return handle;
	
	return -1;
}

XMLDocument* QXMLHandler::QXMLDocument::getDocument()
{
	if (success)
		return doc;

	return NULL;
}

QXMLHandler::QXMLNode::QXMLNode(QXMLDocument *doc)
{
	if (doc != NULL)
	{
		this->doc = doc;
		success = true;
		handle = (++nodeCount);
		node = NULL;
	}
}

QXMLHandler::QXMLNode::~QXMLNode()
{
	if (isCreated())
	{
		if (handle == nodeCount)
			--nodeCount;
	}
}

int QXMLHandler::QXMLNode::isCreated()
{
	return success;
}

int QXMLHandler::QXMLNode::getHandle()
{
	if (isCreated())
		return handle;
	
	return -1;
}

XMLNode* QXMLHandler::QXMLNode::getNode()
{
	if (isCreated())
		return node;

	return NULL;
}

XMLElement* QXMLHandler::QXMLNode::getNodeElement()
{
	if (node != NULL && isCreated())
	{
		XMLElement* element = node->ToElement();

		return element;
	}

	return NULL;
}

int QXMLHandler::QXMLNode::setNode(XMLNode* node)
{
	DEBUG << "노드 id " << getHandle() << " 다음 노드 지정 시작. 기존 포인터: " << reinterpret_cast<int*>(this->node) << std::endl;

	if (node != NULL && isCreated())
	{
		this->node = node;

		DEBUG << "노드 id " << getHandle() << " 다음 노드 지정 성공. 포인터: " << reinterpret_cast<int*>(node) << std::endl;

		return 1;
	}

	DEBUG << "노드 id " << getHandle() << " 다음 노드 지정 실패. 포인터: " << reinterpret_cast<int*>(node) << std::endl;

	return 0;
}

XMLDocument* QXMLHandler::QXMLNode::getDocument()
{
	if (isCreated() && doc->isOpened())
		return doc->getDocument();

	return NULL;
}

QXMLHandler::QXMLDocument* QXMLHandler::QXMLNode::getQXMLDocument()
{
	if (isCreated() && doc->isOpened())
		return doc;
	
	return NULL;
}

QXMLHandler::QXMLAttribute::QXMLAttribute(QXMLHandler::QXMLNode* node)
{
	if (node != NULL && node->isCreated())
	{
		success = true;
		handle = (++attributeCount);
		element = NULL;
		this->node = node;
	}
}

QXMLHandler::QXMLAttribute::~QXMLAttribute()
{
	if (isCreated())
	{
		if (handle == attributeCount)
			--attributeCount;
	}
}

int QXMLHandler::QXMLAttribute::isCreated()
{
	return success;
}

int QXMLHandler::QXMLAttribute::getHandle()
{
	if (isCreated())
		return handle;

	return -1;
}

QXMLHandler::QXMLNode* QXMLHandler::QXMLAttribute::getNode()
{
	if (isCreated() && node->isCreated())
		return node;

	return NULL;
}

XMLElement* QXMLHandler::QXMLAttribute::getElement()
{
	if (!isCreated())
		return NULL;

	return element;
}

int QXMLHandler::QXMLAttribute::updateElement()
{
	if (!isCreated() || node == NULL || !node->isCreated())
		return 0;

	element = node->getNodeElement();

	return 1;
}

const XMLAttribute* QXMLHandler::QXMLAttribute::getAttribute()
{
	if (isCreated())
		return attribute;

	return 0;
}

int QXMLHandler::QXMLAttribute::setAttribute(const XMLAttribute* attribute)
{
	DEBUG << "애트리뷰트 id " << getHandle() << " 다음 애트리뷰트 지정 시작. 기존 포인터: " << reinterpret_cast<const int*>(this->attribute) << std::endl;

	if (attribute == NULL || !isCreated())
	{
		DEBUG << "애트리뷰트 id " << getHandle() << " 다음 애트리뷰트 지정 실패. 포인터: " << reinterpret_cast<const int*>(attribute) << std::endl;

		return 0;
	}

	this->attribute = attribute;

	DEBUG << "애트리뷰트 id " << getHandle() << " 다음 애트리뷰트 지정 성공. 포인터: " << reinterpret_cast<const int*>(this->attribute) << std::endl;

	return 1;
}

int QXMLHandler::openDocument(const std::string& path)
{
	QXMLDocument* doc = new QXMLDocument(path);

	if (doc->isOpened())
	{
		this->docs.push_back(doc);

		return doc->getHandle();
	}
	else
	{
		delete doc;

		return -1;
	}
}

int QXMLHandler::closeDocument(int docHandler)
{
	if (docHandler == -1)
		return 0;
	
	for (unsigned int i = 0, size = attributes.size(); i < size; ++i)
	{
		if (attributes[i]->getNode()->getQXMLDocument()->getHandle() == docHandler)
		{
			delete attributes[i];

			attributes.erase(i + attributes.begin());
		}
	}

	for (unsigned int i = 0, size = nodes.size(); i < size; ++i)
	{
		if (nodes[i]->getQXMLDocument()->getHandle() == docHandler)
		{
			delete nodes[i];

			nodes.erase(i + nodes.begin());
		}
	}

	for (unsigned int i = 0, size = docs.size(); i < size; ++i)
	{
		if (docs[i]->getHandle() == docHandler)
		{
			delete docs[i];

			docs.erase(i + docs.begin());

			break;
		}
	}

	return 1;
}

QXMLHandler::QXMLDocument* QXMLHandler::getDocument(int handle)
{
	if (handle != -1)
	{
		for (unsigned int i = 0; i < docs.size(); ++i)
		{
			if (docs[i]->getHandle() == handle)
				return docs[i];
		}
	}

	return NULL;
}

int QXMLHandler::createNode(QXMLHandler::QXMLDocument* doc)
{
	if (doc->getDocument() == NULL)
		return 0;
	
	QXMLHandler::QXMLNode* node = new QXMLHandler::QXMLNode(doc);

	if (node->isCreated())
	{
		this->nodes.push_back(node);

		return node->getHandle();
	}
	else
	{
		delete node;

		return -1;
	}
}

QXMLHandler::QXMLNode* QXMLHandler::getNode(int handle)
{
	if (handle != -1)
	{
		for (unsigned int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i]->getHandle() == handle)
				return nodes[i];
		}
	}

	return NULL;
}

int QXMLHandler::createAttribute(QXMLHandler::QXMLNode* node)
{
	QXMLHandler::QXMLAttribute* attribute = new QXMLHandler::QXMLAttribute(node);
	
	DEBUG << "애트리뷰트 힙 공간 할당 됨" << std::endl;

	if (attribute->isCreated())
	{
		this->attributes.push_back(attribute);

		return attribute->getHandle();
	}
	else
		delete attribute;

	return -1;
}

QXMLHandler::QXMLAttribute* QXMLHandler::getAttribute(int handle)
{
	if (handle != -1)
	{
		for (unsigned int i = 0; i < attributes.size(); ++i)
		{
			if (attributes[i]->getHandle() == handle)
				return attributes[i];
		}
	}

	return NULL;
}
