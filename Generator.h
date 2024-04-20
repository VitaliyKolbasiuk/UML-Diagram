#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

struct DiagramElement;
class Diagram;
namespace generate_code {

    enum Language
    {
        python,
        cpp
    };

    inline Language gCurrentLanguage = cpp;

    struct Element
    {
        virtual void generatePython( std::ostream&, int indent ) const = 0;
        virtual void generateCpp( std::ostream&, int indent ) const = 0;

        void printText( std::ostream& output, const std::string& text, int indent) const
        {
            for(int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << text << std::endl;
        }
    };

    using ElementList = std::vector<std::shared_ptr<Element>>;
    using Code = ElementList;

    struct Func : public Element
    {
        std::string              m_name;
        std::vector<std::string> m_argList;
        ElementList              m_body;

        virtual void generatePython( std::ostream& output, int indent ) const override
        {
            output << m_name << "(";
            for( const auto& arg: m_argList )
            {
                output << " " << arg << ",";
            }
            if ( !m_argList.empty() )
            {
                output << " ";
            }
            output << "):" << std::endl;

            // Body
            for( const auto& body: m_body )
            {
                body->generatePython( output, indent+1 );
            }

            output << std::endl;
        }

        virtual void generateCpp( std::ostream& output, int indent ) const override
        {

        }
    };

    struct If : public Element
    {
        std::string m_condition;
        ElementList m_yesList;
        ElementList m_noList;

        If (const std::string& condition, const ElementList& yes, const ElementList& no) : m_condition(condition),
                                                                                           m_yesList(yes),
                                                                                           m_noList(no)
        {}

        virtual void generatePython( std::ostream& output, int indent ) const override
        {
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }

            output << "if " << m_condition << std::endl;
            for (const auto& element : m_yesList)
            {
                element->generatePython(output, indent + 4);
            }
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }

            output << "else" << std::endl;
            for (const auto& element : m_noList)
            {
                element->generatePython(output, indent + 4);
            }
        }

        virtual void generateCpp( std::ostream& output, int indent ) const override
        {
            printText(output, "if( " + m_condition + " )", indent);
            printText(output, "{", indent);
            for (const auto& element : m_yesList)
            {
                element->generateCpp(output, indent + 4);
            }
            printText(output, "}", indent);
            printText(output, "else", indent);
            printText(output, "{", indent);
            for (const auto& element : m_noList)
            {
                element->generateCpp(output, indent + 4);
            }
            printText(output, "}", indent);
        }
    };

    struct For : public Element
    {
        std::string m_condition;
        ElementList m_body;

        For (const std::string& condition, const ElementList& body) : m_condition(condition), m_body(body){}

        virtual void generatePython( std::ostream& output, int indent ) const override
        {
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << "for " << m_condition << std::endl;
            for (const auto& element : m_body)
            {
                element->generatePython(output, indent + 4);
            }
        }

        virtual void generateCpp( std::ostream& output, int indent ) const override
        {
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << "for( " << m_condition << " )" << std::endl;
            output << '{' << std::endl;
            for (const auto& element : m_body)
            {
                element->generateCpp(output, indent + 4);
            }
            output << '}' << std::endl;
        }
    };

    struct Block : public Element
    {
        std::vector<std::string> m_body;

        Block() {}
        Block(const std::string& body)
        {
            m_body.emplace_back(body);
        }

        virtual void generatePython( std::ostream& output, int indent ) const override
        {
            for(int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << "BLOCK " << std::endl;
            for(const auto& line : m_body)
            {
                output << line << std::endl;
            }
        }

        virtual void generateCpp( std::ostream& output, int indent ) const override
        {
            printText(output, "BLOCK", indent);
            for(const auto& line : m_body)
            {
                printText(output, line, indent);
            }
        }
    };

    Code diagramToPythonPseudoCode(const Diagram& diagram);
    std::shared_ptr<Element> elementToPseudoCode(const Diagram& diagram, const DiagramElement* diagramElement);
}
