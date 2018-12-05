#pragma once

#include "../TinyXML2/tinyxml2.h"
#include "../AMX/amx/amx.h"
#include "../AMX/plugincommon.h"

#include <unordered_map>
#include <string>

#define INVALID_HANDLE -1

using namespace tinyxml2;

class IdAllocator;

class QXMLBase
{
protected:
	bool created;
	int handle;

public:
	QXMLBase() : created(false), handle(INVALID_HANDLE) {}
	~QXMLBase() {}
	inline int isCreated() const { return created; }
	inline int getHandle() const { return handle; }
};

class QXMLDocument : public QXMLBase
{
private:
	std::string path;
	XMLDocument* document;

public:
	static IdAllocator documentIdAllocator;
	
	QXMLDocument(const std::string& path);
	~QXMLDocument();
	inline XMLDocument* getDocument() { return document; }
};

class QXMLNode : public QXMLBase
{
private:
	QXMLDocument* qxmlDocument;
	XMLNode* node;

public:
	static IdAllocator nodeIdAllocator;
	
	QXMLNode(QXMLDocument* document);
	QXMLNode(const QXMLNode* node);
	~QXMLNode();
	inline XMLNode* getNode() { return node; }
	inline QXMLDocument* getQXMLDocument() { return qxmlDocument; }
	int setNodePointer(XMLNode* nodePointer);
};

class QXMLAttribute : public QXMLBase
{
private:
	QXMLNode* qxmlNode;
	XMLAttribute* attribute;
	XMLElement* element;

public:
	static IdAllocator attributeIdAllocator;

	QXMLAttribute(QXMLNode* node);
	~QXMLAttribute();
	inline QXMLNode* getQXMLNode() { return qxmlNode; }
	inline const XMLAttribute* getAttributePointer() const { return attribute; }
	int setAttributePointer(const XMLAttribute* attributePointer);
	int updateElement();
	inline XMLElement* getElement() { return element; }
};

class QXMLHandler
{
private:
	std::unordered_map<int, QXMLDocument*> documentList;
	std::unordered_map<int, QXMLNode*> nodeList;
	std::unordered_map<int, QXMLAttribute*> attributeList;

public:
	QXMLHandler() {}
	~QXMLHandler() {}

	int createDocument(const std::string& path);
	QXMLDocument* getDocumentByHandle(int handle);
	int destroyDocument(const QXMLDocument* document);
	int createNode(QXMLDocument* document);
	int createNode(const QXMLNode* sourceNode);
	QXMLNode* getNodeByHandle(int handle);
	int destroyNode(QXMLNode* node);
	int createAttribute(QXMLNode* node);
	QXMLAttribute* getAttributeByHandle(int handle);
	int destroyAttribute(const QXMLAttribute* attribute);
};

/*class QXMLNode : public QXMLBase<QXMLNode>
{
	class QXMLDocument;

protected:
	QXMLNode* node;

public:
	static int nodeCount;

	QXMLNode(QXMLDocument* document);
	~QXMLNode();
	XMLElement* getElement();
};

class QXMLAttribute : protected QXMLNode<QXMLAttribute>
{
	class QXMLNode;

private:
	XMLAttribute* attribute;

public:
	static int attributeCount;

	QXMLAttribute(QXMLNode* node);
	~QXMLAttribute();
};*/
