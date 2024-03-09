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
        virtual void draw(int width, int height, int xOffset, int yOffset) = 0;
    };

    struct Block : Element
    {
        Element::Type type() const override
        {
            return Element::Block;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct Select : Element
    {
        Element::Type type() const override
        {
            return Element::Select;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct If : Element
    {
        Element::Type type() const override
        {
            return Element::If;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct For : Element
    {
        Element::Type type() const override
        {
            return Element::For;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct Connect : Element
    {
        Element::Type type() const override
        {
            return Element::Connect;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct Model
    {
        std::vector<std::shared_ptr<Element>> m_elements;

        Model()
        {
            m_elements.push_back(std::make_shared<If>());
            m_elements.push_back(std::make_shared<For>());
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