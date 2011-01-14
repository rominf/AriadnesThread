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
    while (node->floorUin() != 0)
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

void Graph::addNode(QPointF point, quint32 floorUin)
{
    // If doubleclick
    if (m_lastNode)
        if (m_lastNode->pos() == point)
            return;

    bool nodeIsCreated = false;
    GraphNode *node;

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
            if ((n->pos() == point) & (n->floorUin() == floorUin))
            {
                node = n;
                nodeIsCreated = true;
                break;
            }
            for (int i = 0; i != n->arcsNumber(); i++)
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
//    }
    // If node isn't created - we create it!
    if (!nodeIsCreated)
    {
        node = new GraphNode(point, floorUin);
        emit graphItemAdded(node);

        // Checking: is one of arc's contain this node
        stk.clear();
        vec.clear();
        for (int i = 0; i < m_startNodes.size(); i++)
        {
            GraphNode *startNode = m_startNodes.at(i);
            stk.push(startNode);
            vec.append(startNode);
            while (!stk.isEmpty())
            {
                n = stk.pop();
                for (int i = 0; i != n->arcsNumber(); i++)
                {
                    GraphArc *arc = n->arc(i);
                    if ((Geometry::contain(node->pos(), arc->line())) &
                        (arc->floorUin() == floorUin) & !arc->contain(node))
                    {
                        GraphArc *arc1 = new GraphArc(arc->node1(), node);
                        emit graphItemAdded(arc1);
                        GraphArc *arc2 = new GraphArc(arc->node2(), node);
                        emit graphItemAdded(arc2);
                        delete arc;
                    }
                    else
                    {
                        GraphNode *next = n->adjacentNode(arc);
                        if (!vec.contains(next))
                        {
                            stk.push(next);
                            vec.append(next);
                        }
                    }
                }
            }
        }


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

void Graph::copyFloor(quint32 fromUin, quint32 toUin)
{
    GraphNode *n;
    QStack<GraphNode*> stk;
    QVector<GraphNode*> vec;

    int startNodesNumber = m_startNodes.size();
    for (int i = 0; i != startNodesNumber; i++)
    {
        GraphNode *startNode = m_startNodes.at(i);
        stk.push(startNode);
        vec.append(startNode);
        while (!stk.isEmpty())
        {
            n = stk.pop();
            m_lastNode = 0;

            if (n->floorUin() == fromUin)
            {
                addNode(n->pos(), toUin);
                for (int i = 0; i != n->arcsNumber(); i++)
                {
                    GraphNode *adjacentNode = n->adjacentNode(n->arc(i));
                    if (adjacentNode->floorUin() == fromUin)
                    {
                        addNode(adjacentNode->pos(), toUin);
                        addNode(n->pos(), toUin);
                    }
                }
            }
            int size = n->arcsNumber();
            for (int i = 0; i !=size; i++)
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

void Graph::setVisible(bool visible)
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
}

void Graph::startAnew()
{
    m_lastNode = 0;
}

void Graph::way(QVector<GraphNode*> start, QVector<GraphNode*> finish)
{
    qreal min = INFINITY;
    for (int i = 0; i != start.size(); i++)
        min = qMin(djkstra(start.at(i), &finish, min), min);
    if (!m_way.isEmpty())
        paintWay(true);
    else
        QMessageBox::information(0, tr("Нахождение пути"),
                                 tr("Путь между заданными объектами не найден."
                                    "\nВершины графа, относящиеся к объектам, "
                                    "лежат в разных компонентах связности."));
}

void Graph::clearWay()
{
    paintWay(false);
}

qreal Graph::djkstra(GraphNode *start, QVector<GraphNode*> *finish,
                     qreal minWayLength)
{
    typedef GraphNode* pGraphNode;
    QMap<pGraphNode, qreal> dist;
    QMap<pGraphNode, pGraphNode> prev;
    QSet<pGraphNode> visit;
    dist.insert(start, 0);
    GraphNode *min = start;
    while (min)
    {
        // Finding non visited node, which have minimum dest
        min = 0;
        QMap<pGraphNode, qreal>::iterator it = dist.begin();
        for (; it != dist.end(); ++it)
            if (!visit.contains(it.key()))
            {
                if (min)
                {
                    if (it.value() < dist.value(min))
                        min = it.key();
                }
                else
                    min = it.key();
            }

        if (min)
        {
            visit.insert(min);
            for (int i = 0; i != min->arcsNumber(); i++)
            {
                GraphNode *adjacent = min->adjacentNode(min->arc(i));
                bool update = false;
                if (dist.contains(adjacent))
                {
                    if (dist.value(min) + min->arc(i)->lenght() <
                        dist.value(adjacent))
                        update = true;
                }
                else
                    update = true;
                if (update)
                {
                    dist[adjacent] = dist.value(min) + min->arc(i)->lenght();
                    prev[adjacent] = min;
                }

            }
        }
    }
    min = 0;
    for (int i = 0; i != finish->size(); i++)
        if (dist.contains(finish->at(i)))
        {
            if (min != 0)
            {
                if (dist.value(finish->at(i)) < dist.value(min))
                    min = finish->at(i);
            }
            else
                min = finish->at(i);
        }

    if (min != 0)
    {
        if (dist.value(min) < minWayLength)
        {
            m_way.clear();
            GraphNode *node = min;
            while (node != start)
            {
                m_way.prepend(node);
                node = prev.value(node);
            }
            m_way.prepend(start);
        }
        return dist.value(min);
    }
    else
        return INFINITY;
}

void Graph::setLastNode(GraphNode *node)
{
    m_lastNode = node;
    emit lastNodeChanged(m_lastNode);
}

void Graph::paintWay(bool isActive)
{
    Qt::GlobalColor colorNode;
    Qt::GlobalColor colorArc;
    if (isActive)
        colorNode = colorArc = Qt::green;
    else
    {
        colorNode = Qt::white;
        colorArc = Qt::black;
    }
    for (int i = 0; i != m_way.size(); i++)
    {
        GraphNode *node = m_way.at(i);
        if (node)
        {
            if (isActive)
            {
                node->setBrush(GraphNode::cBrushWay);
                node->setVisible(true);
            }
            else
                node->setBrush(GraphNode::cBrushNormal);
        }

        if (i < m_way.size() - 1)
        {
            GraphArc *arc = node->arc(m_way.at(i+1));
            if (arc)
            {
                if (isActive)
                {
                    arc->setPen(GraphArc::cPenWay);
                    arc->setVisible(true);
                }
                else
                    arc->setPen(GraphArc::cPenNormal);
            }
        }

    }
}
