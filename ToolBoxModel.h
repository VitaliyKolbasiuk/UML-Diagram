#pragma once

#include <vector>
#include <memory>

namespace ToolBoxModel
{
    struct Element
    {
        enum Type
        {
            Function,
            Block,
            If,
            For,
            Connector
        };

        virtual Type type() const = 0;
        virtual void draw(int width, int height, int xOffset, int yOffset) = 0;
    };

    struct Function : Element
    {
        Element::Type type() const override
        {
            return Element::Function;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct Block : Element
    {
        Element::Type type() const override
        {
            return Element::Block;
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
            return Element::Connector;
        }

        void draw(int width, int height, int xOffset, int yOffset) override;
    };

    struct Model
    {
        std::vector<std::shared_ptr<Element>> m_elements;

        Model()
        {
            m_elements.push_back(std::make_shared<Function>());
            m_elements.push_back(std::make_shared<Block>());
            m_elements.push_back(std::make_shared<If>());
            m_elements.push_back(std::make_shared<For>());
        }
    };
    inline Model gModel;
}
