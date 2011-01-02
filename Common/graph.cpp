#include "graph.h"

Graph::Graph()
{
    m_isFirstNode = true;
    m_lastNode = 0;
}

void Graph::deleteArc(GraphArc *arc)
{
    emit graphItemDeleted(arc);
    delete arc;
}

void Graph::addNode(QPointF &point, int floor)
{
    bool isNodeCreated = false;
    GraphNode *node;
    for (int i = 0; i != m_freeNodes.size(); i++)
        if (m_freeNodes.at(i)->pos() == point)
        {
            isNodeCreated = true;
            if (!m_isFirstNode)
            {
                node = m_freeNodes[i];
                m_freeNodes.remove(i);
            }
        }
//    m_nodesCoordinates.clear();
    if (m_lastNode)
    {
        GraphNode *n;
        QStack<GraphNode*> stk;
        QVector<GraphNode*> vec;
        stk.push(m_lastNode);
        while (!stk.isEmpty())
        {
            n = stk.pop();
            vec.append(n);
            if (n->pos() == point)
                node = n;
            int size = n->arcsNumber();
            for (int i = 0; i !=size; i++)
            {
                GraphNode *next = n->adjacentNode(n->arc(i));
                if (!vec.contains(next))
                    stk.push(next);
            }
        }
    }
//    return m_nodesCoordinates;
    if (!isNodeCreated)
        node = new GraphNode(point, floor);
    emit graphItemAdded(node);
    if (!m_isFirstNode)
    {
        GraphArc *arc = new GraphArc(m_lastNode, node);
        m_lastNode->addArc(arc);
        node->addArc(arc);
        emit graphItemAdded(arc);
    }
    else
        m_isFirstNode = false;
    m_lastNode = node;
//    nodesCoordinates();
//    emit graphNodesChanged(m_nodesCoordinates, floor);
}

GraphNode* Graph::lastNode()
{
    return m_lastNode;
}

void Graph::deleteNode(GraphNode *node)
{
    for (int i = 0; i != node->arcsNumber(); i++)

    emit graphItemDeleted(node);
    delete node;
}

//void Graph::nodesCoordinates()
//{
//    m_nodesCoordinates.clear();
//    GraphNode *node;
//    QStack<GraphNode*> stk;
//    stk.push(m_lastNode);
//    while (!stk.isEmpty())
//    {
//        node = stk.pop();
//        m_nodesCoordinates.append(new QPointF(node->pos()));
//        int size = node->arcsNumber();
//        for (int i = 0; i !=size; i++)
//            stk.push(node->adjacentNode(node->arc(i)));
//    }
////    return m_nodesCoordinates;
//}

void Graph::setVisible(bool visible)
{
    for (int i = 0; i != m_freeNodes.size(); i++)
        m_freeNodes[i]->setVisible(visible);

//    GraphNode *node;
//    QStack<GraphNode*> stk;
//    stk.push(m_node);
//    while (!stk.isEmpty())
//    {
//        node = stk.pop();
//        node->setVisible(visible);
//        int size = node->areasNumber();
//        for (int i = 0; i !=size; i++)
//            stk.push(node->area(i));
//    }
}

void Graph::startAnew()
{
    m_isFirstNode = true;
}
