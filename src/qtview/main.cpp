/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  HEADERS  **********************/
#include <QApplication>
#ifdef HAVE_QT5_WEB_ENGINE_WIDGETS
	#include <QWebEngineView>
#elif defined(HAVE_QT5_WEBKIT_WIDGETS)
	#include <QWebView>
#else
	#error "Missing webkit headers !"
#endif
#include <QUrl>
#include <QTemporaryFile>
#include <QProcess>
#include <QDebug>
#include <QTcpSocket>
#include <time.h>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <cstdio>

/********************  MACRO  ***********************/
#define PORT_MIN 1025
#define PORT_MAX 65535

/********************  MACROS  **********************/
#ifdef HAVE_QT5_WEB_ENGINE_WIDGETS
	#define Browser QWebEngineView
#elif defined(HAVE_QT5_WEBKIT_WIDGETS)
	#define Browser QWebView
#else
	#error "Missing webkit headers !"
#endif

/*********************  CONSTS  *********************/
const char gblChars[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

/*******************  FUNCTION  *********************/
int randint(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

/*******************  FUNCTION  *********************/
QString genPassString(int size)
{
	QString buffer;
	for (int i = 0 ; i < size ; i++)
		buffer += gblChars[randint(0,sizeof(gblChars)-2)];
	return buffer;
}

/*******************  FUNCTION  *********************/
int main(int argc, char *argv[])
{
	//select a port
	srand(time(NULL));
	int port = randint(PORT_MIN,PORT_MAX);

	//start qapp
	int fakeArgc = 1;
	const char * fakeArgv[] = {"malt-qt5",NULL};
	QApplication app(fakeArgc,(char**)fakeArgv);
	
	//get user/pass
	QString user = genPassString(32);
	QString pass = genPassString(32);
	
	//build auth auth file
	QTemporaryFile authFile("/tmp/malt-tmp-auth-file-XXXXXX");
	authFile.setAutoRemove(true);
	authFile.open();
	
	//create file
	QStringList authFileArgs;
	authFileArgs << "-bc" << authFile.fileName() << user << pass;
	QProcess passwd;
	//qDebug() << MALT_PREFIX "/bin/malt-passwd" << authFileArgs;
	passwd.setProcessChannelMode(QProcess::ForwardedChannels);
	passwd.start(MALT_PREFIX "/bin/malt-passwd",authFileArgs);
	bool status = passwd.waitForFinished();
	if (status == false)
	{
		qDebug() << "Fail to create password file : " << authFile.fileName();
		return EXIT_FAILURE;
	}
	
	//start server
    QStringList serverArgs;
    serverArgs << "-p" << QString::number(port) << "--authfile" << authFile.fileName();
	for (int i = 1 ; i < argc ; i++)
		serverArgs << argv[i];
    QProcess server;
	server.setProcessChannelMode(QProcess::ForwardedChannels);
	//qDebug() << MALT_PREFIX "/bin/malt-webview" << serverArgs;
    server.start(MALT_PREFIX "/bin/malt-webview", serverArgs);
	
	//wait server
	QTcpSocket socket;
	printf("Wait server to be started...\n");
	int cnt = 0;
	while (true)
	{
		socket.connectToHost("localhost", port);
		if (socket.waitForConnected(3000))
			break;
		if (cnt++ > 500)
		{
			server.close();
			qDebug() << "Fail to start server !";
			return EXIT_FAILURE;
		}
		usleep(100000);
	}

	qDebug() << "OK";
	
	//create browser & connect to webserver
	Browser webview;
	QUrl url(QString("http://localhost:")+QString::number(port));
	url.setUserName(user);
	url.setPassword(pass);
	webview.load(url);
	webview.showMaximized();
	
	//wait exit
	int appStatus = app.exec();
	
	//close server
	server.terminate();
	server.waitForFinished(1000);
	
	//return status
	return appStatus;
}
