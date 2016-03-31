#ifndef __QT_DISPLAY_HPP__
#define __QT_DISPLAY_HPP__

#include <QMainWindow>
#include <QWindow>

class QLayout;

class OGL_RenderContext;
class OGL_Renderer;


class QtDisplay
	:public QMainWindow
{
	Q_OBJECT
public:
	~QtDisplay();
	void	Initialize();

protected:
	virtual bool	event(QEvent* _event) override;

private:
	QLayout*			m_RootLayout;

	QWindow				m_RenderWindow;
	OGL_RenderContext*	m_Context;
	OGL_Renderer*		m_Renderer;

};


#endif /*__QT_DISPLAY_HPP__*/