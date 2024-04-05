#include "Python.h"
#include "Diagram.h"

static const DiagramElement* findEndOfIf(const Diagram& diagram, const DiagramElement* element);
static const DiagramElement* findNextElement(const Diagram& diagram, const DiagramElement* element);

std::shared_ptr<python_code::Element> python_code::elementToPseudoCode(const Diagram& diagram, const DiagramElement* diagramElement)
{
    switch(diagramElement->type)
    {
        case ToolBoxModel::Element::Block:
        {
            // const Connector* connector = diagram.getOutputConnector(element);
            // python_code::Element* outputElement = nullptr;
            // if (connector != nullptr && connector->back().m_element)
            // {
            //     outputElement = elementToPseudoCode(diagram, connector->back().m_element);
            // }
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
                for (const auto* element = diagramElement; element != endOfIf; )
                {
                    const DiagramElement* nextElement = findNextElement(diagram, yesConnector->back().m_element);
                    yesElementList.emplace_back(elementToPseudoCode(diagram, nextElement));
                    element = nextElement;
                }

                const Connector* noConnector = diagram.getNoOutputConnector(diagramElement);
                for (const auto* element = diagramElement; element != endOfIf; )
                {
                    const DiagramElement* nextElement = findNextElement(diagram, noConnector->back().m_element);
                    noElementList.emplace_back(elementToPseudoCode(diagram, nextElement));
                    element = nextElement;
                }
            }
            return std::make_shared<If>("", yesElementList, noElementList);
        }
    }
    return {};
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
        // increase branches
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
    switch(element->type)
    {
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

    for (const auto* element = diagram.findFirstElement(); element != nullptr;)
    {
        std::shared_ptr<Element> next = elementToPseudoCode(diagram, element);
        pythonCode.emplace_back(next);
        element = findNextElement(diagram, element);
    }
    return pythonCode;
}
