#include "QtDisplay.hpp"

#include <QCoreApplication>
#include <QWindow>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDockWidget>
#include <QLineEdit>

#include "Device.hpp"
#include "OGL_RenderContext.hpp"
#include "OGL_Renderer.hpp"
#include "OGL_DeferredRenderer.hpp"


#include "TESTSCENE.hpp"

QtDisplay::~QtDisplay()
{
	DEVICE->Kill();
}


void
QtDisplay::Initialize()
{
	resize(1280, 720);

	m_Context = new OGL_RenderContext();
	m_Context->Initialize(&m_RenderWindow);

	m_Renderer = new OGL_DeferredRenderer(1280, 720);
	m_Renderer->Initialize();

	DEVICE->Initialize(1280, 720);
	DEVICE->SetRenderContext(m_Context);
	DEVICE->SetRenderer(m_Renderer);

	//setCentralWidget(center);
	//m_RootLayout = new QVBoxLayout(center);
	//m_RootLayout->addWidget(renderWidget);
	QWidget* renderWidget = QWidget::createWindowContainer(&m_RenderWindow);
	setCentralWidget(renderWidget);

	QDockWidget* dockWidget = new QDockWidget();
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
	QWidget* dockRoot = new QWidget(dockWidget);
	dockWidget->setWidget(dockRoot);

	QVBoxLayout* layout = new QVBoxLayout(dockRoot);
	QWidget* lineedit = new QLineEdit();
	layout->addWidget(lineedit);

	INIT_TEST_SCENE();
}


bool
QtDisplay::event(QEvent* _event)
{
	bool eventWasProcessed = true;

	switch (_event->type())
	{
	case QEvent::UpdateRequest:
	{
		if (!m_Resizing)
		{
			DEVICE->Update(1.f / 60.f);
			m_Context->MakeCurrent();
			DEVICE->Render();
			DEVICE->SwapBuffers();
		}
		QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));

	} break;
	case QEvent::Resize:
	{
		m_Resizing = true;
		QResizeEvent* resizeData = (QResizeEvent*)_event;
		DEVICE->SetDimensions(resizeData->size().width(), resizeData->size().height());
		m_Resizing = false;
	} break;
	default:
		eventWasProcessed = QMainWindow::event(_event);
	}

	return eventWasProcessed;
}
