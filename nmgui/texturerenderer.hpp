#ifndef NMGUI_TEXTURERENDERER_HPP
#define NMGUI_TEXTURERENDERER_HPP

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <nmlib/model.hpp>

namespace nmgui {

class TextureRenderer : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(nm::InputLink* inputLink READ inputLink WRITE setInputLink NOTIFY inputLinkChanged)
    Q_PROPERTY(nm::OutputLink* outputLink READ outputLink WRITE setOutputLink NOTIFY outputLinkChanged)
public:
    explicit TextureRenderer(QQuickItem *the_parent = 0);

    qreal t() const {return m_t;}
    nm::InputLink *inputLink() const {return m_inputLink;}
    nm::OutputLink *outputLink() const {return m_outputLink;}
    void setT(qreal new_t);
    void setInputLink(nm::InputLink* newLink);
    void setOutputLink(nm::OutputLink* newLink);

signals:
    void tChanged();
    void inputLinkChanged();
    void outputLinkChanged();

public slots:
    void paint();
    void cleanup();
    void sync();

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void handleModelChanged();

private:
    void compileProgram();
    QOpenGLShaderProgram *m_program;

    bool m_generatorDirty;
    std::string m_thread_generatorFunctionSource;
    bool m_thread_sourceDirty;
    nm::InputLink *m_inputLink;
    nm::OutputLink *m_outputLink;
    qreal m_t;
    qreal m_thread_t;
};

} // namespace nmgui

#endif // NMGUI_TEXTURERENDERER_HPP