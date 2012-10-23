/**
   @author Shin'ichiro Nakaoka
*/

#include "MessageView.h"
#include "MainWindow.h"
#include "ExtensionManager.h"
#include "InfoBar.h"
#include <stack>
#include <iostream>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <QPlainTextEdit>
#include <QBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>
#include <QThread>
#include "gettext.h"

using namespace std;
using namespace boost;
using namespace cnoid;

namespace {
    
    class TextSink : public iostreams::sink
    {
    public:
        TextSink(MessageViewImpl* messageViewImpl, bool doFlush);
        std::streamsize write(const char* s, std::streamsize n);

        MessageViewImpl* messageViewImpl;
        QPlainTextEdit& textEdit;
        bool doFlush;
    };

    struct StdioInfo {
        streambuf* cout;
        streambuf* cerr;
    };

    enum MvCommand { MV_PUT, MV_CLEAR };

    class MessageViewEvent : public QEvent
    {
    public:
        MessageViewEvent(const QString& message, bool doLF, bool doNotify, bool doFlush)
            : QEvent(QEvent::User),
              command(MV_PUT),
              message(message),
              doLF(doLF),
              doNotify(doNotify),
              doFlush(doFlush) {
        }

        MessageViewEvent(MvCommand command)
            : QEvent(QEvent::User),
              command(command) {

        }
        
        MvCommand command;
        QString message;
        bool doLF;
        bool doNotify;
        bool doFlush;
    };
    
}

namespace cnoid {

    class MessageViewImpl
    {
    public:
        MessageView* self;

        Qt::HANDLE mainThreadId;
        
        QPlainTextEdit textEdit;

        TextSink textSink;
        iostreams::stream_buffer<TextSink> sbuf;
        std::ostream os;
        
        TextSink textSink_flush;
        iostreams::stream_buffer<TextSink> sbuf_flush;
        std::ostream os_flush;
        
        std::stack<StdioInfo> stdios;
        bool exitEventLoopRequested;

        MessageViewImpl(MessageView* self);

        void doPut(const QString& message, bool doLF, bool doNotify, bool doFlush) {

            textEdit.moveCursor(QTextCursor::End);
            if(!doLF){
                textEdit.insertPlainText(message);
            } else {
                textEdit.insertPlainText(message + "\n");
            }
            textEdit.moveCursor(QTextCursor::End);

            if(doNotify){
                InfoBar::instance()->notify(message);
            }
            if(doFlush){
                flush();
            }
        }

        void put(const QString& message, bool doLF, bool doNotify, bool doFlush) {

            if(QThread::currentThreadId() == mainThreadId){
                doPut(message, doLF, doNotify, doFlush);
            } else {
                MessageViewEvent* event = new MessageViewEvent(message, doLF, doNotify, doFlush);
                QCoreApplication::postEvent(self, event, Qt::NormalEventPriority);
            }
        }

        void handleMessageViewEvent(MessageViewEvent* event);
        
        void flush();

        void doClear(){
            textEdit.clear();
        }

        void clear() {
            if(QThread::currentThreadId() == mainThreadId){
                doClear();
            } else {
                QCoreApplication::postEvent(self, new MessageViewEvent(MV_CLEAR), Qt::NormalEventPriority);
            }
        }
    };
}


TextSink::TextSink(MessageViewImpl* messageViewImpl, bool doFlush)
    : messageViewImpl(messageViewImpl),
      textEdit(messageViewImpl->textEdit),
      doFlush(doFlush)
{

}


std::streamsize TextSink::write(const char* s, std::streamsize n)
{
    messageViewImpl->put(QString::fromLocal8Bit(s, n), false, false, doFlush);
    //messageViewImpl->put(QString::fromUtf8(s, n), false, false, doFlush);
    return n;
}


void MessageView::initialize(ExtensionManager* ext)
{
    static bool initialized = false;
    if(!initialized){
        ext->addView(mainInstance());
        initialized = true;
    }
}


MessageView* MessageView::mainInstance()
{
    static MessageView* messageView = new MessageView();
    return messageView;
}


MessageView::MessageView()
{
    impl = new MessageViewImpl(this);
}


