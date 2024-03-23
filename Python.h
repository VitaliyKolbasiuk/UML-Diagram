#pragma once

namespace code {

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

        virtual void generate( std::ostream& output, int indent ) const override
        {
        }
    };

    struct For : public Element
    {
        std::string m_condition;
        ElementList m_body;

        virtual void generate( std::ostream& output, int indent ) const override
        {
        }
    };

    struct Block : public Element
    {
        std::vector<std::string> m_body;

        virtual void generate( std::ostream& output, int indent ) const override
        {
        }
    };
}
