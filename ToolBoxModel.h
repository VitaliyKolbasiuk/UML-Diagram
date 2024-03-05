#pragma once

#include <vector>
#include <memory>

namespace ToolBoxModel
{
    struct Element
    {
        enum Type
        {
            Block,
            Select,
            If,
            For,
            Connect
        };

        virtual Type type() const = 0;
    };

    struct Block : Element
    {
        Element::Type type() const override
        {
            return Element::Block;
        }
    };

    struct Select : Element
    {
        Element::Type type() const override
        {
            return Element::Select;
        }
    };

    struct If : Element
    {
        Element::Type type() const override
        {
            return Element::If;
        }
    };


    struct Model
    {
        std::vector<std::shared_ptr<Element>> m_elements;

        Model()
        {
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<Block>());
        }
    };
    inline Model gModel;
}