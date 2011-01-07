#include "graph.h"

Graph::Graph()
{
    m_lastNode = 0;
}

QDataStream & operator<<(QDataStream &output, const Graph &graph)
{
    QVector<GraphNode*> nodes;
    QVector<GraphArc*> arcs;
    GraphNode *n;
    QStack<GraphNode*> stk;
    for (int i = 0; i < graph.m_startNodes.size(); i++)
    {
        stk.push(graph.m_startNodes.at(i));
        nodes.append(graph.m_startNodes.at(i));
        while (!stk.isEmpty())
        {
            n = stk.pop();
            output << *n  << graph.m_startNodes.contains(n);
            for (int i = 0; i !=n->arcsNumber(); i++)
            {
                GraphNode *next = n->adjacentNode(n->arc(i));
                if (!nodes.contains(next))
                {
                    stk.push(next);
                    nodes.append(next);
                }
            }
        }
    }
    output << GraphNode(QPointF(0, 0), 0) << false; // For marking end of nodes
    for (int i = 0; i != nodes.size(); i++)
        for (int j = 0; j != nodes.at(i)->arcsNumber(); j++)
        {
            GraphArc *arc = nodes.at(i)->arc(j);
            if (!arcs.contains(arc))
            {
                arcs.append(arc);
                output << arc->node1()->uin() << arc->node2()->uin();
            }
        }
    output << 0 << 0;     // For marking end of arcs
    return output;
}

QDataStream & operator>>(QDataStream &input, Graph &graph)
{
    typedef GraphNode* pGraphNode;
    QMap<quint32, pGraphNode> nodes;
    GraphNode *node = new GraphNode();
    bool isStartNode;
    input >> *node >> isStartNode;
    while (node->floor() != 0)
    {
        nodes.insert(node->uin(), node);
        if (isStartNode)
            graph.m_startNodes.append(node);
        emit graph.graphItemAdded(node);
        node = new GraphNode();
        input >> *node >> isStartNode;
    }
    GraphArc *arc;
    quint32 node1uin, node2uin;
    input >> node1uin >> node2uin;
    while (node1uin != 0)
    {
        arc = new GraphArc(nodes[node1uin], nodes[node2uin]);
        emit graph.graphItemAdded(arc);
        input >> node1uin >> node2uin;
    }
    return input;
}

void Graph::deleteArc(GraphArc *arc)
{
    emit graphItemDeleted(arc);
    delete arc;
}

void Graph::addNode(QPointF &point, quint32 floor)
{
    // If doubleclick
    if (m_lastNode)
        if (m_lastNode->pos() == point)
            return;
    // Searching current node between free nodes
    bool nodeIsCreated = false;
    GraphNode *node;
    for (int i = 0; i != m_startNodes.size(); i++)

        if (m_startNodes.at(i)->pos() == point)
        {
            nodeIsCreated = true;
            node = m_startNodes[i];
            break;
        }
    // If current node isn't free, we try to find it between normal nodes
    if (!nodeIsCreated)
    {
        GraphNode *n;
        QStack<GraphNode*> stk;
        QVector<GraphNode*> vec;
        for (int i = 0; i < m_startNodes.size(); i++)
        {
            GraphNode *startNode = m_startNodes.at(i);
            stk.push(startNode);
            vec.append(startNode);
            while (!stk.isEmpty())
            {
                n = stk.pop();
                // Removing surplus nodes
                if (n != startNode)
                {
                    int i = m_startNodes.indexOf(n);
                    if (i > -1)
                        m_startNodes.remove(i);
                }
                if (n->pos() == point)
                {
                    node = n;
                    nodeIsCreated = true;
                    break;
                }
                for (int i = 0; i !=n->arcsNumber(); i++)
                {
                    GraphNode *next = n->adjacentNode(n->arc(i));
                    if (!vec.contains(next))
                    {
                        stk.push(next);
                        vec.append(next);
                    }
                }
            }
        }
    }
    // If it still not found - we create it!
    if (!nodeIsCreated)
    {
        node = new GraphNode(point, floor);
        emit graphItemAdded(node);
    }
    // Adding arc
    if (m_lastNode)
    {
        bool connected = false;
        for (int i = 0; i != node->arcsNumber(); i++)
            if (node->adjacentNode(node->arc(i)) == m_lastNode)
            {
                connected = true;
                break;
            }
        if (!connected)
        {
            GraphArc *arc = new GraphArc(m_lastNode, node);
            emit graphItemAdded(arc);
        }
    }
    else
        m_startNodes.append(node);
    setLastNode(node);
//    nodesCoordinates();
//    emit graphNodesChanged(m_nodesCoordinates, floor);
}

GraphNode* Graph::lastNode()
{
    return m_lastNode;
}

void Graph::deleteNode(GraphNode *node)
{
    if (m_lastNode == node)
    {
        if (node->arcsNumber())
            setLastNode(node->adjacentNode(node->arc(node->arcsNumber() - 1)));
        else
            setLastNode(0);
    }
    int i = m_startNodes.indexOf(node);
    if (i > -1)
        m_startNodes.remove(i);
    GraphNode *adjNode;
    for (int i = 0; i != node->arcsNumber(); i++)
    {
        adjNode = node->adjacentNode(node->arc(i));
        if (!m_startNodes.contains(adjNode))
            m_startNodes.append(adjNode);
    }
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
//    for (int i = 0; i != m_startNodes.size(); i++)
//        m_startNodes[i]->setVisible(visible);

    GraphNode *n;
    QStack<GraphNode*> stk;
    QVector<GraphNode*> vec;
    for (int i = 0; i < m_startNodes.size(); i++)
    {
        GraphNode *startNode = m_startNodes.at(i);
        stk.push(startNode);
        vec.append(startNode);
        while (!stk.isEmpty())
        {
            n = stk.pop();
            // Removing surplus nodes
            if (n != startNode)
            {
                int i = m_startNodes.indexOf(n);
                if (i > -1)
                    m_startNodes.remove(i);
            }
            n->setVisible(visible);
            int size = n->arcsNumber();
            for (int i = 0; i !=size; i++)
            {
                n->arc(i)->setVisible(visible);
                GraphNode *next = n->adjacentNode(n->arc(i));
                if (!vec.contains(next))
                {
                    stk.push(next);
                    vec.append(next);
                }
            }
        }
    }
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
    m_lastNode = 0;
}

void Graph::setLastNode(GraphNode *node)
{
    m_lastNode = node;
    emit lastNodeChanged(m_lastNode);
}
