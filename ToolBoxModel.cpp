#include "ToolBoxModel.h"
#include "DrawElements.h"

void ToolBoxModel::Block::draw(int width, int height, int xOffset, int yOffset)
{
    drawElement(*this, width, height, xOffset, yOffset);
}

void ToolBoxModel::Select::draw(int width, int height, int xOffset, int yOffset)
{
    drawElement(*this, width, height, xOffset, yOffset);
}

void ToolBoxModel::If::draw(int width, int height, int xOffset, int yOffset)
{
    drawElement(*this, width, height, xOffset, yOffset);
}

void ToolBoxModel::For::draw(int width, int height, int xOffset, int yOffset)
{
    drawElement(*this, width, height, xOffset, yOffset);
}

void ToolBoxModel::Connect::draw(int width, int height, int xOffset, int yOffset)
{
    drawElement(*this, width, height, xOffset, yOffset);
}