MessageViewImpl::MessageViewImpl(MessageView* self) :
    self(self),
    mainThreadId(QThread::currentThreadId()), 
    textSink(this, false),
    sbuf(textSink),
    os(&sbuf),
    textSink_flush(this, true),
    sbuf_flush(textSink_flush),
    os_flush(&sbuf_flush)
{
    self->setName(N_("Message"));
    self->setDefaultLayoutArea(View::BOTTOM);

    textEdit.setFrameShape(QFrame::NoFrame);
    textEdit.setReadOnly(true);
    textEdit.setWordWrapMode(QTextOption::WrapAnywhere);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(&textEdit);
    self->setLayout(layout);
}


MessageView::~MessageView()
{
    delete impl;
}


bool MessageView::event(QEvent* e)
{
    MessageViewEvent* event = dynamic_cast<MessageViewEvent*>(e);
    if(event){
        impl->handleMessageViewEvent(event);
        return true;
    }
    return false;
}


void MessageViewImpl::handleMessageViewEvent(MessageViewEvent* event)
{
    switch(event->command){
    case MV_PUT:
        doPut(event->message, event->doLF, event->doNotify, event->doFlush);
        break;
    case MV_CLEAR:
        doClear();
        break;
    default:
        break;
    }
}


void MessageView::put(const std::string& message)
{
    impl->put(message.c_str(), false, false, false);
}


void MessageView::put(const boost::format& message)
{
    impl->put(message.str().c_str(), false, false, false);
}


void MessageView::put(const char* message)
{
    impl->put(message, false, false, false);
}


void MessageView::put(const QString& message)
{
    impl->put(message, false, false, false);
}


void MessageView::putln()
{
    impl->put(QString(), true, false, false);
}


void MessageView::putln(const std::string& message)
{
    impl->put(message.c_str(), true, false, false);
}


void MessageView::putln(const boost::format& message)
{
    impl->put(message.str().c_str(), true, false, false);
}


void MessageView::putln(const char* message)
{
    impl->put(message, true, false, false);
}


void MessageView::putln(const QString& message)
{
    impl->put(message, true, false, false);
}


void MessageView::notify(const std::string& message)
{
    impl->put(message.c_str(), true, true, false);
}


void MessageView::notify(const boost::format& message)
{
    impl->put(message.str().c_str(), true, true, false);
}


void MessageView::notify(const char* message)
{
    impl->put(message, true, true, false);
}


void MessageView::notify(const QString& message)
{
    impl->put(message, true, true, false);
}


/**
   @note Don't call this function from an expose event handler
   because it may cause a hangup of rendering.
*/
void MessageView::flush()
{
    impl->flush();
}


void MessageViewImpl::flush()
{
    if(QThread::currentThreadId() == mainThreadId){
#ifdef Q_OS_WIN32
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
#else
        //QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        while(QCoreApplication::hasPendingEvents()){
            QCoreApplication::processEvents();
        }
#endif
    }
}


void MessageView::clear()
{
    impl->clear();
}


std::ostream& MessageView::cout(bool doFlush)
{
    return doFlush ? impl->os_flush : impl->os;
}


void MessageView::beginStdioRedirect()
{
    StdioInfo info;
    info.cout = std::cout.rdbuf();
    info.cerr = std::cerr.rdbuf();
    impl->stdios.push(info);
    std::cout.rdbuf(impl->os.rdbuf());
    std::cerr.rdbuf(impl->os.rdbuf());
}


void MessageView::endStdioRedirect()
{
    if(!impl->stdios.empty()){
        StdioInfo& info = impl->stdios.top();
        std::cout.rdbuf(info.cout);
        std::cerr.rdbuf(info.cerr);
        impl->stdios.pop();
    }
}


void cnoid::showWarningDialog(const QString& message)
{
    QMessageBox::warning(MainWindow::instance(), _("Warning"), message);
}

void cnoid::showWarningDialog(const char* message)
{
    showWarningDialog(QString(message));
}

void cnoid::showWarningDialog(const std::string& message)
{
    showWarningDialog(message.c_str());
}

void cnoid::showWarningDialog(const boost::format& message)
{
    showWarningDialog(message.str().c_str());
}
