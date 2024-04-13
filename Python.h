    #pragma once

#include <string>
#include <vector>
#include <memory>

struct DiagramElement;
class Diagram;
namespace python_code {

    struct Element
    {
        virtual void generate( std::ostream&, int indent ) const = 0;
    };

    using ElementList = std::vector<std::shared_ptr<Element>>;
    using Code = ElementList;

    struct Func : public Element
    {
        std::string              m_name;
        std::vector<std::string> m_argList;
        ElementList              m_body;

        virtual void generate( std::ostream& output, int indent ) const override
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
                body->generate( output, indent+1 );
            }

            output << std::endl;
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

        virtual void generate( std::ostream& output, int indent ) const override
        {
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << "if " << m_condition << std::endl;
            for (const auto& element : m_yesList)
            {
                element->generate(output, indent + 4);
            }
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << "else" << std::endl;
            for (const auto& element : m_noList)
            {
                element->generate(output, indent + 4);
            }
        }
    };

    struct For : public Element
    {
        std::string m_condition;
        ElementList m_body;

        For (const std::string& condition, const ElementList& body) : m_condition(condition), m_body(body){}

        virtual void generate( std::ostream& output, int indent ) const override
        {
            for (int i = 0; i < indent; ++i)
            {
                output << ' ';
            }
            output << "for " << m_condition << std::endl;
            for (const auto& element : m_body)
            {
                element->generate(output, indent + 4);
            }
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

        virtual void generate( std::ostream& output, int indent ) const override
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
    };

    Code diagramToPythonPseudoCode(const Diagram& diagram);
    std::shared_ptr<Element> elementToPseudoCode(const Diagram& diagram, const DiagramElement* diagramElement);
}
