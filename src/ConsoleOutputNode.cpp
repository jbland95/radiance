#include "ConsoleOutputNode.h"

ConsoleOutputNode::ConsoleOutputNode(Context *context, QSize chainSize)
    : SelfTimedReadBackOutputNode(context, chainSize, 10) {

    connect(this, &SelfTimedReadBackOutputNode::frame, this, &ConsoleOutputNode::onFrame, Qt::DirectConnection);

    start();
}

void ConsoleOutputNode::onFrame(QSize size, QByteArray frame) {
    qDebug() << size << frame;
}

QString ConsoleOutputNode::typeName() {
    return "ConsoleOutputNode";
}

VideoNodeSP *ConsoleOutputNode::deserialize(Context *context, QJsonObject obj) {
    auto e = new ConsoleOutputNode(context, QSize(4, 4));
    return new VideoNodeSP(e);
}

bool ConsoleOutputNode::canCreateFromFile(QString filename) {
    return false;
}

VideoNodeSP *ConsoleOutputNode::fromFile(Context *context, QString filename) {
    return nullptr;
}

QMap<QString, QString> ConsoleOutputNode::customInstantiators() {
    auto m = QMap<QString, QString>();
    m.insert("ConsoleOutput", "ConsoleOutputInstantiator.qml");
    return m;
}
