
// Qt
#include <QDebug>

// Application
#include "CLogManager.h"
#include "CUtility.h"
#include "CSocketStream.h"

//-------------------------------------------------------------------------------------------------

#define MAX_PENDING_BYTES   (2048 * 2048)

//-------------------------------------------------------------------------------------------------

CSocketStream::CSocketStream(const QString& sName, const QMap<QString, QString>& sParamMap)
: m_tMutex(QMutex::Recursive)
, m_sName(sName)
, m_pLocalServer(NULL)
, m_pServer(NULL)
, m_iPort(0)
{
	LOG_DEBUG(QString("CSocketStream::CSocketStream(%1)").arg(sName));

	// On d�termine d'apr�s l'adresse IP si on est serveur ou client
	if (sName.contains("0.0.0.0"))
	{
		// On r�cup�re le num�ro de port
		QStringList sNameList = sName.split(":");
		int iPort = 0;

		if (sNameList.count() > 1)
		{
			iPort = sNameList[1].toInt();
		}

		// Ecoute du port sp�cifi�
		bindTo(iPort);
	}
	else
	{
		// Connexion � l'adresse sp�cifi�e
		connectTo(sName);
	}

	// Gestion des �v�nements
	connect(&m_tSendTimer, SIGNAL(timeout()), this, SLOT(onSendOutput()));

	// On se place en mode ouvert en lecture/�criture dans la classe QIODevice
	QIODevice::open(QIODevice::ReadWrite);

	m_tSendTimer.start(20);
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::bindTo(int iPort)
{
	LOG_INFO(QString("Binding to %1...").arg(iPort));

	m_pLocalServer = new QTcpServer(this);

	connect(m_pLocalServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

	m_sHost = "0.0.0.0";
	m_iPort = iPort;

	if (m_pLocalServer->listen(QHostAddress::Any, (quint16) m_iPort))
	{
		m_pLocalServer->setMaxPendingConnections(10);

		LOG_INFO(QString("... serving on port %1").arg(m_iPort));
	}
	else
	{
		LOG_ERROR(m_pLocalServer->errorString());
	}
}

//-------------------------------------------------------------------------------------------------

bool CSocketStream::connectTo(QString sURL)
{
	LOG_INFO(QString("Connecting to %1...").arg(sURL));

	QStringList lTokens = sURL.split(":");

	// Lecture de l'h�te

	if (lTokens.count() > 0)
	{
		m_sHost = lTokens[0];
	}

	// Lecture du port

	if (lTokens.count() > 1)
	{
		m_iPort = lTokens[1].toInt();
	}

	QTimer::singleShot(1, this, SLOT(onReconnect()));

	return true;
}

//-------------------------------------------------------------------------------------------------

CSocketStream::~CSocketStream()
{
	LOG_DEBUG("CSocketStream::~CSocketStream()");

	// Destruction de tous les clients actifs
	foreach (QTcpSocket* pClient, m_vClients)
	{
		CClientData::deleteFromSocket(pClient);
		pClient->close();
		pClient->deleteLater();
	}

	// Destruction de la socket serveur
	if (m_pServer != NULL)
	{
		CClientData::deleteFromSocket(m_pServer);
		m_pServer->close();
		m_pServer->deleteLater();
	}

	// Fermeture du serveur
	if (m_pLocalServer != NULL)
	{
		m_pLocalServer->close();
		delete m_pLocalServer;
	}

	QIODevice::close();
}

//-------------------------------------------------------------------------------------------------

qint64 CSocketStream::bytesAvailable() const
{
	qint64 uiBytes = 0;

	if (m_pServer != NULL)
	{
		uiBytes = m_pServer->bytesAvailable();
	}
	else
	{
		if (hasConnections())
		{
			uiBytes = m_vClients[0]->bytesAvailable();
		}
	}

	return uiBytes + QIODevice::bytesAvailable();
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::onReconnect()
{
	if (m_pServer != NULL)
	{
		CClientData::deleteFromSocket(m_pServer);
		m_pServer->deleteLater();
		m_pServer = NULL;
	}

	// Cr�ation de la socket client
	m_pServer = new QTcpSocket(this);

	// Cr�ation de l'objet CClientData associ� � la socket
	new CClientData(m_pServer);

	// Connexion des signaux
	connect(m_pServer, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
	connect(m_pServer, SIGNAL(bytesWritten(qint64)), this, SLOT(onSocketBytesWritten(qint64)));
	connect(m_pServer, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

	// Connexion au serveur
	m_pServer->connectToHost(m_sHost, m_iPort);

	if (m_pServer->waitForConnected(1000) == false)
	{
		QTimer::singleShot(2000, this, SLOT(onReconnect()));

		// LOG_WARNING(QString("CSocketStream::onReconnect() : failed to connect to %1").arg(m_sHost));
	}
	else
	{
		LOG_DEBUG(QString("CSocketStream::onReconnect() : connected to %1").arg(m_sHost));
		emit connected();
	}
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::onNewConnection()
{
	LOG_DEBUG("CSocketStream::onNewConnection()");

	// R�cup�ration socket entrante
	QTcpSocket* pSocket = m_pLocalServer->nextPendingConnection();

	// Ajout de la socket au vecteur
	m_vClients.append(pSocket);

	// Cr�ation de l'objet CClientData associ� � la socket
	new CClientData(pSocket);

	// Connexion des signaux
	connect(pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
	connect(pSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(onSocketBytesWritten(qint64)));
	connect(pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::onSocketDisconnected()
{
	QTcpSocket* pSocket = dynamic_cast<QTcpSocket*>(QObject::sender());

	LOG_DEBUG(QString("CSocketStream::onSocketDisconnected(%1)").arg(pSocket->peerAddress().toString()));

	if (pSocket == m_pServer)
	{
		QTimer::singleShot(2000, this, SLOT(onReconnect()));

		emit disconnected();
	}
	else
	{
		// Retrait de la socket du vecteur

		for (int iIndex = 0; iIndex < m_vClients.count(); iIndex++)
		{
			if (m_vClients[iIndex] == pSocket)
			{
				CClientData::deleteFromSocket(pSocket);
				m_vClients[iIndex]->deleteLater();
				m_vClients.remove(iIndex);
				break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::onSocketReadyRead()
{
	int iBytesAvailable = 0;

	if (m_pServer != NULL)
	{
		iBytesAvailable = m_pServer->bytesAvailable();
	}
	else
	{
		if (hasConnections())
		{
			iBytesAvailable = m_vClients[0]->bytesAvailable();
		}
	}

	if (m_iMinBytesForReadyRead == 0 || iBytesAvailable >= m_iMinBytesForReadyRead)
	{
		emit readyRead();
	}
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::onSocketBytesWritten(qint64 iBytes)
{
	QTcpSocket* pSocket = dynamic_cast<QTcpSocket*>(QObject::sender());

	CClientData* pData = CClientData::getFromSocket(pSocket);

	if (pData != NULL)
	{
		pData->m_iBytesToWrite -= iBytes;
	}
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::onSendOutput()
{
	QMutexLocker locker(&m_tMutex);

	foreach (QTcpSocket* pClient, m_vClients)
	{
		sendOutputForSocket(pClient);
	}

	if (m_pServer != NULL)
	{
		sendOutputForSocket(m_pServer);
	}
}

//-------------------------------------------------------------------------------------------------

void CSocketStream::sendOutputForSocket(QTcpSocket* pSocket)
{
	CClientData* pData = CClientData::getFromSocket(pSocket);

	if (pData->m_baOutput.count() > 0)
	{
		// Est-ce que la socket est pr�te?
		if (pSocket->state() == QTcpSocket::ConnectedState)
		{
			// Est-ce que la socket a un buffer de sortie suffisament petit?
			if (pData->m_iBytesToWrite < MAX_PENDING_BYTES)
			{
				// qDebug() << "Sending " + QString::number(pData->m_baOutput.count()) + " bytes for " + QString::number(pSocket->socketDescriptor());

				// Ecriture des donn�es
				pSocket->write(pData->m_baOutput);

				// Incr�mentation du nombre d'octet en attente de partir
				pData->m_iBytesToWrite += pData->m_baOutput.count();

				// Rin�age du flux
				pSocket->flush();
			}
		}

		// Effacement du buffer
		pData->m_baOutput.clear();
	}
}

//-------------------------------------------------------------------------------------------------

//! Lecture de donn�es
qint64 CSocketStream::readData(char* data, qint64 maxSize)
{
	if (m_pServer != NULL)
	{
		return m_pServer->read(data, maxSize);
	}
	else
	{
		if (hasConnections())
		{
			return m_vClients[0]->read(data, maxSize);
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------

//! Ecriture de donn�es
qint64 CSocketStream::writeData(const char* data, qint64 maxSize)
{
	QMutexLocker locker(&m_tMutex);

	if (m_pServer != NULL)
	{
		CClientData* pData = CClientData::getFromSocket(m_pServer);

		if (pData->m_baOutput.count() + maxSize < MAX_PENDING_BYTES * 2)
		{
			pData->m_baOutput.append(data, maxSize);
		}
	}
	else
	{
		if (hasConnections())
		{
			foreach (QTcpSocket* pClient, m_vClients)
			{
				if (pClient->state() == QAbstractSocket::ConnectedState)
				{
					CClientData* pData = CClientData::getFromSocket(pClient);

					if (pData->m_baOutput.count() + maxSize < MAX_PENDING_BYTES * 2)
					{
						pData->m_baOutput.append(data, maxSize);
					}
				}
			}
		}
	}

	return maxSize;
}