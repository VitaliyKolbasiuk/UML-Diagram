#include "Python.h"
#include "Diagram.h"

python_code::Code python_code::elementToPseudoCode(const Diagram& diagram, const DiagramElement* element)
{
    // switch(element->type)
    // {
    //     case ToolBoxModel::Element::Block:
    //     {
    //         const Connector* connector = diagram.getOutputConnector(element);
    //         python_code::Element* outputElement = nullptr;
    //         if (connector != nullptr && connector->back().m_element)
    //         {
    //             outputElement = elementToPseudoCode(diagram, connector->back().m_element);
    //         }
    //         return new python_code::Block{""};
    //     }
    //     case ToolBoxModel::Element:::If:
    //     {
    //         const Connector* yesConnector = diagram.getYesOutputConnector(element);
    //         const Connector* noConnector = diagram.getNoOutputConnector(element);
    //         python_code::Element* yesElement = nullptr;
    //         if (yesConnector && yesConnector->back().m_element)
    //         {
    //             yesElement = elementToPseudoCode(diagram, yesConnector->back().m_element);
    //         }
    //         python_code::Element* noElement = nullptr;
    //         if (noConnector && noConnector->back().m_element)
    //         {
    //             noElement = elementToPseudoCode(diagram, noConnector->back().m_element);
    //         }
    //         return new python_code::If{"", yesElement, noElement};
    //     }
    // }
}

python_code::Code python_code::diagramToPythonPseudoCode(const Diagram& diagram)
{
    const DiagramElement* element = diagram.findFirstElement();
    if (element == nullptr)
    {
        return Code{};
    }
    return elementToPseudoCode(diagram, element);
}
