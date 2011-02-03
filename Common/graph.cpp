#include "graph.h"

Graph::Graph()
{
    m_lastNode = 0;
    m_visible = true;
}

QDataStream & operator<<(QDataStream &output, const Graph &graph)
{
    QVector<GraphNode*> nodes;
    QVector<GraphArc*> arcs;
    GraphNode *n;
    QStack<GraphNode*> stk;
    for (int i = 0; i < graph.m_nodes.size(); i++)
    {
        stk.push(graph.m_nodes.at(i));
        nodes.append(graph.m_nodes.at(i));
        while (!stk.isEmpty())
        {
            n = stk.pop();
            output << *n  << graph.m_nodes.contains(n);
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
                output << *arc;
            }
        }
    output << GraphArc::None << GraphArc::Undefined << 0 << 0; // End of arcs
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
            graph.m_nodes.append(node);
        emit graph.graphItemAdded(node);
        node = new GraphNode();
        input >> *node >> isStartNode;
    }
    GraphArc *arc;
    quint32 node1uin, node2uin;
    int type;
    int direction = 0;
    bool b = false;
    input >> type >> direction >> node1uin >> node2uin;
//    input >> b >> type >> node1uin >> node2uin;
    while (node1uin != 0)
    {
        arc = new GraphArc(nodes.value(node1uin), nodes.value(node2uin),
                           GraphArc::VerticalType(type),
                           GraphArc::VerticalDirection(direction)/*, b*/);
        emit graph.graphItemAdded(arc);
        input >> type >> direction >> node1uin >> node2uin;
//        input >> b >> type >> node1uin >> node2uin;
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
    for (int i = 0; i < m_nodes.size(); i++)
    {
        GraphNode *startNode = m_nodes.at(i);
        stk.push(startNode);
        vec.append(startNode);
        while (!stk.isEmpty())
        {
            n = stk.pop();
            // Removing surplus nodes
            if (n != startNode)
            {
                int i = m_nodes.indexOf(n);
                if (i > -1)
                    m_nodes.remove(i);
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
        for (int i = 0; i < m_nodes.size(); i++)
        {
            GraphNode *startNode = m_nodes.at(i);
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
        m_nodes.append(node);
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
    int i = m_nodes.indexOf(node);
    if (i > -1)
        m_nodes.remove(i);
    GraphNode *adjNode;
    for (int i = 0; i != node->arcsNumber(); i++)
    {
        adjNode = node->adjacentNode(node->arc(i));
        if (!m_nodes.contains(adjNode))
            m_nodes.append(adjNode);
    }
    emit graphItemDeleted(node);
    delete node;
}

void Graph::copyFloor(quint32 fromUin, quint32 toUin)
{
    GraphNode *n;
    QStack<GraphNode*> stk;
    QVector<GraphNode*> vec;

    int startNodesNumber = m_nodes.size();
    for (int i = 0; i != startNodesNumber; i++)
    {
        GraphNode *startNode = m_nodes.at(i);
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
    m_visible = visible;
    GraphNode *n;
    QStack<GraphNode*> stk;
    QVector<GraphNode*> vec;
    for (int i = 0; i < m_nodes.size(); i++)
    {
        GraphNode *startNode = m_nodes.at(i);
        stk.push(startNode);
        vec.append(startNode);
        while (!stk.isEmpty())
        {
            n = stk.pop();
            // Removing surplus nodes
            if (n != startNode)
            {
                int i = m_nodes.indexOf(n);
                if (i > -1)
                    m_nodes.remove(i);
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

void Graph::setStartNodes(const QVector<GraphNode *> nodes)
{
    m_startNodes = nodes;
}

void Graph::setFinishNodes(const QVector<GraphNode *> nodes)
{
    m_finishNodes = nodes;
}

bool Graph::isStartAndFinishNodesValid() const
{
    return !m_startNodes.isEmpty() & !m_finishNodes.isEmpty();
}

void Graph::makeWay(const GraphArc::WayPermissions permissions)
{
    clearWay();
    qreal min = INFINITY;
    for (int i = 0; i != m_startNodes.size(); i++)
        min = qMin(djkstra(
                m_startNodes.at(i), &m_finishNodes, permissions, min), min);
//    if (!m_way.isEmpty())
//        paintWay(true);
//    else
//        QMessageBox::information(0, tr("Нахождение пути"),
//                                 tr("Путь между заданными объектами не найден."
//                                    "\nВершины графа, относящиеся к объектам, "
//                                    "лежат в разных компонентах связности."));
}

void Graph::clearWay()
{
    paintWay(false);
    m_way.clear();
}

QVector<GraphNode*> Graph::way() const
{
    return m_way;
}

//bool Graph::wayInfo(qreal &lengthPix, int &stairsDownNumber,
//                    int &stairsUpNumber, int &liftsNumber, qreal &time) const
//{

//}

qreal Graph::djkstra(GraphNode *start, const QVector<GraphNode *> *finish,
                     const GraphArc::WayPermissions permissions,
                     const qreal minWayLength)
{
    typedef GraphNode* pGraphNode;
    QMap<pGraphNode, qreal> dist;
    QMap<pGraphNode, pGraphNode> prev;
    QSet<pGraphNode> visit;
    dist.insert(0, INFINITY);
    dist.insert(start, 0);
    GraphNode *min = start;
    while (min)
    {
        // Finding non visited node, which have minimum dest
        min = 0;
        QMap<pGraphNode, qreal>::iterator it = dist.begin();
        for (; it != dist.end(); ++it)
            if (!visit.contains(it.key()))
//            {
//                if (min)
//                {
                    if (it.value() < dist.value(min))
                        min = it.key();
//                }
//                else
//                    min = it.key();
//            }

        if (min)
        {
            visit.insert(min);
            for (int i = 0; i != min->arcsNumber(); i++)
            {
                GraphArc::VerticalType vertType = min->arc(i)->verticalType();
                if ((((permissions & vertType) > 0) |
                     (vertType == GraphArc::None)) & min->arc(i)->isRight(min))
                {
                    GraphNode *adjacent = min->adjacentNode(min->arc(i));
                    qreal distPerMin = dist.value(min) + min->arc(i)->lenght();
                    bool update = false;
                    if (dist.contains(adjacent))
//                    {
                          update = distPerMin < dist.value(adjacent);
//                        if (distPerMin < dist.value(adjacent))
//                            update = true;
//                    }
                    else
                        update = true;
                    if (update)
                    {
                        dist[adjacent] = distPerMin;
                        prev[adjacent] = min;
                    }
                }
//                else
//                    visit.insert(min->adjacentNode(min->arc(i)));
            }
        }
    }
    min = 0;
    for (int i = 0; i != finish->size(); i++)
        if (dist.contains(finish->at(i)))
//        {
//            if (min != 0)
//            {
                if (dist.value(finish->at(i)) < dist.value(min))
                    min = finish->at(i);
//            }
//            else
//                min = finish->at(i);
//        }

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
    if (!m_way.isEmpty())
    {
        QPen penArc;
        QPen penNode = Global::pen;
        QBrush brushNode;
        bool visible;
        if (isActive)
            visible = true;
        else
            visible = m_visible;
        if (isActive)
        {
            penArc = GraphArc::cPenWay;
            brushNode = GraphNode::cBrushWay;
        }
        else
        {
            penArc = GraphArc::cPenNormal;
            brushNode = GraphNode::cBrushNormal;
        }
        for (int i = 0; i != m_way.size() - 1; i++)
        {
            GraphNode *node = m_way.at(i);
            if (node)
            {
                node->setPen(penNode);
                node->setBrush(brushNode);
                node->setVisible(visible);
            }

    //        if (i < m_way.size() - 1)
    //        {
                GraphArc *arc = node->arc(m_way.at(i+1));
                if (arc)
                {
                    arc->setPen(penArc);
                    if (isActive)
                        arc->setVisible(true);
                    else
                        arc->setVisible(m_visible);
                }
    //        }

        }
        GraphNode *node = m_way.at(m_way.size() - 1);
        if (node)
        {
            node->setPen(penNode);
            node->setBrush(QBrush(Qt::red));
            node->setVisible(visible);
        }
    }
}
