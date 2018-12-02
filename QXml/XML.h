#pragma once

#include "../TinyXML2/tinyxml2.h"
#include "../AMX/amx/amx.h"
#include "../AMX/plugincommon.h"

#include <vector>

using namespace tinyxml2;

class QXMLHandler
{
public:
	static int docCount;
	static int nodeCount;
	static int attributeCount;

	QXMLHandler();
	~QXMLHandler();

	class QXMLDocument
	{
	private:
		bool success;
		std::string path;
		XMLDocument *doc;
		int handle;

	public:
		QXMLDocument(const std::string& path);
		~QXMLDocument();
		int isOpened();
		int getHandle();
		XMLDocument* getDocument();
	};

	class QXMLNode
	{
	private:
		QXMLDocument* doc;
		XMLNode* node;
		bool success;
		int handle;

	public:
		QXMLNode(QXMLDocument *doc);
		~QXMLNode();
		int isCreated();
		int getHandle();
		XMLDocument* getDocument();
		QXMLDocument* getQXMLDocument();
		XMLNode* getNode();
		XMLElement* getNodeElement();
		int setNode(XMLNode* node);
	};

	class QXMLAttribute
	{
	private:
		//QXMLNode* node;
		const XMLAttribute* attribute;
		XMLElement* element;
		QXMLNode* node;
		bool success;
		int handle;

	public:
		QXMLAttribute(QXMLNode* node);
		~QXMLAttribute();
		int isCreated();
		int getHandle();
		QXMLNode* getNode();
		XMLElement* getElement();
		int updateElement();
		const XMLAttribute* getAttribute();
		int setAttribute(const XMLAttribute* attribute);
	};

private:
	std::vector<QXMLHandler::QXMLDocument*> docs;
	std::vector<QXMLHandler::QXMLNode*> nodes;
	std::vector<QXMLHandler::QXMLAttribute*> attributes;

public:
	int openDocument(const std::string& path);
	int closeDocument(int docHandler);
	int createNode(QXMLHandler::QXMLDocument* doc);
	int createAttribute(QXMLHandler::QXMLNode* node);

	QXMLDocument* getDocument(int handle);
	QXMLNode* getNode(int handle);
	QXMLAttribute* getAttribute(int handle);
};
