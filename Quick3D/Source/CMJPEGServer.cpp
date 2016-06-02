
// Qt
#include <QString>
#include <QVariant>
#include <QImage>
#include <QBuffer>

// Application
#include "CLogManager.h"
#include "CMJPEGServer.h"
#include "CUtility.h"

//-------------------------------------------------------------------------------------------------

// Marqueur de limite HTTP compatible Firefox
static QString MJPEGBoundaryMarker = "7b3cc56e5f51db803f790dad720ed50a";

//-------------------------------------------------------------------------------------------------

CMJPEGThread::CMJPEGThread(CMJPEGServer* pParent)
: m_pParent(pParent)
, m_bRun(true)
{
	start();
}

//-------------------------------------------------------------------------------------------------

CMJPEGThread::~CMJPEGThread()
{
	m_bRun = false;
	wait();
}

//-------------------------------------------------------------------------------------------------

void CMJPEGThread::run()
{
	while (m_bRun)
	{
		m_pParent->processOutputImages();
		msleep(20);
	}
}

//-------------------------------------------------------------------------------------------------

CMJPEGServer::CMJPEGServer(int iPort)
: CHTTPServer(iPort)
, m_tTimer(this)
, m_tMutex(QMutex::Recursive)
, m_pOutputFile(NULL)
{
	m_iCompressionRate = -1;

	if(m_iCompressionRate >= 0) m_iCompressionRate = 100 - m_iCompressionRate;

	LOG_DEBUG(QString("CMJPEGServer::CMJPEGServer(%1)").arg(iPort));

	connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));

	m_tTimer.start(20);

	m_pThread = new CMJPEGThread(this);
}

//-------------------------------------------------------------------------------------------------

CMJPEGServer::CMJPEGServer(QString sFileName)
: CHTTPServer(0)
, m_tTimer(this)
, m_tMutex(QMutex::Recursive)
, m_pOutputFile(NULL)
{
	LOG_DEBUG(QString("CMJPEGServer::CMJPEGServer(%1)").arg(sFileName));

	m_sFileName = sFileName;

	m_pOutputFile = new QFile(m_sFileName);

	if (m_pOutputFile->open(QIODevice::Append))
	{
		QString sMessage = getHeader();
		m_pOutputFile->write(sMessage.toLatin1());
	}
	else
	{
		delete m_pOutputFile;
		m_pOutputFile = NULL;
	}

	connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));

	m_tTimer.start(10);

	m_pThread = new CMJPEGThread(this);
}

//-------------------------------------------------------------------------------------------------

