
// Fondations
#include "CLogManager.h"

// Application
#include "CNamed.h"

//-------------------------------------------------------------------------------------------------

CNamed::CNamed()
{
}

//-------------------------------------------------------------------------------------------------

CNamed::~CNamed()
{
}

//-------------------------------------------------------------------------------------------------

void CNamed::setName(const QString& sName)
{
    m_sName = sName;
}

//-------------------------------------------------------------------------------------------------

const QString& CNamed::getName() const
{
    return m_sName;
}
