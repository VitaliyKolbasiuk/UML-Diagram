#include "Python.h"
#include "Diagram.h"

static const DiagramElement* findEndOfIf(const Diagram& diagram, const DiagramElement* element);
static const DiagramElement* findNextElement(const Diagram& diagram, const DiagramElement* element);
static const DiagramElement* findEndOfForBody(const Diagram& diagram, const DiagramElement* element);

std::shared_ptr<python_code::Element> python_code::elementToPseudoCode(const Diagram& diagram, const DiagramElement* diagramElement)
{
    switch(diagramElement->m_type)
    {
        case ToolBoxModel::Element::Block:
        {
            return std::make_shared<Block>();
        }
        case ToolBoxModel::Element::If:
        {
            ElementList yesElementList;
            ElementList noElementList;
            const DiagramElement* endOfIf = findEndOfIf(diagram, diagramElement);
            if (endOfIf != nullptr)
            {
                const Connector* yesConnector = diagram.getYesOutputConnector(diagramElement);
                for (const auto* element = yesConnector->back().m_element; element != endOfIf; )
                {
                    element = findNextElement(diagram, element);
                    yesElementList.emplace_back(elementToPseudoCode(diagram, element));
                }

                const Connector* noConnector = diagram.getNoOutputConnector(diagramElement);
                for (const auto* element = noConnector->back().m_element; element != endOfIf; )
                {
                    element = findNextElement(diagram, element);
                    noElementList.emplace_back(elementToPseudoCode(diagram, element));
                }
            }
            return std::make_shared<If>("", yesElementList, noElementList);
        }
        case ToolBoxModel::Element::For:
        {
            ElementList body;
            const DiagramElement* endOfBody = findEndOfForBody(diagram, diagramElement);
            if (endOfBody != nullptr)
            {
                const Connector* bodyConnector = diagram.getBodyOutputConnector(diagramElement);
                for (const auto* bodyElement = bodyConnector->back().m_element; bodyElement != nullptr && bodyElement != endOfBody; )
                {
                    body.emplace_back(elementToPseudoCode(diagram, bodyElement));
                    bodyElement = findNextElement(diagram, bodyElement);
                }
            }
            return std::make_shared<For>("", body);
        }
    }
    return {};
}

const DiagramElement* findEndOfForBody(const Diagram& diagram, const DiagramElement* element)
{
    const Connector* bodyConnector = diagram.getBodyOutputConnector(element);
    if (bodyConnector == nullptr)
    {
        return nullptr;
    }

    const DiagramElement* bodyElement = bodyConnector->back().m_element;
    if (bodyElement == nullptr)
    {
        return nullptr;
    }

    for ( ;bodyElement != nullptr; )
    {
        if (bodyElement == element)
        {
            return bodyElement;
        }
        bodyElement = findNextElement(diagram, bodyElement);
    }
    return nullptr;
}

const DiagramElement* findEndOfIf(const Diagram& diagram, const DiagramElement* element)
{
    const Connector* yesConnector = diagram.getYesOutputConnector(element);
    const Connector* noConnector = diagram.getNoOutputConnector(element);
    if (yesConnector == nullptr || noConnector == nullptr)
    {
        return nullptr;
    }

    const DiagramElement* yesElement = yesConnector->back().m_element;
    const DiagramElement* noElement = noConnector->back().m_element;
    if (yesElement == nullptr || noElement == nullptr)
    {
        return nullptr;
    }

    const DiagramElement* yesElementIt = yesElement;
    const DiagramElement* noElementIt = noElement;
    for (;;)
    {
        if (yesElementIt == nullptr && noElementIt == nullptr)
        {
            return nullptr;
        }

        // Find intersection of yes branch with no branch
        for (const DiagramElement* yesElementIt2 = yesElement; ; )
        {
            if (yesElementIt2 == nullptr)
            {
                break;
            }
            const auto* noElementIt2 = noElement;
            if (yesElementIt2 == noElementIt2)
            {
                return yesElementIt2;
            }
            do {
                // Shift no iterator
                noElementIt2 = findNextElement(diagram, noElementIt2);
                if (yesElementIt2 == noElementIt2)
                {
                    return yesElementIt2;
                }
            } while (noElementIt2 != nullptr && noElementIt2 != noElementIt);

            // Reached the end of yes branch
            if (yesElementIt2 == yesElementIt)
            {
                break;
            }

            // Shift yes iterator
            yesElementIt2 = findNextElement(diagram, yesElementIt2);
        }
        // Increase branches
        yesElementIt = findNextElement(diagram, yesElementIt);
        noElementIt = findNextElement(diagram, noElementIt);
    }
}

const DiagramElement* findNextElement(const Diagram& diagram, const DiagramElement* element)
{
    if (element == nullptr)
    {
        return nullptr;
    }
    switch(element->m_type)
    {
        case ToolBoxModel::Element::For:
        case ToolBoxModel::Element::Block:
        {
            const Connector* connector = diagram.getOutputConnector(element);
            if (connector == nullptr)
            {
                return nullptr;
            }
            return connector->back().m_element;
        }
        case ToolBoxModel::Element::If:
        {
            return findEndOfIf(diagram, element);
        }
    }
    return nullptr;
}



python_code::Code python_code::diagramToPythonPseudoCode(const Diagram& diagram)
{
    Code pythonCode;

    for (const auto* element = diagram.findFirstElement(); element != nullptr; )
    {
        std::shared_ptr<Element> next = elementToPseudoCode(diagram, element);
        pythonCode.emplace_back(next);
        element = findNextElement(diagram, element);
    }
    return pythonCode;
}