CMJPEGServer::~CMJPEGServer()
{
	LOG_DEBUG("CMJPEGServer::~CMJPEGServer()");

	m_tTimer.stop();

	delete m_pThread;

	QMutexLocker locker(&m_tMutex);

	if (m_pOutputFile != NULL)
	{
		m_pOutputFile->close();
		delete m_pOutputFile;
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::send(const QByteArray& baData)
{
	// Si on �crit dans un fichier ou qu'on a des connections actives
	if (m_pOutputFile != NULL || hasConnections())
	{
		// Si les donn�es entrantes existent
		if (baData.count() > 0)
		{
			// Verrouillage du mutex de buffer
			QMutexLocker locker(&m_tMutex);

			// On ajoute les donn�es dans notre buffer de sortie
			if (m_vOutput.count() < 10)
			{
				m_vOutput.append(baData);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::sendRaw(const QByteArray& baData, int iWidth, int iHeight)
{
	// Si on �crit dans un fichier ou qu'on a des connections actives
	if (m_pOutputFile != NULL || hasConnections())
	{
		// Cr�ation d'une image � la taille sp�cifi�e
		QImage image(QSize(iWidth, iHeight), QImage::Format_RGB888);

		// Recopie des donn�es brutes d'image dans la QImage
		memcpy(image.bits(), baData.constData(), baData.count());

		// Verrouillage du mutex de buffer
		QMutexLocker locker(&m_tMutex);

		if (m_vOutputImages.count() < 10)
		{
			m_vOutputImages.append(image);
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::sendImage(const QImage& image)
{
	if (m_pOutputFile != NULL || hasConnections())
	{
		if (image.width() > 0 && image.height() > 0)
		{
			// Verrouillage du mutex de buffer
			QMutexLocker locker(&m_tMutex);

			if (m_vOutputImages.count() < 10)
			{
				m_vOutputImages.append(image.copy());
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::flush()
{
	onTimeout();
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::getContent(const CWebContext& tContext, QString& sHead, QString& sBody, QString& xmlResponse, QString& sCustomResponse)
{
	// Ajout de la socket au vecteur
	QMutexLocker locker(&m_tMutex);

	if (m_vSockets.contains(tContext.m_pSocket) == false)
	{
		m_vSockets.append(tContext.m_pSocket);
	}

	CClientData* pData = CClientData::getFromSocket(tContext.m_pSocket);

	if (pData != NULL)
	{
		pData->m_vUserData["BytesToWrite"] = QVariant((qlonglong) 0);

		pData->m_vUserData["IsFirstImage"] = QVariant(true);

		QMap<QString, QString> args = tContext.m_mArguments;
		if(args.contains("ack"))
		{
			pData->m_vUserData["Ack"] = QVariant(true);
		}
		else
		{
			pData->m_vUserData["Ack"] = QVariant(false);
		}
	}

	sCustomResponse = getHeader();
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::handleSocketDisconnection(QTcpSocket* pSocket)
{
	LOG_DEBUG(QString("CMJPEGServer::handleSocketDisconnection() IP:%1 port:%2 ")
		.arg(pSocket->peerAddress().toString())
		.arg(pSocket->peerPort())
		);

	// Retrait de la socket du vecteur
	QMutexLocker locker(&m_tMutex);

	if (m_vSockets.contains(pSocket))
	{
		m_vSockets.remove(m_vSockets.indexOf(pSocket));
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::handleSocketBytesWritten(QTcpSocket* pSocket, qint64 iBytes)
{
	CClientData* pData = CClientData::getFromSocket(pSocket);

	if (pData != NULL)
	{
		qlonglong iBytesToWrite = pData->m_vUserData["BytesToWrite"].toLongLong();

		iBytesToWrite -= (qlonglong) iBytes;

		if (iBytesToWrite < 0)
		{
			iBytesToWrite = 0;
		}

		pData->m_vUserData["BytesToWrite"] = QVariant(iBytesToWrite);
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::handleSocketBytesRead(QTcpSocket* pSocket)
{
	CClientData* pData = CClientData::getFromSocket(pSocket);

	// On lit une ligne dans la socket
	QString sLine = pSocket->readLine();

	if(sLine.compare(QString("Next image")))
	{
		pData->m_vUserData["SendNextImage"] = QVariant(true);
	}
	else
	{
		LOG_DEBUG(QString("CMJPEGServer::handleSocketBytesRead() >>> %1").arg(sLine));
	}
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::onTimeout()
{
	if (m_vOutput.count() > 0)
	{
		if (m_pOutputFile != NULL)
		{
			if (m_pOutputFile->isOpen())
			{
				// Verrouillage du mutex de buffer
				QMutexLocker locker(&m_tMutex);

				// Parcours de tous les buffers en attente de sortie

				foreach (const QByteArray& baOutput, m_vOutput)
				{
					// Composition du message de sortie :
					// - Marqueur de limite
					// - Descriptif HTML de contenu
					// - Image jpeg
					QString sMessage = getImageDescriptor(baOutput.count());

					// Ecriture des donn�es
					m_pOutputFile->write(sMessage.toLatin1());
					m_pOutputFile->write(baOutput);
					m_pOutputFile->flush();
				}

				m_vOutput.clear();
			}
			else
			{	// Verrouillage du mutex de buffer
				QMutexLocker locker(&m_tMutex);

				m_vOutput.clear();
			}
		}
		else
		{
			// Verrouillage du mutex de buffer
			QMutexLocker locker(&m_tMutex);

			// Parcours de tous les clients connect�s
			foreach (QTcpSocket* pSocket, m_vSockets)
			{
				CClientData* pData = CClientData::getFromSocket(pSocket);

				if (pData != NULL)
				{
					// Si la socket est pr�te
					if (pSocket->state() == QTcpSocket::ConnectedState)
					{	
						bool bAck = pData->m_vUserData["Ack"].toBool();

						if(bAck)
						{
							const QByteArray& baOutput =  m_vOutput[m_vOutput.size() - 1];

							bool bSendNextImage = pData->m_vUserData["SendNextImage"].toBool();
							bool bIsFirstImage = pData->m_vUserData["IsFirstImage"].toBool();
							
							if(bSendNextImage || bIsFirstImage)
							{
								qlonglong iBytesToWrite = pData->m_vUserData["BytesToWrite"].toULongLong();

								// Si la socket a un buffer de sortie suffisament petit
								if (iBytesToWrite < baOutput.count() * 4)
								{
									// Composition du message de sortie :
									// - Marqueur de limite
									// - Descriptif HTML de contenu
									// - Image jpeg
									QByteArray sMessage = getImageDescriptor(baOutput.count()).toLatin1();

									// Ecriture des donn�es sur la socket
									pSocket->write(sMessage);
									pSocket->write(baOutput);

									pData->m_vUserData["BytesToWrite"] = iBytesToWrite + (qlonglong) (sMessage.count() + baOutput.count());
								}

								if (pSocket->state() == QTcpSocket::ConnectedState)
								{
									// Rin�age du flux
									pSocket->flush();
								}

								if(bIsFirstImage) pData->m_vUserData["IsFirstImage"] = QVariant(false);

								pData->m_vUserData["SendNextImage"] = QVariant(false);
							}
						}
						else
						{
							foreach (const QByteArray& baOutput, m_vOutput)
							{
								qlonglong iBytesToWrite = pData->m_vUserData["BytesToWrite"].toULongLong();

								// Si la socket a un buffer de sortie suffisament petit
								if (iBytesToWrite < baOutput.count() * 4)
								{
									// Composition du message de sortie :
									// - Marqueur de limite
									// - Descriptif HTML de contenu
									// - Image jpeg
									QByteArray sMessage = getImageDescriptor(baOutput.count()).toLatin1();

									// Ecriture des donn�es sur la socket
									pSocket->write(sMessage);
									pSocket->write(baOutput);

									pData->m_vUserData["BytesToWrite"] = iBytesToWrite + (qlonglong) (sMessage.count() + baOutput.count());
								}

								if (pSocket->state() == QTcpSocket::ConnectedState)
								{
									// Rin�age du flux
									pSocket->flush();
								}
							}
						}
					}
				}
			}

			m_vOutput.clear();
		}
	}
}

//-------------------------------------------------------------------------------------------------
// Attention : cette m�thode est appel�e depuis une thread annexe

void CMJPEGServer::processOutputImages()
{
	// Verrouillage du mutex de buffer
	QMutexLocker locker(&m_tMutex);

	foreach (const QImage& image, m_vOutputImages)
	{
		if (image.width() > 0 && image.height() > 0)
		{
			m_vOutput.append(CUtility::convertQImageToByteArray(image, "JPG", m_iCompressionRate));
		}
	}

	m_vOutputImages.clear();
}

//-------------------------------------------------------------------------------------------------

QString CMJPEGServer::getHeader()
{
	// Composition d'un message HTTP de r�ponse

	return QString(
		"Content-Type: multipart/x-mixed-replace;boundary=%1\r\n"
		"Cache-Control: no-store\r\n"
		"Pragma: no-cache\r\n"
		"\r\n"
		).arg(MJPEGBoundaryMarker);
}

//-------------------------------------------------------------------------------------------------

QString CMJPEGServer::getImageDescriptor(int iSize)
{
	// Composition d'un message de description d'image
	// - Marqueur de limite
	// - Descriptif HTML de contenu

	return QString(
		"--%1\r\n"
		"Content-Type: image/jpeg\r\n"
		"Content-length: %2\r\n"
		"\r\n")
		.arg(MJPEGBoundaryMarker)
		.arg(iSize);
}

//-------------------------------------------------------------------------------------------------

void CMJPEGServer::saveMJPEG(const QVector<QImage>& vImages, QString sFileName)
{
	QFile file(sFileName);

	if (vImages.count() > 0)
	{
		if (file.open(QIODevice::WriteOnly))
		{
			QString sMessage = getHeader();
			file.write(sMessage.toLatin1());

			foreach (QImage image, vImages)
			{
				QByteArray baImage;
				QBuffer buffer(&baImage);
				buffer.open(QIODevice::WriteOnly);
				image.save(&buffer, "JPG");
				buffer.close();

				QString sMessage = getImageDescriptor(baImage.count());
				file.write(sMessage.toLatin1());
				file.write(baImage);
			}

			file.close();
		}
	}
}